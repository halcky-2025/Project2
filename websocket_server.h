// =============================================================================
// HopStar Server - WebSocket Signaling Server
// Handles WebRTC signaling (SDP offer/answer, ICE candidates)
// =============================================================================
#pragma once

#include "server_common.h"
#include <map>
#include <mutex>
#include <thread>
#include <vector>

// Forward declarations for libwebsockets (will be linked later)
#ifdef HOPSTAR_ENABLE_WEBSOCKET
struct lws;
struct lws_context;
struct lws_protocols;
#endif

namespace HopStarServer {

// ============================================================================
// Signaling Message Types
// ============================================================================
enum class SignalingMessageType {
    OFFER,          // SDP offer from client
    ANSWER,         // SDP answer from server/other client
    ICE_CANDIDATE,  // ICE candidate
    JOIN_ROOM,      // Client wants to join a room
    LEAVE_ROOM,     // Client leaves a room
    PEER_JOINED,    // Notify: new peer joined
    PEER_LEFT       // Notify: peer left
};

struct SignalingMessage {
    SignalingMessageType type;
    std::string peer_id;
    std::string room_id;
    std::string sdp;
    std::string ice_candidate;
    std::string ice_sdp_mid;
    int ice_sdp_mline_index = 0;

    std::map<std::string, std::string> extra;
};

// ============================================================================
// WebSocket Client Session
// ============================================================================
struct WebSocketSession {
    void* wsi = nullptr;  // lws* (WebSocket Instance)
    std::string peer_id;
    std::string room_id;
    bool authenticated = false;
    int64_t connected_at = 0;
};

// ============================================================================
// WebSocket Server
// ============================================================================
class WebSocketServer {
private:
#ifdef HOPSTAR_ENABLE_WEBSOCKET
    lws_context* context = nullptr;
#else
    void* context = nullptr;
#endif

    int port;
    bool running = false;
    std::thread server_thread;

    // Session management
    std::map<void*, std::shared_ptr<WebSocketSession>> sessions;
    std::mutex sessions_mutex;

    // Room management (peer_id -> room_id)
    std::map<std::string, std::string> peer_rooms;
    std::mutex rooms_mutex;

    // Callbacks
    std::function<void(const SignalingMessage&)> on_message_callback;
    std::function<void(const std::string& peer_id)> on_peer_connected_callback;
    std::function<void(const std::string& peer_id)> on_peer_disconnected_callback;

    void handleMessage(WebSocketSession* session, const std::string& message) {
        LOG_INFO("WebSocket message from " + session->peer_id + ": " + message.substr(0, 100));

        // TODO: Parse JSON message and convert to SignalingMessage
        // For now, just a placeholder

        if (on_message_callback) {
            SignalingMessage sig_msg;
            sig_msg.peer_id = session->peer_id;
            // TODO: Parse message and fill sig_msg
            on_message_callback(sig_msg);
        }
    }

    void serverLoop() {
#ifdef HOPSTAR_ENABLE_WEBSOCKET
        LOG_INFO("WebSocket server loop started");

        while (running) {
            // libwebsockets service loop
            // lws_service(context, 50);  // Service with 50ms timeout
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        LOG_INFO("WebSocket server loop stopped");
#else
        LOG_WARN("WebSocket support not compiled in");
#endif
    }

public:
    WebSocketServer(int p = 8081) : port(p) {}

    bool start() {
#ifdef HOPSTAR_ENABLE_WEBSOCKET
        LOG_INFO("Starting WebSocket server on port " + std::to_string(port));

        // TODO: Initialize libwebsockets context
        // struct lws_context_creation_info info;
        // memset(&info, 0, sizeof(info));
        // info.port = port;
        // info.protocols = protocols;
        // context = lws_create_context(&info);

        running = true;
        server_thread = std::thread(&WebSocketServer::serverLoop, this);

        LOG_INFO("WebSocket server started");
        return true;
#else
        LOG_ERROR("WebSocket support not enabled (compile with -DHOPSTAR_ENABLE_WEBSOCKET=ON)");
        return false;
#endif
    }

    void stop() {
        running = false;

#ifdef HOPSTAR_ENABLE_WEBSOCKET
        if (context) {
            // lws_context_destroy(context);
            context = nullptr;
        }
#endif

        if (server_thread.joinable()) {
            server_thread.join();
        }

        LOG_INFO("WebSocket server stopped");
    }

    // Send message to specific peer
    void sendToPeer(const std::string& peer_id, const std::string& message) {
        std::lock_guard<std::mutex> lock(sessions_mutex);

        for (auto& [wsi, session] : sessions) {
            if (session->peer_id == peer_id) {
#ifdef HOPSTAR_ENABLE_WEBSOCKET
                // TODO: lws_write()
                LOG_DEBUG("Sending to peer " + peer_id + ": " + message.substr(0, 50));
#endif
                return;
            }
        }

        LOG_WARN("Peer not found: " + peer_id);
    }

    // Broadcast to all peers in a room
    void broadcastToRoom(const std::string& room_id, const std::string& message, const std::string& except_peer_id = "") {
        std::lock_guard<std::mutex> lock(sessions_mutex);

        for (auto& [wsi, session] : sessions) {
            if (session->room_id == room_id && session->peer_id != except_peer_id) {
#ifdef HOPSTAR_ENABLE_WEBSOCKET
                // TODO: lws_write()
                LOG_DEBUG("Broadcasting to " + session->peer_id);
#endif
            }
        }
    }

    // Register peer to room
    void joinRoom(const std::string& peer_id, const std::string& room_id) {
        std::lock_guard<std::mutex> lock(rooms_mutex);
        peer_rooms[peer_id] = room_id;
        LOG_INFO("Peer " + peer_id + " joined room " + room_id);

        // Notify other peers in room
        SignalingMessage msg;
        msg.type = SignalingMessageType::PEER_JOINED;
        msg.peer_id = peer_id;
        msg.room_id = room_id;
        // TODO: Broadcast to room
    }

    // Remove peer from room
    void leaveRoom(const std::string& peer_id) {
        std::lock_guard<std::mutex> lock(rooms_mutex);
        auto it = peer_rooms.find(peer_id);
        if (it != peer_rooms.end()) {
            std::string room_id = it->second;
            peer_rooms.erase(it);
            LOG_INFO("Peer " + peer_id + " left room " + room_id);

            // Notify other peers
            SignalingMessage msg;
            msg.type = SignalingMessageType::PEER_LEFT;
            msg.peer_id = peer_id;
            msg.room_id = room_id;
            // TODO: Broadcast to room
        }
    }

    // Set callbacks
    void setOnMessage(std::function<void(const SignalingMessage&)> callback) {
        on_message_callback = callback;
    }

    void setOnPeerConnected(std::function<void(const std::string&)> callback) {
        on_peer_connected_callback = callback;
    }

    void setOnPeerDisconnected(std::function<void(const std::string&)> callback) {
        on_peer_disconnected_callback = callback;
    }

    ~WebSocketServer() {
        stop();
    }
};

} // namespace HopStarServer
