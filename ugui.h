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

    for(int i = 0; i < text->size / text->esize; i++){
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
class HopStar {
public:
    std::vector<ThreadGC*> thgcs;
    int target_order = -1;
    ThreadGC* target;
    std::mutex m;
    NewLocal* local;
    int render = 0;
    std::condition_variable cv_;

    // === FontManagerで複数フォントを管理 ===
    PatternAtlasBuilder patternAtlas;
    ImageMaster master = ImageMaster{};
	ImageLoader loader = ImageLoader(master);
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

    void buildFrame(uint64_t frameId) {


        if (invalidate == 0) {
            return;
        }
        viewId = 30;
        std::lock_guard lock2(m);
        auto* q = target->commandQueue;

        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime_).count();
        lastTime_ = currentTime;
        time += deltaTime;
        TreeElement* te = (TreeElement*)get_mapy(target->map, createString(target, (char*)"main", 4, 1));
        local = (NewLocal*)te->elem;
		local->dirty = DirtyType::RebuildValue;
        if ((local->dirty & DirtyType::Partial) > 0) {
            q->begin(frameId, 1024);
            auto& layer = q->setCurrentSlotLayer({}, 1.0f, true, true, 800, 600);
            if ((local->dirty & DirtyType::Rebuild) > 0) {
                List* screens = create_list(target, sizeof(Offscreen*), true);
                add_list(target, screens, (char*)local->offscreen);
                RebuildOffscreen(target, screens, local);
                for (int i = 0; i < local->screens->size; i++) {
                    auto offscreen = *(Offscreen**)get_list(local->screens, i);
                    for (int j = 0; ; j++) {
                        if (j == screens->size) {
                            master.destroyOffscreenInternal(offscreen->imPing);
							master.destroyOffscreenInternal(offscreen->imPong);
                        }
                        auto offscreen2 = *(Offscreen**)get_list(screens, j);
                        if (offscreen == offscreen2) break;
                    }
                }
                local->screens = screens;
            }
            for (int i = 0; i < local->screens->size; i++) {
                Offscreen* screen = (Offscreen*)*get_list(local->screens, i);
                if (screen->layout) {
                    NewMeasure measure = NewMeasure();
                    measure.pos.x = 0; measure.pos.y = 0; measure.size.x = 800; measure.size.y = 600;
                    measure.start.x = 0; measure.start.y = 0;
                    int order = 0;
                    screen->elem->Measure(target, (NewElement*)screen->elem, &measure, local, &order);
                }
            }
            for (int i = 0; i < local->screens->size; i++) {
                Offscreen* screen = (Offscreen*)*get_list(local->screens, i);
                if (screen->paint) {
                    Offscreen* screen = (Offscreen*)*get_list(local->screens, i);
                    NewGraphic* graphic = new NewGraphic();
                    graphic->pos.x = 0; graphic->pos.y = 0; graphic->size.x = 800; graphic->size.y = 600;
                    graphic->start.x = 0; graphic->start.y = 0; graphic->end.x = 800; graphic->end.y = 600;
                    graphic->fb = &nullfb; graphic->viewId = viewId--; graphic->layer = &layer;
                    screen->elem->Draw(target, (NewElement*)screen->elem, graphic, local, q);
                }
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
/*Generator MouseButton(HopStar* hoppy, MouseEvent* mv) {
    TreeElement* tree = (TreeElement*)get_mapy(hoppy->target->map, createString(hoppy->target, (char*)"main", 4, 1));
	tree->elem->Mouse(hoppy->target, tree->elem, mv, (Local*)tree->elem);
	hoppy->invalidate = 1;
	co_return (char*)0;

}
Generator KeyButton(HopStar* hoppy, KeyEvent* kv) {
    bool select = false;
	TreeElement* tree = (TreeElement*)get_mapy(hoppy->target->map, createString(hoppy->target, (char*)"main", 4, 1));
    NewLocal* local = (NewLocal*)tree->elem;
	tree->elem->Key(hoppy->target, tree->elem, kv, local, &select);
    local->seln = -1;
    local->selects[0]->state->n = local->selects[1]->state->n = 0;
    hoppy->update = true;
    hoppy->invalidate = 1;
    co_return (char*)0;
}*/
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
                std::vector<bgfx::TextureHandle> textures;

                commands.reserve(render_items.size());
                textures.reserve(render_items.size());

                float base_z = 0.0f;
                for (size_t j = 0; j < render_items.size(); j++) {
                    const auto& item = render_items[j];
                    auto resolved = hoppy_->master.resolve(item.image_id);

                    if (!resolved.isReady()) continue;

                    float px, py, pw, ph;
                    calculate_layout_pixels(j, render_items.size(), item.clip, px, py, pw, ph);

                    textures.push_back(resolved.resolved.texture);

                    UnifiedDrawCommand cmd{};
                    cmd.type = DrawCommandType::Image;
                    cmd.viewId = 30;
                    cmd.zIndex = base_z + j * 0.001f;
                    cmd.texture = &textures.back();
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
            {
                std::lock_guard<std::mutex> lock3(hoppy_->patternAtlas.m);
                hoppy_->patternAtlas.reset();
                hoppy_->patternAtlas.buildTextures();
            }
            auto t1 = std::chrono::high_resolution_clock::now();
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
                        std::vector<bgfx::TextureHandle> textures;  // テクスチャハンドルを保持

                        commands.reserve(render_items.size());
                        textures.reserve(render_items.size());

                        float base_z = 0.0f;
                        for (size_t i = 0; i < render_items.size(); i++) {
                            const auto& item = render_items[i];
                            auto resolved = hoppy_->master.resolve(item.image_id);

                            if (!resolved.isReady()) continue;

                            // レイアウト計算（ピクセル座標）
                            float px, py, pw, ph;
                            calculate_layout_pixels(i, render_items.size(), item.clip, px, py, pw, ph);

                            // テクスチャを保存
                            textures.push_back(resolved.resolved.texture);

                            UnifiedDrawCommand cmd{};
                            cmd.type = DrawCommandType::Image;
                            cmd.viewId = 0;
                            cmd.zIndex = base_z + i * 0.001f;
                            cmd.texture = &textures.back();
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

ImageId myloadTexture2D(ThreadGC* thgc, const char* path) {
    return thgc->hoppy->loader.loadFromFile(path, ImageUsage::Background, false);
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
StandaloneTextureInfo* mygetStandaloneTextureInfo(ThreadGC* thgc, ImageId imageId) {
    return thgc->hoppy->master.getStandaloneTexture(imageId);
}
