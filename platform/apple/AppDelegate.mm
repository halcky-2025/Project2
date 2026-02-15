#import "AppDelegate.h"
#import "AppleBridge-Swift.h"
#import <QuartzCore/CADisplayLink.h>

// SDL
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

// Include chain (same order as main2.cpp for Android)
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

// Timing function for iOS
uint64_t now_us_iOS() {
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

// Inline GoThread implementation for iOS
void GoThread_iOS(ThreadGC* thgc) {
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
        thgc->hoppy->buildFrame(now_us_iOS());
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(end - start).count();
        if (12.0 - ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(12.0 - ms)));
    }
}

void runGoThreadAsync_iOS(ThreadGC* thgc) {
    std::thread([thgc]() {
        initDone.get_future().wait();
        GoThread_iOS(thgc);
    }).detach();
}

@interface AppDelegate () {
    CADisplayLink *_displayLink;
}
@property (nonatomic, assign) SDL_Window *sdlWindow;
@property (nonatomic, assign) BOOL running;
@end

// Global HopStar instance (same as main2.cpp)
static HopStar* g_hoppy = nullptr;
static RenderThread* g_render = nullptr;

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSLog(@"AppDelegate: didFinishLaunchingWithOptions");

    // Initialize ThreadGC (same as main2.cpp)
    magc = GC_init(1000 * 1000 * 1000);

    // SDL initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        NSLog(@"SDL_Init failed: %s", SDL_GetError());
        return [self fallbackToBasicUI];
    }
    NSLog(@"SDL3 initialized successfully");

    // Create fullscreen window for iOS (similar to Android in main2.cpp)
    self.sdlWindow = SDL_CreateWindow("HopStar", 0, 0, SDL_WINDOW_METAL | SDL_WINDOW_FULLSCREEN);
    if (!self.sdlWindow) {
        NSLog(@"SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return [self fallbackToBasicUI];
    }

    int winW, winH;
    SDL_GetWindowSize(self.sdlWindow, &winW, &winH);
    NSLog(@"SDL window created: %dx%d", winW, winH);

    // Initialize HopStar and RenderThread (same as main2.cpp)
    g_hoppy = new HopStar();
    g_hoppy->push_tab(magc);
    magc->hoppy = g_hoppy;

    g_render = new RenderThread(g_hoppy, self.sdlWindow);
    g_render->start();
    runGoThreadAsync_iOS(magc);

    self.running = YES;
    NSLog(@"HopStar started with RenderThread");

    // Use CADisplayLink for VSync-synchronized rendering on main thread
    // CADisplayLink callback runs on the thread where it was created (main thread here)
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderLoop:)];
    [_displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];

    return YES;
}

- (void)renderLoop:(CADisplayLink *)displayLink {
    (void)displayLink;
    if (!self.running) return;

    // Process SDL events on main thread
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            self.running = NO;
            return;
        }
    }

    // Render one frame on main thread (same thread as SDL/Metal layer creation)
    if (g_render) {
        g_render->renderOneFrame();
    }
}

- (BOOL)fallbackToBasicUI {
    NSLog(@"Falling back to basic UI");
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    UIViewController *vc = [[UIViewController alloc] init];
    vc.view.backgroundColor = [UIColor colorWithRed:0.1 green:0.1 blue:0.15 alpha:1.0];

    UILabel *titleLabel = [[UILabel alloc] init];
    titleLabel.text = @"HopStar";
    titleLabel.font = [UIFont boldSystemFontOfSize:36];
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.textAlignment = NSTextAlignmentCenter;
    titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [vc.view addSubview:titleLabel];

    UILabel *errorLabel = [[UILabel alloc] init];
    errorLabel.text = @"Initialization failed\nCheck console for details";
    errorLabel.font = [UIFont systemFontOfSize:14];
    errorLabel.textColor = [UIColor colorWithRed:1.0 green:0.5 blue:0.5 alpha:1.0];
    errorLabel.textAlignment = NSTextAlignmentCenter;
    errorLabel.numberOfLines = 0;
    errorLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [vc.view addSubview:errorLabel];

    [NSLayoutConstraint activateConstraints:@[
        [titleLabel.centerXAnchor constraintEqualToAnchor:vc.view.centerXAnchor],
        [titleLabel.centerYAnchor constraintEqualToAnchor:vc.view.centerYAnchor constant:-40],
        [errorLabel.centerXAnchor constraintEqualToAnchor:vc.view.centerXAnchor],
        [errorLabel.topAnchor constraintEqualToAnchor:titleLabel.bottomAnchor constant:20],
    ]];

    self.window.rootViewController = vc;
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)cleanup {
    self.running = NO;

    // Stop CADisplayLink
    if (_displayLink) {
        [_displayLink invalidate];
        _displayLink = nil;
    }

    if (g_render) {
        g_render->stop();
        delete g_render;
        g_render = nullptr;
    }

    if (g_hoppy) {
        delete g_hoppy;
        g_hoppy = nullptr;
    }

    if (self.sdlWindow) {
        SDL_DestroyWindow(self.sdlWindow);
        self.sdlWindow = NULL;
    }

    SDL_Quit();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [self cleanup];
}

@end
