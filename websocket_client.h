// =============================================================================
// HopStar Server - WebSocket Client
// Outgoing WebSocket connections using libwebsockets
// =============================================================================
#pragma once

#include "server_common.h"
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

#ifdef HOPSTAR_ENABLE_WEBSOCKET
#include <libwebsockets.h>
#endif

namespace HopStarServer {

// ============================================================================
// WebSocket Client Connection
// ============================================================================
class WebSocketClient {
private:
#ifdef HOPSTAR_ENABLE_WEBSOCKET
    struct lws_context* context = nullptr;
    struct lws* wsi = nullptr;
    std::thread service_thread;
#endif

    std::string url;
    std::string host;
    std::string path;
    int port = 443;
    bool use_ssl = true;

    bool connected = false;
    bool running = false;

    std::queue<std::string> send_queue;
    mutable std::mutex queue_mutex;

    // Callbacks
    std::function<void()> on_connected;
    std::function<void()> on_disconnected;
    std::function<void(const std::string&)> on_message;
    std::function<void(const std::string&)> on_error;

#ifdef HOPSTAR_ENABLE_WEBSOCKET
    static int callbackFunction(struct lws* wsi, enum lws_callback_reasons reason,
                                void* user, void* in, size_t len) {
        WebSocketClient* client = static_cast<WebSocketClient*>(lws_context_user(lws_get_context(wsi)));
        if (!client) return 0;

        switch (reason) {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                LOG_INFO("WebSocket client connected to " + client->url);
                client->connected = true;
                if (client->on_connected) {
                    client->on_connected();
                }
                lws_callback_on_writable(wsi);
                break;

            case LWS_CALLBACK_CLIENT_RECEIVE:
                if (in && len > 0) {
                    std::string message(static_cast<char*>(in), len);
                    LOG_DEBUG("WebSocket client received: " + message);
                    if (client->on_message) {
                        client->on_message(message);
                    }
                }
                break;

            case LWS_CALLBACK_CLIENT_WRITEABLE: {
                std::lock_guard<std::mutex> lock(client->queue_mutex);
                if (!client->send_queue.empty()) {
                    std::string msg = client->send_queue.front();
                    client->send_queue.pop();

                    unsigned char buf[LWS_PRE + msg.size()];
                    memcpy(&buf[LWS_PRE], msg.c_str(), msg.size());
                    lws_write(wsi, &buf[LWS_PRE], msg.size(), LWS_WRITE_TEXT);

                    if (!client->send_queue.empty()) {
                        lws_callback_on_writable(wsi);
                    }
                }
                break;
            }

            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                LOG_ERROR("WebSocket client connection error");
                client->connected = false;
                if (client->on_error) {
                    std::string error = in ? std::string(static_cast<char*>(in), len) : "Connection error";
                    client->on_error(error);
                }
                break;

            case LWS_CALLBACK_CLIENT_CLOSED:
                LOG_INFO("WebSocket client connection closed");
                client->connected = false;
                if (client->on_disconnected) {
                    client->on_disconnected();
                }
                break;

            default:
                break;
        }

        return 0;
    }

    void serviceLoop() {
        LOG_INFO("WebSocket client service loop started");

        while (running) {
            if (context) {
                lws_service(context, 50);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        LOG_INFO("WebSocket client service loop stopped");
    }
#endif

public:
    WebSocketClient(const std::string& ws_url) : url(ws_url) {
        parseURL(ws_url);
    }

    ~WebSocketClient() {
        disconnect();
    }

    void parseURL(const std::string& ws_url) {
        // Parse ws://host:port/path or wss://host:port/path
        size_t scheme_end = ws_url.find("://");
        if (scheme_end == std::string::npos) {
            LOG_ERROR("Invalid WebSocket URL: " + ws_url);
            return;
        }

        std::string scheme = ws_url.substr(0, scheme_end);
        use_ssl = (scheme == "wss");

        size_t host_start = scheme_end + 3;
        size_t path_start = ws_url.find('/', host_start);

        std::string host_port;
        if (path_start != std::string::npos) {
            host_port = ws_url.substr(host_start, path_start - host_start);
            path = ws_url.substr(path_start);
        } else {
            host_port = ws_url.substr(host_start);
            path = "/";
        }

        size_t colon_pos = host_port.find(':');
        if (colon_pos != std::string::npos) {
            host = host_port.substr(0, colon_pos);
            port = std::stoi(host_port.substr(colon_pos + 1));
        } else {
            host = host_port;
            port = use_ssl ? 443 : 80;
        }
    }

    bool connect() {
#ifdef HOPSTAR_ENABLE_WEBSOCKET
        if (connected || running) {
            LOG_WARN("WebSocket client already connected or connecting");
            return false;
        }

        LOG_INFO("Connecting WebSocket client to " + url);

        // Define protocols
        static const struct lws_protocols protocols[] = {
            {
                "default",
                callbackFunction,
                0,
                4096,
            },
            { NULL, NULL, 0, 0 }
        };

        // Create context
        struct lws_context_creation_info info;
        memset(&info, 0, sizeof(info));

        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.gid = -1;
        info.uid = -1;
        info.user = this;

        context = lws_create_context(&info);
        if (!context) {
            LOG_ERROR("Failed to create WebSocket client context");
            return false;
        }

        // Connect
        struct lws_client_connect_info connect_info;
        memset(&connect_info, 0, sizeof(connect_info));

        connect_info.context = context;
        connect_info.address = host.c_str();
        connect_info.port = port;
        connect_info.path = path.c_str();
        connect_info.host = host.c_str();
        connect_info.origin = host.c_str();
        connect_info.protocol = protocols[0].name;
        connect_info.ssl_connection = use_ssl ? LCCSCF_USE_SSL : 0;

        wsi = lws_client_connect_via_info(&connect_info);
        if (!wsi) {
            LOG_ERROR("Failed to initiate WebSocket client connection");
            lws_context_destroy(context);
            context = nullptr;
            return false;
        }

        // Start service thread
        running = true;
        service_thread = std::thread(&WebSocketClient::serviceLoop, this);

        return true;
#else
        LOG_ERROR("WebSocket support not compiled in");
        return false;
#endif
    }

    void disconnect() {
#ifdef HOPSTAR_ENABLE_WEBSOCKET
        running = false;

        if (service_thread.joinable()) {
            service_thread.join();
        }

        if (context) {
            lws_context_destroy(context);
            context = nullptr;
        }

        connected = false;
        wsi = nullptr;
#endif
    }

    void send(const std::string& message) {
#ifdef HOPSTAR_ENABLE_WEBSOCKET
        if (!connected) {
            LOG_WARN("Cannot send message: WebSocket client not connected");
            return;
        }

        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            send_queue.push(message);
        }

        if (wsi) {
            lws_callback_on_writable(wsi);
        }
#else
        LOG_ERROR("WebSocket support not compiled in");
#endif
    }

    bool isConnected() const {
        return connected;
    }

    // Set callbacks
    void setOnConnected(std::function<void()> callback) {
        on_connected = callback;
    }

    void setOnDisconnected(std::function<void()> callback) {
        on_disconnected = callback;
    }

    void setOnMessage(std::function<void(const std::string&)> callback) {
        on_message = callback;
    }

    void setOnError(std::function<void(const std::string&)> callback) {
        on_error = callback;
    }
};

} // namespace HopStarServer
