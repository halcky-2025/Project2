#pragma once
/**
 * HopStarBB / Fortrege Unified File I/O Engine
 *
 * Design principles:
 * - Local files are always Source of Truth
 * - Cloud is "remote I/O device accessed by address"
 * - No Dropbox-style sync or OS virtual drives
 * - No differential sync, no overwrites - always new generations
 *
 * Usage:
 *   auto engine = HopStarIO::FileEngineFactory::createDefault();
 *   auto desc = engine->fromExternalPath("C:/video.mp4");
 *   auto result = engine->read(desc);
 */

#include <string>
#include <cstdint>
#include <memory>
#include <functional>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <optional>
#include <list>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstring>
#include "sqlite3.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace HopStarIO {

namespace fs = std::filesystem;

// ============================================================================
// Min/Max helpers (avoid Windows.h macro conflicts)
// ============================================================================
template<typename T>
inline T hs_min(T a, T b) { return (a < b) ? a : b; }

template<typename T>
inline T hs_max(T a, T b) { return (a > b) ? a : b; }

// ============================================================================
// Location - Where the file entity physically resides
// ============================================================================
enum class Location {
    Resource,   // Embedded resource in app (read only)
    Internal,   // HopStarBB managed local storage
    External,   // Normal OS filesystem
    Server,     // HTTP / S3 central server
    Cloud,      // Authenticated cloud object (Google Drive, etc.)
    P2P         // WebRTC peer
};

// ============================================================================
// Access - Operation permissions
// ============================================================================
enum class Access {
    Read,       // Read only
    Write,      // Write only
    ReadWrite   // Both
};

// ============================================================================
// CacheState - Local cache status for remote files
// ============================================================================
enum class CacheState {
    NotPresent, // Not cached locally
    Partial,    // Partially downloaded (can still stream video)
    Complete    // Fully cached
};

// ============================================================================
// FileDescriptor - Canonical internal representation of any file
// ============================================================================
struct FileDescriptor {
    std::string path;           // Virtual path: "/movies/movie.mp4"
    Location location;          // Where the file lives
    Access access;              // Permissions
    std::string address;        // Remote address (for Server/Cloud/P2P)
    uint64_t size = 0;          // File size in bytes
    std::string hash;           // "sha256:..." content hash
    uint64_t generation = 1;    // Version number (immutable, incremented on change)
    CacheState cacheState = CacheState::NotPresent;
    bool pinned = false;        // If true, don't auto-evict from cache
    uint64_t cachedBytes = 0;   // How much is cached (for Partial state)
    std::string localCachePath; // Where cached data is stored
};

// ============================================================================
// ReadResult / WriteResult
// ============================================================================
struct ReadResult {
    bool success = false;
    std::vector<uint8_t> data;
    std::string error;
    uint64_t bytesRead = 0;
};

struct WriteResult {
    bool success = false;
    std::string error;
    std::string newAddress;
    uint64_t newGeneration = 0;
    std::string newHash;
};

// ============================================================================
// Callbacks
// ============================================================================
using StreamCallback = std::function<void(const uint8_t* data, size_t len, bool isLast)>;
using ProgressCallback = std::function<void(uint64_t bytesTransferred, uint64_t totalBytes)>;

// ============================================================================
// Utility Functions
// ============================================================================
inline const char* locationToString(Location loc) {
    switch (loc) {
        case Location::Resource: return "Resource";
        case Location::Internal: return "Internal";
        case Location::External: return "External";
        case Location::Server:   return "Server";
        case Location::Cloud:    return "Cloud";
        case Location::P2P:      return "P2P";
        default:                 return "Unknown";
    }
}

inline const char* accessToString(Access acc) {
    switch (acc) {
        case Access::Read:      return "read";
        case Access::Write:     return "write";
        case Access::ReadWrite: return "read/write";
        default:                return "unknown";
    }
}

inline std::string parseAddressScheme(const std::string& address) {
    size_t pos = address.find("://");
    if (pos == std::string::npos) return "";
    return address.substr(0, pos);
}

inline std::string computeHash(const uint8_t* data, size_t length) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < length; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    std::stringstream ss;
    ss << "fnv1a:" << std::hex << hash;
    return ss.str();
}

inline std::string hashPath(const std::string& address) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : address) {
        hash ^= static_cast<uint8_t>(c);
        hash *= 1099511628211ULL;
    }
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

inline std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d-%H%M%S");
    return ss.str();
}

// ============================================================================
// ICloudControlApp - Interface to external network handler
// ============================================================================
class ICloudControlApp {
public:
    virtual ~ICloudControlApp() = default;
    virtual ReadResult fetch(const std::string& address, uint64_t offset = 0, uint64_t length = UINT64_MAX) = 0;
    virtual bool streamFetch(const std::string& address, StreamCallback callback,
                             ProgressCallback progress = nullptr, uint64_t offset = 0, uint64_t length = UINT64_MAX) = 0;
    virtual WriteResult upload(const std::string& targetAddress, const uint8_t* data, size_t length,
                               ProgressCallback progress = nullptr) = 0;
    virtual std::optional<FileDescriptor> getRemoteMetadata(const std::string& address) = 0;
};

// ============================================================================
// IMetadataDB - Interface for file metadata storage
// ============================================================================
class IMetadataDB {
public:
    virtual ~IMetadataDB() = default;
    virtual bool put(const std::string& path, const FileDescriptor& desc) = 0;
    virtual std::optional<FileDescriptor> get(const std::string& path) = 0;
    virtual bool remove(const std::string& path) = 0;
    virtual std::vector<FileDescriptor> list(const std::string& prefix) = 0;
    virtual std::vector<FileDescriptor> listByLocation(Location loc) = 0;
};

// ============================================================================
// ICacheManager - Manages local cache for remote files
// ============================================================================
class ICacheManager {
public:
    virtual ~ICacheManager() = default;
    virtual std::string getCachePath(const FileDescriptor& desc) = 0;
    virtual bool writeToCache(const FileDescriptor& desc, const uint8_t* data, size_t length, uint64_t offset = 0) = 0;
    virtual ReadResult readFromCache(const FileDescriptor& desc, uint64_t offset = 0, uint64_t length = UINT64_MAX) = 0;
    virtual CacheState getCacheState(const FileDescriptor& desc) = 0;
    virtual void pin(const FileDescriptor& desc, bool pinned) = 0;
    virtual uint64_t evict(uint64_t bytesToFree) = 0;
    virtual uint64_t getCacheSize() = 0;
    virtual uint64_t getAvailableSpace() = 0;
};

// ============================================================================
// LocalCacheManager - LRU cache implementation
// ============================================================================
class LocalCacheManager : public ICacheManager {
public:
    explicit LocalCacheManager(const std::string& cacheDir, uint64_t maxCacheSize = 10ULL * 1024 * 1024 * 1024)
        : m_cacheDir(cacheDir), m_maxCacheSize(maxCacheSize), m_currentSize(0) {
        fs::create_directories(m_cacheDir);
        loadCacheIndex();
    }

    ~LocalCacheManager() override { saveCacheIndex(); }

    std::string getCachePath(const FileDescriptor& desc) override {
        std::string hash = hashPath(desc.address.empty() ? desc.path : desc.address);
        return m_cacheDir + "/" + hash.substr(0, 2) + "/" + hash;
    }

    bool writeToCache(const FileDescriptor& desc, const uint8_t* data, size_t length, uint64_t offset) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string cachePath = getCachePath(desc);
        std::string key = desc.address.empty() ? desc.path : desc.address;

        fs::path p(cachePath);
        fs::create_directories(p.parent_path());

        std::fstream file(cachePath, std::ios::binary | std::ios::in | std::ios::out);
        if (!file) {
            std::ofstream create(cachePath, std::ios::binary);
            create.close();
            file.open(cachePath, std::ios::binary | std::ios::in | std::ios::out);
        }
        if (!file) return false;

        file.seekp(0, std::ios::end);
        uint64_t currentFileSize = file.tellp();
        if (offset + length > currentFileSize) {
            std::vector<char> zeros(static_cast<size_t>(offset + length - currentFileSize), 0);
            file.write(zeros.data(), zeros.size());
        }

        file.seekp(offset);
        file.write(reinterpret_cast<const char*>(data), length);
        file.flush();

        auto& entry = m_entries[key];
        entry.cachePath = cachePath;
        entry.cachedBytes = hs_max(entry.cachedBytes, offset + length);
        entry.fileSize = desc.size;
        entry.complete = (entry.cachedBytes >= desc.size && desc.size > 0);

        uint64_t newSize = static_cast<uint64_t>(fs::file_size(cachePath));
        m_currentSize = m_currentSize - entry.diskSize + newSize;
        entry.diskSize = newSize;

        updateLRU(key);
        if (m_currentSize > m_maxCacheSize) evict(m_currentSize - m_maxCacheSize);
        return true;
    }

    ReadResult readFromCache(const FileDescriptor& desc, uint64_t offset, uint64_t length) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string cachePath = getCachePath(desc);
        std::string key = desc.address.empty() ? desc.path : desc.address;

        ReadResult result;
        std::ifstream file(cachePath, std::ios::binary);
        if (!file) { result.error = "Cache file not found"; return result; }

        file.seekg(0, std::ios::end);
        uint64_t fileSize = file.tellg();
        if (offset >= fileSize) { result.success = true; return result; }

        file.seekg(offset);
        uint64_t toRead = hs_min(length, fileSize - offset);
        result.data.resize(static_cast<size_t>(toRead));
        file.read(reinterpret_cast<char*>(result.data.data()), toRead);
        result.bytesRead = file.gcount();
        result.data.resize(static_cast<size_t>(result.bytesRead));
        result.success = true;
        updateLRU(key);
        return result;
    }

    CacheState getCacheState(const FileDescriptor& desc) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string key = desc.address.empty() ? desc.path : desc.address;
        auto it = m_entries.find(key);
        if (it == m_entries.end()) return CacheState::NotPresent;
        return it->second.complete ? CacheState::Complete : CacheState::Partial;
    }

    void pin(const FileDescriptor& desc, bool pinned) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string key = desc.address.empty() ? desc.path : desc.address;
        auto it = m_entries.find(key);
        if (it != m_entries.end()) it->second.pinned = pinned;
    }

    uint64_t evict(uint64_t bytesToFree) override {
        uint64_t freed = 0;
        while (freed < bytesToFree && !m_lruList.empty()) {
            const std::string& key = m_lruList.back();
            auto it = m_entries.find(key);
            if (it != m_entries.end() && !it->second.pinned) {
                fs::remove(it->second.cachePath);
                freed += it->second.diskSize;
                m_currentSize -= it->second.diskSize;
                m_lruMap.erase(key);
                m_entries.erase(it);
            }
            m_lruList.pop_back();
        }
        return freed;
    }

    uint64_t getCacheSize() override { std::lock_guard<std::mutex> lock(m_mutex); return m_currentSize; }
    uint64_t getAvailableSpace() override { std::lock_guard<std::mutex> lock(m_mutex); return m_maxCacheSize > m_currentSize ? m_maxCacheSize - m_currentSize : 0; }

private:
    struct CacheEntry {
        std::string cachePath;
        uint64_t diskSize = 0;
        uint64_t fileSize = 0;
        uint64_t cachedBytes = 0;
        bool pinned = false;
        bool complete = false;
    };

    void updateLRU(const std::string& key) {
        auto it = m_lruMap.find(key);
        if (it != m_lruMap.end()) m_lruList.erase(it->second);
        m_lruList.push_front(key);
        m_lruMap[key] = m_lruList.begin();
    }

    void loadCacheIndex() {
        std::string indexPath = m_cacheDir + "/cache_index.dat";
        std::ifstream file(indexPath, std::ios::binary);
        if (!file) return;

        uint32_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (uint32_t i = 0; i < count; ++i) {
            uint32_t keyLen = 0;
            file.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
            std::string key(keyLen, '\0');
            file.read(&key[0], keyLen);

            CacheEntry entry;
            uint32_t pathLen = 0;
            file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
            entry.cachePath.resize(pathLen);
            file.read(&entry.cachePath[0], pathLen);

            file.read(reinterpret_cast<char*>(&entry.diskSize), sizeof(entry.diskSize));
            file.read(reinterpret_cast<char*>(&entry.cachedBytes), sizeof(entry.cachedBytes));

            uint8_t flags = 0;
            file.read(reinterpret_cast<char*>(&flags), sizeof(flags));
            entry.pinned = (flags & 1) != 0;
            entry.complete = (flags & 2) != 0;

            if (fs::exists(entry.cachePath)) {
                m_entries[key] = entry;
                m_currentSize += entry.diskSize;
                m_lruList.push_back(key);
                m_lruMap[key] = std::prev(m_lruList.end());
            }
        }
    }

    void saveCacheIndex() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string indexPath = m_cacheDir + "/cache_index.dat";
        std::ofstream file(indexPath, std::ios::binary | std::ios::trunc);
        if (!file) return;

        uint32_t count = static_cast<uint32_t>(m_entries.size());
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& [key, entry] : m_entries) {
            uint32_t keyLen = static_cast<uint32_t>(key.size());
            file.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
            file.write(key.data(), keyLen);

            uint32_t pathLen = static_cast<uint32_t>(entry.cachePath.size());
            file.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
            file.write(entry.cachePath.data(), pathLen);

            file.write(reinterpret_cast<const char*>(&entry.diskSize), sizeof(entry.diskSize));
            file.write(reinterpret_cast<const char*>(&entry.cachedBytes), sizeof(entry.cachedBytes));

            uint8_t flags = (entry.pinned ? 1 : 0) | (entry.complete ? 2 : 0);
            file.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
        }
    }

    std::string m_cacheDir;
    uint64_t m_maxCacheSize;
    uint64_t m_currentSize;
    std::mutex m_mutex;
    std::unordered_map<std::string, CacheEntry> m_entries;
    std::list<std::string> m_lruList;
    std::unordered_map<std::string, std::list<std::string>::iterator> m_lruMap;
};

// ============================================================================
// SqliteMetadataDB - SQLite-based metadata storage
// ============================================================================
class SqliteMetadataDB : public IMetadataDB {
public:
    explicit SqliteMetadataDB(const std::string& dbPath) : m_db(nullptr) {
        fs::path p(dbPath);
        if (p.has_parent_path()) fs::create_directories(p.parent_path());
        if (sqlite3_open(dbPath.c_str(), &m_db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open metadata database");
        }
        initSchema();
    }

    ~SqliteMetadataDB() override { if (m_db) sqlite3_close(m_db); }

    bool put(const std::string& path, const FileDescriptor& desc) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        const char* sql = R"(INSERT OR REPLACE INTO files
            (path, location, access, address, size, hash, generation, cache_state, pinned, cached_bytes, local_cache_path, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, strftime('%s', 'now')))";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

        sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, static_cast<int>(desc.location));
        sqlite3_bind_int(stmt, 3, static_cast<int>(desc.access));
        sqlite3_bind_text(stmt, 4, desc.address.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 5, desc.size);
        sqlite3_bind_text(stmt, 6, desc.hash.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 7, desc.generation);
        sqlite3_bind_int(stmt, 8, static_cast<int>(desc.cacheState));
        sqlite3_bind_int(stmt, 9, desc.pinned ? 1 : 0);
        sqlite3_bind_int64(stmt, 10, desc.cachedBytes);
        sqlite3_bind_text(stmt, 11, desc.localCachePath.c_str(), -1, SQLITE_TRANSIENT);

        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }

    std::optional<FileDescriptor> get(const std::string& path) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        const char* sql = "SELECT * FROM files WHERE path = ?";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
        sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_ROW) { sqlite3_finalize(stmt); return std::nullopt; }

        FileDescriptor desc = rowToDescriptor(stmt);
        sqlite3_finalize(stmt);
        return desc;
    }

    bool remove(const std::string& path) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        const char* sql = "DELETE FROM files WHERE path = ?";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);

        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }

    std::vector<FileDescriptor> list(const std::string& prefix) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<FileDescriptor> results;
        const char* sql = "SELECT * FROM files WHERE path LIKE ? || '%'";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
        sqlite3_bind_text(stmt, 1, prefix.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results.push_back(rowToDescriptor(stmt));
        }
        sqlite3_finalize(stmt);
        return results;
    }

    std::vector<FileDescriptor> listByLocation(Location loc) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<FileDescriptor> results;
        const char* sql = "SELECT * FROM files WHERE location = ?";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
        sqlite3_bind_int(stmt, 1, static_cast<int>(loc));

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results.push_back(rowToDescriptor(stmt));
        }
        sqlite3_finalize(stmt);
        return results;
    }

private:
    void initSchema() {
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS files (
                path TEXT PRIMARY KEY, location INTEGER NOT NULL, access INTEGER NOT NULL,
                address TEXT, size INTEGER DEFAULT 0, hash TEXT, generation INTEGER DEFAULT 1,
                cache_state INTEGER DEFAULT 0, pinned INTEGER DEFAULT 0, cached_bytes INTEGER DEFAULT 0,
                local_cache_path TEXT, created_at INTEGER DEFAULT (strftime('%s', 'now')),
                updated_at INTEGER DEFAULT (strftime('%s', 'now')));
            CREATE INDEX IF NOT EXISTS idx_files_location ON files(location);
            CREATE INDEX IF NOT EXISTS idx_files_address ON files(address);
        )";
        sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr);
    }

    FileDescriptor rowToDescriptor(sqlite3_stmt* stmt) {
        FileDescriptor desc;
        desc.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        desc.location = static_cast<Location>(sqlite3_column_int(stmt, 1));
        desc.access = static_cast<Access>(sqlite3_column_int(stmt, 2));
        const char* addr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        desc.address = addr ? addr : "";
        desc.size = sqlite3_column_int64(stmt, 4);
        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        desc.hash = hash ? hash : "";
        desc.generation = sqlite3_column_int64(stmt, 6);
        desc.cacheState = static_cast<CacheState>(sqlite3_column_int(stmt, 7));
        desc.pinned = sqlite3_column_int(stmt, 8) != 0;
        desc.cachedBytes = sqlite3_column_int64(stmt, 9);
        const char* cachePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        desc.localCachePath = cachePath ? cachePath : "";
        return desc;
    }

    sqlite3* m_db;
    std::mutex m_mutex;
};

// ============================================================================
// HttpCloudControlApp - HTTP/HTTPS cloud access (Windows WinHTTP)
// ============================================================================
class HttpCloudControlApp : public ICloudControlApp {
public:
    HttpCloudControlApp() : m_timeoutMs(30000), m_maxRetries(3) {
#ifdef _WIN32
        m_hSession = WinHttpOpen(L"HopStarBB/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                 WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
#endif
    }

    ~HttpCloudControlApp() override {
#ifdef _WIN32
        if (m_hSession) WinHttpCloseHandle(m_hSession);
#endif
    }

    void setAuthToken(const std::string& token) { m_authToken = token; }
    void setTimeout(uint32_t timeoutMs) { m_timeoutMs = timeoutMs; }

    ReadResult fetch(const std::string& address, uint64_t offset, uint64_t length) override {
        ReadResult result;
#ifdef _WIN32
        if (!m_hSession) { result.error = "HTTP session not initialized"; return result; }

        URL_COMPONENTS urlComp = { 0 };
        urlComp.dwStructSize = sizeof(urlComp);
        wchar_t hostName[256] = { 0 }, urlPath[2048] = { 0 };
        urlComp.lpszHostName = hostName; urlComp.dwHostNameLength = 256;
        urlComp.lpszUrlPath = urlPath; urlComp.dwUrlPathLength = 2048;

        std::wstring wAddress(address.begin(), address.end());
        if (!WinHttpCrackUrl(wAddress.c_str(), 0, 0, &urlComp)) { result.error = "Failed to parse URL"; return result; }

        HINTERNET hConnect = WinHttpConnect(m_hSession, hostName, urlComp.nPort, 0);
        if (!hConnect) { result.error = "Failed to connect"; return result; }

        DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!hRequest) { WinHttpCloseHandle(hConnect); result.error = "Failed to create request"; return result; }

        if (offset > 0 || length != UINT64_MAX) {
            std::wstringstream rangeHeader;
            rangeHeader << L"Range: bytes=" << offset << L"-";
            if (length != UINT64_MAX) rangeHeader << (offset + length - 1);
            WinHttpAddRequestHeaders(hRequest, rangeHeader.str().c_str(), (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD);
        }

        if (!m_authToken.empty()) {
            std::wstring authHeader = L"Authorization: Bearer " + std::wstring(m_authToken.begin(), m_authToken.end());
            WinHttpAddRequestHeaders(hRequest, authHeader.c_str(), (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD);
        }

        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
            !WinHttpReceiveResponse(hRequest, nullptr)) {
            WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect);
            result.error = "Failed to send/receive request"; return result;
        }

        DWORD bytesAvailable;
        while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
            std::vector<uint8_t> buffer(bytesAvailable);
            DWORD bytesRead;
            if (WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead)) {
                result.data.insert(result.data.end(), buffer.begin(), buffer.begin() + bytesRead);
                result.bytesRead += bytesRead;
            }
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        result.success = true;
#else
        result.error = "HTTP not implemented for this platform";
#endif
        return result;
    }

    bool streamFetch(const std::string& address, StreamCallback callback, ProgressCallback progress,
                     uint64_t offset, uint64_t length) override {
        ReadResult result = fetch(address, offset, length);
        if (!result.success) return false;

        const size_t chunkSize = 64 * 1024;
        size_t delivered = 0;
        while (delivered < result.data.size()) {
            size_t thisChunk = hs_min(chunkSize, result.data.size() - delivered);
            bool isLast = (delivered + thisChunk >= result.data.size());
            callback(result.data.data() + delivered, thisChunk, isLast);
            delivered += thisChunk;
            if (progress) progress(delivered, result.data.size());
        }
        return true;
    }

    WriteResult upload(const std::string& targetAddress, const uint8_t* data, size_t length,
                       ProgressCallback progress) override {
        WriteResult result;
        result.error = "Upload not yet implemented";
        return result;
    }

    std::optional<FileDescriptor> getRemoteMetadata(const std::string& address) override { return std::nullopt; }

private:
    std::string m_authToken;
    uint32_t m_timeoutMs;
    uint32_t m_maxRetries;
#ifdef _WIN32
    HINTERNET m_hSession = nullptr;
#endif
};

// ============================================================================
// CompositeCloudControlApp - Routes to appropriate handler based on address
// ============================================================================
class CompositeCloudControlApp : public ICloudControlApp {
public:
    void registerHandler(const std::string& scheme, std::shared_ptr<ICloudControlApp> handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_handlers[scheme] = std::move(handler);
    }

    ReadResult fetch(const std::string& address, uint64_t offset, uint64_t length) override {
        if (auto* h = getHandler(address)) return h->fetch(address, offset, length);
        return { false, {}, "No handler for address scheme", 0 };
    }

    bool streamFetch(const std::string& address, StreamCallback callback, ProgressCallback progress,
                     uint64_t offset, uint64_t length) override {
        if (auto* h = getHandler(address)) return h->streamFetch(address, callback, progress, offset, length);
        return false;
    }

    WriteResult upload(const std::string& targetAddress, const uint8_t* data, size_t length,
                       ProgressCallback progress) override {
        if (auto* h = getHandler(targetAddress)) return h->upload(targetAddress, data, length, progress);
        return { false, "No handler for address scheme", "", 0, "" };
    }

    std::optional<FileDescriptor> getRemoteMetadata(const std::string& address) override {
        if (auto* h = getHandler(address)) return h->getRemoteMetadata(address);
        return std::nullopt;
    }

private:
    ICloudControlApp* getHandler(const std::string& address) {
        std::string scheme = parseAddressScheme(address);
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.find(scheme);
        if (it != m_handlers.end()) return it->second.get();
        if (scheme == "http" || scheme == "https") {
            it = m_handlers.find("http");
            if (it != m_handlers.end()) return it->second.get();
        }
        return nullptr;
    }

    std::unordered_map<std::string, std::shared_ptr<ICloudControlApp>> m_handlers;
    std::mutex m_mutex;
};

// ============================================================================
// FileEngine - Main unified file I/O engine
// ============================================================================
class FileEngine {
public:
    FileEngine(std::shared_ptr<ICloudControlApp> cloudApp,
               std::shared_ptr<IMetadataDB> metaDB,
               std::shared_ptr<ICacheManager> cache)
        : m_cloudApp(std::move(cloudApp)), m_metaDB(std::move(metaDB)), m_cache(std::move(cache)) {
        initPaths();
    }

    // ========================================================================
    // Core Read
    // ========================================================================
    ReadResult read(const FileDescriptor& desc, uint64_t offset = 0, uint64_t length = UINT64_MAX) {
        switch (desc.location) {
            case Location::Resource: return readLocal(getResourcePath(desc.path), offset, length);
            case Location::Internal: return readLocal(getInternalPath(desc.path), offset, length);
            case Location::External: return readLocal(desc.path, offset, length);
            case Location::Server:
            case Location::Cloud:
            case Location::P2P: {
                if (m_cache) {
                    CacheState state = m_cache->getCacheState(desc);
                    if (state == CacheState::Complete) return m_cache->readFromCache(desc, offset, length);
                    if (state == CacheState::Partial && offset + length <= desc.cachedBytes)
                        return m_cache->readFromCache(desc, offset, length);
                }
                if (!m_cloudApp) return { false, {}, "No CloudControlApp configured", 0 };
                auto result = m_cloudApp->fetch(desc.address, offset, length);
                if (result.success && m_cache)
                    m_cache->writeToCache(desc, result.data.data(), result.data.size(), offset);
                return result;
            }
            default: return { false, {}, "Unknown location type", 0 };
        }
    }

    // ========================================================================
    // Stream Read
    // ========================================================================
    bool streamRead(const FileDescriptor& desc, StreamCallback callback, ProgressCallback progress = nullptr,
                    uint64_t offset = 0, uint64_t length = UINT64_MAX) {
        if (!callback) return false;

        if (desc.location == Location::Resource || desc.location == Location::Internal ||
            desc.location == Location::External) {
            std::string osPath = (desc.location == Location::Resource) ? getResourcePath(desc.path) :
                                 (desc.location == Location::Internal) ? getInternalPath(desc.path) : desc.path;

            std::ifstream file(osPath, std::ios::binary);
            if (!file) return false;

            file.seekg(0, std::ios::end);
            uint64_t fileSize = file.tellg();
            if (offset >= fileSize) { callback(nullptr, 0, true); return true; }

            file.seekg(offset);
            uint64_t remaining = hs_min(length, fileSize - offset);
            uint64_t totalRead = 0;
            const size_t chunkSize = 64 * 1024;
            std::vector<uint8_t> buffer(chunkSize);

            while (remaining > 0) {
                size_t toRead = static_cast<size_t>(hs_min(static_cast<uint64_t>(chunkSize), remaining));
                file.read(reinterpret_cast<char*>(buffer.data()), toRead);
                size_t bytesRead = file.gcount();
                if (bytesRead == 0) break;

                remaining -= bytesRead;
                totalRead += bytesRead;
                bool isLast = (remaining == 0) || file.eof();
                callback(buffer.data(), bytesRead, isLast);
                if (progress) progress(totalRead, length);
                if (isLast) break;
            }
            return true;
        }

        if (!m_cloudApp) return false;

        uint64_t currentOffset = offset;
        auto cachingCallback = [this, &desc, &callback, &currentOffset](const uint8_t* data, size_t len, bool isLast) {
            if (m_cache && data && len > 0) {
                m_cache->writeToCache(desc, data, len, currentOffset);
                currentOffset += len;
            }
            callback(data, len, isLast);
        };

        return m_cloudApp->streamFetch(desc.address, cachingCallback, progress, offset, length);
    }

    // ========================================================================
    // Core Write
    // ========================================================================
    WriteResult write(const FileDescriptor& desc, const uint8_t* data, size_t length, bool uploadToRemote = false) {
        WriteResult result;
        result.newGeneration = desc.generation + 1;
        result.newHash = computeHash(data, length);

        switch (desc.location) {
            case Location::Resource:
                result.error = "Cannot write to Resource files (read-only)";
                return result;
            case Location::Internal:
                result = writeLocal(getInternalPath(desc.path), data, length);
                if (result.success && uploadToRemote && m_cloudApp) {
                    auto uploadResult = m_cloudApp->upload(desc.address, data, length, nullptr);
                    result.newAddress = uploadResult.newAddress;
                }
                result.newGeneration = desc.generation + 1;
                return result;
            case Location::External:
                result = writeLocal(desc.path, data, length);
                if (result.success && uploadToRemote && m_cloudApp) {
                    auto uploadResult = m_cloudApp->upload(desc.address, data, length, nullptr);
                    result.newAddress = uploadResult.newAddress;
                }
                result.newGeneration = desc.generation + 1;
                return result;
            case Location::Server:
            case Location::Cloud:
            case Location::P2P:
                if (!m_cloudApp) { result.error = "No CloudControlApp configured"; return result; }
                return m_cloudApp->upload(desc.address, data, length, nullptr);
            default:
                result.error = "Unknown location type";
                return result;
        }
    }

    // ========================================================================
    // Descriptor Creation
    // ========================================================================
    FileDescriptor createDescriptor(const std::string& path, Location location, Access access,
                                    const std::string& address = "") {
        FileDescriptor desc;
        desc.path = path;
        desc.location = location;
        desc.access = access;
        desc.address = address;

        std::string osPath;
        if (location == Location::Resource) osPath = getResourcePath(path);
        else if (location == Location::Internal) osPath = getInternalPath(path);
        else if (location == Location::External) osPath = path;

        if (!osPath.empty() && fs::exists(osPath)) {
            desc.size = fs::file_size(osPath);
            desc.cacheState = CacheState::Complete;
        }
        return desc;
    }

    FileDescriptor fromExternalPath(const std::string& osPath, Access access = Access::ReadWrite) {
        FileDescriptor desc;
        desc.path = osPath;
        desc.location = Location::External;
        desc.access = access;
        desc.cacheState = CacheState::Complete;
        if (fs::exists(osPath)) desc.size = fs::file_size(osPath);
        return desc;
    }

    // ========================================================================
    // Conflict Resolution
    // ========================================================================
    std::string resolveConflict(const std::string& originalPath, const std::string& deviceId) {
        fs::path p(originalPath);
        std::string stem = p.stem().string();
        std::string ext = p.extension().string();
        std::string dir = p.parent_path().string();
        std::string conflictName = stem + " (conflict-" + deviceId + "-" + getCurrentTimestamp() + ")" + ext;
        return dir.empty() ? conflictName : dir + "/" + conflictName;
    }

    // ========================================================================
    // Path Helpers
    // ========================================================================
    std::string getResourcePath(const std::string& resourceName) { return m_resourcePath + "/" + resourceName; }
    std::string getInternalPath(const std::string& relativePath) { return m_internalStoragePath + "/" + relativePath; }

    // ========================================================================
    // Cache Control
    // ========================================================================
    void pinFile(const FileDescriptor& desc, bool pinned) { if (m_cache) m_cache->pin(desc, pinned); }

    CacheState getCacheState(const FileDescriptor& desc) {
        if (desc.location == Location::Resource || desc.location == Location::Internal ||
            desc.location == Location::External) return CacheState::Complete;
        return m_cache ? m_cache->getCacheState(desc) : CacheState::NotPresent;
    }

    bool prefetch(const FileDescriptor& desc, ProgressCallback progress = nullptr) {
        if (desc.location == Location::Resource || desc.location == Location::Internal ||
            desc.location == Location::External) return true;
        if (!m_cloudApp || !m_cache) return false;
        auto cacheCallback = [](const uint8_t*, size_t, bool) {};
        return streamRead(desc, cacheCallback, progress);
    }

    // ========================================================================
    // Metadata Operations
    // ========================================================================
    bool registerFile(const FileDescriptor& desc) { return m_metaDB ? m_metaDB->put(desc.path, desc) : false; }
    std::optional<FileDescriptor> lookupFile(const std::string& path) { return m_metaDB ? m_metaDB->get(path) : std::nullopt; }
    std::vector<FileDescriptor> listFiles(const std::string& prefix) { return m_metaDB ? m_metaDB->list(prefix) : std::vector<FileDescriptor>{}; }

private:
    void initPaths() {
#ifdef _WIN32
        wchar_t appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath))) {
            char utf8Path[MAX_PATH * 3];
            WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, utf8Path, sizeof(utf8Path), NULL, NULL);
            m_internalStoragePath = std::string(utf8Path) + "\\HopStarBB\\Storage";
        } else {
            m_internalStoragePath = ".\\HopStarBB\\Storage";
        }

        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        char utf8ExePath[MAX_PATH * 3];
        WideCharToMultiByte(CP_UTF8, 0, exePath, -1, utf8ExePath, sizeof(utf8ExePath), NULL, NULL);
        m_resourcePath = fs::path(utf8ExePath).parent_path().string() + "\\resources";

        char computerName[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(computerName);
        m_deviceId = GetComputerNameA(computerName, &size) ? computerName : "unknown-device";
#else
        const char* home = getenv("HOME");
        m_internalStoragePath = home ? std::string(home) + "/.hopstarbb/storage" : "./.hopstarbb/storage";

        char exePath[1024];
        ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
        if (len != -1) { exePath[len] = '\0'; m_resourcePath = fs::path(exePath).parent_path().string() + "/resources"; }
        else { m_resourcePath = "./resources"; }

        char hostname[256];
        m_deviceId = (gethostname(hostname, sizeof(hostname)) == 0) ? hostname : "unknown-device";
#endif
        fs::create_directories(m_internalStoragePath);
    }

    ReadResult readLocal(const std::string& osPath, uint64_t offset, uint64_t length) {
        ReadResult result;
        std::ifstream file(osPath, std::ios::binary);
        if (!file) { result.error = "Failed to open file: " + osPath; return result; }

        file.seekg(0, std::ios::end);
        uint64_t fileSize = file.tellg();
        if (offset >= fileSize) { result.success = true; return result; }

        file.seekg(offset);
        uint64_t remaining = hs_min(length, fileSize - offset);
        result.data.resize(static_cast<size_t>(remaining));
        file.read(reinterpret_cast<char*>(result.data.data()), remaining);
        result.bytesRead = file.gcount();
        result.data.resize(static_cast<size_t>(result.bytesRead));
        result.success = true;
        return result;
    }

    WriteResult writeLocal(const std::string& osPath, const uint8_t* data, size_t length) {
        WriteResult result;
        fs::path p(osPath);
        if (p.has_parent_path()) fs::create_directories(p.parent_path());

        std::ofstream file(osPath, std::ios::binary | std::ios::trunc);
        if (!file) { result.error = "Failed to open file for writing: " + osPath; return result; }

        file.write(reinterpret_cast<const char*>(data), length);
        if (!file) { result.error = "Failed to write to file: " + osPath; return result; }

        result.success = true;
        result.newHash = computeHash(data, length);
        return result;
    }

    std::shared_ptr<ICloudControlApp> m_cloudApp;
    std::shared_ptr<IMetadataDB> m_metaDB;
    std::shared_ptr<ICacheManager> m_cache;
    std::string m_internalStoragePath;
    std::string m_resourcePath;
    std::string m_deviceId;
};

// ============================================================================
// FileEngineFactory - Easy creation of FileEngine
// ============================================================================
class FileEngineFactory {
public:
    struct Config {
        std::string cacheDir;
        std::string metadataDbPath;
        uint64_t maxCacheSize = 10ULL * 1024 * 1024 * 1024;
        bool enableHttp = true;
    };

    static std::unique_ptr<FileEngine> create(const Config& config) {
        auto cache = std::make_shared<LocalCacheManager>(config.cacheDir, config.maxCacheSize);
        auto metaDB = std::make_shared<SqliteMetadataDB>(config.metadataDbPath);
        auto cloudApp = std::make_shared<CompositeCloudControlApp>();

        if (config.enableHttp) {
            auto httpHandler = std::make_shared<HttpCloudControlApp>();
            cloudApp->registerHandler("http", httpHandler);
            cloudApp->registerHandler("https", httpHandler);
        }

        return std::make_unique<FileEngine>(cloudApp, metaDB, cache);
    }

    static std::unique_ptr<FileEngine> createDefault() {
        Config config;
#ifdef _WIN32
        wchar_t appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath))) {
            char utf8Path[MAX_PATH * 3];
            WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, utf8Path, sizeof(utf8Path), NULL, NULL);
            config.cacheDir = std::string(utf8Path) + "\\HopStarBB\\Cache";
            config.metadataDbPath = std::string(utf8Path) + "\\HopStarBB\\metadata.db";
        } else {
            config.cacheDir = ".\\HopStarBB\\Cache";
            config.metadataDbPath = ".\\HopStarBB\\metadata.db";
        }
#else
        const char* home = getenv("HOME");
        if (home) {
            config.cacheDir = std::string(home) + "/.hopstarbb/cache";
            config.metadataDbPath = std::string(home) + "/.hopstarbb/metadata.db";
        } else {
            config.cacheDir = "./.hopstarbb/cache";
            config.metadataDbPath = "./.hopstarbb/metadata.db";
        }
#endif
        return create(config);
    }
};

} // namespace HopStarIOIO
