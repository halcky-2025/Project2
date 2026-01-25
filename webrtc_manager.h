// =============================================================================
// HopStar Server - WebRTC Manager
// Manages WebRTC peer connections using libdatachannel
// =============================================================================
#pragma once

#include "server_common.h"
#include <map>
#include <mutex>
#include <memory>

// Forward declarations for libdatachannel
#ifdef HOPSTAR_ENABLE_WEBRTC
namespace rtc {
    class PeerConnection;
    class DataChannel;
    class Track;
}
#endif

namespace HopStarServer {

// ============================================================================
// WebRTC Peer Connection Wrapper
// ============================================================================
class WebRTCPeer {
private:
    std::string peer_id;
    PeerInfo info;

#ifdef HOPSTAR_ENABLE_WEBRTC
    std::shared_ptr<rtc::PeerConnection> peer_connection;
    std::shared_ptr<rtc::DataChannel> data_channel;
    std::vector<std::shared_ptr<rtc::Track>> tracks;
#else
    void* peer_connection = nullptr;
    void* data_channel = nullptr;
#endif

    // Callbacks
    OnMediaFrameCallback on_media_frame;
    OnDataMessageCallback on_data_message;

public:
    WebRTCPeer(const std::string& id) : peer_id(id) {
        info.peer_id = id;
        info.connected_at = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    bool initialize() {
#ifdef HOPSTAR_ENABLE_WEBRTC
        try {
            // rtc::Configuration config;
            // config.iceServers.emplace_back("stun:stun.l.google.com:19302");
            // peer_connection = std::make_shared<rtc::PeerConnection>(config);

            // Setup callbacks
            // peer_connection->onTrack([this](std::shared_ptr<rtc::Track> track) {
            //     handleTrack(track);
            // });

            // peer_connection->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
            //     handleDataChannel(dc);
            // });

            LOG_INFO("WebRTC peer initialized: " + peer_id);
            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to initialize WebRTC peer: " + std::string(e.what()));
            return false;
        }
#else
        LOG_ERROR("WebRTC support not compiled in");
        return false;
#endif
    }

    std::string createOffer() {
#ifdef HOPSTAR_ENABLE_WEBRTC
        // TODO: Create SDP offer
        // auto offer = peer_connection->createOffer();
        // return offer.toString();
        return "v=0\r\no=- 0 0 IN IP4 127.0.0.1\r\n...";  // Placeholder
#else
        return "";
#endif
    }

    bool setRemoteDescription(const std::string& sdp, const std::string& type) {
#ifdef HOPSTAR_ENABLE_WEBRTC
        try {
            // rtc::Description desc(sdp, type);
            // peer_connection->setRemoteDescription(desc);
            LOG_INFO("Set remote description for " + peer_id);
            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to set remote description: " + std::string(e.what()));
            return false;
        }
#else
        return false;
#endif
    }

    bool addIceCandidate(const std::string& candidate, const std::string& mid) {
#ifdef HOPSTAR_ENABLE_WEBRTC
        try {
            // peer_connection->addRemoteCandidate(rtc::Candidate(candidate, mid));
            LOG_DEBUG("Added ICE candidate for " + peer_id);
            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to add ICE candidate: " + std::string(e.what()));
            return false;
        }
#else
        return false;
#endif
    }

    void sendData(const std::vector<uint8_t>& data) {
#ifdef HOPSTAR_ENABLE_WEBRTC
        if (data_channel && data_channel->isOpen()) {
            // data_channel->send(data);
            LOG_DEBUG("Sent data to " + peer_id + ": " + std::to_string(data.size()) + " bytes");
        }
#endif
    }

    void setOnMediaFrame(OnMediaFrameCallback callback) {
        on_media_frame = callback;
    }

    void setOnDataMessage(OnDataMessageCallback callback) {
        on_data_message = callback;
    }

    const PeerInfo& getPeerInfo() const {
        return info;
    }

    std::string getPeerId() const {
        return peer_id;
    }

private:
#ifdef HOPSTAR_ENABLE_WEBRTC
    void handleTrack(std::shared_ptr<rtc::Track> track) {
        LOG_INFO("Received track from " + peer_id);
        tracks.push_back(track);

        // track->onMessage([this](rtc::binary data) {
        //     if (on_media_frame) {
        //         MediaFrame frame;
        //         frame.peer_id = peer_id;
        //         frame.data = std::vector<uint8_t>(data.begin(), data.end());
        //         // TODO: Determine if audio or video
        //         on_media_frame(frame);
        //     }
        // });
    }

    void handleDataChannel(std::shared_ptr<rtc::DataChannel> dc) {
        LOG_INFO("Received data channel from " + peer_id);
        data_channel = dc;

        // dc->onMessage([this](auto data) {
        //     if (on_data_message) {
        //         std::vector<uint8_t> msg;
        //         if (std::holds_alternative<rtc::binary>(data)) {
        //             auto bin = std::get<rtc::binary>(data);
        //             msg = std::vector<uint8_t>(bin.begin(), bin.end());
        //         }
        //         on_data_message(peer_id, msg);
        //     }
        // });
    }
#endif
};

// ============================================================================
// WebRTC Manager
// Manages all peer connections
// ============================================================================
class WebRTCManager {
private:
    std::map<std::string, std::shared_ptr<WebRTCPeer>> peers;
    mutable std::mutex peers_mutex;  // mutable for const methods

    ServerConfig config;

    // Callbacks
    OnPeerConnectedCallback on_peer_connected;
    OnPeerDisconnectedCallback on_peer_disconnected;
    OnMediaFrameCallback on_media_frame;
    OnDataMessageCallback on_data_message;

public:
    WebRTCManager(const ServerConfig& cfg) : config(cfg) {}

    bool initialize() {
#ifdef HOPSTAR_ENABLE_WEBRTC
        LOG_INFO("Initializing WebRTC Manager");
        // TODO: Global WebRTC initialization if needed
        return true;
#else
        LOG_ERROR("WebRTC support not compiled in (use -DHOPSTAR_ENABLE_WEBRTC=ON)");
        return false;
#endif
    }

    std::shared_ptr<WebRTCPeer> createPeer(const std::string& peer_id) {
        std::lock_guard<std::mutex> lock(peers_mutex);

        if (peers.find(peer_id) != peers.end()) {
            LOG_WARN("Peer already exists: " + peer_id);
            return peers[peer_id];
        }

        auto peer = std::make_shared<WebRTCPeer>(peer_id);
        if (!peer->initialize()) {
            LOG_ERROR("Failed to initialize peer: " + peer_id);
            return nullptr;
        }

        // Set callbacks
        peer->setOnMediaFrame(on_media_frame);
        peer->setOnDataMessage(on_data_message);

        peers[peer_id] = peer;

        if (on_peer_connected) {
            on_peer_connected(peer->getPeerInfo());
        }

        LOG_INFO("Created peer: " + peer_id);
        return peer;
    }

    void removePeer(const std::string& peer_id) {
        std::lock_guard<std::mutex> lock(peers_mutex);

        auto it = peers.find(peer_id);
        if (it != peers.end()) {
            peers.erase(it);

            if (on_peer_disconnected) {
                on_peer_disconnected(peer_id);
            }

            LOG_INFO("Removed peer: " + peer_id);
        }
    }

    std::shared_ptr<WebRTCPeer> getPeer(const std::string& peer_id) {
        std::lock_guard<std::mutex> lock(peers_mutex);

        auto it = peers.find(peer_id);
        if (it != peers.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::vector<PeerInfo> getAllPeers() {
        std::lock_guard<std::mutex> lock(peers_mutex);

        std::vector<PeerInfo> result;
        for (const auto& [id, peer] : peers) {
            result.push_back(peer->getPeerInfo());
        }
        return result;
    }

    void broadcastData(const std::vector<uint8_t>& data, const std::string& except_peer_id = "") {
        std::lock_guard<std::mutex> lock(peers_mutex);

        for (const auto& [id, peer] : peers) {
            if (id != except_peer_id) {
                peer->sendData(data);
            }
        }
    }

    // Set callbacks
    void setOnPeerConnected(OnPeerConnectedCallback callback) {
        on_peer_connected = callback;
    }

    void setOnPeerDisconnected(OnPeerDisconnectedCallback callback) {
        on_peer_disconnected = callback;
    }

    void setOnMediaFrame(OnMediaFrameCallback callback) {
        on_media_frame = callback;
    }

    void setOnDataMessage(OnDataMessageCallback callback) {
        on_data_message = callback;
    }

    size_t getPeerCount() const {
        std::lock_guard<std::mutex> lock(peers_mutex);
        return peers.size();
    }
};

} // namespace HopStarServer
