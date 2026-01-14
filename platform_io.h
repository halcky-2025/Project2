#pragma once
/**
 * Platform I/O Abstraction Layer
 *
 * Provides unified file I/O across all platforms:
 * - Windows: Direct file access
 * - macOS/iOS: NSBundle resource access
 * - Android: SDL asset access
 * - Linux: Direct file access
 *
 * Uses FileEngine internally for caching and remote access.
 */

#include "file_engine.h"
#include <string>
#include <vector>
#include <functional>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) && !defined(__ANDROID__)
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR || TARGET_OS_MAC
#import <Foundation/Foundation.h>
#endif
#endif

// SDL is used on all platforms for logging and Android asset access
#include <SDL3/SDL.h>

namespace PlatformIO {

// ============================================================================
// Global FileEngine accessor
// ============================================================================
inline HopStarIO::FileEngine* getEngine() {
    static std::unique_ptr<HopStarIO::FileEngine> s_engine;
    if (!s_engine) {
        s_engine = HopStarIO::FileEngineFactory::createDefault();
    }
    return s_engine.get();
}

// ============================================================================
// Platform-specific path resolution
// ============================================================================

#ifdef __APPLE__
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR || TARGET_OS_MAC
inline std::string resolveBundlePath(const std::string& filename) {
    @autoreleasepool {
        NSString* name = [[NSString alloc] initWithUTF8String:filename.c_str()];
        NSString* lastComponent = [name lastPathComponent];
        NSString* ext = [lastComponent pathExtension];
        NSString* base = [lastComponent stringByDeletingPathExtension];
        NSString* path = [[NSBundle mainBundle] pathForResource:base ofType:ext];
        if (path) {
            return std::string([path UTF8String]);
        }
    }
    return filename;
}
#endif
#endif

// Helper: Get executable directory
inline std::string getExeDirectory() {
#if defined(_WIN32)
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    char utf8ExePath[MAX_PATH * 3];
    WideCharToMultiByte(CP_UTF8, 0, exePath, -1, utf8ExePath, sizeof(utf8ExePath), NULL, NULL);
    return std::filesystem::path(utf8ExePath).parent_path().string();
#elif defined(__linux__) && !defined(__ANDROID__)
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
        return std::filesystem::path(exePath).parent_path().string();
    }
    return ".";
#else
    return ".";
#endif
}

// Resolve path based on Location - central path resolution function
inline std::string resolvePath(const std::string& path, HopStarIO::Location location) {
#if defined(__ANDROID__)
    // Android: Don't use FileEngine, use SDL functions directly
    switch (location) {
    case HopStarIO::Location::Resource:
        return path;  // Will use SDL_IOFromFile for APK assets
    case HopStarIO::Location::Internal:
        {
            const char* prefPath = SDL_GetPrefPath("org.libsdl.app", "HopStarBB");
            if (prefPath) return std::string(prefPath) + path;
            return path;
        }
    default:
        return path;
    }
#elif TARGET_OS_IOS || TARGET_OS_SIMULATOR
    // iOS: Use bundle path for resources
    if (location == HopStarIO::Location::Resource) {
        return resolveBundlePath(path);
    }
    auto* engine = getEngine();
    if (location == HopStarIO::Location::Internal) {
        return engine->getInternalPath(path);
    }
    return path;
#elif TARGET_OS_MAC
    // macOS: Try bundle first for resources
    if (location == HopStarIO::Location::Resource) {
        std::string bundlePath = resolveBundlePath(path);
        if (std::filesystem::exists(bundlePath)) {
            return bundlePath;
        }
        return path;
    }
    auto* engine = getEngine();
    if (location == HopStarIO::Location::Internal) {
        return engine->getInternalPath(path);
    }
    return path;
#else
    // Windows/Linux
    auto* engine = getEngine();
    switch (location) {
    case HopStarIO::Location::Resource:
        {
            std::string resourcePath = engine->getResourcePath(path);
            if (std::filesystem::exists(resourcePath)) {
                return resourcePath;
            }
            std::filesystem::path exeDirPath = std::filesystem::path(getExeDirectory()) / path;
            if (std::filesystem::exists(exeDirPath)) {
                return exeDirPath.string();
            }
            return path;
        }
    case HopStarIO::Location::Internal:
        return engine->getInternalPath(path);
    case HopStarIO::Location::External:
    case HopStarIO::Location::Server:
    case HopStarIO::Location::Cloud:
    case HopStarIO::Location::P2P:
    default:
        return path;
    }
#endif
}

// ============================================================================
// Unified file reading with Location support
// ============================================================================

// Read entire file into memory with explicit Location
inline std::vector<uint8_t> readFile(const std::string& path, HopStarIO::Location location) {
#ifdef __ANDROID__
    // Android: Use SDL_IOFromFile for all file access
    std::string fullPath = path;
    if (location == HopStarIO::Location::Internal) {
        const char* prefPath = SDL_GetPrefPath("org.libsdl.app", "HopStarBB");
        if (prefPath) fullPath = std::string(prefPath) + path;
    }
    SDL_IOStream* io = SDL_IOFromFile(fullPath.c_str(), "rb");
    if (!io) {
        SDL_Log("PlatformIO::readFile: Failed to open: %s", fullPath.c_str());
        return {};
    }
    Sint64 size = SDL_GetIOSize(io);
    if (size <= 0) {
        SDL_CloseIO(io);
        return {};
    }
    std::vector<uint8_t> data(static_cast<size_t>(size));
    size_t read = SDL_ReadIO(io, data.data(), data.size());
    SDL_CloseIO(io);
    if (read != data.size()) {
        SDL_Log("PlatformIO::readFile: Incomplete read: %s", fullPath.c_str());
        return {};
    }
    return data;
#else
    auto* engine = getEngine();

    // Remote locations: prefetch first
    if (location == HopStarIO::Location::Server ||
        location == HopStarIO::Location::Cloud ||
        location == HopStarIO::Location::P2P) {
        auto desc = engine->createDescriptor(path, location, HopStarIO::Access::Read, path);
        if (!engine->prefetch(desc)) {
            SDL_Log("PlatformIO::readFile: Prefetch failed for remote: %s", path.c_str());
            return {};
        }
        auto result = engine->read(desc);
        return result.success ? std::move(result.data) : std::vector<uint8_t>{};
    }

    // Local locations: resolve path and read via FileEngine
    std::string resolvedPath = resolvePath(path, location);
    auto desc = engine->fromExternalPath(resolvedPath, HopStarIO::Access::Read);
    auto result = engine->read(desc);
    return result.success ? std::move(result.data) : std::vector<uint8_t>{};
#endif
}

// Read entire file with default Location::Resource (backwards compatible)
inline std::vector<uint8_t> readFile(const std::string& path) {
    return readFile(path, HopStarIO::Location::Resource);
}

// Read file with callback for streaming (with Location support)
inline bool readFileStream(const std::string& path,
                           std::function<void(const uint8_t*, size_t, bool)> callback,
                           HopStarIO::Location location = HopStarIO::Location::Resource) {
    // For simplicity, use readFile and callback once
    // A more sophisticated implementation could stream from FileEngine
    auto data = readFile(path, location);
    if (data.empty()) return false;
    callback(data.data(), data.size(), true);
    return true;
}

// ============================================================================
// Write file with Location support
// ============================================================================

inline bool writeFile(const std::string& path, const uint8_t* data, size_t length, HopStarIO::Location location) {
#ifdef __ANDROID__
    // Android: Use SDL for all file writing
    if (location == HopStarIO::Location::Internal) {
        const char* prefPath = SDL_GetPrefPath("org.libsdl.app", "HopStarBB");
        if (!prefPath) return false;
        std::string fullPath = std::string(prefPath) + path;
        SDL_IOStream* io = SDL_IOFromFile(fullPath.c_str(), "wb");
        if (!io) return false;
        size_t written = SDL_WriteIO(io, data, length);
        SDL_CloseIO(io);
        return written == length;
    }
    // Cannot write to other locations on Android
    SDL_Log("PlatformIO::writeFile: Cannot write to location type %d on Android", (int)location);
    return false;
#else
    auto* engine = getEngine();

    switch (location) {
    case HopStarIO::Location::Internal:
        {
            std::string internalPath = engine->getInternalPath(path);
            auto desc = engine->fromExternalPath(internalPath, HopStarIO::Access::Write);
            auto result = engine->write(desc, data, length);
            return result.success;
        }

    case HopStarIO::Location::External:
        // External filesystem - direct path
        {
            auto desc = engine->fromExternalPath(path, HopStarIO::Access::Write);
            auto result = engine->write(desc, data, length);
            return result.success;
        }

    case HopStarIO::Location::Resource:
    case HopStarIO::Location::Server:
    case HopStarIO::Location::Cloud:
    case HopStarIO::Location::P2P:
        // Cannot write to these locations directly
        SDL_Log("PlatformIO::writeFile: Cannot write to location type %d", (int)location);
        return false;

    default:
        return false;
    }
#endif
}

// Write file with default Location::Internal (backwards compatible)
inline bool writeFile(const std::string& path, const uint8_t* data, size_t length) {
    return writeFile(path, data, length, HopStarIO::Location::Internal);
}

// ============================================================================
// Android asset extraction (for FFmpeg which needs real file paths)
// ============================================================================

#ifdef __ANDROID__
inline std::string extractAssetToCache(const std::string& assetPath) {
    auto data = readFile(assetPath);
    if (data.empty()) return "";

    const char* prefPath = SDL_GetPrefPath("org.libsdl.app", "cache");
    if (!prefPath) return "";

    // Extract filename from path
    std::string filename = assetPath;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        filename = filename.substr(lastSlash + 1);
    }

    std::string outputPath = std::string(prefPath) + filename;

    SDL_IOStream* outIo = SDL_IOFromFile(outputPath.c_str(), "wb");
    if (!outIo) return "";

    size_t written = SDL_WriteIO(outIo, data.data(), data.size());
    SDL_CloseIO(outIo);

    if (written != data.size()) return "";

    return outputPath;
}
#endif

// Get real filesystem path for APIs that need actual file paths (FFmpeg, etc.)
// - Resource: resolves to bundle/asset path (extracts to cache on Android)
// - Internal: resolves to internal storage path
// - External: returns path as-is
// - Server/Cloud/P2P: downloads to cache and returns cache path
inline std::string getRealPath(const std::string& path, HopStarIO::Location location) {
#ifdef __ANDROID__
    // Android: Handle without FileEngine
    switch (location) {
    case HopStarIO::Location::Resource:
        return extractAssetToCache(path);
    case HopStarIO::Location::Internal:
    case HopStarIO::Location::External:
        return resolvePath(path, location);
    default:
        return path;
    }
#else
    auto* engine = getEngine();

    switch (location) {
    case HopStarIO::Location::Resource:
        return resolvePath(path, location);

    case HopStarIO::Location::Internal:
    case HopStarIO::Location::External:
        return resolvePath(path, location);

    case HopStarIO::Location::Server:
    case HopStarIO::Location::Cloud:
    case HopStarIO::Location::P2P:
        // Remote: download via FileEngine and cache locally
        {
            auto desc = engine->createDescriptor(path, location, HopStarIO::Access::Read, path);
            if (!engine->prefetch(desc)) {
                SDL_Log("getRealPath: Failed to prefetch remote: %s", path.c_str());
                return "";
            }
            // Read data and write to cache
            auto result = engine->read(desc);
            if (!result.success) {
                SDL_Log("getRealPath: Failed to read remote: %s", path.c_str());
                return "";
            }
            // Cache to internal storage
            std::string cachePath = engine->getInternalPath("cache/" + HopStarIO::hashPath(path));
            auto writeDesc = engine->fromExternalPath(cachePath, HopStarIO::Access::Write);
            engine->write(writeDesc, result.data.data(), result.data.size());
            return cachePath;
        }

    default:
        return path;
    }
#endif
}

// ============================================================================
// File existence check
// ============================================================================

inline bool fileExists(const std::string& path, HopStarIO::Location location) {
#ifdef __ANDROID__
    if (location == HopStarIO::Location::Resource) {
        SDL_IOStream* io = SDL_IOFromFile(path.c_str(), "rb");
        if (io) {
            SDL_CloseIO(io);
            return true;
        }
        return false;
    }
#endif
    std::string resolvedPath = resolvePath(path, location);
    return std::filesystem::exists(resolvedPath);
}

// ============================================================================
// Directory operations
// ============================================================================

inline bool createDirectory(const std::string& path, HopStarIO::Location location) {
#ifdef __ANDROID__
    if (location == HopStarIO::Location::Resource) {
        // Can't create directories in APK
        return false;
    }
#endif
    std::string resolvedPath = resolvePath(path, location);
    return std::filesystem::create_directories(resolvedPath);
}

inline std::vector<std::string> listDirectory(const std::string& dirPath,
                                               HopStarIO::Location location,
                                               const std::string& extension = "") {
    std::vector<std::string> files;

#ifdef __ANDROID__
    if (location == HopStarIO::Location::Resource) {
        // Android: Can't list asset directories easily
        return files;
    }
#endif
    std::string resolvedPath = resolvePath(dirPath, location);
    if (!std::filesystem::exists(resolvedPath)) return files;

    for (const auto& entry : std::filesystem::directory_iterator(resolvedPath)) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            if (extension.empty()) {
                files.push_back(filepath);
            } else {
                size_t dotPos = filepath.find_last_of(".");
                if (dotPos != std::string::npos &&
                    filepath.substr(dotPos + 1) == extension) {
                    files.push_back(filepath);
                }
            }
        }
    }
    return files;
}

// ============================================================================
// Internal storage path
// ============================================================================

inline std::string getInternalStoragePath(const std::string& relativePath = "") {
#ifdef __ANDROID__
    const char* prefPath = SDL_GetPrefPath("org.libsdl.app", "HopStarBB");
    if (!prefPath) return "";
    return std::string(prefPath) + relativePath;
#else
    auto* engine = getEngine();
    return engine->getInternalPath(relativePath);
#endif
}

} // namespace PlatformIO
