// audio_engine.hpp
// ヘッダーオンリー実装 v2
// Generator抽象化 + ロックフリーコールバック

#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <string>
#include <array>
#include <algorithm>
#include <iostream>
#include <numbers>

extern "C" {
#include <portaudio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
inline std::string utf16_to_utf8(const std::wstring& w) {
    if (w.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0,
        w.data(), (int)w.size(),
        nullptr, 0, nullptr, nullptr);

    std::string s(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0,
        w.data(), (int)w.size(),
        s.data(), len, nullptr, nullptr);

    return s;
}
namespace audio {

    // =============================================================================
    // 定数
    // =============================================================================
    constexpr int MASTER_SAMPLE_RATE = 48000;
    constexpr int MASTER_CHANNELS = 2;
    constexpr int FRAMES_PER_BUFFER = 512;
    constexpr size_t RING_BUFFER_FRAMES = 48000;
    constexpr size_t MAX_ACTIVE_SOURCES = 64;

    using SoundId = uint32_t;

    // =============================================================================
    // ロックフリーリングバッファ（SPSC）
    // =============================================================================
    template<typename T>
    class RingBuffer {
    public:
        explicit RingBuffer(size_t capacity)
            : buffer_(capacity), capacity_(capacity), write_pos_(0), read_pos_(0) {
        }

        size_t write(const T* data, size_t count) {
            const size_t write = write_pos_.load(std::memory_order_relaxed);
            const size_t read = read_pos_.load(std::memory_order_acquire);
            const size_t available = capacity_ - (write - read);
            const size_t to_write = std::min(count, available);

            for (size_t i = 0; i < to_write; ++i) {
                buffer_[(write + i) % capacity_] = data[i];
            }
            write_pos_.store(write + to_write, std::memory_order_release);
            return to_write;
        }

        size_t read(T* data, size_t count) {
            const size_t read = read_pos_.load(std::memory_order_relaxed);
            const size_t write = write_pos_.load(std::memory_order_acquire);
            const size_t available = write - read;
            const size_t to_read = std::min(count, available);

            for (size_t i = 0; i < to_read; ++i) {
                data[i] = buffer_[(read + i) % capacity_];
            }
            read_pos_.store(read + to_read, std::memory_order_release);
            return to_read;
        }

        size_t available_write() const {
            const size_t write = write_pos_.load(std::memory_order_relaxed);
            const size_t read = read_pos_.load(std::memory_order_acquire);
            return capacity_ - (write - read);
        }

        void clear() {
            read_pos_.store(write_pos_.load(std::memory_order_relaxed), std::memory_order_release);
        }

    private:
        std::vector<T> buffer_;
        size_t capacity_;
        std::atomic<size_t> write_pos_;
        std::atomic<size_t> read_pos_;
    };

    // =============================================================================
    // Generator インターフェース（音源抽象）
    // =============================================================================
    struct GeneratorState {
        bool finished = false;
        int64_t position = 0;
        int64_t duration = 0;
    };

    class IAudioGenerator {
    public:
        virtual ~IAudioGenerator() = default;

        // PCMを生成（コールバックから呼ばれる）
        virtual size_t generate(float* out, size_t frames) = 0;

        // 状態取得
        virtual GeneratorState state() const = 0;

        // シーク
        virtual void seek(int64_t position_samples) { (void)position_samples; }

        // リセット
        virtual void reset() { seek(0); }
    };

    // =============================================================================
    // FileGenerator - ファイル音源（FFmpeg）
    // =============================================================================
    class FileGenerator : public IAudioGenerator {
    public:
        explicit FileGenerator(const std::string& path) : path_(path) {
            ring_buffer_ = std::make_unique<RingBuffer<float>>(RING_BUFFER_FRAMES * MASTER_CHANNELS);
            if (open_file()) {
                start_decode_thread();
            }
        }

        ~FileGenerator() {
            stop_decode_thread();
            close_file();
        }

        size_t generate(float* out, size_t frames) override {
            size_t samples = frames * MASTER_CHANNELS;
            size_t read = ring_buffer_->read(out, samples);

            if (read == 0 && finished_.load(std::memory_order_relaxed)) {
                return 0;
            }

            if (read < samples) {
                std::memset(out + read, 0, (samples - read) * sizeof(float));
            }

            position_.fetch_add(read / MASTER_CHANNELS, std::memory_order_relaxed);
            return read / MASTER_CHANNELS;
        }

        GeneratorState state() const override {
            return {
                .finished = finished_.load(std::memory_order_relaxed),
                .position = position_.load(std::memory_order_relaxed),
                .duration = duration_
            };
        }

        void seek(int64_t position_samples) override {
            if (!format_ctx_) return;

            seeking_.store(true, std::memory_order_release);
            ring_buffer_->clear();

            int64_t timestamp = av_rescale_q(position_samples,
                { 1, MASTER_SAMPLE_RATE },
                format_ctx_->streams[audio_stream_index_]->time_base);
            av_seek_frame(format_ctx_, audio_stream_index_, timestamp, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(codec_ctx_);

            position_.store(position_samples, std::memory_order_relaxed);
            finished_.store(false, std::memory_order_relaxed);
            seeking_.store(false, std::memory_order_release);
        }

        void reset() override { seek(0); }
        void set_looping(bool loop) { looping_.store(loop, std::memory_order_relaxed); }
        bool is_valid() const { return format_ctx_ != nullptr; }

    private:
        bool open_file() {
            if (avformat_open_input(&format_ctx_, path_.c_str(), nullptr, nullptr) < 0) {
                return false;
            }
            if (avformat_find_stream_info(format_ctx_, nullptr) < 0) {
                avformat_close_input(&format_ctx_);
                return false;
            }

            audio_stream_index_ = -1;
            for (unsigned i = 0; i < format_ctx_->nb_streams; ++i) {
                if (format_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                    audio_stream_index_ = i;
                    break;
                }
            }
            if (audio_stream_index_ < 0) {
                avformat_close_input(&format_ctx_);
                return false;
            }

            AVCodecParameters* codecpar = format_ctx_->streams[audio_stream_index_]->codecpar;
            const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
            if (!codec) {
                avformat_close_input(&format_ctx_);
                return false;
            }

            codec_ctx_ = avcodec_alloc_context3(codec);
            if (!codec_ctx_ ||
                avcodec_parameters_to_context(codec_ctx_, codecpar) < 0 ||
                avcodec_open2(codec_ctx_, codec, nullptr) < 0) {
                if (codec_ctx_) avcodec_free_context(&codec_ctx_);
                avformat_close_input(&format_ctx_);
                return false;
            }

            AVChannelLayout out_layout = AV_CHANNEL_LAYOUT_STEREO;
            AVChannelLayout in_layout;
            if (codec_ctx_->ch_layout.nb_channels > 0) {
                av_channel_layout_copy(&in_layout, &codec_ctx_->ch_layout);
            }
            else {
                av_channel_layout_default(&in_layout, codecpar->ch_layout.nb_channels);
            }

            int ret = swr_alloc_set_opts2(&swr_ctx_, &out_layout, AV_SAMPLE_FMT_FLT,
                MASTER_SAMPLE_RATE, &in_layout,
                codec_ctx_->sample_fmt,
                codec_ctx_->sample_rate, 0, nullptr);
            av_channel_layout_uninit(&in_layout);

            if (ret < 0 || swr_init(swr_ctx_) < 0) {
                if (swr_ctx_) swr_free(&swr_ctx_);
                avcodec_free_context(&codec_ctx_);
                avformat_close_input(&format_ctx_);
                return false;
            }

            if (format_ctx_->duration != AV_NOPTS_VALUE) {
                duration_ = av_rescale(format_ctx_->duration, MASTER_SAMPLE_RATE, AV_TIME_BASE);
            }

            return true;
        }

        void close_file() {
            if (swr_ctx_) swr_free(&swr_ctx_);
            if (codec_ctx_) avcodec_free_context(&codec_ctx_);
            if (format_ctx_) avformat_close_input(&format_ctx_);
        }

        void start_decode_thread() {
            decode_running_.store(true, std::memory_order_release);
            decode_thread_ = std::thread([this]() { decode_loop(); });
        }

        void stop_decode_thread() {
            decode_running_.store(false, std::memory_order_release);
            if (decode_thread_.joinable()) {
                decode_thread_.join();
            }
        }

        void decode_loop() {
            AVPacket* packet = av_packet_alloc();
            AVFrame* frame = av_frame_alloc();
            std::vector<float> resample_buffer(8192 * MASTER_CHANNELS);

            while (decode_running_.load(std::memory_order_relaxed)) {
                if (seeking_.load(std::memory_order_acquire)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }

                if (ring_buffer_->available_write() < 4096) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    continue;
                }

                int ret = av_read_frame(format_ctx_, packet);
                if (ret < 0) {
                    if (looping_.load(std::memory_order_relaxed)) {
                        av_seek_frame(format_ctx_, audio_stream_index_, 0, AVSEEK_FLAG_BACKWARD);
                        avcodec_flush_buffers(codec_ctx_);
                        position_.store(0, std::memory_order_relaxed);
                        continue;
                    }
                    finished_.store(true, std::memory_order_release);
                    break;
                }

                if (packet->stream_index != audio_stream_index_) {
                    av_packet_unref(packet);
                    continue;
                }

                ret = avcodec_send_packet(codec_ctx_, packet);
                av_packet_unref(packet);
                if (ret < 0) continue;

                while (ret >= 0) {
                    ret = avcodec_receive_frame(codec_ctx_, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
                    if (ret < 0) break;

                    int out_samples = swr_get_out_samples(swr_ctx_, frame->nb_samples);
                    if (out_samples * MASTER_CHANNELS > (int)resample_buffer.size()) {
                        resample_buffer.resize(out_samples * MASTER_CHANNELS);
                    }

                    uint8_t* out_ptr = reinterpret_cast<uint8_t*>(resample_buffer.data());
                    int converted = swr_convert(swr_ctx_, &out_ptr, out_samples,
                        (const uint8_t**)frame->extended_data, frame->nb_samples);

                    if (converted > 0) {
                        ring_buffer_->write(resample_buffer.data(), converted * MASTER_CHANNELS);
                    }
                    av_frame_unref(frame);
                }
            }

            av_frame_free(&frame);
            av_packet_free(&packet);
        }

    private:
        std::string path_;

        AVFormatContext* format_ctx_ = nullptr;
        AVCodecContext* codec_ctx_ = nullptr;
        SwrContext* swr_ctx_ = nullptr;
        int audio_stream_index_ = -1;

        std::unique_ptr<RingBuffer<float>> ring_buffer_;

        std::atomic<bool> decode_running_{ false };
        std::atomic<bool> seeking_{ false };
        std::atomic<bool> finished_{ false };
        std::atomic<bool> looping_{ false };
        std::atomic<int64_t> position_{ 0 };
        int64_t duration_ = 0;

        std::thread decode_thread_;
    };

    // =============================================================================
    // SineGenerator - サイン波
    // =============================================================================
    class SineGenerator : public IAudioGenerator {
    public:
        SineGenerator(double frequency = 440.0, double duration_sec = 0.0)
            : frequency_(frequency)
            , duration_samples_(duration_sec > 0 ? static_cast<int64_t>(duration_sec * MASTER_SAMPLE_RATE) : -1) {
        }

        size_t generate(float* out, size_t frames) override {
            if (duration_samples_ > 0 && position_ >= duration_samples_) {
                std::memset(out, 0, frames * MASTER_CHANNELS * sizeof(float));
                return 0;
            }

            size_t to_generate = frames;
            if (duration_samples_ > 0) {
                int64_t remaining = duration_samples_ - position_;
                to_generate = std::min(frames, static_cast<size_t>(remaining));
            }

            const double inc = frequency_ / MASTER_SAMPLE_RATE;

            for (size_t i = 0; i < to_generate; ++i) {
                float v = static_cast<float>(std::sin(phase_ * 2.0 * std::numbers::pi)) * amplitude_;
                phase_ += inc;
                if (phase_ >= 1.0) phase_ -= 1.0;

                out[i * MASTER_CHANNELS + 0] = v;
                out[i * MASTER_CHANNELS + 1] = v;
            }

            for (size_t i = to_generate; i < frames; ++i) {
                out[i * MASTER_CHANNELS + 0] = 0.0f;
                out[i * MASTER_CHANNELS + 1] = 0.0f;
            }

            position_ += to_generate;
            return to_generate;
        }

        GeneratorState state() const override {
            return {
                .finished = (duration_samples_ > 0 && position_ >= duration_samples_),
                .position = position_,
                .duration = duration_samples_ > 0 ? duration_samples_ : 0
            };
        }

        void seek(int64_t position_samples) override { position_ = position_samples; }
        void set_frequency(double freq) { frequency_ = freq; }
        void set_amplitude(float amp) { amplitude_ = amp; }

    private:
        double frequency_;
        double phase_ = 0.0;
        float amplitude_ = 0.3f;
        int64_t position_ = 0;
        int64_t duration_samples_;
    };

    // =============================================================================
    // NoiseGenerator - ホワイトノイズ
    // =============================================================================
    class NoiseGenerator : public IAudioGenerator {
    public:
        explicit NoiseGenerator(double duration_sec = 0.0)
            : duration_samples_(duration_sec > 0 ? static_cast<int64_t>(duration_sec * MASTER_SAMPLE_RATE) : -1) {
        }

        size_t generate(float* out, size_t frames) override {
            if (duration_samples_ > 0 && position_ >= duration_samples_) {
                std::memset(out, 0, frames * MASTER_CHANNELS * sizeof(float));
                return 0;
            }

            size_t to_generate = frames;
            if (duration_samples_ > 0) {
                int64_t remaining = duration_samples_ - position_;
                to_generate = std::min(frames, static_cast<size_t>(remaining));
            }

            for (size_t i = 0; i < to_generate; ++i) {
                seed_ = seed_ * 1103515245 + 12345;
                float v = (static_cast<float>(seed_ & 0x7FFFFFFF) / 0x7FFFFFFF * 2.0f - 1.0f) * amplitude_;
                out[i * MASTER_CHANNELS + 0] = v;
                out[i * MASTER_CHANNELS + 1] = v;
            }

            for (size_t i = to_generate; i < frames; ++i) {
                out[i * MASTER_CHANNELS + 0] = 0.0f;
                out[i * MASTER_CHANNELS + 1] = 0.0f;
            }

            position_ += to_generate;
            return to_generate;
        }

        GeneratorState state() const override {
            return {
                .finished = (duration_samples_ > 0 && position_ >= duration_samples_),
                .position = position_,
                .duration = duration_samples_ > 0 ? duration_samples_ : 0
            };
        }

        void set_amplitude(float amp) { amplitude_ = amp; }

    private:
        uint32_t seed_ = 12345;
        float amplitude_ = 0.2f;
        int64_t position_ = 0;
        int64_t duration_samples_;
    };

    // =============================================================================
    // AudioSource（Generator + 再生状態）
    // =============================================================================
    struct AudioSource {
        SoundId id = 0;
        std::unique_ptr<IAudioGenerator> generator;

        std::atomic<bool> playing{ false };
        std::atomic<bool> remove_when_finished{ false };
        std::atomic<float> volume{ 1.0f };
        std::atomic<float> pan{ 0.0f };
    };

    // =============================================================================
    // オーディオエンジン
    // =============================================================================
    class AudioEngine {
    public:
        AudioEngine() {
            for (auto& slot : active_sources_) {
                slot.store(nullptr, std::memory_order_relaxed);
            }
        }

        ~AudioEngine() { shutdown(); }

        bool initialize() {
            if (initialized_) return true;

            PaError err = Pa_Initialize();
            if (err != paNoError) {
                std::cerr << "PortAudio init failed: " << Pa_GetErrorText(err) << std::endl;
                return false;
            }

            err = Pa_OpenDefaultStream(&stream_, 0, MASTER_CHANNELS, paFloat32,
                MASTER_SAMPLE_RATE, FRAMES_PER_BUFFER,
                pa_callback, this);
            if (err != paNoError) {
                Pa_Terminate();
                return false;
            }

            err = Pa_StartStream(stream_);
            if (err != paNoError) {
                Pa_CloseStream(stream_);
                Pa_Terminate();
                return false;
            }

            initialized_ = true;
            return true;
        }

        void shutdown() {
            if (!initialized_) return;

            {
                std::lock_guard<std::mutex> lock(sources_mutex_);
                for (auto& slot : active_sources_) {
                    slot.store(nullptr, std::memory_order_release);
                }
                sources_.clear();
            }

            if (stream_) {
                Pa_StopStream(stream_);
                Pa_CloseStream(stream_);
                stream_ = nullptr;
            }
            Pa_Terminate();
            initialized_ = false;
        }

        // =========================================================================
        // ファイル音源
        // =========================================================================
        SoundId load(const std::string& path) {
            auto gen = std::make_unique<FileGenerator>(path);
            if (!gen->is_valid()) {
                std::cerr << "Failed to open: " << path << std::endl;
                return 0;
            }
            return add_source(std::move(gen));
        }

        // =========================================================================
        // プログラマブル音源
        // =========================================================================
        SoundId create_sine(double frequency, double duration_sec = 0.0) {
            return add_source(std::make_unique<SineGenerator>(frequency, duration_sec));
        }

        SoundId create_noise(double duration_sec = 0.0) {
            return add_source(std::make_unique<NoiseGenerator>(duration_sec));
        }

        SoundId add_generator(std::unique_ptr<IAudioGenerator> generator) {
            return add_source(std::move(generator));
        }

        // =========================================================================
        // 制御
        // =========================================================================
        void play(SoundId id) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it == sources_.end()) return;

            auto* source = it->second.get();
            if (source->playing.load(std::memory_order_relaxed)) return;

            source->playing.store(true, std::memory_order_release);
            activate_source(source);
        }

        void pause(SoundId id) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                it->second->playing.store(false, std::memory_order_release);
            }
        }

        void stop(SoundId id) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it == sources_.end()) return;

            auto* source = it->second.get();
            source->playing.store(false, std::memory_order_release);
            deactivate_source(source);
            source->generator->reset();
        }

        void unload(SoundId id) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                deactivate_source(it->second.get());
                sources_.erase(it);
            }
        }

        void set_volume(SoundId id, float volume) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                it->second->volume.store(std::clamp(volume, 0.0f, 2.0f), std::memory_order_relaxed);
            }
        }

        void set_pan(SoundId id, float pan) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                it->second->pan.store(std::clamp(pan, -1.0f, 1.0f), std::memory_order_relaxed);
            }
        }

        void seek(SoundId id, double position_sec) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                int64_t samples = static_cast<int64_t>(position_sec * MASTER_SAMPLE_RATE);
                it->second->generator->seek(samples);
            }
        }

        void set_looping(SoundId id, bool loop) {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                if (auto* fg = dynamic_cast<FileGenerator*>(it->second->generator.get())) {
                    fg->set_looping(loop);
                }
            }
        }

        // =========================================================================
        // 状態取得
        // =========================================================================
        bool is_playing(SoundId id) const {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            return it != sources_.end() && it->second->playing.load(std::memory_order_relaxed);
        }

        double get_position_seconds(SoundId id) const {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                return static_cast<double>(it->second->generator->state().position) / MASTER_SAMPLE_RATE;
            }
            return 0.0;
        }

        double get_duration_seconds(SoundId id) const {
            std::lock_guard<std::mutex> lock(sources_mutex_);
            auto it = sources_.find(id);
            if (it != sources_.end()) {
                int64_t dur = it->second->generator->state().duration;
                return dur > 0 ? static_cast<double>(dur) / MASTER_SAMPLE_RATE : 0.0;
            }
            return 0.0;
        }

        int64_t get_master_position() const {
            return master_position_.load(std::memory_order_relaxed);
        }

        double get_master_time() const {
            return static_cast<double>(master_position_.load(std::memory_order_relaxed)) / MASTER_SAMPLE_RATE;
        }

    private:
        SoundId add_source(std::unique_ptr<IAudioGenerator> generator) {
            auto source = std::make_unique<AudioSource>();
            source->id = next_id_++;
            source->generator = std::move(generator);

            SoundId id = source->id;
            std::lock_guard<std::mutex> lock(sources_mutex_);
            sources_[id] = std::move(source);
            return id;
        }

        void activate_source(AudioSource* source) {
            for (auto& slot : active_sources_) {
                AudioSource* expected = nullptr;
                if (slot.compare_exchange_strong(expected, source, std::memory_order_release)) {
                    return;
                }
            }
            std::cerr << "Warning: No free slot for audio source" << std::endl;
        }

        void deactivate_source(AudioSource* source) {
            for (auto& slot : active_sources_) {
                AudioSource* expected = source;
                if (slot.compare_exchange_strong(expected, nullptr, std::memory_order_release)) {
                    return;
                }
            }
        }

        static int pa_callback(const void*, void* output, unsigned long frames,
            const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
            void* user_data) {
            auto* engine = static_cast<AudioEngine*>(user_data);
            engine->mix_to_output(static_cast<float*>(output), frames);
            engine->master_position_.fetch_add(frames, std::memory_order_relaxed);
            return paContinue;
        }

        void mix_to_output(float* output, unsigned long frames) {
            const size_t total_samples = frames * MASTER_CHANNELS;
            std::memset(output, 0, total_samples * sizeof(float));

            thread_local std::vector<float> temp_buffer(FRAMES_PER_BUFFER * MASTER_CHANNELS);
            if (temp_buffer.size() < total_samples) {
                temp_buffer.resize(total_samples);
            }

            // ロックフリーでアクティブリストを走査
            for (auto& slot : active_sources_) {
                AudioSource* source = slot.load(std::memory_order_acquire);
                if (!source) continue;
                if (!source->playing.load(std::memory_order_relaxed)) continue;

                size_t generated = source->generator->generate(temp_buffer.data(), frames);

                if (generated == 0) {
                    source->playing.store(false, std::memory_order_relaxed);
                    if (source->remove_when_finished.load(std::memory_order_relaxed)) {
                        slot.store(nullptr, std::memory_order_release);
                    }
                    continue;
                }

                const float vol = source->volume.load(std::memory_order_relaxed);
                const float pan = source->pan.load(std::memory_order_relaxed);
                const float vol_l = vol * std::sqrt(0.5f * (1.0f - pan));
                const float vol_r = vol * std::sqrt(0.5f * (1.0f + pan));

                for (size_t i = 0; i < generated; ++i) {
                    output[i * 2 + 0] += temp_buffer[i * 2 + 0] * vol_l;
                    output[i * 2 + 1] += temp_buffer[i * 2 + 1] * vol_r;
                }
            }

            for (size_t i = 0; i < total_samples; ++i) {
                output[i] = std::clamp(output[i], -1.0f, 1.0f);
            }
        }

    private:
        PaStream* stream_ = nullptr;
        bool initialized_ = false;
        std::atomic<int64_t> master_position_{ 0 };

        mutable std::mutex sources_mutex_;
        std::unordered_map<SoundId, std::unique_ptr<AudioSource>> sources_;
        SoundId next_id_ = 1;

        // ロックフリーアクティブリスト
        std::array<std::atomic<AudioSource*>, MAX_ACTIVE_SOURCES> active_sources_;
    };

} // namespace audio