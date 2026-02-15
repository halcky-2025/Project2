// =============================================================================
// HopStar Linux Server (main7.cpp)
// Full-featured server with WebRTC, HTTP, WebSocket support
// =============================================================================

#define _GNU_SOURCE
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <linux/limits.h>
#include <libgen.h>
#include <sys/stat.h>
#include <pwd.h>

// Display detection function (before including GUI headers)
inline bool hasDisplay() {
    const char* display = getenv("DISPLAY");
    const char* wayland = getenv("WAYLAND_DISPLAY");

    if (!display && !wayland) {
        return false;
    }

    if (display && display[0]) {
        void* libX11 = dlopen("libX11.so.6", RTLD_LAZY | RTLD_LOCAL);
        if (libX11) {
            typedef void* (*XOpenDisplay_t)(const char*);
            typedef int (*XCloseDisplay_t)(void*);

            auto pXOpenDisplay = (XOpenDisplay_t)dlsym(libX11, "XOpenDisplay");
            auto pXCloseDisplay = (XCloseDisplay_t)dlsym(libX11, "XCloseDisplay");

            if (pXOpenDisplay && pXCloseDisplay) {
                void* dpy = pXOpenDisplay(display);
                if (dpy) {
                    pXCloseDisplay(dpy);
                    dlclose(libX11);
                    return true;
                }
            }
            dlclose(libX11);
        }
    }

    if (wayland && wayland[0]) {
        return true;
    }

    return false;
}

static const bool g_hasDisplay = hasDisplay();

// SDL3 (only if display available)
#if defined(HOPSTAR_ENABLE_GUI)
    #define SDL_MAIN_HANDLED
    #include <SDL3/SDL.h>
    #include <SDL3/SDL_main.h>
    #include <bx/math.h>
    #include <bgfx/bgfx.h>
    #include <bgfx/platform.h>
#endif

// Optional dependencies
#ifdef HOPSTAR_ENABLE_TORCH
#include <torch/torch.h>
#undef max
#undef min
#endif

#ifdef HOPSTAR_ENABLE_ONNX
#include <onnxruntime_c_api.h>
#endif

#ifdef HOPSTAR_ENABLE_FFMPEG
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libswresample/swresample.h>
}
#endif

// Platform helper
#ifndef BUNDLE_PATH_DEFINED
#define BUNDLE_PATH_DEFINED

inline std::string getExecutableDir() {
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        char* dir = dirname(path);
        return std::string(dir);
    }
    return ".";
}

inline std::string getBundlePath(const char* filename) {
    std::string execDir = getExecutableDir();
    std::string path1 = execDir + "/" + filename;
    if (access(path1.c_str(), F_OK) == 0) return path1;
    std::string path2 = execDir + "/Resources/" + filename;
    if (access(path2.c_str(), F_OK) == 0) return path2;
    std::string path3 = execDir + "/../share/hopstar/" + filename;
    if (access(path3.c_str(), F_OK) == 0) return path3;
    if (access(filename, F_OK) == 0) return std::string(filename);
    return std::string(filename);
}
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "file_engine.h"

#ifdef HOPSTAR_ENABLE_TORCH
namespace F = torch::nn::functional;
#endif

#include "gc.h"
#include "arr.h"
#include "sqlite3.h"
#include "db2.h"
#include "elem.h"

#if defined(HOPSTAR_ENABLE_GUI)
#include "shader.h"
#include "shader2.h"
#include "thumnailAtlas.h"
#include "imas.h"
#include "audio.h"
#include "ugui.h"
#endif

#ifdef HOPSTAR_ENABLE_TORCH
bool cuda = false;
void* torch_cuda_dll = nullptr;
#endif

// HopStar Server headers
#include "server_common.h"
#include "thread_pool.h"
#include "file_io_async.h"
#include "http_client.h"
#include "http_client_curl.h"  // HTTPS support with libcurl
#include "websocket_client.h"
#include "command_queue.h"
#include "http_server.h"
#include "websocket_server.h"
#include "webrtc_manager.h"
#include "media_processor.h"

using namespace HopStarServer;

// ============================================================================
// Global FileEngine instance
// ============================================================================
static std::unique_ptr<HopStarIO::FileEngine> g_fileEngine;

HopStarIO::FileEngine* getFileEngine() {
    if (!g_fileEngine) {
        g_fileEngine = HopStarIO::FileEngineFactory::createDefault();
    }
    return g_fileEngine.get();
}

// ============================================================================
// FileEngine helpers
// ============================================================================
std::vector<uint8_t> readFileToMemory(const std::string& path) {
    auto* engine = getFileEngine();
    auto desc = engine->fromExternalPath(path, HopStarIO::Access::Read);
    auto result = engine->read(desc);
    if (result.success) {
        return std::move(result.data);
    }
    return {};
}

std::vector<uint8_t> readFileFromAddress(const std::string& address) {
    auto* engine = getFileEngine();
    std::string scheme = HopStarIO::parseAddressScheme(address);
    if (scheme.empty()) {
        return readFileToMemory(address);
    }
    auto desc = engine->createDescriptor(address, HopStarIO::Location::Server,
                                         HopStarIO::Access::Read, address);
    auto result = engine->read(desc);
    if (result.success) {
        return std::move(result.data);
    }
    return {};
}

bool writeFileFromMemory(const std::string& path, const uint8_t* data, size_t length) {
    auto* engine = getFileEngine();
    auto desc = engine->fromExternalPath(path, HopStarIO::Access::Write);
    auto result = engine->write(desc, data, length);
    return result.success;
}

unsigned char* loadImageViaFileEngine(const std::string& path, int* width, int* height, int* channels, int desired_channels) {
    auto data = readFileFromAddress(path);
    if (data.empty()) {
        return nullptr;
    }
    return stbi_load_from_memory(data.data(), static_cast<int>(data.size()),
                                  width, height, channels, desired_channels);
}

bool fileExists(const std::string& path, HopStarIO::Location location) {
    return PlatformIO::fileExists(path, location);
}

std::vector<std::string> listDirectory(const std::string& dirPath, HopStarIO::Location location, const std::string& extension = "") {
    return PlatformIO::listDirectory(dirPath, location, extension);
}

bool createDirectory(const std::string& path, HopStarIO::Location location) {
    return PlatformIO::createDirectory(path, location);
}

// ============================================================================
// Linux Platform Bridge
// ============================================================================
static std::string g_documentsDirectory;

extern "C" {
    void LinuxBridge_initialize(void) {
        const char* home = getenv("HOME");
        if (!home) {
            struct passwd* pw = getpwuid(getuid());
            if (pw) home = pw->pw_dir;
        }
        if (home) {
            const char* xdgData = getenv("XDG_DATA_HOME");
            if (xdgData && xdgData[0]) {
                g_documentsDirectory = std::string(xdgData) + "/hopstar";
            } else {
                g_documentsDirectory = std::string(home) + "/.local/share/hopstar";
            }
            mkdir(g_documentsDirectory.c_str(), 0755);
        } else {
            g_documentsDirectory = ".";
        }
    }
    void LinuxBridge_shutdown(void) {}
    const char* LinuxBridge_getDocumentsDirectory(void) {
        return g_documentsDirectory.c_str();
    }
    void AppleBridge_initialize(void) { LinuxBridge_initialize(); }
    void AppleBridge_shutdown(void) { LinuxBridge_shutdown(); }
    const char* AppleBridge_getDocumentsDirectory(void) { return LinuxBridge_getDocumentsDirectory(); }
}

// ============================================================================
// GUI Thread helpers
// ============================================================================
#if defined(HOPSTAR_ENABLE_GUI)
uint64_t now_us_Linux() {
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

void GoThread_Linux(ThreadGC* thgc) {
    GC_register_class(thgc, _String, "Str", sizeof(String), NULL, NULL);
    GC_register_class(thgc, _List, "List", sizeof(List), ListCheck, NULL);
    GC_register_class(thgc, _Element, "Elem", sizeof(NewElement), NULL, NULL);
    GC_register_class(thgc, _FRect, "FRect", sizeof(SDL_FRect), NULL, NULL);
    GC_register_class(thgc, _LetterC, "Letter", sizeof(NewLetter), NULL, NULL);
    GC_register_class(thgc, _LineC, "Line", sizeof(NewLine), NULL, NULL);
    GC_register_class(thgc, _LocalC, "Local", sizeof(NewLocal), NULL, NULL);
    GC_register_class(thgc, _KV, "KeyValue", sizeof(KV), KVCheck, NULL);
    GC_register_class(thgc, _MapData, "MapData", sizeof(MapData), MapDataCheck, NULL);
    GC_register_class(thgc, _Map, "Map", sizeof(Map), MapCheck, NULL);
    GC_register_class(thgc, _EndC, "End", sizeof(NewEndElement), NULL, NULL);
    GC_register_class(thgc, _ColumnMeta, "ColumnMeta", sizeof(ColumnMeta), NULL, NULL);
    GC_register_class(thgc, _Table, "Table", sizeof(Table), NULL, NULL);
    GC_register_class(thgc, _Column, "Column", sizeof(Column), NULL, NULL);
    GC_register_class(thgc, _TreeElement, "TreeElement", sizeof(TreeElement), NULL, NULL);
    GC_register_class(thgc, _MemTable, "MemTable", sizeof(MemTable), NULL, NULL);
    GC_register_class(thgc, _MemFunc, "MemFunc", sizeof(MemFunc), NULL, NULL);
    GC_register_class(thgc, _FuncType, "FuncType", sizeof(FuncType), NULL, NULL);
    GC_register_class(thgc, _Offscreen, "Offscreen", sizeof(Offscreen), NULL, NULL);

    NewLocal* local = (NewLocal*)GC_alloc(thgc, _LocalC);
    initNewLocal(thgc, local);
    NewLetter* let = (NewLetter*)GC_alloc(thgc, _LetterC);
    initNewLetter(thgc, let, getFont("sans", 16), _Letter);
    let->text = createString(thgc, (char*)"HopStar WebRTC Server Running", 30, 1);
    let->color = 0xFFFFFFFF;
    NewElementAddLast(thgc, local, (NewElement*)local, (NewElement*)let);
    thgc->map = create_mapy(magc, _Struct);
    String* str = createString(thgc, (char*)"main", 4, 1);
    TreeElement* te = (TreeElement*)GC_alloc(thgc, _TreeElement);
    te->id = str;
    te->elem = (NewElement*)local;
    te->children = create_list(thgc, sizeof(TreeElement*), _List);
    add_mapy(thgc, thgc->map, str, (char*)te);

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        thgc->hoppy->buildFrame(now_us_Linux());
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(end - start).count();
        if (12.0 - ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(12.0 - ms)));
    }
}

void runGoThreadAsync_Linux(ThreadGC* thgc) {
    std::thread([thgc]() {
        initDone.get_future().wait();
        GoThread_Linux(thgc);
    }).detach();
}
#endif

// ============================================================================
// Signal Handler
// ============================================================================
static volatile bool g_running = true;

void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down..." << std::endl;
    g_running = false;
}

// ============================================================================
// Main Entry Point
// ============================================================================
int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << "  HopStar WebRTC Server" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Display detection
    if (g_hasDisplay) {
        LOG_INFO("Display detected: GUI mode enabled");
    } else {
        LOG_INFO("No display: Headless mode");
    }

    // Signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Initialize platform
    LinuxBridge_initialize();
    LOG_INFO("Documents: " + std::string(LinuxBridge_getDocumentsDirectory()));

    // FileEngine
    auto* fileEngine = getFileEngine();
    LOG_INFO("FileEngine initialized");

    // GC
    magc = GC_init(1000 * 1000 * 1000);
    LOG_INFO("GC initialized");

    // SQLite
    std::string dbPath = fileEngine->getInternalPath("server.db");
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOG_ERROR("Can't open database: " + std::string(sqlite3_errmsg(db)));
        return rc;
    }
    LOG_INFO("Database opened: " + dbPath);

    const char* sql = "CREATE TABLE IF NOT EXISTS peers(id TEXT PRIMARY KEY, connected_at INTEGER);";
    char* err = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        LOG_ERROR("SQL error: " + std::string(err));
        sqlite3_free(err);
    }
    sqlite3_close(db);

    // Test optional features
    try {
#ifdef HOPSTAR_ENABLE_TORCH
        if (torch::cuda::is_available()) {
            LOG_INFO("CUDA available! Devices: " + std::to_string(torch::cuda::device_count()));
        } else {
            LOG_INFO("CUDA not available, using CPU");
        }
#else
        LOG_INFO("PyTorch support disabled");
#endif

#ifdef HOPSTAR_ENABLE_ONNX
        LOG_INFO("ONNX Runtime support enabled");
#else
        LOG_INFO("ONNX Runtime support disabled");
#endif

#ifdef HOPSTAR_ENABLE_TORCH
    } catch (const c10::Error& e) {
        LOG_ERROR("LibTorch Error: " + std::string(e.what()));
#endif
    } catch (const std::exception& e) {
        LOG_ERROR("Exception: " + std::string(e.what()));
    }

    // ========================================================================
    // Initialize Server Components
    // ========================================================================

    ServerConfig config;
    config.http_port = (argc > 1) ? std::atoi(argv[1]) : 80;
    config.websocket_port = (argc > 2) ? std::atoi(argv[2]) : 443;

    // Hybrid Thread Pool Architecture
    // Fast Tasks Pool: HTTP, WebSocket, lightweight operations (8 threads)
    ThreadPool fastTaskPool(8, "FastTaskPool");
    fastTaskPool.start();
    LOG_INFO("Fast task pool started (8 threads)");

    // Blocking I/O Pool: File I/O, Database, heavy operations (4 threads)
    ThreadPool blockingIOPool(4, "BlockingIOPool");
    blockingIOPool.start();
    LOG_INFO("Blocking I/O pool started (4 threads)");

    // Async File I/O Manager
    AsyncFileIO asyncFileIO(blockingIOPool);
    LOG_INFO("Async file I/O initialized");
    (void)asyncFileIO;  // Available for future use

    // HTTP/HTTPS Client (for outgoing requests)
#ifdef HOPSTAR_ENABLE_CURL
    HttpClientCurl httpClient(fastTaskPool);
    LOG_INFO("HTTP/HTTPS client initialized (libcurl)");
#else
    HttpClient httpClient(fastTaskPool);
    LOG_INFO("HTTP client initialized (basic, HTTPS not supported)");
#endif
    (void)httpClient;  // Available for future use

    // Command Queue & Processor
    CommandQueue cmdQueue;
    CommandProcessor cmdProcessor(cmdQueue);
    cmdProcessor.setDisplayAvailable(g_hasDisplay);
    cmdProcessor.setRunningFlag(&g_running);
    cmdProcessor.start();
    LOG_INFO("Command processor started");

    // HTTP Server
    HTTPServer httpServer(cmdQueue, fastTaskPool, config.http_port);
    if (!httpServer.start()) {
        LOG_ERROR("Failed to start HTTP server");
        return 1;
    }

    // WebSocket Server
    WebSocketServer wsServer(config.websocket_port);
    if (!wsServer.start()) {
        LOG_WARN("WebSocket server failed to start (may not be compiled in)");
    }

    // WebRTC Manager
    WebRTCManager webrtcManager(config);
    if (!webrtcManager.initialize()) {
        LOG_WARN("WebRTC manager failed to initialize (may not be compiled in)");
    }

    // Media Processor
    MediaProcessor mediaProcessor;
    if (!mediaProcessor.initialize(1280, 720)) {
        LOG_WARN("Media processor failed to initialize");
    }

    // Connect WebRTC callbacks
    webrtcManager.setOnMediaFrame([&mediaProcessor](const MediaFrame& frame) {
        mediaProcessor.enqueueFrame(frame);
    });

    webrtcManager.setOnPeerConnected([](const PeerInfo& info) {
        LOG_INFO("Peer connected: " + info.peer_id);
    });

    webrtcManager.setOnPeerDisconnected([](const std::string& peer_id) {
        LOG_INFO("Peer disconnected: " + peer_id);
    });

    // Connect WebSocket to WebRTC
    wsServer.setOnMessage([&webrtcManager](const SignalingMessage& msg) {
        auto peer = webrtcManager.getPeer(msg.peer_id);
        if (!peer) {
            peer = webrtcManager.createPeer(msg.peer_id);
        }
        if (peer) {
            if (msg.type == SignalingMessageType::OFFER) {
                peer->setRemoteDescription(msg.sdp, "offer");
            } else if (msg.type == SignalingMessageType::ICE_CANDIDATE) {
                peer->addIceCandidate(msg.ice_candidate, msg.ice_sdp_mid);
            }
        }
    });

    // Link command processor to managers
    cmdProcessor.setWebRTCManager(&webrtcManager);
    cmdProcessor.setMediaProcessor(&mediaProcessor);

    // ========================================================================
    // GUI Initialization (if display available)
    // ========================================================================

#if defined(HOPSTAR_ENABLE_GUI)
    SDL_Window* window = nullptr;
    HopStar* g_hoppy = nullptr;
    RenderThread* g_render = nullptr;

    if (g_hasDisplay) {
        LOG_INFO("Initializing GUI...");

        SDL_SetMainReady();
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland,x11");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            LOG_ERROR("SDL_Init failed: " + std::string(SDL_GetError()));
        } else {
            const char* videoDriver = SDL_GetCurrentVideoDriver();
            LOG_INFO("SDL initialized (driver: " + std::string(videoDriver ? videoDriver : "unknown") + ")");

            window = SDL_CreateWindow("HopStar WebRTC Server", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
            if (!window) {
                LOG_ERROR("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
                SDL_Quit();
            } else {
                int winW, winH;
                SDL_GetWindowSize(window, &winW, &winH);
                LOG_INFO("Window created: " + std::to_string(winW) + "x" + std::to_string(winH));

                g_hoppy = new HopStar();
                g_hoppy->push_tab(magc);
                magc->hoppy = g_hoppy;

                g_render = new RenderThread(g_hoppy, window);
                g_render->start();
                runGoThreadAsync_Linux(magc);

                cmdProcessor.setHoppy(g_hoppy);

                LOG_INFO("GUI initialized successfully");
            }
        }
    }
#endif

    // ========================================================================
    // Server Ready
    // ========================================================================

    std::cout << "\n==========================================" << std::endl;
    std::cout << "  Server is ready!" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Mode:      " << (g_hasDisplay ? "GUI" : "Headless") << std::endl;

    // HTTP protocol (80 = http, 443 = https, others = http)
    std::string http_proto = (config.http_port == 443) ? "https" : "http";
    std::cout << "HTTP:      " << http_proto << "://localhost:" << config.http_port << std::endl;

    // WebSocket protocol (443 = wss, others = ws)
    std::string ws_proto = (config.websocket_port == 443) ? "wss" : "ws";
    std::cout << "WebSocket: " << ws_proto << "://localhost:" << config.websocket_port << std::endl;

    std::cout << "WebRTC:    Ports " << config.webrtc_min_port << "-" << config.webrtc_max_port << std::endl;

    if (config.http_port < 1024 || config.websocket_port < 1024) {
        std::cout << "\n⚠ Warning: Using privileged ports (<1024) requires root or CAP_NET_BIND_SERVICE" << std::endl;
    }

    std::cout << "\nEndpoints:" << std::endl;
    std::cout << "  GET  /status       - Server status" << std::endl;
    std::cout << "  GET  /ping         - Ping test" << std::endl;
    std::cout << "  GET  /webrtc/peers - List peers" << std::endl;
    std::cout << "  POST /shutdown     - Shutdown server" << std::endl;
    std::cout << "\nPress Ctrl+C to stop" << std::endl;
    std::cout << "==========================================" << std::endl;

    // ========================================================================
    // Main Event Loop
    // ========================================================================

#if defined(HOPSTAR_ENABLE_GUI)
    if (g_hasDisplay && window) {
        SDL_Event event;
        while (g_running) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_EVENT_QUIT:
                        g_running = false;
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        if (event.key.key == SDLK_ESCAPE) {
                            g_running = false;
                        }
                        break;
                    default:
                        break;
                }
            }
            SDL_Delay(16);
        }
    } else
#endif
    {
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    // ========================================================================
    // Cleanup
    // ========================================================================

    LOG_INFO("Shutting down...");

#if defined(HOPSTAR_ENABLE_GUI)
    if (g_render) {
        g_render->stop();
        delete g_render;
    }
    if (g_hoppy) {
        delete g_hoppy;
    }
    if (window) {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
#endif

    // Stop server components
    mediaProcessor.stop();
    wsServer.stop();
    httpServer.stop();
    cmdProcessor.stop();

    // Shutdown thread pools
    fastTaskPool.shutdown();
    blockingIOPool.shutdown();

    // Platform cleanup
    LinuxBridge_shutdown();
    g_fileEngine.reset();

    LOG_INFO("Shutdown complete");
    return 0;
}
