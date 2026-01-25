// =============================================================================
// HopStar Server - Async File I/O
// Non-blocking file operations using thread pool
// =============================================================================
#pragma once

#include "server_common.h"
#include "thread_pool.h"
#include <fstream>
#include <filesystem>
#include <future>

namespace HopStarServer {

// ============================================================================
// File Operation Results
// ============================================================================
struct FileReadResult {
    bool success = false;
    std::vector<uint8_t> data;
    std::string error_message;
};

struct FileWriteResult {
    bool success = false;
    size_t bytes_written = 0;
    std::string error_message;
};

struct FileInfoResult {
    bool success = false;
    bool exists = false;
    bool is_directory = false;
    size_t file_size = 0;
    int64_t last_modified = 0;
    std::string error_message;
};

// ============================================================================
// Async File I/O Manager
// ============================================================================
class AsyncFileIO {
private:
    ThreadPool& io_pool;

public:
    AsyncFileIO(ThreadPool& pool) : io_pool(pool) {}

    // Async read entire file
    std::future<FileReadResult> readFile(const std::string& path) {
        return io_pool.submit([path]() -> FileReadResult {
            FileReadResult result;

            try {
                std::ifstream file(path, std::ios::binary | std::ios::ate);
                if (!file) {
                    result.error_message = "Cannot open file: " + path;
                    return result;
                }

                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);

                result.data.resize(size);
                if (file.read(reinterpret_cast<char*>(result.data.data()), size)) {
                    result.success = true;
                } else {
                    result.error_message = "Failed to read file: " + path;
                }
            } catch (const std::exception& e) {
                result.error_message = "Exception reading file: " + std::string(e.what());
            }

            return result;
        });
    }

    // Async read text file as string
    std::future<std::pair<bool, std::string>> readTextFile(const std::string& path) {
        return io_pool.submit([path]() -> std::pair<bool, std::string> {
            try {
                std::ifstream file(path);
                if (!file) {
                    return {false, ""};
                }

                std::string content((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
                return {true, content};
            } catch (...) {
                return {false, ""};
            }
        });
    }

    // Async write file
    std::future<FileWriteResult> writeFile(const std::string& path, const std::vector<uint8_t>& data) {
        return io_pool.submit([path, data]() -> FileWriteResult {
            FileWriteResult result;

            try {
                std::ofstream file(path, std::ios::binary | std::ios::trunc);
                if (!file) {
                    result.error_message = "Cannot open file for writing: " + path;
                    return result;
                }

                file.write(reinterpret_cast<const char*>(data.data()), data.size());
                if (file.good()) {
                    result.success = true;
                    result.bytes_written = data.size();
                } else {
                    result.error_message = "Failed to write file: " + path;
                }
            } catch (const std::exception& e) {
                result.error_message = "Exception writing file: " + std::string(e.what());
            }

            return result;
        });
    }

    // Async write text file
    std::future<bool> writeTextFile(const std::string& path, const std::string& content) {
        return io_pool.submit([path, content]() -> bool {
            try {
                std::ofstream file(path, std::ios::trunc);
                if (!file) return false;

                file << content;
                return file.good();
            } catch (...) {
                return false;
            }
        });
    }

    // Async append to file
    std::future<FileWriteResult> appendFile(const std::string& path, const std::vector<uint8_t>& data) {
        return io_pool.submit([path, data]() -> FileWriteResult {
            FileWriteResult result;

            try {
                std::ofstream file(path, std::ios::binary | std::ios::app);
                if (!file) {
                    result.error_message = "Cannot open file for appending: " + path;
                    return result;
                }

                file.write(reinterpret_cast<const char*>(data.data()), data.size());
                if (file.good()) {
                    result.success = true;
                    result.bytes_written = data.size();
                } else {
                    result.error_message = "Failed to append to file: " + path;
                }
            } catch (const std::exception& e) {
                result.error_message = "Exception appending to file: " + std::string(e.what());
            }

            return result;
        });
    }

    // Async delete file
    std::future<bool> deleteFile(const std::string& path) {
        return io_pool.submit([path]() -> bool {
            try {
                return std::filesystem::remove(path);
            } catch (...) {
                return false;
            }
        });
    }

    // Async check file exists
    std::future<bool> fileExists(const std::string& path) {
        return io_pool.submit([path]() -> bool {
            try {
                return std::filesystem::exists(path);
            } catch (...) {
                return false;
            }
        });
    }

    // Async get file info
    std::future<FileInfoResult> getFileInfo(const std::string& path) {
        return io_pool.submit([path]() -> FileInfoResult {
            FileInfoResult result;

            try {
                if (!std::filesystem::exists(path)) {
                    result.error_message = "Path does not exist: " + path;
                    return result;
                }

                result.exists = true;
                result.is_directory = std::filesystem::is_directory(path);

                if (!result.is_directory) {
                    result.file_size = std::filesystem::file_size(path);

                    auto ftime = std::filesystem::last_write_time(path);
                    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
                    );
                    result.last_modified = std::chrono::duration_cast<std::chrono::milliseconds>(
                        sctp.time_since_epoch()
                    ).count();
                }

                result.success = true;
            } catch (const std::exception& e) {
                result.error_message = "Exception getting file info: " + std::string(e.what());
            }

            return result;
        });
    }

    // Async create directory
    std::future<bool> createDirectory(const std::string& path) {
        return io_pool.submit([path]() -> bool {
            try {
                return std::filesystem::create_directories(path);
            } catch (...) {
                return false;
            }
        });
    }

    // Async list directory
    std::future<std::vector<std::string>> listDirectory(const std::string& path) {
        return io_pool.submit([path]() -> std::vector<std::string> {
            std::vector<std::string> entries;

            try {
                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                    entries.push_back(entry.path().filename().string());
                }
            } catch (...) {
                // Return empty vector on error
            }

            return entries;
        });
    }

    // Async copy file
    std::future<bool> copyFile(const std::string& src, const std::string& dst) {
        return io_pool.submit([src, dst]() -> bool {
            try {
                std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing);
                return true;
            } catch (...) {
                return false;
            }
        });
    }

    // Async move/rename file
    std::future<bool> moveFile(const std::string& src, const std::string& dst) {
        return io_pool.submit([src, dst]() -> bool {
            try {
                std::filesystem::rename(src, dst);
                return true;
            } catch (...) {
                return false;
            }
        });
    }
};

} // namespace HopStarServer
