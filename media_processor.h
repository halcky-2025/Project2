// =============================================================================
// HopStar Server - Media Processor
// FFmpeg encoding/decoding + PyTorch inference pipeline
// =============================================================================
#pragma once

#include "server_common.h"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

#ifdef HOPSTAR_ENABLE_FFMPEG
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
}
#endif

#ifdef HOPSTAR_ENABLE_TORCH
#include <torch/torch.h>
#endif

namespace HopStarServer {

// ============================================================================
// Video Encoder/Decoder
// ============================================================================
class VideoCodec {
private:
#ifdef HOPSTAR_ENABLE_FFMPEG
    AVCodecContext* codec_ctx = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* packet = nullptr;
    SwsContext* sws_ctx = nullptr;
#endif

    bool is_encoder;
    int width = 0;
    int height = 0;

public:
    VideoCodec(bool encoder = true) : is_encoder(encoder) {}

    bool initialize(int w, int h, const std::string& codec_name = "libx264") {
#ifdef HOPSTAR_ENABLE_FFMPEG
        width = w;
        height = h;

        const AVCodec* codec = is_encoder ?
            avcodec_find_encoder_by_name(codec_name.c_str()) :
            avcodec_find_decoder_by_name(codec_name.c_str());

        if (!codec) {
            LOG_ERROR("Codec not found: " + codec_name);
            return false;
        }

        codec_ctx = avcodec_alloc_context3(codec);
        if (!codec_ctx) {
            LOG_ERROR("Failed to allocate codec context");
            return false;
        }

        if (is_encoder) {
            codec_ctx->width = width;
            codec_ctx->height = height;
            codec_ctx->time_base = {1, 30};  // 30 FPS
            codec_ctx->framerate = {30, 1};
            codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
            codec_ctx->bit_rate = 2000000;  // 2 Mbps
        }

        if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
            LOG_ERROR("Failed to open codec");
            return false;
        }

        frame = av_frame_alloc();
        packet = av_packet_alloc();

        LOG_INFO("Video codec initialized: " + codec_name);
        return true;
#else
        LOG_ERROR("FFmpeg support not compiled in");
        return false;
#endif
    }

    std::vector<uint8_t> encode(const uint8_t* rgb_data, int64_t pts) {
#ifdef HOPSTAR_ENABLE_FFMPEG
        if (!is_encoder || !codec_ctx) return {};

        // Convert RGB to YUV420P
        if (!sws_ctx) {
            sws_ctx = sws_getContext(
                width, height, AV_PIX_FMT_RGB24,
                width, height, AV_PIX_FMT_YUV420P,
                SWS_BILINEAR, nullptr, nullptr, nullptr
            );
        }

        frame->format = codec_ctx->pix_fmt;
        frame->width = width;
        frame->height = height;
        frame->pts = pts;

        av_frame_get_buffer(frame, 0);

        const uint8_t* src_data[1] = {rgb_data};
        int src_linesize[1] = {width * 3};

        sws_scale(sws_ctx, src_data, src_linesize, 0, height,
                  frame->data, frame->linesize);

        // Encode
        int ret = avcodec_send_frame(codec_ctx, frame);
        if (ret < 0) {
            LOG_ERROR("Failed to send frame to encoder");
            return {};
        }

        std::vector<uint8_t> encoded_data;
        while (ret >= 0) {
            ret = avcodec_receive_packet(codec_ctx, packet);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            }
            if (ret < 0) {
                LOG_ERROR("Failed to receive packet from encoder");
                break;
            }

            encoded_data.insert(encoded_data.end(),
                                packet->data,
                                packet->data + packet->size);
            av_packet_unref(packet);
        }

        av_frame_unref(frame);
        return encoded_data;
#else
        return {};
#endif
    }

    MediaFrame decode(const uint8_t* data, size_t size) {
#ifdef HOPSTAR_ENABLE_FFMPEG
        MediaFrame result;
        result.type = MediaType::VIDEO;

        if (is_encoder || !codec_ctx) return result;

        packet->data = const_cast<uint8_t*>(data);
        packet->size = size;

        int ret = avcodec_send_packet(codec_ctx, packet);
        if (ret < 0) {
            LOG_ERROR("Failed to send packet to decoder");
            return result;
        }

        ret = avcodec_receive_frame(codec_ctx, frame);
        if (ret == 0) {
            result.width = frame->width;
            result.height = frame->height;
            result.timestamp_us = frame->pts;

            // Convert to RGB24 for processing
            // TODO: Copy frame data to result.data
        }

        return result;
#else
        return {};
#endif
    }

    ~VideoCodec() {
#ifdef HOPSTAR_ENABLE_FFMPEG
        if (sws_ctx) sws_freeContext(sws_ctx);
        if (frame) av_frame_free(&frame);
        if (packet) av_packet_free(&packet);
        if (codec_ctx) avcodec_free_context(&codec_ctx);
#endif
    }
};

// ============================================================================
// PyTorch Inference Engine
// ============================================================================
class InferenceEngine {
private:
#ifdef HOPSTAR_ENABLE_TORCH
    torch::jit::script::Module model;
#endif
    bool model_loaded = false;
    std::string model_path;

public:
    bool loadModel(const std::string& path) {
#ifdef HOPSTAR_ENABLE_TORCH
        try {
            model = torch::jit::load(path);
            model.eval();
            model_loaded = true;
            model_path = path;
            LOG_INFO("Model loaded: " + path);
            return true;
        } catch (const c10::Error& e) {
            LOG_ERROR("Failed to load model: " + std::string(e.what()));
            return false;
        }
#else
        LOG_ERROR("PyTorch support not compiled in");
        return false;
#endif
    }

    std::vector<float> infer(const MediaFrame& frame) {
#ifdef HOPSTAR_ENABLE_TORCH
        if (!model_loaded) {
            LOG_ERROR("Model not loaded");
            return {};
        }

        try {
            // Convert MediaFrame to torch::Tensor
            // Assuming RGB24 format
            torch::Tensor input = torch::from_blob(
                const_cast<uint8_t*>(frame.data.data()),
                {1, frame.height, frame.width, 3},
                torch::kUInt8
            );

            // Normalize and permute to NCHW
            input = input.permute({0, 3, 1, 2}).to(torch::kFloat32).div(255.0);

            // Run inference
            auto output = model.forward({input}).toTensor();

            // Convert to vector
            std::vector<float> result(output.data_ptr<float>(),
                                      output.data_ptr<float>() + output.numel());

            return result;
        } catch (const c10::Error& e) {
            LOG_ERROR("Inference failed: " + std::string(e.what()));
            return {};
        }
#else
        return {};
#endif
    }

    bool isLoaded() const {
        return model_loaded;
    }
};

// ============================================================================
// Media Processing Pipeline
// ============================================================================
class MediaProcessor {
private:
    std::queue<MediaFrame> frame_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::thread processor_thread;
    bool running = false;

    VideoCodec encoder{true};
    VideoCodec decoder{false};
    InferenceEngine inference;

    // Recording
    bool recording = false;
    std::string recording_path;
#ifdef HOPSTAR_ENABLE_FFMPEG
    AVFormatContext* rec_format_ctx = nullptr;
#endif

    // Callbacks
    OnMediaFrameCallback on_processed_frame;

    void processorLoop() {
        LOG_INFO("Media processor started");

        while (running) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [this] { return !frame_queue.empty() || !running; });

            if (!running && frame_queue.empty()) break;

            if (!frame_queue.empty()) {
                MediaFrame frame = std::move(frame_queue.front());
                frame_queue.pop();
                lock.unlock();

                processFrame(frame);
            }
        }

        LOG_INFO("Media processor stopped");
    }

    void processFrame(const MediaFrame& frame) {
        // Run inference if model loaded
        if (inference.isLoaded() && frame.type == MediaType::VIDEO) {
            auto results = inference.infer(frame);
            LOG_DEBUG("Inference results: " + std::to_string(results.size()) + " outputs");
            // TODO: Handle inference results
        }

        // Record if enabled
        if (recording && frame.type == MediaType::VIDEO) {
            // TODO: Write frame to file
        }

        // Forward processed frame
        if (on_processed_frame) {
            on_processed_frame(frame);
        }
    }

public:
    bool initialize(int width, int height) {
        if (!encoder.initialize(width, height, "libx264")) {
            return false;
        }

        if (!decoder.initialize(width, height, "h264")) {
            return false;
        }

        running = true;
        processor_thread = std::thread(&MediaProcessor::processorLoop, this);

        LOG_INFO("Media processor initialized");
        return true;
    }

    void enqueueFrame(const MediaFrame& frame) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            frame_queue.push(frame);
        }
        queue_cv.notify_one();
    }

    bool startRecording(const std::string& output_path) {
#ifdef HOPSTAR_ENABLE_FFMPEG
        recording_path = output_path;
        // TODO: Initialize recording context
        recording = true;
        LOG_INFO("Recording started: " + output_path);
        return true;
#else
        return false;
#endif
    }

    void stopRecording() {
#ifdef HOPSTAR_ENABLE_FFMPEG
        if (recording) {
            recording = false;
            // TODO: Finalize recording
            LOG_INFO("Recording stopped");
        }
#endif
    }

    bool loadInferenceModel(const std::string& model_path) {
        return inference.loadModel(model_path);
    }

    void setOnProcessedFrame(OnMediaFrameCallback callback) {
        on_processed_frame = callback;
    }

    void stop() {
        running = false;
        queue_cv.notify_all();

        if (processor_thread.joinable()) {
            processor_thread.join();
        }

        stopRecording();
    }

    ~MediaProcessor() {
        stop();
    }
};

} // namespace HopStarServer
