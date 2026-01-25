// =============================================================================
// HopStar Server - Common Definitions
// =============================================================================
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

namespace HopStarServer {

// ============================================================================
// Server Configuration
// ============================================================================
struct ServerConfig {
    int http_port = 80;          // Standard HTTP port (requires root/CAP_NET_BIND_SERVICE)
    int websocket_port = 443;    // Standard HTTPS/WSS port (requires root/CAP_NET_BIND_SERVICE)
    int webrtc_min_port = 10000;
    int webrtc_max_port = 20000;

    bool enable_recording = true;
    std::string recording_path = "./recordings";

    bool enable_media_processing = true;
    bool enable_torch_inference = false;

    int max_peers = 100;
    int max_bandwidth_kbps = 5000;
};

// ============================================================================
// Client/Peer Information
// ============================================================================
struct PeerInfo {
    std::string peer_id;
    std::string client_ip;
    uint16_t client_port;
    int64_t connected_at;

    bool has_audio = false;
    bool has_video = false;
    bool has_data_channel = false;

    std::map<std::string, std::string> metadata;
};

// ============================================================================
// Media Frame (for processing pipeline)
// ============================================================================
enum class MediaType {
    AUDIO,
    VIDEO,
    DATA
};

struct MediaFrame {
    MediaType type;
    std::vector<uint8_t> data;
    int64_t timestamp_us;

    // Video specific
    int width = 0;
    int height = 0;
    int format = 0;  // AVPixelFormat or similar

    // Audio specific
    int sample_rate = 0;
    int channels = 0;

    std::string peer_id;
};

// ============================================================================
// Callbacks
// ============================================================================
using OnPeerConnectedCallback = std::function<void(const PeerInfo&)>;
using OnPeerDisconnectedCallback = std::function<void(const std::string& peer_id)>;
using OnMediaFrameCallback = std::function<void(const MediaFrame&)>;
using OnDataMessageCallback = std::function<void(const std::string& peer_id, const std::vector<uint8_t>& data)>;

// ============================================================================
// Logging
// ============================================================================
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

inline void Log(LogLevel level, const std::string& message) {
    const char* level_str[] = {"[DEBUG]", "[INFO]", "[WARN]", "[ERROR]"};
    std::cout << level_str[static_cast<int>(level)] << " " << message << std::endl;
}

#define LOG_DEBUG(msg) HopStarServer::Log(HopStarServer::LogLevel::DEBUG, msg)
#define LOG_INFO(msg) HopStarServer::Log(HopStarServer::LogLevel::INFO, msg)
#define LOG_WARN(msg) HopStarServer::Log(HopStarServer::LogLevel::WARNING, msg)
#define LOG_ERROR(msg) HopStarServer::Log(HopStarServer::LogLevel::ERROR, msg)

} // namespace HopStarServer
