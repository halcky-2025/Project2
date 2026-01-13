// ugui_ios_minimal.h - Minimal iOS shader implementation
// Self-contained version for iOS without complex dependencies

#pragma once

#include <bgfx/bgfx.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstring>

// ============================================================
// Basic Types
// ============================================================

struct PointI {
    int x;
    int y;
};

enum DrawCommandType {
    DelFBO = -2, FBO = -1, Fill = 0, Gradient = 1, Stripe = 2,
    Checker = 3, GradientChecker = 4, Image = 5, End = 7, PageCurl = 6
};

// ============================================================
// Render Resources
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

// ============================================================
// Draw Commands
// ============================================================

struct DrawCommand {
    DrawCommandType type;
    bgfx::FrameBufferHandle* targetFBO = nullptr;
    PointI* fbsize = nullptr;
    uint8_t viewId = 0;
    float zIndex = 0.0f;
    bgfx::TextureHandle* texture = nullptr;
    bgfx::TextureHandle* texture2 = nullptr;
};

struct UnifiedDrawCommand : DrawCommand {
    float x, y, width, height;
    float colorCount;
    float dataOffset;
    float backUVMinX = 0.0f;
    float backUVMinY = 0.0f;
    float angle;
    float curlRadius = 0.0f;
    float scrollX;
    float scrollY;
    float radius;
    float aa;
    float shadowX;
    float shadowY;
    float shadowBlur;
    float borderWidth;
    uint32_t shadowColor;
    uint32_t fillColor;
    uint32_t borderColor;
    PointI* texsize = nullptr;
};

// ============================================================
// Instance Data (80 bytes)
// ============================================================

struct alignas(16) UnifiedInstanceData {
    float data0[4];
    float data1[4];
    float data2[4];
    float data3[4];
    float data4[4];
};

// ============================================================
// Pack Functions
// ============================================================

inline float packUint16x2(uint16_t hi, uint16_t lo) {
    uint32_t packed = (uint32_t(hi) << 16) | uint32_t(lo);
    float result;
    std::memcpy(&result, &packed, 4);
    return result;
}

inline float packColorAsFloat(uint32_t color) {
    float result;
    std::memcpy(&result, &color, 4);
    return result;
}

inline void packInstance(UnifiedDrawCommand& cmd, UnifiedInstanceData& out, DrawCommandType type) {
    out.data0[0] = cmd.x;
    out.data0[1] = cmd.y;
    out.data0[2] = cmd.width;
    out.data0[3] = cmd.height;

    if (type == DrawCommandType::Image) {
        out.data1[0] = 0.0f;
        out.data1[1] = 0.0f;
        out.data1[2] = cmd.angle;
        out.data1[3] = cmd.dataOffset;
    }
    else if (type == DrawCommandType::PageCurl) {
        out.data1[0] = packUint16x2(
            static_cast<uint16_t>(cmd.colorCount * 65535.0f),
            static_cast<uint16_t>(cmd.dataOffset * 65535.0f)
        );
        out.data1[1] = packUint16x2(
            static_cast<uint16_t>(cmd.backUVMinX * 65535.0f),
            static_cast<uint16_t>(cmd.backUVMinY * 65535.0f)
        );
        out.data1[2] = cmd.angle;
        out.data1[3] = cmd.curlRadius;
    }
    else {
        out.data1[0] = packUint16x2(
            static_cast<uint16_t>(cmd.colorCount),
            static_cast<uint16_t>(cmd.dataOffset)
        );
        out.data1[1] = 0.0f;
        out.data1[2] = cmd.angle;
        out.data1[3] = 0.0f;
    }

    out.data2[0] = cmd.scrollX;
    out.data2[1] = cmd.scrollY;
    out.data2[2] = cmd.radius;
    out.data2[3] = cmd.aa;

    out.data3[0] = cmd.shadowX;
    out.data3[1] = cmd.shadowY;
    out.data3[2] = cmd.shadowBlur;
    out.data3[3] = cmd.borderWidth;

    out.data4[0] = packColorAsFloat(cmd.shadowColor);
    out.data4[1] = packColorAsFloat(cmd.fillColor);
    out.data4[2] = packColorAsFloat(cmd.borderColor);
    out.data4[3] = cmd.zIndex;
}

// ============================================================
// LayerInfo
// ============================================================

static bgfx::TextureHandle g_nulltex = BGFX_INVALID_HANDLE;

struct LayerInfo {
    std::map<std::string, std::vector<float>> defaultUniforms;
    std::map<std::string, bgfx::TextureHandle> defaultTextures;

    float opacity = 1.0f;
    bool visible = true;
    bool needsUpdate = true;
    int width = 800;
    int height = 600;
    std::vector<DrawCommand*> cmds;

    ~LayerInfo() {
        for (auto* cmd : cmds) {
            delete cmd;
        }
    }

    LayerInfo(const LayerInfo&) = delete;
    LayerInfo& operator=(const LayerInfo&) = delete;

    LayerInfo(LayerInfo&& other) noexcept
        : cmds(std::move(other.cmds)) {}

    LayerInfo& operator=(LayerInfo&& other) noexcept {
        if (this != &other) {
            for (auto* cmd : cmds) {
                delete cmd;
            }
            cmds = std::move(other.cmds);
        }
        return *this;
    }

    LayerInfo() = default;

    void push(DrawCommand* cmd) {
        cmds.push_back(cmd);
    }

    void clear() {
        for (auto* cmd : cmds) {
            delete cmd;
        }
        cmds.clear();
    }

    void pushFill(float x, float y, float w, float h,
        float radius, float borderWidth, float aaPixels,
        uint32_t fillColor, uint32_t borderColor,
        float shadowX, float shadowY, float shadowBlur,
        uint32_t shadowColor, float zIndex,
        bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId)
    {
        UnifiedDrawCommand* cmd = new UnifiedDrawCommand();
        cmd->type = DrawCommandType::Fill;
        cmd->targetFBO = targetFBO;
        cmd->viewId = viewId;
        cmd->x = x; cmd->y = y;
        cmd->width = w; cmd->height = h;
        cmd->radius = radius;
        cmd->aa = aaPixels;
        cmd->shadowX = shadowX; cmd->shadowY = shadowY;
        cmd->shadowBlur = shadowBlur;
        cmd->borderWidth = borderWidth;
        cmd->shadowColor = shadowColor;
        cmd->fillColor = fillColor;
        cmd->borderColor = borderColor;
        cmd->zIndex = zIndex;
        cmd->fbsize = fbsize;
        cmd->texture = &g_nulltex;
        cmd->texture2 = &g_nulltex;
        cmds.push_back(cmd);
    }
};

// ============================================================
// Pattern Atlas (Simplified)
// ============================================================

struct PatternAtlasBuilder {
    std::vector<float> palette;
    std::vector<float> widths;
    bgfx::TextureHandle paletteTexture = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle widthsTexture = BGFX_INVALID_HANDLE;

    bool hasData() const { return !palette.empty(); }
    bgfx::TextureHandle getPaletteTexture() const { return paletteTexture; }
    bgfx::TextureHandle getWidthsTexture() const { return widthsTexture; }
    void reset() {
        palette.clear();
        widths.clear();
        if (bgfx::isValid(paletteTexture)) {
            bgfx::destroy(paletteTexture);
            paletteTexture = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(widthsTexture)) {
            bgfx::destroy(widthsTexture);
            widthsTexture = BGFX_INVALID_HANDLE;
        }
    }
};

// ============================================================
// Batch Drawing
// ============================================================

struct DrawBatch {
    std::vector<UnifiedDrawCommand*> commands;
};

struct RenderPass {
    std::vector<DrawBatch> batches;
};

struct GroupedDrawCommands {
    std::vector<RenderPass> passes;
    PatternAtlasBuilder* patternAtlas = nullptr;
};

inline GroupedDrawCommands groupDrawCommands(
    std::vector<DrawCommand*>& cmdPtrs,
    PatternAtlasBuilder& atlas)
{
    GroupedDrawCommands result;
    result.patternAtlas = &atlas;

    if (cmdPtrs.empty()) return result;

    RenderPass pass;
    DrawBatch batch;

    for (auto* cmd : cmdPtrs) {
        batch.commands.push_back(static_cast<UnifiedDrawCommand*>(cmd));
    }

    pass.batches.push_back(std::move(batch));
    result.passes.push_back(std::move(pass));

    return result;
}

inline void drawUnifiedBatch(
    std::vector<UnifiedDrawCommand*>& commands,
    RenderResources& resources,
    bgfx::TextureHandle batchTexture,
    bgfx::TextureHandle batchTexture2,
    uint8_t viewId)
{
    if (commands.empty()) return;

    uint32_t count = static_cast<uint32_t>(commands.size());
    bgfx::InstanceDataBuffer idb;
    bgfx::allocInstanceDataBuffer(&idb, count, 80);
    if (!idb.data) return;

    DrawCommandType batchType = commands[0]->type;
    for (size_t i = 0; i < count; ++i) {
        UnifiedInstanceData inst;
        packInstance(*commands[i], inst, batchType);
        std::memcpy(idb.data + i * 80, &inst, 80);
    }

    // Set textures
    if (bgfx::isValid(batchTexture)) {
        bgfx::setTexture(0, resources.paletteUniform, batchTexture);
    }
    if (bgfx::isValid(batchTexture2)) {
        bgfx::setTexture(1, resources.widthsUniform, batchTexture2);
    }

    // Set mode parameter
    float param1[4] = { float(batchType), 0.0f, 0.0f, 0.0f };
    bgfx::setUniform(resources.param1Uniform, param1);

    // Set vertex/index buffers
    bgfx::setVertexBuffer(0, resources.quadVB);
    bgfx::setIndexBuffer(resources.quadIB);
    bgfx::setInstanceDataBuffer(&idb);

    // Set state and submit
    bgfx::setState(
        BGFX_STATE_WRITE_RGB |
        BGFX_STATE_WRITE_A |
        BGFX_STATE_BLEND_ALPHA
    );

    bgfx::submit(viewId, resources.uniteProgram);
}

// ============================================================
// Resource Creation
// ============================================================

struct PosTexCoord0Vertex {
    float x, y;
    float u, v;
};

inline void createUnitQuad(bgfx::VertexBufferHandle& vb, bgfx::IndexBufferHandle& ib) {
    static bgfx::VertexLayout layout;
    static bool layoutInit = false;
    if (!layoutInit) {
        layout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        layoutInit = true;
    }

    static PosTexCoord0Vertex vertices[] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
    };

    static uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

    vb = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(vertices)),
        layout
    );

    ib = bgfx::createIndexBuffer(
        bgfx::makeRef(indices, sizeof(indices))
    );
}

inline void destroyRenderResources(RenderResources& res) {
    if (bgfx::isValid(res.quadVB)) {
        bgfx::destroy(res.quadVB);
        res.quadVB = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(res.quadIB)) {
        bgfx::destroy(res.quadIB);
        res.quadIB = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(res.paletteUniform)) {
        bgfx::destroy(res.paletteUniform);
        res.paletteUniform = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(res.widthsUniform)) {
        bgfx::destroy(res.widthsUniform);
        res.widthsUniform = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(res.param1Uniform)) {
        bgfx::destroy(res.param1Uniform);
        res.param1Uniform = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(res.uniteProgram)) {
        bgfx::destroy(res.uniteProgram);
        res.uniteProgram = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(res.pageProgram)) {
        bgfx::destroy(res.pageProgram);
        res.pageProgram = BGFX_INVALID_HANDLE;
    }
}
