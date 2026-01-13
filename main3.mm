// =============================================================================
// iOS Application Entry Point (main3.cpp)
// =============================================================================

#include <TargetConditionals.h>

#if !TARGET_OS_IOS && !TARGET_OS_SIMULATOR
    #error "main3.cpp is for iOS only. Use main4.cpp for macOS."
#endif

// Tell SDL we're handling main ourselves
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// Use Objective-C runtime for iOS app lifecycle
#import <UIKit/UIKit.h>
#import "AppDelegate.h"

// Standard main function - we handle main ourselves
int main(int argc, char* argv[]) {
    @autoreleasepool {
        // Tell SDL that we're handling main ourselves
        SDL_SetMainReady();
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
