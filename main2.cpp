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

    // Initialize ThreadGC
    magc = GC_init(1000 * 1000 * 1000);

    // SDL initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        return 1;
    }

    // Create fullscreen window for Android
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

    // Initialize HopStar and RenderThread
    HopStar* hoppy = new HopStar();
    hoppy->push_tab(magc);
    magc->hoppy = hoppy;
    RenderThread* render = new RenderThread(hoppy, window);
    render->start();
    runGoThreadAsync(magc);

    // Event loop
    int running = 1;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }
    }

    return 0;
}
