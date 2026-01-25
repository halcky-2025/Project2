// =============================================================================
// HopStar Server - HTTP RESTful API Server
// =============================================================================
#pragma once

#include "server_common.h"
#include "command_queue.h"
#include "thread_pool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>

namespace HopStarServer {

// ============================================================================
// HTTP Request Parser
// ============================================================================
struct ParsedRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> headers;
    std::string body;
};

inline ParsedRequest parseHttpRequest(const std::string& request) {
    ParsedRequest parsed;

    // Extract first line: "GET /path?params HTTP/1.1"
    size_t first_line_end = request.find('\n');
    std::string first_line = request.substr(0, first_line_end);

    // Parse method
    size_t space1 = first_line.find(' ');
    if (space1 != std::string::npos) {
        parsed.method = first_line.substr(0, space1);

        size_t space2 = first_line.find(' ', space1 + 1);
        std::string path_and_query = first_line.substr(space1 + 1, space2 - space1 - 1);

        // Split path and query
        size_t question_mark = path_and_query.find('?');
        if (question_mark != std::string::npos) {
            parsed.path = path_and_query.substr(0, question_mark);
            std::string query = path_and_query.substr(question_mark + 1);

            // Parse query params (simple: key=value&key=value)
            size_t pos = 0;
            while (pos < query.length()) {
                size_t eq = query.find('=', pos);
                size_t amp = query.find('&', pos);
                if (amp == std::string::npos) amp = query.length();

                if (eq != std::string::npos && eq < amp) {
                    std::string key = query.substr(pos, eq - pos);
                    std::string value = query.substr(eq + 1, amp - eq - 1);
                    parsed.params[key] = value;
                }
                pos = amp + 1;
            }
        } else {
            parsed.path = path_and_query;
        }
    }

    // Parse headers (simplified)
    size_t header_start = first_line_end + 1;
    size_t header_end = request.find("\r\n\r\n", header_start);
    if (header_end == std::string::npos) {
        header_end = request.find("\n\n", header_start);
    }

    if (header_end != std::string::npos) {
        std::string headers_section = request.substr(header_start, header_end - header_start);
        // TODO: Parse headers if needed

        // Body starts after headers
        size_t body_start = header_end + 4;  // Skip \r\n\r\n
        if (body_start < request.length()) {
            parsed.body = request.substr(body_start);
        }
    }

    return parsed;
}

// ============================================================================
// HTTP Response Builder
// ============================================================================
inline std::string buildHttpResponse(const std::string& json_body, int status_code = 200) {
    std::string status_text = (status_code == 200) ? "OK" : "Error";

    std::string response =
        "HTTP/1.1 " + std::to_string(status_code) + " " + status_text + "\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: " + std::to_string(json_body.length()) + "\r\n"
        "\r\n" + json_body;

    return response;
}

// ============================================================================
// HTTP Server
// ============================================================================
class HTTPServer {
private:
    int server_fd;
    int port;
    bool running;
    std::thread server_thread;
    CommandQueue& cmd_queue;
    ThreadPool& task_pool;

    void handleClient(int client_socket) {
        char buffer[8192] = {0};
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

        if (bytes_read <= 0) {
            close(client_socket);
            return;
        }

        std::string request_str(buffer, bytes_read);
        ParsedRequest req = parseHttpRequest(request_str);

        LOG_INFO("HTTP " + req.method + " " + req.path);

        std::string json_response;

        // Route based on path
        if (req.path == "/" || req.path == "/status") {
            auto cmd = std::make_unique<Command>(CommandType::GET_STATUS);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else if (req.path == "/ping") {
            auto cmd = std::make_unique<Command>(CommandType::PING);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else if (req.path == "/gui/message") {
            auto cmd = std::make_unique<Command>(CommandType::GUI_MESSAGE, req.params);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else if (req.path == "/torch/test") {
            auto cmd = std::make_unique<Command>(CommandType::EXECUTE_TORCH);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else if (req.path == "/webrtc/peers") {
            auto cmd = std::make_unique<Command>(CommandType::WEBRTC_GET_PEERS);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else if (req.path == "/recording/start") {
            auto cmd = std::make_unique<Command>(CommandType::START_RECORDING, req.params);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else if (req.path == "/recording/stop") {
            auto cmd = std::make_unique<Command>(CommandType::STOP_RECORDING);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else if (req.path == "/shutdown") {
            auto cmd = std::make_unique<Command>(CommandType::SHUTDOWN);
            auto future = cmd->result.get_future();
            cmd_queue.enqueue(std::move(cmd));
            json_response = future.get();
        }
        else {
            json_response = R"({"status":"error","message":"Unknown endpoint"})";
        }

        std::string response = buildHttpResponse(json_response);
        send(client_socket, response.c_str(), response.length(), 0);
        close(client_socket);
    }

    void serverLoop() {
        while (running) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);

            int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0) {
                if (running) {
                    LOG_ERROR("Accept failed");
                }
                continue;
            }

            // Handle client using thread pool for concurrent requests
            task_pool.enqueue([this, client_socket]() {
                this->handleClient(client_socket);
            });
        }
    }

public:
    HTTPServer(CommandQueue& queue, ThreadPool& pool, int p = 8080)
        : cmd_queue(queue), task_pool(pool), port(p), running(false), server_fd(-1) {}

    bool start() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            LOG_ERROR("Socket creation failed");
            return false;
        }

        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            LOG_ERROR("Setsockopt failed");
            close(server_fd);
            return false;
        }

        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            LOG_ERROR("Bind failed on port " + std::to_string(port));
            close(server_fd);
            return false;
        }

        if (listen(server_fd, 10) < 0) {
            LOG_ERROR("Listen failed");
            close(server_fd);
            return false;
        }

        running = true;
        server_thread = std::thread(&HTTPServer::serverLoop, this);

        LOG_INFO("HTTP Server started on port " + std::to_string(port));
        return true;
    }

    void stop() {
        running = false;
        if (server_fd >= 0) {
            shutdown(server_fd, SHUT_RDWR);
            close(server_fd);
            server_fd = -1;
        }
        if (server_thread.joinable()) {
            server_thread.join();
        }
        LOG_INFO("HTTP Server stopped");
    }

    ~HTTPServer() {
        stop();
    }
};

} // namespace HopStarServer
