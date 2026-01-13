// =============================================================================
// iOS AppDelegate Implementation
// =============================================================================

#import "AppDelegate.h"
#import <QuartzCore/CADisplayLink.h>

#include <SDL3/SDL.h>

// Global state
static SDL_Window* g_window = nullptr;
static int g_running = 0;

@implementation AppDelegate {
    CADisplayLink* _displayLink;
}

- (void)gameLoop:(CADisplayLink*)displayLink {
    (void)displayLink;

    if (!g_running) return;

    // Process SDL events on main thread
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            g_running = 0;
        }
    }
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application;
    (void)launchOptions;

    SDL_Log("=== HopStar iOS Starting ===");

    // SDL initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        return NO;
    }
    SDL_Log("SDL initialized");

    // Create fullscreen window for iOS
    g_window = SDL_CreateWindow("HopStar iOS", 0, 0, SDL_WINDOW_METAL | SDL_WINDOW_FULLSCREEN);
    if (!g_window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return NO;
    }

    // Log actual window size
    int winW, winH;
    SDL_GetWindowSize(g_window, &winW, &winH);
    SDL_Log("Window created: %dx%d", winW, winH);

    g_running = 1;

    // Use CADisplayLink for main thread event loop (required for iOS)
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(gameLoop:)];
    [_displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];

    SDL_Log("=== HopStar iOS Ready ===");

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    (void)application;
    // Pause rendering when going to background
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    (void)application;
    // Save state if needed
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    (void)application;
    // Restore state if needed
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    (void)application;
    // Resume rendering
}

- (void)applicationWillTerminate:(UIApplication *)application {
    (void)application;
    // Cleanup
    g_running = 0;

    // Stop display link
    if (_displayLink) {
        [_displayLink invalidate];
        _displayLink = nil;
    }

    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }

    SDL_Quit();
}

@end
