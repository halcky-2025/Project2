// =============================================================================
// Linux Application Entry Point (main5.cpp) - bgfx Version
// =============================================================================

#ifdef __APPLE__
    #error "main5.cpp is for Linux only. Use main4.mm for macOS or main3.mm for iOS."
#endif

// Tell SDL we're handling main ourselves
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <string>
#include <cstring>
#include <unistd.h>
#include <linux/limits.h>
#include <libgen.h>
#include <dlfcn.h>

#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

// Define getBundlePath for Linux before including ugui.h
#ifndef BUNDLE_PATH_DEFINED
#define BUNDLE_PATH_DEFINED

// Get executable directory
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
    // For Linux, look for resources relative to executable directory
    std::string execDir = getExecutableDir();

    // Try several locations:
    // 1. Same directory as executable
    std::string path1 = execDir + "/" + filename;
    if (access(path1.c_str(), F_OK) == 0) {
        return path1;
    }

    // 2. Resources subdirectory
    std::string path2 = execDir + "/Resources/" + filename;
    if (access(path2.c_str(), F_OK) == 0) {
        return path2;
    }

    // 3. ../share/appname directory (standard Linux install location)
    std::string path3 = execDir + "/../share/hopstar/" + filename;
    if (access(path3.c_str(), F_OK) == 0) {
        return path3;
    }

    // 4. Current working directory
    if (access(filename, F_OK) == 0) {
        return std::string(filename);
    }

    // Fallback to original filename
    return std::string(filename);
}
#endif

// Include chain (same order as macOS/iOS)
#include "gc.h"
#include "arr.h"
#include "sqlite3.h"
#include "db2.h"
#include "elem.h"
#include "shader.h"
#include "shader2.h"
#include "thumnailAtlas.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imas.h"
#include "audio.h"
#include "ugui.h"
// #include "extorch.h"  // Disabled for Linux - requires PyTorch

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <sys/stat.h>
#include <pwd.h>

// =============================================================================
// Linux Platform Bridge (replacement for AppleBridge)
// =============================================================================

static std::string g_documentsDirectory;

extern "C" {
    void LinuxBridge_initialize(void) {
        // Get home directory
        const char* home = getenv("HOME");
        if (!home) {
            struct passwd* pw = getpwuid(getuid());
            if (pw) {
                home = pw->pw_dir;
            }
        }

        if (home) {
            // Use XDG standard: ~/.local/share/hopstar
            const char* xdgData = getenv("XDG_DATA_HOME");
            if (xdgData && xdgData[0]) {
                g_documentsDirectory = std::string(xdgData) + "/hopstar";
            } else {
                g_documentsDirectory = std::string(home) + "/.local/share/hopstar";
            }

            // Create directory if it doesn't exist
            mkdir(g_documentsDirectory.c_str(), 0755);
        } else {
            g_documentsDirectory = ".";
        }
    }

    void LinuxBridge_shutdown(void) {
        // Nothing to clean up
    }

    const char* LinuxBridge_getDocumentsDirectory(void) {
        return g_documentsDirectory.c_str();
    }
}

// Alias for compatibility with code expecting Apple bridge
extern "C" {
    void AppleBridge_initialize(void) {
        LinuxBridge_initialize();
    }

    void AppleBridge_shutdown(void) {
        LinuxBridge_shutdown();
    }

    const char* AppleBridge_getDocumentsDirectory(void) {
        return LinuxBridge_getDocumentsDirectory();
    }
}

// =============================================================================
// Timing function for Linux
// =============================================================================
uint64_t now_us_Linux() {
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

// =============================================================================
// GoThread implementation for Linux
// =============================================================================
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
    let->text = createString(thgc, (char*)"Hello,world!", 13, 1);
    let->color = 0xFFFFFFFF;
    NewElementAddLast(thgc, local, (NewElement*)local, (NewElement*)let);
    thgc->map = create_mapy(magc, false);
    String* str = createString(thgc, (char*)"main", 4, 1);
    TreeElement* te = (TreeElement*)GC_alloc(thgc, _TreeElement);
    te->id = str;
    te->elem = (NewElement*)local;
    te->children = create_list(thgc, sizeof(TreeElement*), true);
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

// =============================================================================
// Global instances
// =============================================================================
static HopStar* g_hoppy = nullptr;
static RenderThread* g_render = nullptr;

// =============================================================================
// Main Entry Point
// =============================================================================
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::cout << "Starting HopStar on Linux" << std::endl;

    // Initialize Linux platform bridge
    LinuxBridge_initialize();

    std::cout << "Documents: " << LinuxBridge_getDocumentsDirectory() << std::endl;

    // Initialize ThreadGC
    magc = GC_init(1000 * 1000 * 1000);

    // Tell SDL that we're handling main ourselves
    SDL_SetMainReady();

    // Prefer Wayland over X11 (Wayland doesn't have the flicker issue)
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland,x11");

    // SDL initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    const char* videoDriver = SDL_GetCurrentVideoDriver();
    std::cout << "SDL3 initialized successfully (video driver: " << (videoDriver ? videoDriver : "unknown") << ")" << std::endl;

    // Create window for Linux
    SDL_Window* window = SDL_CreateWindow(
        "HopStar",
        1280, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    std::cout << "SDL window created: " << winW << "x" << winH << std::endl;

    // X11 only: Fill window with black immediately to prevent flicker
    // This is necessary because X11 compositor shows uninitialized window buffer
    // before Vulkan swapchain is ready. Wayland doesn't have this issue.
    if (videoDriver && strcmp(videoDriver, "x11") == 0) {
        SDL_PropertiesID props = SDL_GetWindowProperties(window);
        void* dpy = SDL_GetPointerProperty(props, "SDL.window.x11.display", nullptr);
        unsigned long x11win = (unsigned long)SDL_GetNumberProperty(props, "SDL.window.x11.window", 0);

        if (dpy && x11win) {
            void* libX11 = dlopen("libX11.so.6", RTLD_LAZY);
            if (libX11) {
                // Function pointers (using dlopen to avoid Xlib.h header conflicts)
                typedef void* (*XCreateGC_t)(void*, unsigned long, unsigned long, void*);
                typedef int (*XSetForeground_t)(void*, void*, unsigned long);
                typedef int (*XFillRectangle_t)(void*, unsigned long, void*, int, int, unsigned int, unsigned int);
                typedef int (*XFreeGC_t)(void*, void*);
                typedef int (*XFlush_t)(void*);
                typedef unsigned long (*XBlackPixel_t)(void*, int);
                typedef int (*XDefaultScreen_t)(void*);

                auto pXCreateGC = (XCreateGC_t)dlsym(libX11, "XCreateGC");
                auto pXSetForeground = (XSetForeground_t)dlsym(libX11, "XSetForeground");
                auto pXFillRectangle = (XFillRectangle_t)dlsym(libX11, "XFillRectangle");
                auto pXFreeGC = (XFreeGC_t)dlsym(libX11, "XFreeGC");
                auto pXFlush = (XFlush_t)dlsym(libX11, "XFlush");
                auto pXBlackPixel = (XBlackPixel_t)dlsym(libX11, "XBlackPixel");
                auto pXDefaultScreen = (XDefaultScreen_t)dlsym(libX11, "XDefaultScreen");

                if (pXCreateGC && pXSetForeground && pXFillRectangle && pXFreeGC && pXFlush && pXBlackPixel && pXDefaultScreen) {
                    int screen = pXDefaultScreen(dpy);
                    unsigned long black = pXBlackPixel(dpy, screen);
                    void* gc = pXCreateGC(dpy, x11win, 0, nullptr);
                    if (gc) {
                        pXSetForeground(dpy, gc, black);
                        pXFillRectangle(dpy, x11win, gc, 0, 0, winW, winH);
                        pXFreeGC(dpy, gc);
                        pXFlush(dpy);
                        std::cout << "X11 window filled with black" << std::endl;
                    }
                }
                dlclose(libX11);
            }
        }
    }

    // Initialize HopStar and RenderThread
    g_hoppy = new HopStar();
    g_hoppy->push_tab(magc);
    magc->hoppy = g_hoppy;

    g_render = new RenderThread(g_hoppy, window);
    g_render->start();
    runGoThreadAsync_Linux(magc);

    std::cout << "HopStar started with RenderThread" << std::endl;

    // Main event loop
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
                default:
                    break;
            }
        }
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    if (g_render) {
        g_render->stop();
        delete g_render;
        g_render = nullptr;
    }

    if (g_hoppy) {
        delete g_hoppy;
        g_hoppy = nullptr;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    LinuxBridge_shutdown();

    return 0;
}
