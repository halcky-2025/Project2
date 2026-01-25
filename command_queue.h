// =============================================================================
// HopStar Server - Command Queue System
// Thread-safe command processing for server operations
// =============================================================================
#pragma once

#include "server_common.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <sstream>

namespace HopStarServer {

// ============================================================================
// Command Types
// ============================================================================
enum class CommandType {
    // System
    PING,
    GET_STATUS,
    SHUTDOWN,

    // GUI
    GUI_MESSAGE,

    // WebRTC
    WEBRTC_CREATE_PEER,
    WEBRTC_DELETE_PEER,
    WEBRTC_GET_PEERS,

    // Media
    START_RECORDING,
    STOP_RECORDING,

    // AI/ML
    EXECUTE_TORCH,
    LOAD_MODEL,
    INFERENCE
};

// ============================================================================
// Command Structure
// ============================================================================
struct Command {
    CommandType type;
    std::map<std::string, std::string> params;
    std::promise<std::string> result;

    Command(CommandType t) : type(t) {}
    Command(CommandType t, const std::map<std::string, std::string>& p)
        : type(t), params(p) {}
};

// ============================================================================
// Command Queue (Thread-safe)
// ============================================================================
class CommandQueue {
private:
    std::queue<std::unique_ptr<Command>> queue;
    mutable std::mutex mutex;  // mutable for const methods
    std::condition_variable cv;
    bool shutdown_requested = false;

public:
    void enqueue(std::unique_ptr<Command> cmd) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(cmd));
        }
        cv.notify_one();
    }

    std::unique_ptr<Command> dequeue() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return !queue.empty() || shutdown_requested; });

        if (shutdown_requested && queue.empty()) {
            return nullptr;
        }

        auto cmd = std::move(queue.front());
        queue.pop();
        return cmd;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            shutdown_requested = true;
        }
        cv.notify_all();
    }

    bool isShutdown() const {
        return shutdown_requested;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }
};

// ============================================================================
// Command Processor
// Executes commands in dedicated thread
// ============================================================================
class CommandProcessor {
private:
    CommandQueue& queue;
    std::thread worker_thread;
    bool running = false;
    bool g_hasDisplay = false;
    volatile bool* g_running_flag = nullptr;

    // Optional references to server components
#if defined(HOPSTAR_ENABLE_GUI)
    void* hoppy = nullptr;  // HopStar* (avoid including ugui.h here)
#endif
    void* webrtc_manager = nullptr;  // Forward declaration
    void* media_processor = nullptr; // Forward declaration

    std::string executeCommand(Command& cmd) {
        switch (cmd.type) {
            case CommandType::PING:
                return R"({"status":"ok","message":"pong"})";

            case CommandType::GET_STATUS: {
                std::string mode = g_hasDisplay ? "GUI" : "headless";
#if defined(HOPSTAR_ENABLE_GUI)
                bool gui_active = (hoppy != nullptr);
#else
                bool gui_active = false;
#endif
                std::ostringstream oss;
                oss << R"({"status":"ok","mode":")" << mode
                    << R"(","gui_active":)" << (gui_active ? "true" : "false")
                    << R"(,"queue_size":)" << queue.size() << "}";
                return oss.str();
            }

            case CommandType::GUI_MESSAGE: {
#if defined(HOPSTAR_ENABLE_GUI)
                if (hoppy && g_hasDisplay) {
                    auto it = cmd.params.find("message");
                    if (it != cmd.params.end()) {
                        std::string msg = it->second;
                        LOG_INFO("GUI Message: " + msg);
                        // TODO: Actually send to GUI
                        return R"({"status":"ok","message":"GUI message sent"})";
                    }
                    return R"({"status":"error","message":"Missing 'message' parameter"})";
                } else {
                    return R"({"status":"error","message":"GUI not available"})";
                }
#else
                return R"({"status":"error","message":"GUI support disabled"})";
#endif
            }

            case CommandType::EXECUTE_TORCH: {
#ifdef HOPSTAR_ENABLE_TORCH
                try {
                    // Example: create random tensor
                    // In real usage, you'd do inference here
                    LOG_INFO("Executing PyTorch test");
                    return R"({"status":"ok","message":"PyTorch execution successful"})";
                } catch (const std::exception& e) {
                    return R"({"status":"error","message":")" + std::string(e.what()) + "\"}";
                }
#else
                return R"({"status":"error","message":"PyTorch support disabled"})";
#endif
            }

            case CommandType::WEBRTC_GET_PEERS: {
                if (webrtc_manager) {
                    // TODO: Query WebRTC manager for peer list
                    return R"({"status":"ok","peers":[]})";
                }
                return R"({"status":"error","message":"WebRTC not initialized"})";
            }

            case CommandType::SHUTDOWN:
                LOG_INFO("Shutdown command received");
                if (g_running_flag) {
                    *g_running_flag = false;
                }
                return R"({"status":"ok","message":"Shutting down"})";

            default:
                return R"({"status":"error","message":"Unknown command"})";
        }
    }

    void processorLoop() {
        LOG_INFO("CommandProcessor started");

        while (running) {
            auto cmd = queue.dequeue();
            if (!cmd) break;  // Shutdown

            try {
                std::string result = executeCommand(*cmd);
                cmd->result.set_value(result);
            } catch (const std::exception& e) {
                std::string error = R"({"status":"error","message":")" + std::string(e.what()) + "\"}";
                cmd->result.set_value(error);
            }
        }

        LOG_INFO("CommandProcessor stopped");
    }

public:
    CommandProcessor(CommandQueue& q) : queue(q) {}

    void start() {
        running = true;
        worker_thread = std::thread(&CommandProcessor::processorLoop, this);
    }

    void stop() {
        running = false;
        queue.shutdown();
        if (worker_thread.joinable()) {
            worker_thread.join();
        }
    }

    void setDisplayAvailable(bool available) {
        g_hasDisplay = available;
    }

    void setRunningFlag(volatile bool* flag) {
        g_running_flag = flag;
    }

#if defined(HOPSTAR_ENABLE_GUI)
    void setHoppy(void* h) {
        hoppy = h;
    }
#endif

    void setWebRTCManager(void* manager) {
        webrtc_manager = manager;
    }

    void setMediaProcessor(void* processor) {
        media_processor = processor;
    }
};

} // namespace HopStarServer
