// =============================================================================
// macOS Application Entry Point (main4.mm) - bgfx Version
// =============================================================================

#include <TargetConditionals.h>

#if !TARGET_OS_OSX
    #error "main4.mm is for macOS only. Use main3.mm for iOS."
#endif

// Tell SDL we're handling main ourselves
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#import <Cocoa/Cocoa.h>

#include <string>  // Needed for getBundlePath

#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

// Define getBundlePath for macOS before including ugui.h
#ifndef BUNDLE_PATH_DEFINED
#define BUNDLE_PATH_DEFINED
inline std::string getBundlePath(const char* filename) {
    NSString* name = [[NSString alloc] initWithUTF8String:filename];
    // Extract just the filename (last path component) for bundle lookup
    NSString* lastComponent = [name lastPathComponent];
    NSString* ext = [lastComponent pathExtension];
    NSString* base = [lastComponent stringByDeletingPathExtension];
    NSString* path = [[NSBundle mainBundle] pathForResource:base ofType:ext];
    if (path) {
        return std::string([path UTF8String]);
    }
    return std::string(filename); // fallback
}
#endif

// Include chain (same order as iOS)
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
// #include "extorch.h"  // Disabled for macOS - requires PyTorch

#include <iostream>
#include <thread>
#include <chrono>

// Forward declaration - implemented in Objective-C
extern "C" {
    void AppleBridge_initialize(void);
    void AppleBridge_shutdown(void);
    const char* AppleBridge_getDocumentsDirectory(void);
}

// Timing function for macOS
uint64_t now_us_macOS() {
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

// Inline GoThread implementation for macOS
void GoThread_macOS(ThreadGC* thgc) {
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
    thgc->map = create_mapy(magc, _Struct);
    String* str = createString(thgc, (char*)"main", 4, 1);
    TreeElement* te = (TreeElement*)GC_alloc(thgc, _TreeElement);
    te->id = str;
    te->elem = (NewElement*)local;
    te->children = create_list(thgc, sizeof(TreeElement*), _List);
    add_mapy(thgc, thgc->map, str, (char*)te);

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        thgc->hoppy->buildFrame(now_us_macOS());
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(end - start).count();
        if (12.0 - ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(12.0 - ms)));
    }
}

void runGoThreadAsync_macOS(ThreadGC* thgc) {
    std::thread([thgc]() {
        initDone.get_future().wait();
        GoThread_macOS(thgc);
    }).detach();
}

// Global HopStar instance
static HopStar* g_hoppy = nullptr;
static RenderThread* g_render = nullptr;

// =============================================================================
// Main Entry Point
// =============================================================================
int main(int argc, char* argv[]) {
    @autoreleasepool {
        (void)argc;
        (void)argv;

        std::cout << "Starting HopStar on macOS" << std::endl;

        // Initialize Apple platform bridge
        AppleBridge_initialize();

        std::cout << "Documents: " << AppleBridge_getDocumentsDirectory() << std::endl;

        // Initialize ThreadGC
        magc = GC_init(1000 * 1000 * 1000);

        // Tell SDL that we're handling main ourselves
        SDL_SetMainReady();

        // SDL initialization
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return 1;
        }
        std::cout << "SDL3 initialized successfully" << std::endl;

        // Create window for macOS
        SDL_Window* window = SDL_CreateWindow(
            "HopStar",
            1280, 720,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_METAL
        );

        if (!window) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        int winW, winH;
        SDL_GetWindowSize(window, &winW, &winH);
        std::cout << "SDL window created: " << winW << "x" << winH << std::endl;

        // Initialize HopStar and RenderThread
        g_hoppy = new HopStar();
        g_hoppy->push_tab(magc);
        magc->hoppy = g_hoppy;

        g_render = new RenderThread(g_hoppy, window);
        g_render->start();
        runGoThreadAsync_macOS(magc);

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
        AppleBridge_shutdown();

        return 0;
    }
}
