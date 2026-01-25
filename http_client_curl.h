// =============================================================================
// HopStar Server - HTTP/HTTPS Client (libcurl version)
// Full HTTPS support with libcurl
// =============================================================================
#pragma once

#include "server_common.h"
#include "thread_pool.h"
#include "http_client.h"  // Use common definitions from http_client.h
#include <future>
#include <map>
#include <vector>

#ifdef HOPSTAR_ENABLE_CURL
#include <curl/curl.h>
#endif

namespace HopStarServer {

// Note: HttpMethod, HttpRequest, HttpResponse are defined in http_client.h
// We extend HttpRequest to add SSL verification option

// ============================================================================
// HTTP Client with libcurl (HTTPS support)
// ============================================================================
class HttpClientCurl {
private:
    ThreadPool& task_pool;
    static bool curl_initialized;

#ifdef HOPSTAR_ENABLE_CURL
    // Callback for writing response body
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::vector<uint8_t>* buffer = static_cast<std::vector<uint8_t>*>(userp);
        size_t total_size = size * nmemb;

        const uint8_t* data = static_cast<const uint8_t*>(contents);
        buffer->insert(buffer->end(), data, data + total_size);

        return total_size;
    }

    // Callback for reading request body (for POST/PUT)
    static size_t readCallback(char* buffer, size_t size, size_t nitems, void* userp) {
        std::vector<uint8_t>* body = static_cast<std::vector<uint8_t>*>(userp);
        size_t buffer_size = size * nitems;

        if (body->empty()) {
            return 0;  // No more data
        }

        size_t copy_size = std::min(buffer_size, body->size());
        memcpy(buffer, body->data(), copy_size);
        body->erase(body->begin(), body->begin() + copy_size);

        return copy_size;
    }

    // Callback for response headers
    static size_t headerCallback(char* buffer, size_t size, size_t nitems, void* userp) {
        std::map<std::string, std::string>* headers = static_cast<std::map<std::string, std::string>*>(userp);
        size_t total_size = size * nitems;

        std::string header_line(buffer, total_size);

        // Parse header: "Key: Value\r\n"
        size_t colon_pos = header_line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = header_line.substr(0, colon_pos);
            std::string value = header_line.substr(colon_pos + 1);

            // Trim whitespace
            size_t value_start = value.find_first_not_of(" \t");
            size_t value_end = value.find_last_not_of(" \t\r\n");

            if (value_start != std::string::npos && value_end != std::string::npos) {
                value = value.substr(value_start, value_end - value_start + 1);
                (*headers)[key] = value;
            }
        }

        return total_size;
    }

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

        CURL* curl = curl_easy_init();
        if (!curl) {
            response.error_message = "Failed to initialize CURL";
            return response;
        }

        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());

        // Set method
        switch (req.method) {
            case HttpMethod::GET:
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                break;
            case HttpMethod::POST:
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                break;
            case HttpMethod::PUT:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                break;
            case HttpMethod::DELETE:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;
            case HttpMethod::PATCH:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                break;
            case HttpMethod::HEAD:
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
                break;
            case HttpMethod::OPTIONS:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
                break;
        }

        // Set timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, (long)req.timeout_ms);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, (long)req.timeout_ms);

        // SSL verification
        if (req.verify_ssl) {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        } else {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        // Set headers
        struct curl_slist* header_list = nullptr;
        for (const auto& [key, value] : req.headers) {
            std::string header = key + ": " + value;
            header_list = curl_slist_append(header_list, header.c_str());
        }
        if (header_list) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        }

        // Set request body (for POST/PUT/PATCH)
        std::vector<uint8_t> body_copy = req.body;  // Copy for read callback
        if (!req.body.empty() && (req.method == HttpMethod::POST ||
                                  req.method == HttpMethod::PUT ||
                                  req.method == HttpMethod::PATCH)) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.data());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)req.body.size());
        }

        // Response body callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);

        // Response headers callback
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);

        // Follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);

        // Perform request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            response.error_message = "CURL error: " + std::string(curl_easy_strerror(res));
            response.success = false;
        } else {
            // Get response code
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            response.status_code = static_cast<int>(http_code);
            response.success = (http_code >= 200 && http_code < 300);

            // Get status message (approximation)
            if (http_code >= 200 && http_code < 300) {
                response.status_message = "OK";
            } else if (http_code >= 400 && http_code < 500) {
                response.status_message = "Client Error";
            } else if (http_code >= 500) {
                response.status_message = "Server Error";
            }
        }

        // Cleanup
        if (header_list) {
            curl_slist_free_all(header_list);
        }
        curl_easy_cleanup(curl);

        return response;
    }
#endif

public:
    HttpClientCurl(ThreadPool& pool) : task_pool(pool) {
#ifdef HOPSTAR_ENABLE_CURL
        if (!curl_initialized) {
            curl_global_init(CURL_GLOBAL_ALL);
            curl_initialized = true;
            LOG_INFO("libcurl initialized (HTTPS support enabled)");
        }
#else
        LOG_WARN("HOPSTAR_ENABLE_CURL not defined - HTTPS support disabled");
#endif
    }

    ~HttpClientCurl() {
        // Note: curl_global_cleanup() should only be called once at program exit
        // We don't call it here because other instances might still be using it
    }

    // Async HTTP request
    std::future<HttpResponse> request(const HttpRequest& req) {
#ifdef HOPSTAR_ENABLE_CURL
        return task_pool.submit([req]() -> HttpResponse {
            return executeRequest(req);
        });
#else
        HttpResponse err;
        err.error_message = "HTTPS support not compiled in (use -DHOPSTAR_ENABLE_CURL=ON)";
        std::promise<HttpResponse> p;
        p.set_value(err);
        return p.get_future();
#endif
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

#ifdef HOPSTAR_ENABLE_CURL
bool HttpClientCurl::curl_initialized = false;
#endif

} // namespace HopStarServer
