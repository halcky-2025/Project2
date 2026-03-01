#include "shader.h"
#include <fstream>
#include <chrono>
#include <future>

// getBundlePath is now provided by platform_io.h (included via shader.h)
// For backwards compatibility, define it if not already defined
#ifndef BUNDLE_PATH_DEFINED
#define BUNDLE_PATH_DEFINED
inline std::string getBundlePath(const char* filename) {
    return PlatformIO::resolvePath(filename, HopStarIO::Location::Resource);
}
#endif

// 描画コマンド構造体

#pragma once
#include <portaudio.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <bgfx/bgfx.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "thumnailAtlas.h"
#include "imas.h"

class RenderCommandQueue {
public:
    // === Producer（タブ側スレッド）API ===
    // 1フレーム生成の開始（必要なら予約数を渡して再割当回数を減らす）
    void begin(uint64_t frameId, size_t reserveHint = 0) {
        auto& s = slots_[write_];
        s.frameId = frameId;
        s.layers.clear();
        if (reserveHint > 0) {
            s.layers.reserve(reserveHint);  // ★ 追加
        }
    }
    // フレーム公開（最新スロットを readable にする）
    void publish() {
        // 最新フレームの公開
        front_.store(write_, std::memory_order_release);

        // 次に Producer が書くスロットを選ぶ（front と reading を避ける）
        const int front = front_.load(std::memory_order_relaxed);
        const int reading = reading_.load(std::memory_order_acquire);
        for (int i = 0; i < 3; ++i) {
            if (i != front && i != reading) { write_ = i; break; }
        }
    }

    // Producer が現在の write スロットにレイヤー設定を書くためのユーティリティ
    LayerInfo& setCurrentSlotLayer(
        const std::vector<std::string>& uniformNames,
        float opacity = 1.0f,
        bool visible = true,
        bool needsUpdate = true,
        int w = 800,
        int h = 600)
    {
        Slot& s = slots_[write_];
        s.layers.emplace_back();
        LayerInfo& layer = s.layers.back();

        layer.opacity = opacity;
        layer.visible = visible;
        layer.needsUpdate = needsUpdate;
        layer.width = w;
        layer.height = h;

        return layer;
    }

    // === Consumer（レンダースレッド）API ===
    // 最新フレームを取得（新規が無ければ false）: out にムーブし、token を返す
    bool acquire(std::vector<LayerInfo>** outLayers, uint64_t& outFrameId, int& token) {
        const int idx = front_.load(std::memory_order_acquire);
        if (idx == -1) return false;                              // まだ公開無し
        if (idx == reading_.load(std::memory_order_acquire)) return false; // 既に取得済み

        reading_.store(idx, std::memory_order_release);           // このスロットを“使用中”に
        auto& s = slots_[idx];
        *outLayers = &s.layers;                                    // ムーブしてコピー回避
        outFrameId = s.frameId;
        token = idx;                                              // release 用
        return true;
    }

    // レンダがスロットの使用を終えたら呼ぶ（書き込みに開放）
    void release(int token) {
        // token が現在の reading なら -1 に戻す
        int expected = token;
        reading_.compare_exchange_strong(expected, -1, std::memory_order_acq_rel);
    }

private:
    struct Slot {
        uint64_t frameId = 0;
        std::vector<LayerInfo> layers; // ← スロットにレイヤーを持たせる（トリプルバッファの各スロットがレイヤー）
    };
    Slot slots_[3];

    // Producer が書き込むスロット（Producer専有なので非atomicでOK）
    int write_ = 0;

    // 公開済みの最新スロット（-1 = なし）
    std::atomic<int> front_{ -1 };
    // Consumer が現在描画中のスロット（-1 = なし）
    std::atomic<int> reading_{ -1 };
};
struct FontKey {
    std::string family;  // "NotoSans", "Roboto", "Consolas" など
    int size;            // 24, 32, 48 など

    bool operator<(const FontKey& other) const {
        if (family != other.family) return family < other.family;
        return size < other.size;
    }
};
// === ヘルパー関数：テキスト描画 ===
void drawText(LayerInfo* layer, FontAtlas& atlas, FontId font,
    const char* text, float x, float y,
    float zIndex, uint32_t color, bgfx::FrameBufferHandle* targetFBO, RenderGroup* group, PointI* fbsize, uint8_t viewId) {
    float cursorX = x;
    float cursorY = y;

    const char* p = text;
    while (*p) {
        uint32_t codepoint = (uint32_t)(unsigned char)*p; // 簡易版（UTF-8対応は別途必要）
        p++;

        auto gi = atlas.getOrAddGlyph(font, codepoint, group);

        if (gi.width > 0 && gi.height > 0) {
            layer->pushText(cursorX + gi.bearingX, cursorY - gi.bearingY, gi.width, gi.height, gi.u0, gi.v0, gi.u1 - gi.u0, gi.v1 - gi.v0, color, zIndex, &atlas.getPageTexture(gi.pageIndex), targetFBO, fbsize, viewId);
        }

        cursorX += gi.advance;
    }
}
void drawString(LayerInfo* layer, FontAtlas& atlas, FontId font,
    String* text, float x, float y,
    float zIndex, uint32_t color, ExtendedRenderGroup* group, bgfx::FrameBufferHandle * targetFBO, PointI* fbsize, uint8_t viewId) {
    float cursorX = x;
    float cursorY = y;

    for(int i = 0; i < text->size; i++){
        uint32_t codepoint = (uint32_t)GetChar(text, i);

        auto gi = atlas.getOrAddGlyph(font, codepoint, group);

        if (gi.width > 0 && gi.height > 0) {
            layer->pushText(cursorX + gi.bearingX, cursorY - gi.bearingY, gi.width, gi.height, gi.u0, gi.v0, gi.u1 - gi.u0, gi.v1 - gi.v0, color, zIndex , &atlas.getPageTexture(gi.pageIndex), targetFBO, fbsize, viewId);
        }

        cursorX += gi.advance;
    }
}
void drawChar16(LayerInfo* layer, FontAtlas& atlas, FontId font,
    uint32_t codepoint, float x, float y,
    float zIndex, uint32_t color, ExtendedRenderGroup* group, bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId) {
    float cursorX = x;
    float cursorY = y;


    auto gi = atlas.getOrAddGlyph(font, codepoint, group);

    if (gi.width > 0 && gi.height > 0) {
        layer->pushText(cursorX + gi.bearingX, cursorY - gi.bearingY, gi.width, gi.height, gi.u0, gi.v0, gi.u1 - gi.u0, gi.v1 - gi.v0, color, zIndex, &atlas.getPageTexture(gi.pageIndex), targetFBO, fbsize, viewId);
    }
}
void drawUnderPagingBar(LayerInfo* layer, FontAtlas& atlas, FontId font, float x, float y, float width, float height, float currentPage, float totalPages, float zIndex,
    ExtendedRenderGroup* group, bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId) {


    const uint32_t arrowColor = 0xFFFFFFFF;
    const uint32_t arrowDisabledColor = 0x60FFFFFF;
    const uint32_t trackColor = 0x40FFFFFF;
    const uint32_t thumbColor = 0xFFFFFFFF;

    // グリフサイズ取得
    const auto& leftGlyph = atlas.getOrAddGlyph(font, 0x25C0, group);   // ◀
    const auto& rightGlyph = atlas.getOrAddGlyph(font, 0x25B6, group); // ▶

    float arrowWidth = leftGlyph.width;
    float padding = 6.f;

    // レイアウト計算
    float leftArrowX = x;
    float rightArrowX = x + width - arrowWidth;
    float trackX = leftArrowX + arrowWidth + padding;
    float trackWidth = rightArrowX - trackX - padding;
    float trackHeight = 4.f;
    float thumbRadius = 6.f;

    float centerY = y + height * 0.5f;

    // 左矢印 ◀
    uint32_t leftColor = (currentPage > 0) ? arrowColor : arrowDisabledColor;
    drawChar16(layer, atlas,font,  0x25C0,
        leftArrowX,
        centerY - leftGlyph.height * 0.5f,
        zIndex, leftColor, group, targetFBO, fbsize, viewId);

    // 右矢印 ▶
    uint32_t rightColor = (currentPage < totalPages - 1) ? arrowColor : arrowDisabledColor;
    drawChar16(layer, atlas, font, 0x25B6,
        rightArrowX,
        centerY - rightGlyph.height * 0.5f,
        zIndex, rightColor, group, targetFBO, fbsize, viewId);

    // トラック（角丸矩形）
    // ※ layer->pushRectがあると仮定
    layer->pushFill(
        trackX, centerY - trackHeight * 0.5f,
        trackWidth,
        trackHeight,
        trackHeight * 0.5f, 0.0f, 0.0f,
        trackColor, 0x00000000,
        0.5f, 1.0f, 0.0f,
        0x000000AA,
        zIndex,
        targetFBO, fbsize, viewId);

    // つまみ位置
    float progress = (totalPages > 1)
        ? (float)currentPage / (totalPages - 1)
        : 0.5f;
    float thumbX = trackX + trackWidth * progress;

    // つまみ（円）
    layer->pushFill(
        thumbX - thumbRadius / 2,
        centerY - thumbRadius,
        thumbRadius, thumbRadius * 2,
        0.0, 0.0f, 0.0f,
        thumbColor, 0x00000000,
        0.0f, 0.0f, 0.0f,
        0x000000000,
        zIndex + 0.1f,  // トラックより前
        targetFBO, fbsize, viewId);
}
void drawRightPagingBar(LayerInfo* layer, FontAtlas& atlas, FontId font,
    float x, float y, float width, float height,
	float currentPage, float totalPages, float zIndex, ExtendedRenderGroup* group,
    bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId) {

    const uint32_t arrowColor = 0xFFFFFFFF;
    const uint32_t arrowDisabledColor = 0x60FFFFFF;
    const uint32_t trackColor = 0x40FFFFFF;
    const uint32_t thumbColor = 0xFFFFFFFF;

    // グリフサイズ取得
    const auto& upGlyph = atlas.getOrAddGlyph(font, 0x25B2, group);    // ▲
    const auto& downGlyph = atlas.getOrAddGlyph(font, 0x25BC, group);  // ▼

    float arrowHeight = upGlyph.height;
    float padding = 1.5f;

    // レイアウト計算
    float topArrowY = y;
    float bottomArrowY = y + height - arrowHeight;
    float trackY = topArrowY + arrowHeight + padding;
    float trackHeight = bottomArrowY - trackY - padding;
    float trackWidth = 4.f;
    float thumbRadius = 6.f;
    float centerX = x + width * 0.5f;

    // 上矢印 ▲
    uint32_t upColor = (currentPage > 0) ? arrowColor : arrowDisabledColor;
    drawChar16(layer, atlas, font, 0x25B2,
        centerX - upGlyph.width * 0.5f,
        topArrowY,
        zIndex, upColor, group, targetFBO, fbsize, viewId);

    // 下矢印 ▼
    uint32_t downColor = (currentPage < totalPages - 1) ? arrowColor : arrowDisabledColor;
    drawChar16(layer, atlas, font, 0x25BC,
        centerX - downGlyph.width * 0.5f,
        bottomArrowY,
        zIndex, downColor, group, targetFBO, fbsize, viewId);

    // トラック（縦の角丸矩形）
    layer->pushFill(
        centerX - trackWidth * 0.5f,
        trackY,
        trackWidth,
        trackHeight,
        trackWidth * 0.5f, 0.0f, 0.0f,
        trackColor, 0x00000000,
        0.5f, 1.0f, 0.0f,
        0x000000AA,
        zIndex,
        targetFBO, fbsize, viewId);

    // つまみ位置
    float progress = (totalPages > 1)
        ? (float)currentPage / (totalPages - 1)
        : 0.5f;
    float thumbY = trackY + trackHeight * progress;

    // つまみ（縦長）
    layer->pushFill(
        centerX - thumbRadius,
        thumbY - thumbRadius / 2,
        thumbRadius * 2,
        thumbRadius,
        0.0f, 0.0f, 0.0f,
        thumbColor, 0x00000000,
        0.0f, 0.0f, 0.0f,
        0x00000000,
        zIndex + 0.1f,
        targetFBO, fbsize, viewId);
}
void drawUnderScrollBar(LayerInfo* layer,
    float x, float y, float width, float height,
    float currentX, float pageWidth, float totalWidth,
    float zIndex,
    bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId) {

    // スクロール不要
    if (totalWidth <= pageWidth) {
        return;
    }

    const uint32_t trackColor = 0x40FFFFFF;
    const uint32_t thumbColor = 0xFFFFFFFF;

    float trackHeight = 4.f;
    float centerY = y + height * 0.5f;

    // トラック（幅いっぱい）
    layer->pushFill(
        x, centerY - trackHeight * 0.5f,
        width,
        trackHeight,
        trackHeight * 0.5f, 0.0f, 0.0f,
        trackColor, 0x00000000,
        0.5f, 1.0f, 0.0f,
        0x000000AA,
        zIndex,
        targetFBO, fbsize, viewId);

    // つまみサイズ（pageWidth / totalWidth の比率）
    float thumbRatio = pageWidth / totalWidth;
    float thumbWidth = width * thumbRatio;
    thumbWidth = std::max(thumbWidth, 16.f);  // 最小幅

    // つまみ位置
    float maxScroll = totalWidth - pageWidth;
    float scrollRatio = (maxScroll > 0) ? currentX / maxScroll : 0.f;
    scrollRatio = std::clamp(scrollRatio, 0.f, 1.f);
    float thumbX = x + (width - thumbWidth) * scrollRatio;

    // つまみ（角丸長方形）
    layer->pushFill(
        thumbX,
        centerY - trackHeight * 1.5f,
        thumbWidth,
        trackHeight * 2,
        trackHeight, 0.0f, 0.0f,
        thumbColor, 0x00000000,
        0.0f, 0.0f, 0.0f,
        0x00000000,
        zIndex + 0.1f,
        targetFBO, fbsize, viewId);
}

void drawRightScrollBar(LayerInfo* layer,
    float x, float y, float width, float height,
    float currentY, float pageHeight, float totalHeight,
    float zIndex,
    bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId) {

    // スクロール不要
    if (totalHeight <= pageHeight) {
        return;
    }

    const uint32_t trackColor = 0x40FFFFFF;
    const uint32_t thumbColor = 0xFFFFFFFF;

    float trackWidth = 4.f;
    float centerX = x + width * 0.5f;

    // トラック（高さいっぱい）
    layer->pushFill(
        centerX - trackWidth * 0.5f, y,
        trackWidth,
        height,
        trackWidth * 0.5f, 0.0f, 0.0f,
        trackColor, 0x00000000,
        0.5f, 1.0f, 0.0f,
        0x000000AA,
        zIndex,
        targetFBO, fbsize, viewId);

    // つまみサイズ（pageHeight / totalHeight の比率）
    float thumbRatio = pageHeight / totalHeight;
    float thumbHeight = height * thumbRatio;
    thumbHeight = std::max(thumbHeight, 16.f);  // 最小高さ

    // つまみ位置
    float maxScroll = totalHeight - pageHeight;
    float scrollRatio = (maxScroll > 0) ? currentY / maxScroll : 0.f;
    scrollRatio = std::clamp(scrollRatio, 0.f, 1.f);
    float thumbY = y + (height - thumbHeight) * scrollRatio;

    // つまみ（角丸長方形）
    layer->pushFill(
        centerX - trackWidth * 1.5f,
        thumbY,
        trackWidth * 2,
        thumbHeight,
        trackWidth, 0.0f, 0.0f,
        thumbColor, 0x00000000,
        0.0f, 0.0f, 0.0f,
        0x00000000,
        zIndex + 0.1f,
        targetFBO, fbsize, viewId);
}
void MeasureString(FontAtlas& atlas, FontId font, String* text, int n, float width, float* retwid, float* rethei, size_t* len, ExtendedRenderGroup* group) {

    *retwid = 0;
    *rethei = 0;

    for (*len = 0; *len < n; (*len)++) {
        uint32_t codepoint = (uint32_t)GetChar(text, *len);

        const auto& gi = atlas.getOrAddGlyph(font, codepoint, group);

        if (gi.width > 0 && gi.height > 0) {
            if (*retwid + gi.advance > width) {
                return;
            }
            *retwid += gi.advance;
            if (*rethei < gi.height) *rethei = gi.height;
        }
    }
    return;
}
// UTF-8対応版（1文字ずつデコード）
uint32_t utf8Decode(const char*& p) {
    uint32_t c = (unsigned char)*p++;
    if (c < 0x80) return c;

    int extraBytes = 0;
    if ((c & 0xE0) == 0xC0) { extraBytes = 1; c &= 0x1F; }
    else if ((c & 0xF0) == 0xE0) { extraBytes = 2; c &= 0x0F; }
    else if ((c & 0xF8) == 0xF0) { extraBytes = 3; c &= 0x07; }

    for (int i = 0; i < extraBytes; i++) {
        c = (c << 6) | ((unsigned char)*p++ & 0x3F);
    }
    return c;
}

void drawTextUTF8(LayerInfo* layer, FontAtlas& atlas, FontId font,
    const char* text, int length, float x, float y,
    int zIndex, uint32_t color, RenderGroup* group, bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId) {
    float cursorX = x;
    float cursorY = y;

    const char* p = text;
    for (int i = 0; i < length; i++) {
        uint32_t codepoint = utf8Decode(p);
        const auto& gi = atlas.getOrAddGlyph(font, codepoint, group);

        if (gi.width > 0 && gi.height > 0) {
            layer->pushText(cursorX + gi.bearingX, cursorY - gi.bearingY, gi.width, gi.height, gi.u0, gi.v0, gi.u1 - gi.u0, gi.v1 - gi.v0, color, zIndex, &atlas.getPageTexture(gi.pageIndex), targetFBO, fbsize, viewId);
        }

        cursorX += gi.advance;
    }
}



// bgfx初期化
bool initBgfx(SDL_Window* window) {
    bgfx::PlatformData pd;
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    memset(&pd, 0, sizeof(pd));

#ifdef __APPLE__
    __block int width = 800, height = 600;
#else
    int width = 800, height = 600;
#endif

#if defined(__ANDROID__)
    void* nwh = nullptr;
    // Wait for ANativeWindow to be ready (up to 5 seconds)
    for (int retry = 0; retry < 50 && nwh == nullptr; retry++) {
        nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
        if (nwh == nullptr) {
            SDL_Log("initBgfx: Waiting for ANativeWindow... (attempt %d)", retry + 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            props = SDL_GetWindowProperties(window);  // Refresh properties
        }
    }
    if (nwh == nullptr) {
        SDL_Log("initBgfx: FATAL - ANativeWindow is NULL after waiting!");
        return false;
    }
    pd.nwh = nwh;
    SDL_GetWindowSize(window, &width, &height);
    SDL_Log("initBgfx: Android nwh=%p, size=%dx%d", nwh, width, height);

#elif defined(_WIN32)
    void* hwnd = SDL_GetPointerProperty(props, "SDL.window.win32.hwnd", nullptr);
    pd.nwh = hwnd;

#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS || TARGET_OS_SIMULATOR
        // iOS: Metal layer from SDL
        // bgfx init must be called from main thread for iOS
        // Store metalView as static to prevent deallocation
        static SDL_MetalView s_metalView = nullptr;
        __block void* metalLayer = nullptr;
        __block bool bgfxInitResult = false;

        auto initBlock = ^{
            s_metalView = SDL_Metal_CreateView(window);
            metalLayer = SDL_Metal_GetLayer(s_metalView);
            SDL_GetWindowSize(window, &width, &height);
            SDL_Log("initBgfx: iOS metalView=%p, metalLayer=%p, size=%dx%d",
                    s_metalView, metalLayer, width, height);

            if (!metalLayer) {
                SDL_Log("initBgfx: Failed to get Metal layer!");
                bgfxInitResult = false;
                return;
            }

            bgfx::PlatformData pdLocal;
            memset(&pdLocal, 0, sizeof(pdLocal));
            pdLocal.nwh = metalLayer;

            // iOS single-thread mode: call renderFrame() before init()
            // This prevents bgfx from creating internal render thread
            bgfx::renderFrame();

            bgfx::Init initLocal;
            initLocal.platformData = pdLocal;
            initLocal.type = bgfx::RendererType::Metal;
            initLocal.resolution.width = width;
            initLocal.resolution.height = height;
            initLocal.resolution.reset = BGFX_RESET_VSYNC;
            initLocal.resolution.formatDepthStencil = bgfx::TextureFormat::D24S8;
            initLocal.debug = true;

            bgfxInitResult = bgfx::init(initLocal);
            if (bgfxInitResult) {
                SDL_Log("bgfx initialized successfully");
                // Force depth buffer creation via reset
                bgfx::reset(width, height, BGFX_RESET_VSYNC, bgfx::TextureFormat::D24S8);
                SDL_Log("bgfx reset with depth buffer");
            } else {
                SDL_Log("bgfx::init failed");
            }
        };

        if ([NSThread isMainThread]) {
            initBlock();
        } else {
            dispatch_sync(dispatch_get_main_queue(), initBlock);
        }

        return bgfxInitResult;
    #else
        // macOS
        void* cocoa_win = SDL_GetPointerProperty(props, "SDL.window.cocoa.window", nullptr);
        pd.nwh = cocoa_win;
    #endif

#elif defined(__linux__)
    // X11
    void* x11_display = SDL_GetPointerProperty(props, "SDL.window.x11.display", nullptr);
    uint64_t x11_window = (uint64_t)SDL_GetNumberProperty(props, "SDL.window.x11.window", 0);
    // Wayland
    void* wl_display = SDL_GetPointerProperty(props, "SDL.window.wayland.display", nullptr);
    void* wl_surface = SDL_GetPointerProperty(props, "SDL.window.wayland.surface", nullptr);

    if (wl_surface) {            // Wayland 優先
        pd.ndt = wl_display;
        pd.nwh = wl_surface;
        pd.type = bgfx::NativeWindowHandleType::Wayland;
        SDL_Log("initBgfx: Using Wayland");
    }
    else {                     // X11
        pd.ndt = x11_display;
        pd.nwh = (void*)(uintptr_t)x11_window;
        pd.type = bgfx::NativeWindowHandleType::Default;
        SDL_Log("initBgfx: Using X11 display=%p window=0x%lx", x11_display, (unsigned long)x11_window);
    }
    SDL_GetWindowSize(window, &width, &height);
    SDL_Log("initBgfx: Linux window size=%dx%d", width, height);
#endif

    bgfx::Init init;
    init.platformData = pd;
#if defined(__ANDROID__) || defined(__linux__)
    init.type = bgfx::RendererType::Vulkan;
#elif TARGET_OS_IOS || TARGET_OS_SIMULATOR
    init.type = bgfx::RendererType::Metal;
#endif
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_VSYNC;
#if defined(__linux__) && !defined(__ANDROID__)
    init.resolution.formatDepthStencil = bgfx::TextureFormat::D24S8;  // Enable depth buffer (Desktop Linux bgfx only)
#endif
    init.debug = true;  // デバッグ情報を有効

    if (!bgfx::init(init)) {
        SDL_Log("bgfx::init failed");
        return false;
    }
    SDL_Log("bgfx initialized successfully");

    // Force first black frame to prevent flicker on X11
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width, height);
    bgfx::touch(0);
    bgfx::frame();
    SDL_Log("First black frame sent");

    return true;
}
struct BatchCache {
    bgfx::DynamicVertexBufferHandle dvb = BGFX_INVALID_HANDLE;
    bgfx::DynamicIndexBufferHandle  dib = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;  // テク
    uint32_t numVertices = 0;
    uint32_t numIndices = 0;
    bool valid = false;
};
// NativeWindow は newelem.h で定義済み

// === メインスレッドへの SDL 委任用イベント（RenderThread → MainThread） ===
inline Uint32 SDL_EVENT_SDL_REQUEST = 0;

// SDL委任リクエスト（RenderThread → MainThread、SDL操作のみ）
struct SDLRequest {
    enum Type { CreateWindow, ResizeWindow, DestroyWindow, ShowWindow, HideWindow, MoveWindow };
    Type type;
    // CreateWindow 用
    NativeWindowType winType;
    SDL_Window* parentSDLWindow = nullptr;
    int x, y, w, h;
    bool visible = true;  // CreateWindow時に表示するか
    // ResizeWindow 用
    NativeWindow* target = nullptr;
    int newW, newH;
    // 結果
    SDL_Window* resultSDLWindow = nullptr;
    void* resultNwh = nullptr;
    bool success = false;
    std::promise<void> done;
};

// === ポップアップコマンド（GoThread → RenderThread、描画と同じキューで順序保証） ===
enum class PopupCmdType { Create, Resize, Destroy, Show, Hide, Move };

struct PopupCmd {
    PopupCmdType type;
    ThreadGC* thgc = nullptr;  // どのタブに属するか
    // Create 用
    NativeWindowType winType = WindowType_Popup;
    PopupAnchor anchor = Anchor_None;
    int x = 0, y = 0, w = 0, h = 0;
    NewElement* anchorElem = nullptr;
    bool visible = true;  // Create時に表示するか
    // Resize 用 / Move 用
    NativeWindow* target = nullptr;
    int newW = 0, newH = 0;
};

class HopStar {
public:
    std::vector<ThreadGC*> thgcs;
    int target_order = -1;
    ThreadGC* target;
    std::mutex m;
    int render = 0;
    std::condition_variable cv_;

    // === FontManagerで複数フォントを管理 ===
    PatternAtlasBuilder patternAtlas;
    ImageMaster master = ImageMaster{};
	ImageLoader loader = ImageLoader(master);

    // === ウィンドウ管理 ===
    // windows はタブ毎に ThreadGC::windows で管理
    uint8_t basicViewId = 30;             // 30から連番で割り当て

    // ポップアップコマンドキュー（GoThread → RenderThread）
    std::mutex popupCmdMutex;
    std::vector<PopupCmd> popupCmdQueue;

    // --- SDL操作をメインスレッドに委任して同期待ち ---
    void requestSDL(SDLRequest& req) {
        std::future<void> f = req.done.get_future();
        SDL_Event ev = {};
        ev.type = SDL_EVENT_SDL_REQUEST;
        ev.user.data1 = &req;
        SDL_PushEvent(&ev);
        f.get();  // メインスレッドが処理するまでブロック（~1ms）
    }

    // --- RenderThread から呼ばれるポップアップ破棄 ---
    void destroyPopupWindow(ThreadGC* thgc, NativeWindow* popup) {
        if (!popup) return;
        // タブのウィンドウリストから削除
        for (auto it = thgc->windows.begin(); it != thgc->windows.end(); ++it) {
            if (*it == popup) {
                thgc->windows.erase(it);
                break;
            }
        }
        // bgfx FBO 解放（RenderThread 上なので直接OK）
        if (bgfx::isValid(popup->fbo)) {
            bgfx::destroy(popup->fbo);
            popup->fbo = BGFX_INVALID_HANDLE;
        }
        // SDL ウィンドウ破棄をメインスレッドに委任
        if (popup->sdlWindow) {
            SDLRequest req{};
            req.type = SDLRequest::DestroyWindow;
            req.target = popup;
            requestSDL(req);
            popup->sdlWindow = nullptr;
        }
        delete popup;
    }

    void resolvePopupPosition(NativeWindow* popup) {
        if (!popup || !popup->sdlWindow) return;
        switch (popup->anchor) {
        case Anchor_Element: {
            auto* e = popup->anchorElement;
            if (!e) break;
            int relX = (int)(e->pos2.x + e->pos.x) + popup->anchorX;
            int relY = (int)(e->pos2.y + e->pos.y + e->size.y) + popup->anchorY;
            SDL_SetWindowPosition(popup->sdlWindow, relX, relY);
            break;
        }
        case Anchor_ParentWindow:
            SDL_SetWindowPosition(popup->sdlWindow, popup->anchorX, popup->anchorY);
            break;
        case Anchor_Screen: {
            if (popup->parent && popup->parent->sdlWindow) {
                int px, py;
                SDL_GetWindowPosition(popup->parent->sdlWindow, &px, &py);
                SDL_SetWindowPosition(popup->sdlWindow, popup->anchorX - px, popup->anchorY - py);
            } else {
                SDL_SetWindowPosition(popup->sdlWindow, popup->anchorX, popup->anchorY);
            }
            break;
        }
        default:
            break;
        }
    }

    // --- RenderThread から呼ばれるリサイズ処理（SDL部分はメインに委任） ---
    bool resizePopupWindow(NativeWindow* nw, int newW, int newH) {
        if (!nw || !nw->sdlWindow) return false;

        // SDL ウィンドウリサイズをメインスレッドに委任
        SDLRequest req{};
        req.type = SDLRequest::ResizeWindow;
        req.target = nw;
        req.newW = newW; req.newH = newH;
        requestSDL(req);

        if (!req.success) return false;

        // 内部サイズを更新
        nw->size = { newW, newH };

        // FBO を再作成（RenderThread 上なので直接OK）
        if (bgfx::isValid(nw->fbo)) {
            bgfx::destroy(nw->fbo);
        }
        nw->fbo = bgfx::createFrameBuffer(nw->nwh, (uint16_t)newW, (uint16_t)newH);
        return bgfx::isValid(nw->fbo);
    }

    NativeWindow* findWindowBySDLId(SDL_WindowID id) {
        for (auto* thgc : thgcs) {
            for (auto* nw : thgc->windows) {
                if (nw->sdlWindow && SDL_GetWindowID(nw->sdlWindow) == id) {
                    return nw;
                }
            }
        }
        return nullptr;
    }

    HopStar() {
	}
    void initFonts() {
        // TTF初期化
        if (!TTF_Init()) {
            SDL_Log("TTF_Init failed: %s", SDL_GetError());
        }
        // 複数のフォントを登録
        master.registerFont("sans", "fonts/NotoSansJP-Regular.ttf", 16);
        master.registerFont("sans", "fonts/NotoSansJP-Regular.ttf", 24);
        master.registerFont("sans", "fonts/NotoSansJP-Regular.ttf", 32);

        master.registerFont("noto", "fonts/NotoSerifJP-Regular.ttf", 16);
        master.registerFont("noto", "fonts/NotoSerifJP-Regular.ttf", 24);

        master.registerFont("robo", "fonts/RobotoMono-Regular.ttf", 14);
        master.registerFont("robo", "fonts/RobotoMono-Regular.ttf", 16);
    }
    void push_tab(ThreadGC* thgc) {
        std::lock_guard lock(m);
        target_order++;
        thgcs.insert(thgcs.begin() + static_cast<std::ptrdiff_t>(target_order), thgc);
        target = thgc;
    }

    // タブを削除
    void remove_tab(int n) {
        std::lock_guard lock(m);
        thgcs.erase(thgcs.begin() + n);
    }

    // タブを選択
    void select_tab(int n) {
        std::lock_guard lock(m);
        target = thgcs.begin()[n];
    }
    void RootOffscreen(ThreadGC* thgc, List* list, Offscreen* offscreen) {
        add_list(thgc, list, (char*)offscreen);
        for (Offscreen* off = offscreen->child; off != offscreen->child; off = off->next) {
            RootOffscreen(thgc, list, off);
        }
    }
    void buildFrame(uint64_t frameId) {
        if (invalidate == 0) {
            return;
        }
        std::lock_guard lock2(m);
        auto* q = target->commandQueue;

        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime_).count();
        lastTime_ = currentTime;
        time += deltaTime;

        // 各ウィンドウのツリーに対してRebuild/OffscreenLayoutを実行
        String* str = createString(target, "main", 4, 1);
        NewLocal* local = (NewLocal*)((TreeElement*)get_mapy(target->map, str))->elem;
        if ((local->dirty & DirtyType::Rebuild) > 0) {
            int n = 0;
            for (auto* nw : target->windows) {
                if (!nw->local) continue;
                nw->local->offscreen->child->next = nw->local->offscreen->child->before = nw->local->offscreen->child;
                RebuildOffscreen(target, nw->local->offscreen->child, nw->local, &n);
            }
        }
        if ((local->dirty & DirtyType::OffscreenLayout) > 0) {
            local->screens = (List*)create_list(target, sizeof(Offscreen*), CType::_Offscreen);
            for (auto* nw : target->windows) {
                if (!nw->local) continue;
                RootOffscreen(target, local->screens, nw->local->offscreen);
            }
        }

        TreeElement* te = (TreeElement*)get_mapy(target->map, createString(target, (char*)"main", 4, 1));
        local = (NewLocal*)te->elem;
        if ((local->dirty & DirtyType::Partial) > 0) {
            q->begin(frameId, 1024);
            auto& layer = q->setCurrentSlotLayer({}, 1.0f, true, true, 800, 600);
            for (int i = 0; i < local->screens->size; i++) {
                Offscreen* screen = (Offscreen*)*get_list(local->screens, i);
                if (screen->layout) {
                    NewMeasure measure = NewMeasure();
                    measure.pos.x = 0; measure.pos.y = 0; measure.size.x = screen->window->size.x; measure.size.y = screen->window->size.y;
                    measure.start.x = 0; measure.start.y = 0;
                    int order = 0;
                    screen->elem->Measure(target, (NewElement*)screen->elem, &measure, local, &order);
                }
            }
            int viewId = 30;
            for (int i = 0; i < local->screens->size; i++) {
                Offscreen* screen = (Offscreen*)*get_list(local->screens, i);
                if (screen->paint) {
                    Offscreen* screen = (Offscreen*)*get_list(local->screens, i);
                    NewGraphic* graphic = new NewGraphic();
                    graphic->pos.x = 0; graphic->pos.y = 0; graphic->size.x = screen->window->size.x; graphic->size.y = screen->window->size.y;
                    graphic->start.x = 0; graphic->start.y = 0; graphic->end.x = screen->window->size.x; graphic->end.y = screen->window->size.y;
                    graphic->fb = &screen->window->fbo; graphic->viewId = screen->window->viewId; graphic->viewId2 = --viewId; graphic->layer = &layer;
                    graphic->group = NULL; graphic->fbsize = &screen->window->size;
                    screen->elem->Draw(target, (NewElement*)screen->elem, graphic, local, q);
                }
            }
            if (local->select.from != NULL && local->select.window != nullptr) {
                NativeWindow* selWin = local->select.window;
                NewGraphic* graphic = new NewGraphic();
                graphic->pos.x = 0; graphic->pos.y = 0;
                graphic->size.x = (float)selWin->size.x; graphic->size.y = (float)selWin->size.y;
                graphic->start.x = 0; graphic->start.y = 0;
                graphic->end.x = (float)selWin->size.x; graphic->end.y = (float)selWin->size.y;
                graphic->fb = &selWin->fbo; graphic->viewId = selWin->viewId; graphic->layer = &layer;
                SelectDraw(target, local, graphic, q);
            }
            q->publish();
            render++;

            cv_.notify_one();
            invalidate = 1;
        }
    }

    int invalidate = 1;
    bool update = true;
    float time = 0;
    std::chrono::steady_clock::time_point lastTime_ = std::chrono::steady_clock::now();
};
int addPattern(ThreadGC* thgc, std::vector<float>& colors, std::vector<float>& widthes) {
    return thgc->hoppy->patternAtlas.addPattern(colors, widthes);
}
Generator MouseButton(HopStar* hoppy, MouseEvent* mv) {
    // メインウィンドウクリック時、開いているポップアップを閉じてイベント消費
    if (mv->action == SDL_EVENT_MOUSE_BUTTON_DOWN && !hoppy->target->openPopups.empty()) {
        auto popups = hoppy->target->openPopups;  // コピー（myHideWindowがopenPopupsを変更するため）
        for (auto* nw : popups) {
            if (nw->local && nw->local->type == LetterType::_Popup) {
                ((PopupWindow*)nw->local)->visible = false;
            }
            myHideWindow(hoppy->target, nw);
        }
        hoppy->invalidate = 1;
    }
    TreeElement* tree = (TreeElement*)get_mapy(hoppy->target->map, createString(hoppy->target, (char*)"main", 4, 1));
	tree->elem->Mouse(hoppy->target, tree->elem, mv, (NewLocal*)tree->elem);
	hoppy->invalidate = 1;
	co_return (char*)0;

}
Generator KeyButton(HopStar* hoppy, KeyEvent* kv) {
    bool select = false;
	TreeElement* tree = (TreeElement*)get_mapy(hoppy->target->map, createString(hoppy->target, (char*)"main", 4, 1));
    NewLocal* local = (NewLocal*)tree->elem;
	SelectKey(hoppy->target, local, kv);
    hoppy->update = true;
    hoppy->invalidate = 1;
    co_return (char*)0;
}
// ポップアップ用マウスイベント
Generator PopupMouseButton(HopStar* hoppy, NativeWindow* nw, MouseEvent* mv) {
    if (nw && nw->local) {
        TreeElement* tree = (TreeElement*)get_mapy(hoppy->target->map, createString(hoppy->target, (char*)"main", 4, 1));
        ((NewElement*)nw->local)->Mouse(hoppy->target, nw->local, mv, (NewLocal*)tree->elem);
        hoppy->invalidate = 1;
    }
    co_return (char*)0;
}
// ポップアップ用キーイベント
Generator PopupKeyButton(HopStar* hoppy, NativeWindow* nw, KeyEvent* kv) {
    if (nw && nw->local) {
        TreeElement* tree = (TreeElement*)get_mapy(hoppy->target->map, createString(hoppy->target, (char*)"main", 4, 1));
        SelectKey(hoppy->target, (NewLocal*)tree->elem, kv);
        hoppy->update = true;
        hoppy->invalidate = 1;
    }
    co_return (char*)0;
}
std::promise<void> initDone;
class RenderThread {
public:
	RenderThread(HopStar* hoppy, SDL_Window* window) : hoppy_(hoppy), window_(window), running_(false) {
    }
    void start() {
        running_ = true;
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
        // iOS: Don't create thread, will be called from main thread via CADisplayLink
        // Initialize on main thread
        initOnMainThread();
#else
        thread_ = std::thread(&RenderThread::run, this);
#endif
    }

    void stop() {
        running_ = false;
#if !(TARGET_OS_IOS || TARGET_OS_SIMULATOR)
        if (thread_.joinable()) {
            thread_.join();
        }
#endif
    }

#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
    bool initialized_ = false;
    std::unique_ptr<nle::AudioOutput> audio_output_ios_;

    void initOnMainThread() {
        initBgfx(window_);
        hoppy_->master.initialize();
        hoppy_->initFonts();
        initDone.set_value();

        auto video_track = timeline_.add_video_track();
        auto video_clip = video_track->add_clip();

        std::string videoPath = getBundlePath("pyonpyon.mp4");
        if (!video_clip->open(videoPath.c_str())) {
            SDL_Log("Video file not found: %s", videoPath.c_str());
            return;
        }

        video_clip->timeline_range.start = 0;
        video_clip->timeline_range.end = video_clip->source_range.duration();

        auto audio_track = timeline_.add_audio_track();
        auto audio_clip = audio_track->add_clip();

        if (audio_clip->open(videoPath.c_str())) {
            audio_clip->timeline_range.start = 0;
            audio_clip->timeline_range.end = audio_clip->source_range.duration();
        }

        audio_output_ios_ = std::make_unique<nle::AudioOutput>(timeline_);
        if (!audio_output_ios_->initialize()) {
            SDL_Log("Audio output init failed");
        }

        renderer_ = std::make_unique<nle::TimelineRenderer<ImageMaster>>(timeline_, hoppy_->master);
        timeline_.play();

        initialized_ = true;
        SDL_Log("Render thread started");
    }

    RenderResources resources_ios_{};
    bool resources_initialized_ = false;

    void renderOneFrame() {
        if (!running_ || !initialized_) return;

        // Initialize resources once
        if (!resources_initialized_) {
            QuadVertex::init();
            initRenderResources(resources_ios_);
            resources_initialized_ = true;
        }

        // Process deferred GPU uploads from other threads
        // Must happen on main thread in bgfx single-thread mode
        hoppy_->loader.processGpuUploadsMainThread();

        std::vector<LayerInfo>* layers = NULL;
        uint64_t frameId = 0;
        int token = -1;
        RenderCommandQueue* queue = NULL;

        // Non-blocking check for render data (iOS can't block main thread)
        {
            std::unique_lock<std::mutex> lock(hoppy_->m, std::try_to_lock);
            if (lock.owns_lock() && hoppy_->render > 0) {
                hoppy_->render--;
                queue = hoppy_->target->commandQueue;
                queue->acquire(&layers, frameId, token);
            }
        }

        {
            std::unique_lock<std::mutex> lock3(hoppy_->patternAtlas.m, std::try_to_lock);
            if (lock3.owns_lock()) {
                hoppy_->patternAtlas.reset();
                hoppy_->patternAtlas.buildTextures();
            }
        }

        if (layers != NULL) {
            hoppy_->master.beginFrame();
            for (auto& layer : *layers) {
                renderLayer(NULL, layer, resources_ios_, hoppy_->master, hoppy_->patternAtlas);
            }

            renderer_->update();
            auto render_items = renderer_->get_render_items();

            // Setup view 30 for video rendering (with depth buffer)
            if (!render_items.empty()) {
                int winW, winH;
                SDL_GetWindowSize(window_, &winW, &winH);
                // Update layout size to match actual window size
                width_ = winW;
                height_ = winH;
                bgfx::setViewFrameBuffer(30, BGFX_INVALID_HANDLE);
                bgfx::setViewClear(30, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
                bgfx::setViewRect(30, 0, 0, uint16_t(winW), uint16_t(winH));
                float orthoProj[16];
                bx::mtxOrtho(orthoProj, 0.0f, float(winW), float(winH), 0.0f, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
                bgfx::setViewTransform(30, NULL, orthoProj);
            }

            for (size_t i = 0; i < render_items.size(); i++) {
                std::vector<UnifiedDrawCommand> commands;
                commands.reserve(render_items.size());

                float base_z = 0.0f;
                for (size_t j = 0; j < render_items.size(); j++) {
                    const auto& item = render_items[j];
                    auto resolved = hoppy_->master.resolve(item.image_id);

                    if (!resolved.isReady()) continue;

                    float px, py, pw, ph;
                    calculate_layout_pixels(j, render_items.size(), item.clip, px, py, pw, ph);

                    UnifiedDrawCommand cmd{};
                    cmd.type = DrawCommandType::Image;
                    cmd.viewId = hoppy_->target->windows[0]->viewId;
                    cmd.zIndex = base_z + j * 0.001f;
                    cmd.texture = resolved.resolved.texture;
                    cmd.texture2 = &nulltex;

                    cmd.x = px;
                    cmd.y = py;
                    cmd.width = pw;
                    cmd.height = ph;

                    cmd.angle = 0.0f;
                    cmd.dataOffset = 0.0f;
                    cmd.scrollX = 1.0f;
                    cmd.scrollY = 1.0f;

                    cmd.radius = 0.0f;
                    cmd.aa = 1.0f;
                    cmd.shadowX = 0.0f;
                    cmd.shadowY = 0.0f;
                    cmd.shadowBlur = 0.0f;
                    cmd.borderWidth = 0.0f;

                    cmd.shadowColor = 0x00000000;
                    cmd.fillColor = 0xFFFFFFFF;
                    cmd.borderColor = 0x00000000;
                    std::vector<UnifiedDrawCommand*> single_batch = { &cmd };
                    drawUnifiedBatch(single_batch, resources_ios_,
                        BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, 30);
                }
            }
        }

        bgfx::frame();
        // bgfx::renderFrame() は init前の1回だけ。毎フレームは呼ばない
    }
#endif
    enum class LayoutMode {
        Stack,   // PiP形式
        Grid,    // グリッド
        Single   // 単一表示
    };
	nle::Timeline timeline_;
    std::unique_ptr<nle::TimelineRenderer<ImageMaster>> renderer_;
    std::unique_ptr<nle::AudioOutput> audio_output_;
    size_t selected_track_ = 0;
    int width_ = 1280, height_ = 720;
    LayoutMode layout_mode_ = LayoutMode::Stack;
    private:void calculate_layout_pixels(size_t index, size_t total,
        std::shared_ptr<nle::VideoClip> clip,
        float& x, float& y, float& w, float& h) {
        float video_aspect = (float)clip->get_width() / clip->get_height();

        switch (layout_mode_) {
        case LayoutMode::Stack:
            if (index == 0) {
                // フルスクリーン（アスペクト比維持）
                float screen_aspect = (float)width_ / height_;
                if (video_aspect > screen_aspect) {
                    w = (float)width_;
                    h = w / video_aspect;
                    x = 0;
                    y = (height_ - h) * 0.5f;
                }
                else {
                    h = (float)height_;
                    w = h * video_aspect;
                    x = (width_ - w) * 0.5f;
                    y = 0;
                }
            }
            else {
                // PiP: 右上に並べる
                float pip_scale = 0.25f;
                w = width_ * pip_scale;
                h = w / video_aspect;
                float margin = 10.0f;
                x = width_ - w - margin;
                y = margin + (h + margin) * (index - 1);
            }
            break;

        case LayoutMode::Grid: {
            int cols = (int)std::ceil(std::sqrt((double)total));
            int rows = (int)std::ceil((double)total / cols);
            int col = index % cols;
            int row = index / cols;

            float cell_w = (float)width_ / cols;
            float cell_h = (float)height_ / rows;
            float cell_aspect = cell_w / cell_h;

            if (video_aspect > cell_aspect) {
                w = cell_w;
                h = w / video_aspect;
            }
            else {
                h = cell_h;
                w = h * video_aspect;
            }

            x = col * cell_w + (cell_w - w) * 0.5f;
            y = row * cell_h + (cell_h - h) * 0.5f;
            break;
        }

        case LayoutMode::Single:
        default:
            if (index == selected_track_) {
                float screen_aspect = (float)width_ / height_;
                if (video_aspect > screen_aspect) {
                    w = (float)width_;
                    h = w / video_aspect;
                    x = 0;
                    y = (height_ - h) * 0.5f;
                }
                else {
                    h = (float)height_;
                    w = h * video_aspect;
                    x = (width_ - w) * 0.5f;
                    y = 0;
                }
            }
            else {
                x = y = w = h = 0;  // 非表示
            }
            break;
        }
    }
    void run() {
        if (!initBgfx(window_)) {
            SDL_Log("RenderThread: bgfx initialization failed, aborting render thread");
            initDone.set_value();  // Signal anyway to unblock waiting threads
            return;
        }
        //bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_IFH);
        hoppy_->master.initialize();
        hoppy_->initFonts();
        initDone.set_value();
        videoLoaded_ = false;
        std::string videoPath = getBundlePath("pyonpyon.mp4");
        auto video_track = timeline_.add_video_track();
        auto video_clip = video_track->add_clip();

        if (video_clip->open(videoPath.c_str())) {
            video_clip->timeline_range.start = 0;
            video_clip->timeline_range.end = video_clip->source_range.duration();
            videoLoaded_ = true;
            SDL_Log("Video loaded successfully: %s", videoPath.c_str());
        } else {
            SDL_Log("Video file not found: %s (continuing without video)", videoPath.c_str());
        }

        // オーディオトラック（同じファイルから音声を抽出）
        auto audio_track = timeline_.add_audio_track();
        auto audio_clip = audio_track->add_clip();

        if (videoLoaded_ && audio_clip->open(videoPath.c_str())) {
            audio_clip->timeline_range.start = 0;
            audio_clip->timeline_range.end = audio_clip->source_range.duration();
            SDL_Log("Audio clip loaded successfully");
        } else {
            SDL_Log("Audio clip failed to load or video not loaded");
        }
        audio_output_ = std::make_unique<nle::AudioOutput>(timeline_);
        if (!audio_output_->initialize()) {
            SDL_Log("AudioOutput initialization failed");
        }
        renderer_ = std::make_unique <nle::TimelineRenderer<ImageMaster >>(timeline_, hoppy_->master);
        if (videoLoaded_) {
            timeline_.play();
        }
        /*auto id = engine.load("banban.mp3");
        engine.play(id);*/
        /*std::wstring wpath = L"おどれ！！マグロっ子.mp3";
        std::string utf8 = utf16_to_utf8(wpath);
		auto id2 = engine.load(utf8.c_str());
        engine.play(id2);*/
        QuadVertex::init();
        // シェーダー読み込み
        RenderResources resources{};
        initRenderResources(resources);
        // ビュー設定
        bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x808080ff, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, 800, 600);
        // 射影行列設定（2D用）
        float orthoProj[16];
        bx::mtxOrtho(orthoProj, 0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, NULL, orthoProj);
        int frameCount = 0;
        auto lastFpsTime = std::chrono::high_resolution_clock::now();

        int scrollX = 0;
        int scrollY = 0;// ★ ビュー0の設定（スクリーン描画用）

        SDL_Log("Render thread started");
        bool rendered = false;
        NewLocal* local = NULL;
        int oldtoken = -1;
        while (running_) {
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<LayerInfo>* layers = NULL;
            uint64_t frameId = 0;
            int token = -1;
            bool gotNew = false;
            RenderCommandQueue* queue = NULL;
            {
                std::unique_lock<std::mutex> lock(hoppy_->m);
                hoppy_->cv_.wait(lock, [this] {
                    return hoppy_->render > 0;
                });
                if (hoppy_->render > 0) hoppy_->render--;;
                bool gotNewLocal = false;
                queue = hoppy_->target->commandQueue;
                gotNew = hoppy_->target->commandQueue->acquire(&layers, frameId, token);
            }
            // === ポップアップコマンドを処理（描画前に順序保証） ===
            {
                std::vector<PopupCmd> cmds;
                {
                    std::lock_guard<std::mutex> lock(hoppy_->popupCmdMutex);
                    cmds.swap(hoppy_->popupCmdQueue);
                }
                for (auto& cmd : cmds) {
                    switch (cmd.type) {
                    case PopupCmdType::Create: {
                        // GoThread が確保済みの NativeWindow に SDL/FBO を埋める
                        NativeWindow* nw = cmd.target;
                        ThreadGC* cmdThgc = cmd.thgc;
                        NativeWindow* parent = nullptr;
                        if (cmdThgc && !cmdThgc->windows.empty()) parent = cmdThgc->windows[0];
                        nw->parent = parent;

                        // SDL座標を計算
                        int relX = cmd.x, relY = cmd.y;
                        if (cmd.anchor == Anchor_Element && cmd.anchorElem) {
                            relX = (int)(cmd.anchorElem->pos2.x + cmd.anchorElem->pos.x) + cmd.x;
                            relY = (int)(cmd.anchorElem->pos2.y + cmd.anchorElem->pos.y + cmd.anchorElem->size.y) + cmd.y;
                        }

                        // SDL ウィンドウ作成をメインスレッドに委任
                        SDLRequest req{};
                        req.type = SDLRequest::CreateWindow;
                        req.winType = cmd.winType;
                        req.parentSDLWindow = parent ? parent->sdlWindow : nullptr;
                        req.x = relX; req.y = relY; req.w = cmd.w; req.h = cmd.h;
                        req.visible = cmd.visible;
                        hoppy_->requestSDL(req);

                        if (req.success) {
                            nw->sdlWindow = req.resultSDLWindow;
                            nw->nwh = req.resultNwh;
                            nw->visible = cmd.visible;
                            // FBO作成（RenderThread上なので直接OK）
                            nw->fbo = bgfx::createFrameBuffer(nw->nwh, (uint16_t)cmd.w, (uint16_t)cmd.h);
                            nw->viewId = hoppy_->basicViewId++;
                            if (cmdThgc) cmdThgc->windows.push_back(nw);
                        } else {
                            SDL_Log("createPopupWindow failed via RenderThread");
                        }
                        break;
                    }
                    case PopupCmdType::Resize:
                        if (cmd.target) hoppy_->resizePopupWindow(cmd.target, cmd.newW, cmd.newH);
                        break;
                    case PopupCmdType::Destroy:
                        if (cmd.target && cmd.thgc) hoppy_->destroyPopupWindow(cmd.thgc, cmd.target);
                        break;
                    case PopupCmdType::Show:
                        if (cmd.target && cmd.target->sdlWindow) {
                            SDLRequest req{};
                            req.type = SDLRequest::ShowWindow;
                            req.target = cmd.target;
                            hoppy_->requestSDL(req);
                            cmd.target->visible = true;
                        }
                        break;
                    case PopupCmdType::Hide:
                        if (cmd.target && cmd.target->sdlWindow) {
                            SDLRequest req{};
                            req.type = SDLRequest::HideWindow;
                            req.target = cmd.target;
                            hoppy_->requestSDL(req);
                            cmd.target->visible = false;
                        }
                        break;
                    case PopupCmdType::Move:
                        if (cmd.target && cmd.target->sdlWindow) {
                            cmd.target->anchorX = cmd.x;
                            cmd.target->anchorY = cmd.y;
                            SDLRequest req{};
                            req.type = SDLRequest::MoveWindow;
                            req.target = cmd.target;
                            req.x = cmd.x; req.y = cmd.y;
                            hoppy_->requestSDL(req);
                        }
                        break;
                    }
                }
            }

            {
                std::lock_guard<std::mutex> lock3(hoppy_->patternAtlas.m);
                hoppy_->patternAtlas.reset();
                hoppy_->patternAtlas.buildTextures();
            }
            auto t1 = std::chrono::high_resolution_clock::now();
            hoppy_->loader.processGpuUploadsMainThread();
            if (layers != NULL) {
                hoppy_->master.beginFrame();
                for (auto& layer : *layers) {
                    renderLayer(local, layer, resources, hoppy_->master, hoppy_->patternAtlas);
                }
                rendered = true;
                // layers構築処理
                auto end = std::chrono::high_resolution_clock::now();

                // Only process video renderer if video was loaded
                if (videoLoaded_) {
                    renderer_->update();
                    auto render_items = renderer_->get_render_items();

                    // レイヤー順に描画（下から上へ）
                    for (size_t i = 0; i < render_items.size(); i++) {
                        std::vector<UnifiedDrawCommand> commands;
                        commands.reserve(render_items.size());

                        float base_z = 0.0f;
                        for (size_t i = 0; i < render_items.size(); i++) {
                            const auto& item = render_items[i];
                            auto resolved = hoppy_->master.resolve(item.image_id);

                            if (!resolved.isReady()) continue;

                            // レイアウト計算（ピクセル座標）
                            float px, py, pw, ph;
                            calculate_layout_pixels(i, render_items.size(), item.clip, px, py, pw, ph);

                            UnifiedDrawCommand cmd{};
                            cmd.type = DrawCommandType::Image;
                            cmd.viewId = 0;
                            cmd.zIndex = base_z + i * 0.001f;
                            cmd.texture = resolved.resolved.texture;
                            cmd.texture2 = &nulltex;

                            // 位置とサイズ（ピクセル座標）
                            cmd.x = px;
                            cmd.y = py;
                            cmd.width = pw;
                            cmd.height = ph;

                            // Image モード: UV座標
                            cmd.angle = 0.0f;      // uvMin.x
                            cmd.dataOffset = 0.0f; // uvMin.y
                            cmd.scrollX = 1.0f;    // uvMax.x
                            cmd.scrollY = 1.0f;    // uvMax.y

                            // 角丸、シャドウなし
                            cmd.radius = 0.0f;
                            cmd.aa = 1.0f;
                            cmd.shadowX = 0.0f;
                            cmd.shadowY = 0.0f;
                            cmd.shadowBlur = 0.0f;
                            cmd.borderWidth = 0.0f;

                            cmd.shadowColor = 0x00000000;
                            cmd.fillColor = 0xFFFFFFFF;
                            cmd.borderColor = 0x00000000;
                            std::vector<UnifiedDrawCommand*> single_batch = { &cmd };
                            drawUnifiedBatch(single_batch, resources,
                                BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, 30);
                        }
                    }
                }

                frameN = bgfx::frame();
                // bgfx::renderFrame() は init前の1回だけ。毎フレームは呼ばない
            }
        }
    }
    HopStar* hoppy_;
    SDL_Window* window_;
    std::thread thread_;
    std::atomic<bool> running_;
    bool videoLoaded_ = false;
};
FontAtlas* getAtlas(ThreadGC* thgc) {
    return &thgc->hoppy->master.fontAtlas();
}
RenderCommandQueue* createRCQ() {
	return new RenderCommandQueue();
}
ExtendedRenderGroup& createGroup(ThreadGC* thgc) {
    return thgc->hoppy->master.createGroup(0);  // SurfaceId = 0
}

ImageId myloadTexture2D(ThreadGC* thgc, const char* path, ImageUsage usage) {
    return thgc->hoppy->loader.loadFromFile(path, usage, false);
}
ImageId queueOffscreenNew(ThreadGC* thgc, int width, int height) {
    return thgc->hoppy->master.reserveOffscreenTexture(width, height, false);
}
void queueOffscreenResize(ThreadGC* thgc, ImageId offscreenid, int width, int height) {
    thgc->hoppy->master.queueOffscreenResize(offscreenid, width, height);
}
ResolvedTexture myresolveTexture(ImageMaster& master, ImageId imageId) {
    return master.resolveForDraw(imageId);
}
bgfx::TextureHandle* myResolveTexturePtr(ThreadGC* thgc, ImageId imageId) {
    return thgc->hoppy->master.resolveTexturePtr(imageId);
}
ResolvedTexture myResolveForDraw(ThreadGC* thgc, ImageId imageId) {
    return thgc->hoppy->master.resolveForDraw(imageId);
}
StandaloneTextureInfo* mygetStandaloneTextureInfo(ThreadGC* thgc, ImageId imageId) {
    return thgc->hoppy->master.getStandaloneTexture(imageId);
}
// === ポップアップウィンドウ管理ラッパー（GoThread → RenderThread キュー経由） ===

// ポップアップ生成: NativeWindow* を即座に返す（SDL/FBO は RenderThread が後で埋める）
NativeWindow* myCreatePopupWindow(ThreadGC* thgc, NativeWindowType type, PopupAnchor anchor,
                                   int x, int y, int w, int h, NewElement* anchorElem,
                                   bool visible) {
    // GoThread側でNativeWindowを先に確保（ポインタを即座に返せる）
    NativeWindow* nw = new NativeWindow{};
    nw->type = type;
    nw->anchor = anchor;
    nw->anchorX = x; nw->anchorY = y;
    nw->size = { w, h };
    nw->anchorElement = anchorElem;
    nw->visible = visible;

    PopupCmd cmd{};
    cmd.type = PopupCmdType::Create;
    cmd.thgc = thgc;
    cmd.winType = type;
    cmd.anchor = anchor;
    cmd.x = x; cmd.y = y; cmd.w = w; cmd.h = h;
    cmd.anchorElem = anchorElem;
    cmd.visible = visible;
    cmd.target = nw;  // 先に確保したNativeWindowを渡す
    {
        std::lock_guard<std::mutex> lock(thgc->hoppy->popupCmdMutex);
        thgc->hoppy->popupCmdQueue.push_back(cmd);
    }
    return nw;  // 即座に返す。描画命令より先にRenderThreadが初期化する
}

// ポップアップリサイズ（キューに積むだけ、ブロックしない）
void myResizePopupWindow(ThreadGC* thgc, NativeWindow* popup, int newW, int newH) {
    if (!popup) return;
    PopupCmd cmd{};
    cmd.type = PopupCmdType::Resize;
    cmd.target = popup;
    cmd.newW = newW; cmd.newH = newH;
    {
        std::lock_guard<std::mutex> lock(thgc->hoppy->popupCmdMutex);
        thgc->hoppy->popupCmdQueue.push_back(cmd);
    }
}

// ポップアップ破棄（キューに積むだけ、ブロックしない）
void myDestroyPopupWindow(ThreadGC* thgc, NativeWindow* popup) {
    if (!popup) return;
    PopupCmd cmd{};
    cmd.type = PopupCmdType::Destroy;
    cmd.thgc = thgc;
    cmd.target = popup;
    {
        std::lock_guard<std::mutex> lock(thgc->hoppy->popupCmdMutex);
        thgc->hoppy->popupCmdQueue.push_back(cmd);
    }
}

// ウィンドウ表示（キューに積むだけ、ブロックしない）
void myShowWindow(ThreadGC* thgc, NativeWindow* nw) {
    if (!nw) return;
    {
        std::lock_guard<std::mutex> lock(thgc->hoppy->popupCmdMutex);
        // 同じウィンドウのHideが未処理なら相殺して何もしない
        auto& q = thgc->hoppy->popupCmdQueue;
        for (auto it = q.begin(); it != q.end(); ++it) {
            if (it->target == nw && it->type == PopupCmdType::Hide) {
                q.erase(it);
                goto done_show;
            }
        }
        {
            PopupCmd cmd{};
            cmd.type = PopupCmdType::Show;
            cmd.target = nw;
            q.push_back(cmd);
        }
        done_show:;
    }
    // openPopups に追加（重複防止）
    auto& op = thgc->openPopups;
    if (std::find(op.begin(), op.end(), nw) == op.end()) {
        op.push_back(nw);
    }
}

// ウィンドウ非表示（キューに積むだけ、ブロックしない）
void myHideWindow(ThreadGC* thgc, NativeWindow* nw) {
    if (!nw) return;
    {
        std::lock_guard<std::mutex> lock(thgc->hoppy->popupCmdMutex);
        // 同じウィンドウのShowが未処理なら相殺して何もしない
        auto& q = thgc->hoppy->popupCmdQueue;
        for (auto it = q.begin(); it != q.end(); ++it) {
            if (it->target == nw && it->type == PopupCmdType::Show) {
                q.erase(it);
                goto done_hide;
            }
        }
        {
            PopupCmd cmd{};
            cmd.type = PopupCmdType::Hide;
            cmd.target = nw;
            q.push_back(cmd);
        }
        done_hide:;
    }
    // openPopups から削除
    auto& op = thgc->openPopups;
    op.erase(std::remove(op.begin(), op.end(), nw), op.end());
}

// ウィンドウ位置変更（キューに積むだけ、ブロックしない）
void myMoveWindow(ThreadGC* thgc, NativeWindow* nw, int x, int y) {
    if (!nw) return;
    PopupCmd cmd{};
    cmd.type = PopupCmdType::Move;
    cmd.target = nw;
    cmd.x = x; cmd.y = y;
    {
        std::lock_guard<std::mutex> lock(thgc->hoppy->popupCmdMutex);
        thgc->hoppy->popupCmdQueue.push_back(cmd);
    }
}
