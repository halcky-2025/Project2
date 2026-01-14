// =============================================================================
// Android専用アプリケーション - main.cppと同様の構造
// =============================================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
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
#include "extorch.h"

#include <thread>
#include <chrono>

// =============================================================================
// Main
// =============================================================================
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SDL_Log("=== Android App Starting ===");

    // Initialize ThreadGC (use smaller size for Android: 100MB instead of 1GB)
    SDL_Log("Initializing GC...");
    magc = GC_init(100 * 1000 * 1000);
    SDL_Log("GC initialized");

    // SDL initialization
    SDL_Log("Initializing SDL...");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        return 1;
    }
    SDL_Log("SDL initialized");

    // Create fullscreen window for Android
    SDL_Log("Creating window...");
    SDL_Window* window = SDL_CreateWindow("Android App", 0, 0, SDL_WINDOW_VULKAN | SDL_WINDOW_FULLSCREEN);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Log actual window size
    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    SDL_Log("Window created: %dx%d", winW, winH);

    // Wait for window to be shown/exposed before starting render thread
    SDL_Log("Waiting for window to be ready...");
    bool windowReady = false;
    int waitAttempts = 0;
    const int maxWaitAttempts = 100;  // 10 seconds max
    while (!windowReady && waitAttempts < maxWaitAttempts) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_WINDOW_SHOWN ||
                event.type == SDL_EVENT_WINDOW_EXPOSED ||
                event.type == SDL_EVENT_WINDOW_DISPLAY_CHANGED) {
                SDL_Log("Window ready event received: %d", event.type);
                windowReady = true;
                break;
            }
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_TERMINATING) {
                SDL_Log("Quit event received during window wait");
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }
        }
        if (!windowReady) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            waitAttempts++;
        }
    }

    // Check if ANativeWindow is available
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    void* nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
    SDL_Log("ANativeWindow pointer: %p", nwh);

    if (!windowReady) {
        SDL_Log("Window wait timed out, but continuing anyway...");
    }

    // Initialize HopStar and RenderThread
    SDL_Log("Creating HopStar...");
    HopStar* hoppy = new HopStar();
    hoppy->push_tab(magc);
    magc->hoppy = hoppy;
    SDL_Log("Creating RenderThread...");
    RenderThread* render = new RenderThread(hoppy, window);
    SDL_Log("Starting RenderThread...");
    render->start();
    SDL_Log("Starting GoThread...");
    runGoThreadAsync(magc);

    // Event loop
    int running = 1;
    SDL_Log("Entering main event loop");
    while (running) {
        SDL_Event event;
        // Use SDL_WaitEventTimeout instead of sleep + poll for better power efficiency
        while (SDL_WaitEventTimeout(&event, 10)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    SDL_Log("SDL_EVENT_QUIT received");
                    running = 0;
                    break;
                case SDL_EVENT_TERMINATING:
                    SDL_Log("SDL_EVENT_TERMINATING received");
                    running = 0;
                    break;
                case SDL_EVENT_LOW_MEMORY:
                    SDL_Log("SDL_EVENT_LOW_MEMORY received");
                    break;
                case SDL_EVENT_WILL_ENTER_BACKGROUND:
                    SDL_Log("SDL_EVENT_WILL_ENTER_BACKGROUND received");
                    break;
                case SDL_EVENT_DID_ENTER_BACKGROUND:
                    SDL_Log("SDL_EVENT_DID_ENTER_BACKGROUND received");
                    break;
                case SDL_EVENT_WILL_ENTER_FOREGROUND:
                    SDL_Log("SDL_EVENT_WILL_ENTER_FOREGROUND received");
                    break;
                case SDL_EVENT_DID_ENTER_FOREGROUND:
                    SDL_Log("SDL_EVENT_DID_ENTER_FOREGROUND received");
                    break;
                case SDL_EVENT_WINDOW_DESTROYED:
                    SDL_Log("SDL_EVENT_WINDOW_DESTROYED received");
                    running = 0;
                    break;
                default:
                    break;
            }
            if (!running) break;
        }
    }

    SDL_Log("Exiting main event loop");
    render->stop();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
