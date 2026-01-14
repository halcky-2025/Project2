// nle_timeline.hpp
// Non-Linear Editing Timeline System
// 
// 設計:
//   Timeline (時間の支配者)
//   ├─ MasterClock
//   ├─ VideoTrack[] → VideoClip[]
//   ├─ AudioTrack[] → AudioClip[]
//   └─ OverlayTrack[] → OverlayClip[]
//
// 各Clipは独立したデコーダーとテクスチャを持つ

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <functional>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <atomic>

#include "platform_io.h"

#ifdef __ANDROID__
#include <SDL3/SDL.h>
#endif

// FFmpeg
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace nle {

    // =============================================================================
    // 時間単位
    // =============================================================================
    using TimeUs = int64_t;
    constexpr TimeUs SECOND_US = 1'000'000;

    inline double to_seconds(TimeUs us) { return static_cast<double>(us) / SECOND_US; }
    inline TimeUs from_seconds(double s) { return static_cast<TimeUs>(s * SECOND_US); }

    // =============================================================================
    // 時間範囲
    // =============================================================================
    struct TimeRange {
        TimeUs start = 0;
        TimeUs end = 0;

        TimeUs duration() const { return end - start; }
        bool contains(TimeUs t) const { return t >= start && t < end; }
        bool overlaps(const TimeRange& other) const {
            return start < other.end && end > other.start;
        }
    };

    // =============================================================================
    // Cross-platform media file access via FileEngine
    // FFmpeg needs real file paths, so we cache remote/asset files locally
    // =============================================================================
    inline std::string getMediaRealPath(const std::string& path) {
#ifdef __ANDROID__
        // Android: Extract asset to cache for FFmpeg access
        return PlatformIO::extractAssetToCache(path);
#else
        // Check if it's a URL
        std::string scheme = HopStarIO::parseAddressScheme(path);

        if (!scheme.empty()) {
            // Remote file: download and cache via FileEngine
            auto* engine = PlatformIO::getEngine();
            auto desc = engine->createDescriptor(path, HopStarIO::Location::Server,
                                                  HopStarIO::Access::Read, path);

            // Prefetch to cache
            if (!engine->prefetch(desc)) {
                return "";
            }

            // Get cache path
            std::string cachePath = engine->getInternalPath("media_cache/" + HopStarIO::hashPath(path));

            // Write cached data to local file for FFmpeg
            auto result = engine->read(desc);
            if (result.success) {
                auto writeDesc = engine->fromExternalPath(cachePath, HopStarIO::Access::Write);
                engine->write(writeDesc, result.data.data(), result.data.size());
                return cachePath;
            }
            return "";
        }

        // Local/Resource file: Read through FileEngine and cache for FFmpeg
        auto* engine = PlatformIO::getEngine();
        auto fileData = PlatformIO::readFile(path, HopStarIO::Location::Resource);
        if (fileData.empty()) {
            SDL_Log("getMediaRealPath: Failed to read resource via FileEngine: %s", path.c_str());
            return "";
        }

        // Cache to internal storage for FFmpeg access
        std::string cachePath = engine->getInternalPath("media_cache/" + HopStarIO::hashPath(path));
        if (PlatformIO::writeFile(cachePath, fileData.data(), fileData.size(), HopStarIO::Location::External)) {
            return cachePath;
        }

        SDL_Log("getMediaRealPath: Failed to cache file: %s", path.c_str());
        return "";
#endif
    }

    // =============================================================================
    // MasterClock - タイムライン全体の時間管理
    // =============================================================================
    class MasterClock {
    public:
        void start() {
            if (!running_) {
                start_time_ = std::chrono::steady_clock::now();
                start_position_ = position_;
                running_ = true;
            }
        }

        void pause() {
            if (running_) {
                position_ = now();
                running_ = false;
            }
        }

        void stop() {
            pause();
            position_ = 0;
        }

        void seek(TimeUs time) {
            position_ = time;
            if (running_) {
                start_time_ = std::chrono::steady_clock::now();
                start_position_ = position_;
            }
        }

        TimeUs now() const {
            if (!running_) return position_;

            auto elapsed = std::chrono::steady_clock::now() - start_time_;
            auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            return start_position_ + static_cast<TimeUs>(elapsed_us * speed_);
        }

        bool is_running() const { return running_; }

        void set_speed(double speed) { speed_ = speed; }
        double get_speed() const { return speed_; }

    private:
        std::chrono::steady_clock::time_point start_time_;
        TimeUs start_position_ = 0;
        TimeUs position_ = 0;
        double speed_ = 1.0;
        bool running_ = false;
    };

    // =============================================================================
    // VideoDecoder - 動画デコーダー（単一ファイル用）
    // =============================================================================
    class VideoDecoder {
    public:
        ~VideoDecoder() { close(); }

        bool open(const std::string& path) {
            close();

            // Use PlatformIO for cross-platform path resolution
            std::string realPath = getMediaRealPath(path);
            if (realPath.empty()) return false;

            int ret = avformat_open_input(&format_ctx_, realPath.c_str(), nullptr, nullptr);
            if (ret < 0) return false;

            if (avformat_find_stream_info(format_ctx_, nullptr) < 0) {
                close();
                return false;
            }

            // ビデオストリーム検索
            for (unsigned i = 0; i < format_ctx_->nb_streams; i++) {
                if (format_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                    video_stream_index_ = i;
                    break;
                }
            }

            if (video_stream_index_ < 0) {
                close();
                return false;
            }

            AVStream* stream = format_ctx_->streams[video_stream_index_];
            const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
            if (!codec) {
                close();
                return false;
            }

            codec_ctx_ = avcodec_alloc_context3(codec);
            avcodec_parameters_to_context(codec_ctx_, stream->codecpar);

            ret = avcodec_open2(codec_ctx_, codec, nullptr);
            if (ret < 0) {
                close();
                return false;
            }

            // スケーラー（RGBA出力）
            sws_ctx_ = sws_getContext(
                codec_ctx_->width, codec_ctx_->height, codec_ctx_->pix_fmt,
                codec_ctx_->width, codec_ctx_->height, AV_PIX_FMT_RGBA,
                SWS_BILINEAR, nullptr, nullptr, nullptr);

            if (!sws_ctx_) {
                close();
                return false;
            }

            width_ = codec_ctx_->width;
            height_ = codec_ctx_->height;
            time_base_ = stream->time_base;

            // フレームレート
            if (stream->avg_frame_rate.num > 0) {
                frame_rate_ = av_q2d(stream->avg_frame_rate);
            }
            else {
                frame_rate_ = 30.0;
            }
            frame_duration_us_ = static_cast<TimeUs>(SECOND_US / frame_rate_);

            // 総時間
            if (format_ctx_->duration != AV_NOPTS_VALUE) {
                duration_us_ = av_rescale_q(format_ctx_->duration, AV_TIME_BASE_Q, { 1, 1000000 });
            }

            frame_ = av_frame_alloc();
            packet_ = av_packet_alloc();
            frame_buffer_.resize(width_ * height_ * 4);

            is_open_ = true;
            return true;
        }

        void close() {
            is_open_ = false;
            if (sws_ctx_) { sws_freeContext(sws_ctx_); sws_ctx_ = nullptr; }
            if (frame_) { av_frame_free(&frame_); }
            if (packet_) { av_packet_free(&packet_); }
            if (codec_ctx_) { avcodec_free_context(&codec_ctx_); }
            if (format_ctx_) { avformat_close_input(&format_ctx_); }
            frame_buffer_.clear();
        }

        bool seek(TimeUs time_us) {
            if (!is_open_) return false;

            int64_t ts = av_rescale_q(time_us, { 1, 1000000 }, time_base_);
            if (av_seek_frame(format_ctx_, video_stream_index_, ts, AVSEEK_FLAG_BACKWARD) < 0) {
                return false;
            }
            avcodec_flush_buffers(codec_ctx_);
            current_pts_ = time_us;
            eof_ = false;
            return true;
        }

        // 指定時刻のフレームをデコード
        bool decode_frame_at(TimeUs target_time) {
            if (!is_open_ || eof_) return false;

            // 既に目標時刻のフレームを持っている場合
            if (has_frame_ && current_pts_ <= target_time &&
                current_pts_ + frame_duration_us_ > target_time) {
                return true;
            }

            // 目標より前にいる場合、またはシーク必要な場合
            if (current_pts_ > target_time + frame_duration_us_ * 10 ||
                current_pts_ + frame_duration_us_ * 30 < target_time) {
                seek(target_time - frame_duration_us_ * 2);
            }

            // 目標時刻までデコード
            while (!eof_) {
                if (!decode_next_frame()) {
                    if (eof_) return has_frame_;
                    continue;
                }

                if (current_pts_ + frame_duration_us_ > target_time) {
                    return true;
                }
            }

            return has_frame_;
        }

        bool decode_next_frame() {
            if (!is_open_) return false;

            while (av_read_frame(format_ctx_, packet_) >= 0) {
                if (packet_->stream_index != video_stream_index_) {
                    av_packet_unref(packet_);
                    continue;
                }

                int ret = avcodec_send_packet(codec_ctx_, packet_);
                av_packet_unref(packet_);

                if (ret < 0) continue;

                ret = avcodec_receive_frame(codec_ctx_, frame_);
                if (ret == AVERROR(EAGAIN)) continue;
                if (ret < 0) continue;

                // PTS計算
                int64_t pts = frame_->best_effort_timestamp;
                if (pts != AV_NOPTS_VALUE) {
                    current_pts_ = av_rescale_q(pts, time_base_, { 1, 1000000 });
                }
                else {
                    current_pts_ += frame_duration_us_;
                }

                // RGBA変換
                uint8_t* dst_data[1] = { frame_buffer_.data() };
                int dst_linesize[1] = { width_ * 4 };
                sws_scale(sws_ctx_, frame_->data, frame_->linesize, 0, height_,
                    dst_data, dst_linesize);

                av_frame_unref(frame_);
                has_frame_ = true;
                return true;
            }

            eof_ = true;
            return false;
        }

        const uint8_t* get_frame_data() const { return frame_buffer_.data(); }
        int get_width() const { return width_; }
        int get_height() const { return height_; }
        TimeUs get_pts() const { return current_pts_; }
        TimeUs get_duration() const { return duration_us_; }
        TimeUs get_frame_duration() const { return frame_duration_us_; }
        double get_frame_rate() const { return frame_rate_; }
        bool is_open() const { return is_open_; }
        bool is_eof() const { return eof_; }
        bool has_valid_frame() const { return has_frame_; }

    private:
        AVFormatContext* format_ctx_ = nullptr;
        AVCodecContext* codec_ctx_ = nullptr;
        SwsContext* sws_ctx_ = nullptr;
        AVFrame* frame_ = nullptr;
        AVPacket* packet_ = nullptr;

        int video_stream_index_ = -1;
        AVRational time_base_{ 1, 1000000 };

        int width_ = 0;
        int height_ = 0;
        double frame_rate_ = 30.0;
        TimeUs frame_duration_us_ = 33333;
        TimeUs duration_us_ = 0;

        std::vector<uint8_t> frame_buffer_;
        TimeUs current_pts_ = 0;

        bool is_open_ = false;
        bool eof_ = false;
        bool has_frame_ = false;
    };

    // =============================================================================
    // ClipBase - 全クリップの基底
    // =============================================================================
    struct ClipBase {
        TimeRange timeline_range;  // タイムライン上の配置
        TimeRange source_range;    // ソース内の使用範囲

        // タイムライン時刻からソース時刻へ変換
        TimeUs timeline_to_source(TimeUs timeline_time) const {
            TimeUs offset = timeline_time - timeline_range.start;
            return source_range.start + offset;
        }

        // このクリップがタイムライン時刻でアクティブか
        bool is_active_at(TimeUs timeline_time) const {
            return timeline_range.contains(timeline_time);
        }
    };

    // =============================================================================
    // VideoClip - 動画クリップ
    // =============================================================================
    class VideoClip : public ClipBase {
    public:
        bool open(const std::string& path) {
            path_ = path;
            if (!decoder_.open(path)) {
                return false;
            }

            // デフォルトではソース全体を使用
            source_range.start = 0;
            source_range.end = decoder_.get_duration();

            return true;
        }

        void close() {
            decoder_.close();
        }

        // タイムライン時刻でフレームを更新
        bool update(TimeUs timeline_time) {
            if (!is_active_at(timeline_time)) {
                return false;
            }

            TimeUs source_time = timeline_to_source(timeline_time);

            // ソース範囲を超えたらクリップ
            source_time = std::clamp(source_time, source_range.start, source_range.end);

            return decoder_.decode_frame_at(source_time);
        }

        const uint8_t* get_frame_data() const { return decoder_.get_frame_data(); }
        int get_width() const { return decoder_.get_width(); }
        int get_height() const { return decoder_.get_height(); }
        bool has_valid_frame() const { return decoder_.has_valid_frame(); }

        const std::string& get_path() const { return path_; }
        VideoDecoder& decoder() { return decoder_; }

        // ImageMaster用
        uint64_t image_id = 0;
        bool frame_dirty = false;

    private:
        std::string path_;
        VideoDecoder decoder_;
    };

    // =============================================================================
    // AudioDecoder - 音声デコーダー
    // =============================================================================
    class AudioDecoder {
    public:
        static constexpr int SAMPLE_RATE = 48000;
        static constexpr int CHANNELS = 2;

        ~AudioDecoder() { close(); }

        bool open(const std::string& path) {
            close();

            // Use PlatformIO/FileEngine for cross-platform path resolution
            std::string realPath = getMediaRealPath(path);
            if (realPath.empty()) {
                return false;
            }
            if (avformat_open_input(&format_ctx_, realPath.c_str(), nullptr, nullptr) < 0) {
                return false;
            }

            if (avformat_find_stream_info(format_ctx_, nullptr) < 0) {
                close();
                return false;
            }

            for (unsigned i = 0; i < format_ctx_->nb_streams; i++) {
                if (format_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                    audio_stream_index_ = i;
                    break;
                }
            }

            if (audio_stream_index_ < 0) {
                close();
                return false;
            }

            AVStream* stream = format_ctx_->streams[audio_stream_index_];
            const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
            if (!codec) {
                close();
                return false;
            }

            codec_ctx_ = avcodec_alloc_context3(codec);
            avcodec_parameters_to_context(codec_ctx_, stream->codecpar);

            if (avcodec_open2(codec_ctx_, codec, nullptr) < 0) {
                close();
                return false;
            }

            // リサンプラー設定
            AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
            AVChannelLayout in_ch_layout;
            av_channel_layout_copy(&in_ch_layout, &codec_ctx_->ch_layout);

            swr_alloc_set_opts2(&swr_ctx_,
                &out_ch_layout, AV_SAMPLE_FMT_FLT, SAMPLE_RATE,
                &in_ch_layout, codec_ctx_->sample_fmt, codec_ctx_->sample_rate,
                0, nullptr);

            if (!swr_ctx_ || swr_init(swr_ctx_) < 0) {
                close();
                return false;
            }

            time_base_ = stream->time_base;

            if (format_ctx_->duration != AV_NOPTS_VALUE) {
                duration_us_ = av_rescale_q(format_ctx_->duration, AV_TIME_BASE_Q, { 1, 1000000 });
            }

            frame_ = av_frame_alloc();
            packet_ = av_packet_alloc();
            is_open_ = true;
            return true;
        }

        void close() {
            is_open_ = false;
            if (swr_ctx_) { swr_free(&swr_ctx_); }
            if (frame_) { av_frame_free(&frame_); }
            if (packet_) { av_packet_free(&packet_); }
            if (codec_ctx_) { avcodec_free_context(&codec_ctx_); }
            if (format_ctx_) { avformat_close_input(&format_ctx_); }
            pcm_buffer_.clear();
        }

        bool seek(TimeUs time_us) {
            if (!is_open_) return false;

            int64_t ts = av_rescale_q(time_us, { 1, 1000000 }, time_base_);
            if (av_seek_frame(format_ctx_, audio_stream_index_, ts, AVSEEK_FLAG_BACKWARD) < 0) {
                return false;
            }
            avcodec_flush_buffers(codec_ctx_);
            current_pts_ = time_us;
            eof_ = false;
            return true;
        }

        // 全体をデコードしてPCMバッファに格納
        bool decode_all() {
            if (!is_open_) return false;

            pcm_buffer_.clear();
            seek(0);

            while (av_read_frame(format_ctx_, packet_) >= 0) {
                if (packet_->stream_index != audio_stream_index_) {
                    av_packet_unref(packet_);
                    continue;
                }

                int ret = avcodec_send_packet(codec_ctx_, packet_);
                av_packet_unref(packet_);
                if (ret < 0) continue;

                while (avcodec_receive_frame(codec_ctx_, frame_) >= 0) {
                    int out_samples = av_rescale_rnd(
                        swr_get_delay(swr_ctx_, codec_ctx_->sample_rate) + frame_->nb_samples,
                        SAMPLE_RATE, codec_ctx_->sample_rate, AV_ROUND_UP);

                    size_t offset = pcm_buffer_.size();
                    pcm_buffer_.resize(offset + out_samples * CHANNELS);

                    uint8_t* out_data[1] = { reinterpret_cast<uint8_t*>(pcm_buffer_.data() + offset) };
                    int converted = swr_convert(swr_ctx_,
                        out_data, out_samples,
                        (const uint8_t**)frame_->data, frame_->nb_samples);

                    if (converted > 0) {
                        pcm_buffer_.resize(offset + converted * CHANNELS);
                    }
                    else {
                        pcm_buffer_.resize(offset);
                    }

                    av_frame_unref(frame_);
                }
            }

            total_samples_ = pcm_buffer_.size() / CHANNELS;
            return !pcm_buffer_.empty();
        }

        const std::vector<float>& get_pcm_buffer() const { return pcm_buffer_; }
        size_t get_total_samples() const { return total_samples_; }
        TimeUs get_duration() const { return duration_us_; }
        bool is_open() const { return is_open_; }

    private:
        AVFormatContext* format_ctx_ = nullptr;
        AVCodecContext* codec_ctx_ = nullptr;
        SwrContext* swr_ctx_ = nullptr;
        AVFrame* frame_ = nullptr;
        AVPacket* packet_ = nullptr;

        int audio_stream_index_ = -1;
        AVRational time_base_{ 1, 1000000 };
        TimeUs duration_us_ = 0;
        TimeUs current_pts_ = 0;

        std::vector<float> pcm_buffer_;
        size_t total_samples_ = 0;

        bool is_open_ = false;
        bool eof_ = false;
    };

    // =============================================================================
    // AudioClip - 音声クリップ
    // =============================================================================
    class AudioClip : public ClipBase {
    public:
        bool open(const std::string& path) {
            path_ = path;
            if (!decoder_.open(path)) {
                return false;
            }

            // 全体をデコード（小〜中規模ファイル向け）
            if (!decoder_.decode_all()) {
                return false;
            }

            source_range.start = 0;
            source_range.end = decoder_.get_duration();

            return true;
        }

        void close() {
            decoder_.close();
        }

        // 指定時刻のサンプルを取得
        // output: インターリーブされたステレオサンプル
        // returns: 書き込んだサンプル数
        size_t get_samples(TimeUs timeline_time, float* output, size_t num_samples) const {
            if (!is_active_at(timeline_time)) {
                return 0;
            }

            TimeUs source_time = timeline_to_source(timeline_time);
            source_time = std::clamp(source_time, source_range.start, source_range.end);

            // サンプル位置を計算
            size_t sample_pos = static_cast<size_t>(
                (double)source_time / SECOND_US * AudioDecoder::SAMPLE_RATE);

            const auto& pcm = decoder_.get_pcm_buffer();
            size_t total = decoder_.get_total_samples();

            if (sample_pos >= total) {
                return 0;
            }

            size_t available = total - sample_pos;
            size_t to_copy = std::min(num_samples, available);

            // インターリーブ形式でコピー
            std::memcpy(output, pcm.data() + sample_pos * 2, to_copy * 2 * sizeof(float));

            return to_copy;
        }

        const std::string& get_path() const { return path_; }
        AudioDecoder& decoder() { return decoder_; }

        float volume = 1.0f;
        float pan = 0.0f;  // -1.0 (L) to 1.0 (R)

    private:
        std::string path_;
        AudioDecoder decoder_;
    };

    // =============================================================================
    // Track - クリップのコンテナ
    // =============================================================================
    template<typename ClipType>
    class Track {
    public:
        using ClipPtr = std::shared_ptr<ClipType>;

        ClipPtr add_clip() {
            auto clip = std::make_shared<ClipType>();
            clips_.push_back(clip);
            return clip;
        }

        void remove_clip(ClipPtr clip) {
            clips_.erase(
                std::remove(clips_.begin(), clips_.end(), clip),
                clips_.end());
        }

        // 指定時刻でアクティブなクリップを取得
        std::vector<ClipPtr> get_active_clips(TimeUs timeline_time) const {
            std::vector<ClipPtr> active;
            for (const auto& clip : clips_) {
                if (clip->is_active_at(timeline_time)) {
                    active.push_back(clip);
                }
            }
            return active;
        }

        const std::vector<ClipPtr>& clips() const { return clips_; }
        std::vector<ClipPtr>& clips() { return clips_; }

        bool muted = false;
        float opacity = 1.0f;  // VideoTrack用
        float volume = 1.0f;   // AudioTrack用

    private:
        std::vector<ClipPtr> clips_;
    };

    using VideoTrack = Track<VideoClip>;
    using AudioTrack = Track<AudioClip>;

    // =============================================================================
    // Timeline - 全体を統括
    // =============================================================================
    class Timeline {
    public:
        // トラック管理
        std::shared_ptr<VideoTrack> add_video_track() {
            auto track = std::make_shared<VideoTrack>();
            video_tracks_.push_back(track);
            return track;
        }

        std::shared_ptr<AudioTrack> add_audio_track() {
            auto track = std::make_shared<AudioTrack>();
            audio_tracks_.push_back(track);
            return track;
        }

        // 再生制御
        void play() { clock_.start(); }
        void pause() { clock_.pause(); }
        void stop() { clock_.stop(); notify_seek(0); }
        void seek(TimeUs time) { clock_.seek(time); notify_seek(time); }

        bool is_playing() const { return clock_.is_running(); }
        TimeUs now() const { return clock_.now(); }

        // 全クリップを更新
        void update() {
            TimeUs current_time = clock_.now();

            for (auto& track : video_tracks_) {
                if (track->muted) continue;

                for (auto& clip : track->clips()) {
                    if (clip->is_active_at(current_time)) {
                        if (clip->update(current_time)) {
                            clip->frame_dirty = true;
                        }
                    }
                }
            }
        }

        // オーディオミキシング（指定時刻から連続的にサンプルを取得）
        void mix_audio_at(float* output, size_t num_samples, TimeUs audio_time) {
            // 出力バッファをゼロクリア
            std::memset(output, 0, num_samples * 2 * sizeof(float));

            // 一時バッファ
            thread_local std::vector<float> temp_buffer;
            temp_buffer.resize(num_samples * 2);

            for (auto& track : audio_tracks_) {
                if (track->muted) continue;

                for (auto& clip : track->clips()) {
                    if (!clip->is_active_at(audio_time)) continue;

                    size_t got = clip->get_samples(audio_time, temp_buffer.data(), num_samples);
                    if (got == 0) continue;

                    // ミックス（ボリューム・パン適用）
                    float vol_l = clip->volume * track->volume * (1.0f - std::max(0.0f, clip->pan));
                    float vol_r = clip->volume * track->volume * (1.0f + std::min(0.0f, clip->pan));

                    for (size_t i = 0; i < got; i++) {
                        output[i * 2 + 0] += temp_buffer[i * 2 + 0] * vol_l;
                        output[i * 2 + 1] += temp_buffer[i * 2 + 1] * vol_r;
                    }
                }
            }

            // マスターボリューム適用 & クリッピング
            for (size_t i = 0; i < num_samples * 2; i++) {
                output[i] *= master_volume_;
                output[i] = std::clamp(output[i], -1.0f, 1.0f);
            }
        }

        // シーク通知用コールバック登録
        using SeekCallback = std::function<void(TimeUs)>;
        void set_seek_callback(SeekCallback callback) {
            seek_callback_ = callback;
        }

        // 現在アクティブな全VideoClipを取得
        std::vector<std::shared_ptr<VideoClip>> get_active_video_clips() const {
            std::vector<std::shared_ptr<VideoClip>> result;
            TimeUs current_time = clock_.now();

            for (const auto& track : video_tracks_) {
                if (track->muted) continue;

                auto active = track->get_active_clips(current_time);
                result.insert(result.end(), active.begin(), active.end());
            }

            return result;
        }

        // 現在アクティブな全AudioClipを取得
        std::vector<std::shared_ptr<AudioClip>> get_active_audio_clips() const {
            std::vector<std::shared_ptr<AudioClip>> result;
            TimeUs current_time = clock_.now();

            for (const auto& track : audio_tracks_) {
                if (track->muted) continue;

                auto active = track->get_active_clips(current_time);
                result.insert(result.end(), active.begin(), active.end());
            }

            return result;
        }

        // タイムライン全体の長さを計算
        TimeUs get_duration() const {
            TimeUs max_end = 0;
            for (const auto& track : video_tracks_) {
                for (const auto& clip : track->clips()) {
                    max_end = std::max(max_end, clip->timeline_range.end);
                }
            }
            for (const auto& track : audio_tracks_) {
                for (const auto& clip : track->clips()) {
                    max_end = std::max(max_end, clip->timeline_range.end);
                }
            }
            return max_end;
        }

        MasterClock& clock() { return clock_; }
        const MasterClock& clock() const { return clock_; }

        std::vector<std::shared_ptr<VideoTrack>>& video_tracks() { return video_tracks_; }
        std::vector<std::shared_ptr<AudioTrack>>& audio_tracks() { return audio_tracks_; }

        void set_master_volume(float vol) { master_volume_ = std::clamp(vol, 0.0f, 2.0f); }
        float get_master_volume() const { return master_volume_; }

    private:
        void notify_seek(TimeUs time) {
            if (seek_callback_) {
                seek_callback_(time);
            }
        }

        MasterClock clock_;
        std::vector<std::shared_ptr<VideoTrack>> video_tracks_;
        std::vector<std::shared_ptr<AudioTrack>> audio_tracks_;
        float master_volume_ = 1.0f;
        SeekCallback seek_callback_;
    };

    // =============================================================================
    // TimelineRenderer - ImageMasterと統合してレンダリング
    // =============================================================================
    template<typename ImageMasterT>
    class TimelineRenderer {
    public:
        TimelineRenderer(Timeline& timeline, ImageMasterT& image_master)
            : timeline_(timeline), image_master_(image_master) {
        }

        // 各クリップにテクスチャを割り当て
        void prepare_clip(std::shared_ptr<VideoClip> clip) {
            if (clip->image_id == 0) {
                clip->image_id = image_master_.createEmptyStandaloneTexture(
                    clip->get_width(), clip->get_height(), false);
            }
        }

        // フレーム更新
        // フレーム更新（beginFrame()は外部で呼ぶこと）
        void update() {
            timeline_.update();

            // アクティブなクリップのテクスチャを更新
            auto active_clips = timeline_.get_active_video_clips();
            for (auto& clip : active_clips) {
                if (clip->image_id == 0) {
                    prepare_clip(clip);
                }

                if (clip->frame_dirty && clip->has_valid_frame()) {
                    image_master_.updateStandaloneTexture(
                        clip->image_id,
                        clip->get_frame_data(),
                        0, 0,
                        clip->get_width(), clip->get_height(),
                        clip->get_width() * 4);
                    image_master_.touch(clip->image_id);
                    clip->frame_dirty = false;
                }
            }
        }

        // 描画用にアクティブクリップとテクスチャ情報を取得
        struct RenderItem {
            std::shared_ptr<VideoClip> clip;
            uint64_t image_id;
            float opacity;
        };

        std::vector<RenderItem> get_render_items() const {
            std::vector<RenderItem> items;

            for (const auto& track : timeline_.video_tracks()) {
                if (track->muted) continue;

                auto active = track->get_active_clips(timeline_.now());
                for (const auto& clip : active) {
                    if (clip->image_id != 0 && clip->has_valid_frame()) {
                        items.push_back({ clip, clip->image_id, track->opacity });
                    }
                }
            }

            return items;
        }

    private:
        Timeline& timeline_;
        ImageMasterT& image_master_;
    };

    // =============================================================================
    // AudioOutput - PortAudioによる音声出力
    // =============================================================================
#include <portaudio.h>

    class AudioOutput {
    public:
        static constexpr int SAMPLE_RATE = 48000;
        static constexpr int CHANNELS = 2;
        static constexpr int FRAMES_PER_BUFFER = 256;

        AudioOutput(Timeline& timeline) : timeline_(timeline) {
            // シークコールバックを登録
            timeline_.set_seek_callback([this](TimeUs time) {
                seek_to(time);
                });
        }

        ~AudioOutput() { shutdown(); }

        bool initialize() {
            PaError err = Pa_Initialize();
            if (err != paNoError) {
                return false;
            }
            pa_initialized_ = true;

            err = Pa_OpenDefaultStream(
                &stream_,
                0,              // 入力チャンネル
                CHANNELS,       // 出力チャンネル
                paFloat32,      // サンプルフォーマット
                SAMPLE_RATE,
                FRAMES_PER_BUFFER,
                audio_callback,
                this);

            if (err != paNoError) {
                return false;
            }

            err = Pa_StartStream(stream_);
            if (err != paNoError) {
                Pa_CloseStream(stream_);
                stream_ = nullptr;
                return false;
            }

            return true;
        }

        void shutdown() {
            if (stream_) {
                Pa_StopStream(stream_);
                Pa_CloseStream(stream_);
                stream_ = nullptr;
            }
            if (pa_initialized_) {
                Pa_Terminate();
                pa_initialized_ = false;
            }
        }

        // 指定時刻にシーク（サンプル位置をリセット）
        void seek_to(TimeUs time) {
            sample_position_.store(time_to_samples(time));
        }

    private:
        // 時刻からサンプル位置に変換
        static size_t time_to_samples(TimeUs time) {
            return static_cast<size_t>((time * SAMPLE_RATE) / SECOND_US);
        }

        // サンプル位置から時刻に変換
        static TimeUs samples_to_time(size_t samples) {
            return static_cast<TimeUs>((samples * SECOND_US) / SAMPLE_RATE);
        }

        static int audio_callback(
            const void* input,
            void* output,
            unsigned long frame_count,
            const PaStreamCallbackTimeInfo* time_info,
            PaStreamCallbackFlags status_flags,
            void* user_data)
        {
            auto* self = static_cast<AudioOutput*>(user_data);
            float* out = static_cast<float*>(output);

            bool is_playing = self->timeline_.is_playing();
            bool was_playing = self->was_playing_;
            self->was_playing_ = is_playing;

            if (is_playing) {
                // 再生開始時にサンプル位置を同期
                if (!was_playing) {
                    self->sample_position_.store(time_to_samples(self->timeline_.now()));
                }

                // 現在のサンプル位置から時刻を計算
                size_t current_samples = self->sample_position_.load();
                TimeUs audio_time = samples_to_time(current_samples);

                // ミキシング
                self->timeline_.mix_audio_at(out, frame_count, audio_time);

                // サンプル位置を進める
                self->sample_position_.fetch_add(frame_count);
            }
            else {
                std::memset(out, 0, frame_count * CHANNELS * sizeof(float));
            }

            return paContinue;
        }

        Timeline& timeline_;
        PaStream* stream_ = nullptr;
        bool pa_initialized_ = false;

        std::atomic<size_t> sample_position_{ 0 };  // 現在のサンプル位置
        bool was_playing_ = false;                 // 前回の再生状態
    };

} // namespace nle