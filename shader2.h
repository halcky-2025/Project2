#pragma once
#include <bgfx/bgfx.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
#import <Foundation/Foundation.h>
#ifndef BUNDLE_PATH_DEFINED
#define BUNDLE_PATH_DEFINED
// Helper to get bundle resource path on iOS (for shaders and resources)
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
#endif
#endif

// ============================================================
// 共通定義
// ============================================================

struct RenderResources {
    bgfx::VertexBufferHandle quadVB = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle quadIB = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle paletteUniform = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle widthsUniform = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle param1Uniform = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle uniteProgram = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle pageProgram = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle placeholderTexture = BGFX_INVALID_HANDLE; // 1x1 white texture for placeholder
};

// 基底コマンド
struct DrawCommand {
    enum DrawCommandType type;
    bgfx::FrameBufferHandle* targetFBO = NULL;
    PointI* fbsize;
    uint8_t viewId = 0;
	float zIndex = 0.0f;
    bgfx::TextureHandle* texture = nullptr;
    bgfx::TextureHandle* texture2 = nullptr;
};

struct UnifiedDrawCommand : DrawCommand {
    // i_data0
    float x, y, width, height;

    // i_data1
    float colorCount;   // Pattern: colorCount, Image: 未使用, PageCurl: uvMin.x (pack用)
    float dataOffset;   // Pattern: dataOffset, Image: uvMin.y, PageCurl: uvMin.y (pack用)
    float backUVMinX = 0.0f;    // PageCurl: backUVMin.x (pack用)
    float backUVMinY = 0.0f;    // PageCurl: backUVMin.y (pack用)

    float angle;        // Pattern: angle, Image: uvMin.x, PageCurl: curlAngle
    // i_data1.w / PageCurl用
    float curlRadius = 0.0f;    // PageCurl: curlRadius

    // i_data1.y 用 (PageCurlのみ)

    // i_data2
    float scrollX;      // Pattern: scrollX, Image: uvMax.x, PageCurl: uvSize.x
    float scrollY;      // Pattern: scrollY, Image: uvMax.y, PageCurl: uvSize.y
    float radius;       // Pattern/Image: radius, PageCurl: progress
    float aa;

    // i_data3
    float shadowX;
    float shadowY;
    float shadowBlur;
    float borderWidth;

    // i_data4 (colors)
    uint32_t shadowColor;
    uint32_t fillColor;     // PageCurl: currentPageColor
    uint32_t borderColor;   // PageCurl: nextPageColor

    PointI* texsize;
};

// ============================================================
// インスタンスデータ（80バイト）
// ============================================================
// 新レイアウト:
// data0: x, y, width, height
// data1: 
//   Pattern: colorCount_dataOffset(packed), (unused), angle, (unused)
//   Image:   (unused), (unused), uvMin.x, uvMin.y
//   PageCurl: uvMin(packed), backUVMin(packed), curlAngle, curlRadius
// data2: scrollX/uvMax.x/uvSize.x, scrollY/uvMax.y/uvSize.y, radius/progress, aa
// data3: shadowX, shadowY, shadowBlur, borderWidth
// data4: shadowColor(packed), fillColor(packed), borderColor(packed), zIndex

struct alignas(16) UnifiedInstanceData {
    float data0[4];  // x, y, width, height
    float data1[4];  // (モード別)
    float data2[4];  // scrollX/uvMax.x/uvSize.x, scrollY/uvMax.y/uvSize.y, radius/progress, aa
    float data3[4];  // shadowX, shadowY, shadowBlur, borderWidth
    float data4[4];  // shadowColor(packed), fillColor(packed), borderColor(packed), zIndex
};

// ============================================================
// Pack関数
// ============================================================

// 16bit x 2 を float にpack
inline float packUint16x2(uint16_t hi, uint16_t lo) {
    uint32_t packed = (uint32_t(hi) << 16) | uint32_t(lo);
    float result;
    std::memcpy(&result, &packed, 4);
    return result;
}

// uint32_t (RGBA) を float にpack
inline float packColorAsFloat(uint32_t color) {
    float result;
    std::memcpy(&result, &color, 4);
    return result;
}

// ============================================================
// インスタンスデータ pack
// ============================================================

inline void packInstance(UnifiedDrawCommand& cmd, UnifiedInstanceData& out, DrawCommandType type) {
    // i_data0: 共通
    out.data0[0] = cmd.x;
    out.data0[1] = cmd.y;
    out.data0[2] = cmd.width;
    out.data0[3] = cmd.height;

    // i_data1: モード別
    if (type == DrawCommandType::Image) {
        // Imageモード: float精度でUV座標
        out.data1[0] = 0.0f;           // (未使用)
        out.data1[1] = 0.0f;           // (未使用)
        out.data1[2] = cmd.angle;      // uvMin.x
        out.data1[3] = cmd.dataOffset; // uvMin.y
    }
    else if (type == DrawCommandType::PageCurl) {
        // PageCurlモード: UV座標を16bit pack
        out.data1[0] = packUint16x2(
            static_cast<uint16_t>(cmd.colorCount * 65535.0f),   // uvMin.x
            static_cast<uint16_t>(cmd.dataOffset * 65535.0f)    // uvMin.y
        );
        out.data1[1] = packUint16x2(
            static_cast<uint16_t>(cmd.backUVMinX * 65535.0f),   // backUVMin.x
            static_cast<uint16_t>(cmd.backUVMinY * 65535.0f)    // backUVMin.y
        );
        out.data1[2] = cmd.angle;      // curlAngle
        out.data1[3] = cmd.curlRadius; // curlRadius
    }
    else {
        // Patternモード: colorCountとdataOffsetをpack
        out.data1[0] = packUint16x2(
            static_cast<uint16_t>(cmd.colorCount),
            static_cast<uint16_t>(cmd.dataOffset)
        );
        out.data1[1] = 0.0f;           // (未使用)
        out.data1[2] = cmd.angle;
        out.data1[3] = 0.0f;           // (未使用)
    }

    // i_data2: 共通 (意味はモードによって異なる)
    out.data2[0] = cmd.scrollX;    // scrollX / uvMax.x / uvSize.x
    out.data2[1] = cmd.scrollY;    // scrollY / uvMax.y / uvSize.y
    out.data2[2] = cmd.radius;     // radius / progress
    out.data2[3] = cmd.aa;

    // i_data3: 共通
    out.data3[0] = cmd.shadowX;
    out.data3[1] = cmd.shadowY;
    out.data3[2] = cmd.shadowBlur;
    out.data3[3] = cmd.borderWidth;

    // i_data4: 共通
    out.data4[0] = packColorAsFloat(cmd.shadowColor);
    out.data4[1] = packColorAsFloat(cmd.fillColor);
    out.data4[2] = packColorAsFloat(cmd.borderColor);
    out.data4[3] = 50 - cmd.zIndex / 1000;
}

// ============================================================
// バッチ描画
// ============================================================

struct ImageMaster;
ResolvedTexture myresolveTexture(ImageMaster& master, ImageId imageId);
inline void drawUnifiedBatch(
    std::vector<UnifiedDrawCommand*>& commands,
    RenderResources& resources,
    bgfx::TextureHandle batchTexture,
    bgfx::TextureHandle batchTexture2,
    uint8_t viewId)
{
    if (commands.empty()) return;

    // ソート
    std::vector<UnifiedDrawCommand*> sorted = commands;

    // インスタンスデータ生成
    uint32_t count = static_cast<uint32_t>(sorted.size());
    bgfx::InstanceDataBuffer idb;
    bgfx::allocInstanceDataBuffer(&idb, count, 80);
    if (!idb.data) return;

    DrawCommandType batchType = sorted[0]->type;
    for (size_t i = 0; i < count; ++i) {
        UnifiedInstanceData inst;
        packInstance(*sorted[i], inst, batchType);
        std::memcpy(idb.data + i * 80, &inst, 80);
    }

    bgfx::setVertexBuffer(0, resources.quadVB);
    bgfx::setIndexBuffer(resources.quadIB);

    // テクスチャ設定（ポインタをデリファレンス）
    bgfx::TextureHandle tex = batchTexture;
    if (sorted[0]->texture && bgfx::isValid(*sorted[0]->texture)) tex = *sorted[0]->texture;
    bgfx::TextureHandle tex2 = batchTexture2;
    if (sorted[0]->texture2 && bgfx::isValid(*sorted[0]->texture2)) tex2 = *sorted[0]->texture2;

    // Use placeholder texture if tex or tex2 is invalid (Metal requires valid texture bindings)
    if (!bgfx::isValid(tex)) tex = resources.placeholderTexture;
    if (!bgfx::isValid(tex2)) tex2 = resources.placeholderTexture;

    bgfx::setTexture(0, resources.paletteUniform, tex);
    bgfx::setTexture(1, resources.widthsUniform, tex2);

    float mode = static_cast<float>(batchType);
    float modeVec[4] = { mode, 0.0f, 0.0f, 0.0f };
    bgfx::setUniform(resources.param1Uniform, modeVec);

    bgfx::setInstanceDataBuffer(&idb);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A
        | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_WRITE_Z
        | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    bgfx::submit(viewId, resources.uniteProgram);
}
struct PageCurlDrawCommand : DrawCommand {
    // i_data0
    float x, y, width, height;

    // i_data1
    float progress;     // 0〜1
    float curlRadius;   // 折り目の曲率
    float curlAngle;    // 折り目の角度
    // zIndex は DrawCommand から継承

    // i_data2 (UV範囲)
    float uvMinX, uvMinY, uvMaxX, uvMaxY;

    // テクスチャ3枚
    bgfx::TextureHandle* frontPage = nullptr;   // 表面
    bgfx::TextureHandle* backPage = nullptr;    // 裏面
};

// ============================================================
// インスタンスデータ（48バイト）
// ============================================================
struct alignas(16) PageCurlInstanceData {
    float data0[4];  // x, y, width, height
    float data1[4];  // progress, curlRadius, curlAngle, zIndex
    float data2[4];  // uvMinX, uvMinY, uvMaxX, uvMaxY
};

// ============================================================
// パック関数
// ============================================================
inline void packPageCurlInstance(PageCurlDrawCommand& cmd, PageCurlInstanceData& out) {
    out.data0[0] = cmd.x;
    out.data0[1] = cmd.y;
    out.data0[2] = cmd.width;
    out.data0[3] = cmd.height;

    out.data1[0] = cmd.progress;
    out.data1[1] = cmd.curlRadius;
    out.data1[2] = cmd.curlAngle;
    out.data1[3] = cmd.zIndex / 1000.0f;

    out.data2[0] = cmd.uvMinX;
    out.data2[1] = cmd.uvMinY;
    out.data2[2] = cmd.uvMaxX;
    out.data2[3] = cmd.uvMaxY;
}
inline void drawPageCurlBatch(
    std::vector<PageCurlDrawCommand*>& commands,
    RenderResources& resources,
    uint8_t viewId)
{
    if (commands.empty()) return;

    // zIndexでソート
    std::vector<PageCurlDrawCommand*> sorted = commands;
    std::sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b) { return a->zIndex > b->zIndex; });

    // テクスチャ3枚なので、同じテクスチャセットでバッチ化
    size_t batchStart = 0;

    auto getTexHandle = [&](bgfx::TextureHandle* tex, bgfx::TextureHandle fallback) {
        return (tex && bgfx::isValid(*tex)) ? *tex : fallback;
        };

    auto flushBatch = [&](size_t start, size_t end) {
        uint32_t count = static_cast<uint32_t>(end - start);
        if (count == 0) return;

        bgfx::InstanceDataBuffer idb;
        bgfx::allocInstanceDataBuffer(&idb, count, sizeof(PageCurlInstanceData));

        if (idb.data) {
            for (size_t i = start; i < end; ++i) {
                PageCurlInstanceData inst;
                packPageCurlInstance(*sorted[i], inst);
                std::memcpy(idb.data + (i - start) * sizeof(PageCurlInstanceData),
                    &inst, sizeof(PageCurlInstanceData));
            }

            bgfx::setVertexBuffer(0, resources.quadVB);
            bgfx::setIndexBuffer(resources.quadIB);

            // テクスチャ設定
            bgfx::setTexture(0, resources.paletteUniform,
                getTexHandle(sorted[start]->frontPage, *sorted[start]->frontPage));
            bgfx::setTexture(1, resources.widthsUniform,
                getTexHandle(sorted[start]->backPage, *sorted[start]->backPage));

            bgfx::setInstanceDataBuffer(&idb);

            bgfx::setState(
                BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_WRITE_Z |
                BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA));

            bgfx::submit(viewId, resources.pageProgram);
        }
        };

    // テクスチャセットが同じものをバッチ化
    for (size_t i = 1; i <= sorted.size(); ++i) {
        bool needFlush = (i == sorted.size());

        if (!needFlush) {
            auto* curr = sorted[batchStart];
            auto* next = sorted[i];

            // 3つのテクスチャが全部同じならバッチ継続
            needFlush = (curr->frontPage != next->frontPage) ||
                (curr->backPage != next->backPage);
        }

        if (needFlush) {
            flushBatch(batchStart, i);
            batchStart = i;
        }
    }
}
// ============================================================
// 13. FBO（フレームバッファオブジェクト）
// ============================================================

struct FBODrawCommand : DrawCommand {
    bgfx::FrameBufferHandle* fbo = NULL;
    NewElement* elem;
    int fboWidth, fboHeight;
    bgfx::TextureHandle* texture;
    bool resized;
};
struct DelFBODrawCommand : DrawCommand {
    bgfx::FrameBufferHandle fbo;
};

struct RetiredFBO {
    bgfx::FrameBufferHandle fbo;
    uint32_t retireFrame;
};

std::vector<RetiredFBO> retiredFbos;
uint32_t frameN;
inline void destroyFBO(FBODrawCommand& cmd) {
    if (!cmd.targetFBO || !cmd.texture) return;

    if (bgfx::isValid(*cmd.fbo)) {
        retiredFbos.push_back({
                *cmd.fbo,
                frameN
            });
        *cmd.targetFBO = BGFX_INVALID_HANDLE;
    }
    *cmd.texture = BGFX_INVALID_HANDLE;
}

inline bgfx::FrameBufferHandle ensureFBO(FBODrawCommand& cmd) {
    cmd.fbsize->x = cmd.fboWidth;
    cmd.fbsize->y = cmd.fboHeight;
    if (cmd.resized) {
        destroyFBO(cmd);
        const uint64_t flags =
            BGFX_TEXTURE_RT |
            BGFX_SAMPLER_U_CLAMP |
            BGFX_SAMPLER_V_CLAMP |
            BGFX_SAMPLER_MIN_POINT |
            BGFX_SAMPLER_MAG_POINT;
        *cmd.texture = bgfx::createTexture2D(
            (uint16_t)cmd.fboWidth,
            (uint16_t)cmd.fboHeight,
            false, 1,
            bgfx::TextureFormat::RGBA8,
            flags);

        bgfx::Attachment attachments[1];
        attachments[0].init(*cmd.texture);
        *cmd.fbo = bgfx::createFrameBuffer(1, attachments, true);
    }
    else {
        if (!cmd.texture || !cmd.fbo) {
            return BGFX_INVALID_HANDLE;
        }

        if (bgfx::isValid(*cmd.fbo)) {
            return *cmd.fbo;
        }

        if (!bgfx::isValid(*cmd.texture)) {
            const uint64_t flags =
                BGFX_TEXTURE_RT |
                BGFX_SAMPLER_U_CLAMP |
                BGFX_SAMPLER_V_CLAMP |
                BGFX_SAMPLER_MIN_POINT |
                BGFX_SAMPLER_MAG_POINT;
            *cmd.texture = bgfx::createTexture2D(
                (uint16_t)cmd.fboWidth,
                (uint16_t)cmd.fboHeight,
                false, 1,
                bgfx::TextureFormat::RGBA8,
                flags
            );
        }

        if (!bgfx::isValid(*cmd.texture)) {
            return BGFX_INVALID_HANDLE;
        }

        bgfx::Attachment attachments[1];
        attachments[0].init(*cmd.texture);
        *cmd.fbo = bgfx::createFrameBuffer(1, attachments, true);
    }

    return *cmd.fbo;
}

inline void resizeFBO(FBODrawCommand& cmd, int newWidth, int newHeight) {
    if (cmd.fboWidth != newWidth || cmd.fboHeight != newHeight) {
        destroyFBO(cmd);
        cmd.fboWidth = newWidth;
        cmd.fboHeight = newHeight;
    }
}
// ============================================================
// 共通ユーティリティ
// ============================================================

// 単位四角形の頂点
struct QuadVertex {
    float x, y;

    static bgfx::VertexLayout layout;
    static void init() {
        layout
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .end();
    }
};
bgfx::VertexLayout QuadVertex::layout;

void createUnitQuad(
    bgfx::VertexBufferHandle& outVB,
    bgfx::IndexBufferHandle& outIB)
{
    QuadVertex vertices[4] = {
        { 0.0f, 0.0f},
        { 1.0f, 0.0f},
        { 1.0f, 1.0f},
        { 0.0f, 1.0f},
    };

    uint16_t indices[6] = { 0, 1, 2, 0, 2, 3 };

    outVB = bgfx::createVertexBuffer(
        bgfx::copy(vertices, sizeof(vertices)),  // copy に変更
        QuadVertex::layout
    );
    outIB = bgfx::createIndexBuffer(
        bgfx::copy(indices, sizeof(indices))     // copy に変更
    );
}



// ============================================================
// 初期化関数
// ============================================================

