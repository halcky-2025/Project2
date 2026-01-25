// =============================================================================
// HopStar Server - HTTP Client
// Async HTTP client for outgoing requests
// =============================================================================
#pragma once

#include "server_common.h"
#include "thread_pool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <future>
#include <cstring>

namespace HopStarServer {

// ============================================================================
// HTTP Request/Response
// ============================================================================
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS
};

struct HttpRequest {
    HttpMethod method = HttpMethod::GET;
    std::string url;
    std::map<std::string, std::string> headers;
    std::vector<uint8_t> body;
    int timeout_ms = 30000;  // 30 seconds default
    bool verify_ssl = true;  // SSL certificate verification (used by libcurl)
};

struct HttpResponse {
    int status_code = 0;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::vector<uint8_t> body;
    bool success = false;
    std::string error_message;
};

// ============================================================================
// URL Parser
// ============================================================================
struct ParsedURL {
    std::string scheme;      // http or https
    std::string host;
    int port = 80;
    std::string path;
    bool is_valid = false;
};

inline ParsedURL parseURL(const std::string& url) {
    ParsedURL result;

    // Basic URL parsing: scheme://host:port/path
    size_t scheme_end = url.find("://");
    if (scheme_end == std::string::npos) {
        return result;
    }

    result.scheme = url.substr(0, scheme_end);
    size_t host_start = scheme_end + 3;

    size_t path_start = url.find('/', host_start);
    std::string host_port;

    if (path_start != std::string::npos) {
        host_port = url.substr(host_start, path_start - host_start);
        result.path = url.substr(path_start);
    } else {
        host_port = url.substr(host_start);
        result.path = "/";
    }

    // Parse host:port
    size_t colon_pos = host_port.find(':');
    if (colon_pos != std::string::npos) {
        result.host = host_port.substr(0, colon_pos);
        result.port = std::stoi(host_port.substr(colon_pos + 1));
    } else {
        result.host = host_port;
        result.port = (result.scheme == "https") ? 443 : 80;
    }

    result.is_valid = true;
    return result;
}

// ============================================================================
// HTTP Client
// ============================================================================
class HttpClient {
private:
    ThreadPool& task_pool;

    static std::string methodToString(HttpMethod method) {
        switch (method) {
            case HttpMethod::GET: return "GET";
            case HttpMethod::POST: return "POST";
            case HttpMethod::PUT: return "PUT";
            case HttpMethod::DELETE: return "DELETE";
            case HttpMethod::PATCH: return "PATCH";
            case HttpMethod::HEAD: return "HEAD";
            case HttpMethod::OPTIONS: return "OPTIONS";
            default: return "GET";
        }
    }

    static HttpResponse executeRequest(const HttpRequest& req) {
        HttpResponse response;

        // Parse URL
        ParsedURL url = parseURL(req.url);
        if (!url.is_valid) {
            response.error_message = "Invalid URL: " + req.url;
            return response;
        }

        // Note: This is a basic HTTP client without SSL support
        if (url.scheme == "https") {
            response.error_message = "HTTPS not supported in basic HTTP client. Use HttpClientCurl for HTTPS.";
            LOG_WARN("HTTPS requested but not supported. Use HOPSTAR_ENABLE_CURL for HTTPS support.");
            return response;
        }

        // Resolve hostname using getaddrinfo (modern, thread-safe, IPv4/IPv6)
        struct addrinfo hints;
        struct addrinfo* result = nullptr;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;      // Allow IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM;  // TCP socket
        hints.ai_flags = 0;
        hints.ai_protocol = 0;

        std::string port_str = std::to_string(url.port);
        int ret = getaddrinfo(url.host.c_str(), port_str.c_str(), &hints, &result);
        if (ret != 0) {
            response.error_message = "Cannot resolve hostname: " + url.host + " (" + std::string(gai_strerror(ret)) + ")";
            return response;
        }

        // Try each address until we successfully connect
        int sock = -1;
        struct addrinfo* rp = nullptr;
        for (rp = result; rp != nullptr; rp = rp->ai_next) {
            sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sock < 0) {
                continue;  // Try next address
            }

            // Set timeout
            struct timeval timeout;
            timeout.tv_sec = req.timeout_ms / 1000;
            timeout.tv_usec = (req.timeout_ms % 1000) * 1000;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

            // Try to connect
            if (connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) {
                break;  // Success
            }

            close(sock);
            sock = -1;
        }

        freeaddrinfo(result);

        if (sock < 0) {
            response.error_message = "Failed to connect to " + url.host + ":" + std::to_string(url.port);
            return response;
        }

        // Build HTTP request
        std::ostringstream request_stream;
        request_stream << methodToString(req.method) << " " << url.path << " HTTP/1.1\r\n";
        request_stream << "Host: " << url.host << "\r\n";
        request_stream << "Connection: close\r\n";

        // Add custom headers
        for (const auto& [key, value] : req.headers) {
            request_stream << key << ": " << value << "\r\n";
        }

        // Add body if present
        if (!req.body.empty()) {
            request_stream << "Content-Length: " << req.body.size() << "\r\n";
        }

        request_stream << "\r\n";

        // Send request headers
        std::string request_str = request_stream.str();
        send(sock, request_str.c_str(), request_str.size(), 0);

        // Send body if present
        if (!req.body.empty()) {
            send(sock, req.body.data(), req.body.size(), 0);
        }

        // Read response
        std::vector<uint8_t> response_data;
        char buffer[4096];
        ssize_t bytes_read;

        while ((bytes_read = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
            response_data.insert(response_data.end(), buffer, buffer + bytes_read);
        }

        close(sock);

        if (response_data.empty()) {
            response.error_message = "Empty response from server";
            return response;
        }

        // Parse response (basic parsing)
        std::string response_str(response_data.begin(), response_data.end());
        size_t header_end = response_str.find("\r\n\r\n");

        if (header_end == std::string::npos) {
            response.error_message = "Invalid HTTP response format";
            return response;
        }

        std::string headers = response_str.substr(0, header_end);
        std::string body = response_str.substr(header_end + 4);

        // Parse status line
        size_t status_line_end = headers.find("\r\n");
        std::string status_line = headers.substr(0, status_line_end);

        std::istringstream status_stream(status_line);
        std::string http_version;
        status_stream >> http_version >> response.status_code;
        std::getline(status_stream, response.status_message);
        if (!response.status_message.empty() && response.status_message[0] == ' ') {
            response.status_message = response.status_message.substr(1);
        }

        // Parse headers
        std::istringstream header_stream(headers.substr(status_line_end + 2));
        std::string line;
        while (std::getline(header_stream, line) && !line.empty() && line != "\r") {
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value = line.substr(colon_pos + 1);
                // Trim leading space
                if (!value.empty() && value[0] == ' ') {
                    value = value.substr(1);
                }
                // Remove \r if present
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                response.headers[key] = value;
            }
        }

        // Set body
        response.body = std::vector<uint8_t>(body.begin(), body.end());
        response.success = (response.status_code >= 200 && response.status_code < 300);

        return response;
    }

public:
    HttpClient(ThreadPool& pool) : task_pool(pool) {}

    // Async HTTP request
    std::future<HttpResponse> request(const HttpRequest& req) {
        return task_pool.submit([req]() -> HttpResponse {
            return executeRequest(req);
        });
    }

    // Convenience methods
    std::future<HttpResponse> get(const std::string& url,
                                   const std::map<std::string, std::string>& headers = {}) {
        HttpRequest req;
        req.method = HttpMethod::GET;
        req.url = url;
        req.headers = headers;
        return request(req);
    }

    std::future<HttpResponse> post(const std::string& url,
                                    const std::vector<uint8_t>& body,
                                    const std::map<std::string, std::string>& headers = {}) {
        HttpRequest req;
        req.method = HttpMethod::POST;
        req.url = url;
        req.body = body;
        req.headers = headers;
        return request(req);
    }

    std::future<HttpResponse> post(const std::string& url,
                                    const std::string& body,
                                    const std::map<std::string, std::string>& headers = {}) {
        return post(url, std::vector<uint8_t>(body.begin(), body.end()), headers);
    }

    std::future<HttpResponse> put(const std::string& url,
                                   const std::vector<uint8_t>& body,
                                   const std::map<std::string, std::string>& headers = {}) {
        HttpRequest req;
        req.method = HttpMethod::PUT;
        req.url = url;
        req.body = body;
        req.headers = headers;
        return request(req);
    }

    std::future<HttpResponse> del(const std::string& url,
                                   const std::map<std::string, std::string>& headers = {}) {
        HttpRequest req;
        req.method = HttpMethod::DELETE;
        req.url = url;
        req.headers = headers;
        return request(req);
    }
};

} // namespace HopStarServer
