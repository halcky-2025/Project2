#include "shader2.h"

// ============================================================
// LayerInfo（レイヤー管理）
// ============================================================


void LayerInfo::push(DrawCommand* cmd) {
    cmds.push_back(cmd);
}

bgfx::TextureHandle nulltex = BGFX_INVALID_HANDLE;
// ============================================================
// Fill
// ============================================================

void LayerInfo::pushFill(float x, float y, float width, float height,
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
    cmd->width = width; cmd->height = height;
    cmd->radius = radius;
    cmd->aa = aaPixels;
    cmd->shadowX = shadowX; cmd->shadowY = shadowY;
    cmd->shadowBlur = shadowBlur;
    cmd->borderWidth = borderWidth;   // i_data3.w
    cmd->shadowColor = shadowColor;   // i_data4.x
    cmd->fillColor = fillColor;       // i_data4.y
    cmd->borderColor = borderColor;   // i_data4.z
    cmd->zIndex = zIndex;
    cmd->fbsize = fbsize;
    cmd->texture = &nulltex;
    cmd->texture2 = &nulltex;
    cmds.push_back(cmd);
}

// ============================================================
// PageCurl (UnifiedDrawCommandに統合)
// ============================================================
// レイアウト:
//   i_data1.x: uvMin (16bit pack)
//   i_data1.y: backUVMin (16bit pack)
//   i_data1.z: curlAngle
//   i_data1.w: curlRadius
//   i_data2.x: uvSize.x
//   i_data2.y: uvSize.y
//   i_data2.z: progress
//   i_data2.w: aa

void LayerInfo::pushPageCurl(float x, float y, float width, float height,
    float progress, float curlRadius, float curlAngle,
    uint32_t fillColor,
    float uvMinX, float uvMinY, float uvSizeX, float uvSizeY,
    float backUVMinX, float backUVMinY, float borderWidth,
    uint32_t borderColor,
    float shadowX, float shadowY, float shadowBlur,
    uint32_t shadowColor,
    bgfx::TextureHandle* tex1, bgfx::TextureHandle* tex2, float zIndex,
    bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId)
{
    UnifiedDrawCommand* cmd = new UnifiedDrawCommand();
    cmd->type = DrawCommandType::PageCurl;
    cmd->targetFBO = targetFBO;
    cmd->viewId = viewId;
    cmd->x = x; cmd->y = y;
    cmd->width = width; cmd->height = height;

    // i_data1: uvMin, backUVMin をpack用に格納
    cmd->colorCount = uvMinX;         // packInstance で 16bit pack
    cmd->dataOffset = uvMinY;         // packInstance で 16bit pack
    cmd->angle = curlAngle;           // i_data1.z
    cmd->curlRadius = curlRadius;     // i_data1.w (新フィールド)

    // i_data1.y 用: backUVMin
    cmd->backUVMinX = backUVMinX;     // packInstance で 16bit pack (新フィールド)
    cmd->backUVMinY = backUVMinY;     // packInstance で 16bit pack (新フィールド)

    // i_data2
    cmd->scrollX = uvSizeX;   // uvSize.x
    cmd->scrollY = uvSizeY;   // uvSize.y
    cmd->radius = progress;           // progress (0〜1)
    cmd->aa = 1.0f;

    // colors
    cmd->fillColor = fillColor;
    cmd->borderColor = borderColor;
    cmd->shadowColor = shadowColor;
    cmd->borderWidth = borderWidth;
    cmd->shadowX = shadowX; cmd->shadowY = shadowY; cmd->shadowBlur = shadowBlur;

    cmd->zIndex = zIndex;
    cmd->texture = tex1;
    cmd->texture2 = tex2;      // 新フィールド: 裏面テクスチャ
    cmd->fbsize = fbsize;
    cmds.push_back(cmd);
}

// ============================================================
// Image
// ============================================================
// 新レイアウト (float精度、packなし):
//   i_data1.z (angle)      = uvMin.x = atlasX
//   i_data1.w (dataOffset) = uvMin.y = atlasY
//   i_data2.x (scrollX)    = uvMax.x = atlasX + atlasW
//   i_data2.y (scrollY)    = uvMax.y = atlasY + atlasH

void LayerInfo::pushImage(float x, float y, float width, float height,
    float atlasX, float atlasY, float atlasW, float atlasH,
    float radius, float aaPixels, float borderWidth,
    uint32_t borderColor,
    float shadowX, float shadowY, float shadowBlur,
    uint32_t shadowColor, uint32_t modulate,
    float zIndex, bgfx::TextureHandle* tex1,
    bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId)
{
    UnifiedDrawCommand* cmd = new UnifiedDrawCommand();
    cmd->type = DrawCommandType::Image;
    cmd->targetFBO = targetFBO;
    cmd->viewId = viewId;
    cmd->x = x; cmd->y = y;
    cmd->width = width; cmd->height = height;
    cmd->dataOffset = 0; cmd->colorCount = 0;
    // 新レイアウト: float精度でUV座標を渡す
    cmd->angle = atlasX;              // uvMin.x (i_data1.z)
    cmd->curlRadius = atlasY;         // uvMin.y (i_data1.w に転送)
    cmd->scrollX = atlasX + atlasW;   // uvMax.x (i_data2.x)
    cmd->scrollY = atlasY + atlasH;   // uvMax.y (i_data2.y)

    cmd->radius = radius;
    cmd->aa = aaPixels;
    cmd->shadowX = shadowX; cmd->shadowY = shadowY;
    cmd->shadowBlur = shadowBlur;
    cmd->borderWidth = borderWidth;   // i_data3.w
    cmd->shadowColor = shadowColor;   // i_data4.x
    cmd->fillColor = modulate;        // i_data4.y
    cmd->borderColor = borderColor;   // i_data4.z
    cmd->zIndex = zIndex;
    cmd->texture = tex1;
    cmd->texture2 = &nulltex;
    cmd->fbsize = fbsize;
    cmds.push_back(cmd);
}

// ============================================================
// Pattern
// ============================================================

void LayerInfo::pushPattern(enum DrawCommandType patternMode,
    float x, float y, float width, float height,
    float colorCount, float angle,
    float scrollX, float scrollY,
    float radius, float aaPixels,
    float borderWidth, uint32_t borderColor,
    float shadowX, float shadowY, float shadowBlur,
    uint32_t shadowColor,
    int dataOffset, float zIndex,
    bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId)
{
    UnifiedDrawCommand* cmd = new UnifiedDrawCommand();
    cmd->type = patternMode;
    cmd->targetFBO = targetFBO;
    cmd->viewId = viewId;
    cmd->x = x; cmd->y = y;
    cmd->width = width; cmd->height = height;
    cmd->colorCount = colorCount;
    cmd->angle = angle;
    cmd->scrollX = scrollX; cmd->scrollY = scrollY;
    cmd->radius = radius;
    cmd->aa = aaPixels;
    cmd->shadowX = shadowX; cmd->shadowY = shadowY;
    cmd->shadowBlur = shadowBlur;
    cmd->borderWidth = borderWidth;   // i_data3.w
    cmd->shadowColor = shadowColor;   // i_data4.x
    cmd->fillColor = 0xFFFFFFFF;      // i_data4.y (白)
    cmd->borderColor = borderColor;   // i_data4.z
    cmd->dataOffset = static_cast<float>(dataOffset);
    cmd->zIndex = zIndex;
    cmd->fbsize = fbsize;
    cmd->texture = &nulltex;
    cmd->texture2 = &nulltex;
    cmds.push_back(cmd);
}

// ============================================================
// Text (Imageと同じレイアウト)
// ============================================================

void LayerInfo::pushText(float x, float y, float width, float height,
    float atlasX, float atlasY, float atlasW, float atlasH,
    uint32_t color, float zIndex, bgfx::TextureHandle* tex1,
    bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId)
{
    UnifiedDrawCommand* cmd = new UnifiedDrawCommand();
    cmd->type = DrawCommandType::Image;
    cmd->targetFBO = targetFBO;
    cmd->viewId = viewId;
    cmd->x = x; cmd->y = y;
    cmd->width = width; cmd->height = height;
    cmd->dataOffset = 0; cmd->colorCount = 0;
    // 新レイアウト: float精度でUV座標を渡す
    cmd->angle = atlasX;              // uvMin.x (i_data1.z)
    cmd->curlRadius = atlasY;         // uvMin.y (i_data1.w に転送)
    cmd->scrollX = atlasX + atlasW;   // uvMax.x (i_data2.x)
    cmd->scrollY = atlasY + atlasH;   // uvMax.y (i_data2.y)

    cmd->radius = 0;
    cmd->aa = 1.0f;
    cmd->shadowBlur = 0;
    cmd->borderWidth = 0;             // i_data3.w
    cmd->shadowColor = 0;             // i_data4.x
    cmd->fillColor = color;           // i_data4.y
    cmd->borderColor = 0;             // i_data4.z
    cmd->zIndex = zIndex;
    cmd->texture = tex1;
    cmd->texture2 = &nulltex;
    cmd->fbsize = fbsize;
    cmds.push_back(cmd);
}
// ============================================================
// ユーティリティ
// ============================================================
LayerInfo& LayerInfo::setUniform(const std::string& name, const std::vector<float>& data) {
    defaultUniforms[name] = data;
    return *this;
}

LayerInfo& LayerInfo::setTexture(const std::string& uniformName, bgfx::TextureHandle texture) {
    defaultTextures[uniformName] = texture;
    return *this;
}

void LayerInfo::pushFBO(NewElement* elem, int fboWidth, int fboHeight,
    bgfx::TextureHandle* texture, bgfx::FrameBufferHandle* fbo, PointI* fbsize, bool resized) {
    FBODrawCommand* cmd = new FBODrawCommand();
    cmd->type = DrawCommandType::FBO;
    cmd->elem = elem;
    cmd->fboWidth = fboWidth;
    cmd->fboHeight = fboHeight;
    cmd->texture = texture;
    cmd->targetFBO = NULL;
    cmd->fbo = fbo;
    cmd->resized = resized;
    cmd->zIndex = -100000.0f;
    cmd->fbsize = fbsize;
    cmds.push_back((DrawCommand*)cmd);
}
void LayerInfo::pushDelFBO(bgfx::FrameBufferHandle fbo) {
    DelFBODrawCommand* cmd = new DelFBODrawCommand();
    cmd->type = DrawCommandType::DelFBO;
    cmd->fbo = fbo;
    cmd->zIndex = -100000.0f;
    cmds.push_back((DrawCommand*)cmd);
}
void LayerInfo::clear() {
    cmds.clear();
}
// ============================================================
// グローバルリソース（実際のプロジェクトでは適切なクラスで管理）
// ============================================================



static RenderResources g_resources;

// ============================================================
// Pattern用アトラスビルダー（フレームごとに再構築）
// ============================================================

class PatternAtlasBuilder {
public:
    struct PatternEntry {
        int offset;
        int colorCount;
    };

    // 色パターンのキー（色データ + 幅データのハッシュ）
    struct PatternKey {
        std::vector<float> colors;  // RGBA × colorCount
        std::vector<float> widths;  // colorCount個

        bool operator==(const PatternKey& other) const {
            return colors == other.colors && widths == other.widths;
        }
    };

    struct PatternKeyHash {
        size_t operator()(const PatternKey& key) const {
            size_t hash = 0;
            for (float v : key.colors) {
                hash ^= std::hash<float>{}(v)+0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            for (float v : key.widths) {
                hash ^= std::hash<float>{}(v)+0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };
    mutable std::mutex m;

private:
    std::vector<float> paletteData_;
    std::vector<float> widthsData_;
    int currentOffset_ = 0;

    bgfx::TextureHandle paletteTexture_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle widthsTexture_ = BGFX_INVALID_HANDLE;

    // 色パターンをキーにしたキャッシュ
    std::unordered_map<PatternKey, PatternEntry, PatternKeyHash> patternCache_;

public:
    void reset() {

        if (bgfx::isValid(paletteTexture_)) {
            bgfx::destroy(paletteTexture_);
            paletteTexture_ = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(widthsTexture_)) {
            bgfx::destroy(widthsTexture_);
            widthsTexture_ = BGFX_INVALID_HANDLE;
        }
    }

    // 色パターンを直接指定して追加
    int addPattern(const std::vector<float>& colors, const std::vector<float>& widths) {
        std::lock_guard lock(m);
        int colorCount = static_cast<int>(widths.size());

        // 入力検証
        if (colors.size() != colorCount * 4) {
            SDL_Log("PatternAtlasBuilder: colors.size() must be widths.size() * 4");
            return -1;
        }

        // キーを作成
        PatternKey key;
        key.colors = colors;
        key.widths = widths;

        // キャッシュにあれば再利用
        auto it = patternCache_.find(key);
        if (it != patternCache_.end()) {
            return it->second.offset;
        }

        // 新規追加
        int newOffset = currentOffset_;

        paletteData_.insert(paletteData_.end(), colors.begin(), colors.end());
        widthsData_.insert(widthsData_.end(), widths.begin(), widths.end());

        currentOffset_ += colorCount;

        PatternEntry entry = { newOffset, colorCount };
        patternCache_[key] = entry;

        return newOffset;
    }


    // パターンが既に存在するか確認
    int findPattern(const std::vector<float>& colors, const std::vector<float>& widths) const {
        PatternKey key;
        key.colors = colors;
        key.widths = widths;

        auto it = patternCache_.find(key);
        if (it != patternCache_.end()) {
            return it->second.offset;
        }
        return -1;
    }

    // パターン情報を取得
    bool getPatternInfo(int offset, int& outColorCount) const {
        for (const auto& [key, entry] : patternCache_) {
            if (entry.offset == offset) {
                outColorCount = entry.colorCount;
                return true;
            }
        }
        return false;
    }

    void buildTextures() {
        if (currentOffset_ == 0) return;

        const int ATLAS_SIZE = 1024;  // 固定サイズ

        // パレットテクスチャ（1024幅で作成）
        std::vector<uint8_t> paletteBytes(ATLAS_SIZE * 4, 0);  // 0で初期化
        for (size_t i = 0; i < paletteData_.size(); ++i) {
            paletteBytes[i] = static_cast<uint8_t>(paletteData_[i] * 255.0f);
        }

        const bgfx::Memory* paletteMem = bgfx::copy(
            paletteBytes.data(),
            static_cast<uint32_t>(paletteBytes.size()));
        paletteTexture_ = bgfx::createTexture2D(
            ATLAS_SIZE, 1, false, 1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            paletteMem
        );

        // 幅テクスチャ（1024幅で作成）
        std::vector<float> widthsBytes(ATLAS_SIZE, 0.0f);
        float cumulative = 0.0f;
        for (size_t i = 0; i < widthsData_.size(); ++i) {
            cumulative += widthsData_[i];
            widthsBytes[i] = cumulative;  // 累積幅を格納
        }

        const bgfx::Memory* widthsMem = bgfx::copy(
            widthsBytes.data(),
            static_cast<uint32_t>(ATLAS_SIZE * sizeof(float)));
        widthsTexture_ = bgfx::createTexture2D(
            ATLAS_SIZE, 1, false, 1,
            bgfx::TextureFormat::R32F,
            BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, // POINTサンプリング
            widthsMem
        );
    }
    bgfx::TextureHandle getPaletteTexture() const { return paletteTexture_; }
    bgfx::TextureHandle getWidthsTexture() const { return widthsTexture_; }
    bool hasData() const { return currentOffset_ > 0; }
};

// ============================================================
// シェーダータイプのカテゴリ判定
// ============================================================

// ============================================================
// 描画バッチ
// ============================================================

struct DrawBatch {
    bgfx::TextureHandle *texture = NULL;
    bgfx::TextureHandle* texture2 = NULL;
    std::vector<DrawCommand*> commands;
};

// ============================================================
// レンダリングパス
// ============================================================

struct RenderPass {
    bgfx::FrameBufferHandle* fbo = nullptr;
    PointI* fbsize = nullptr;
    uint8_t viewId = 0;
    float dctype;
    float minZIndex = 0;
    float maxZIndex = 0;
    std::vector<DrawBatch> batches;
};

// ============================================================
// レンダリングパスキー
// ============================================================

struct RenderPassKey {
    bgfx::FrameBufferHandle* fbo;
    PointI* fbsize;
    uint8_t viewId;
    float dctype;
    int zIndexBucket;
    bgfx::TextureHandle* texture;
    bgfx::TextureHandle* texture2;

    bool operator<(const RenderPassKey& other) const {
        if (fbo != other.fbo) return fbo < other.fbo;
        if (viewId != other.viewId) return viewId < other.viewId;
        if (zIndexBucket != other.zIndexBucket) return zIndexBucket < other.zIndexBucket;
        if (texture != other.texture) return texture < other.texture;
        if (texture2 != other.texture2) return texture2 < other.texture2;
        return static_cast<int>(dctype) < static_cast<int>(other.dctype);
    }
};

// ============================================================
// コマンドグループ化とアトラスビルド
// ============================================================

struct GroupedCommands {
    std::vector<RenderPass> passes;
    PatternAtlasBuilder* patternAtlas;
};

GroupedCommands groupDrawCommands(
    std::vector<DrawCommand*>& commands,
    PatternAtlasBuilder& sourcePatternAtlas)
{
    GroupedCommands result;
    result.patternAtlas = &sourcePatternAtlas;

    // RenderPassKey でグループ化（テクスチャポインタ含む）
    std::map<RenderPassKey, std::vector<DrawCommand*>> passGroups;

    for (auto* cmd : commands) {
        RenderPassKey passKey;
        passKey.fbo = cmd->targetFBO;
        passKey.viewId = cmd->viewId;
        passKey.dctype = cmd->type;
        passKey.fbsize = cmd->fbsize;
        passKey.zIndexBucket = static_cast<int>(cmd->zIndex);
        passKey.texture = cmd->texture;    // ★ テクスチャポインタ
        passKey.texture2 = cmd->texture2;

        passGroups[passKey].push_back(cmd);
    }

    // パターンテクスチャをビルド
    if (result.patternAtlas->hasData()) {
        result.patternAtlas->buildTextures();
    }

    // RenderPass 配列に変換
    for (auto& [passKey, cmds] : passGroups) {
        RenderPass pass;
        pass.fbo = passKey.fbo;
        pass.viewId = passKey.viewId;
        pass.fbsize = passKey.fbsize;
        pass.dctype = passKey.dctype;
        pass.minZIndex = passKey.zIndexBucket;
        pass.maxZIndex = -FLT_MAX;

        DrawBatch batch;
        batch.texture = passKey.texture;
        batch.texture2 = passKey.texture2;

        batch.commands = std::move(cmds);
        pass.batches.push_back(std::move(batch));

        result.passes.push_back(std::move(pass));
    }

    // パス全体をソート
    std::sort(result.passes.begin(), result.passes.end(),
        [](const RenderPass& a, const RenderPass& b) {
            if (a.fbo != b.fbo) return a.fbo < b.fbo;
            if (a.viewId != b.viewId) return a.viewId < b.viewId;
            return a.minZIndex < b.minZIndex;
        });

    return result;
}

// ============================================================
// コマンドベクタ抽出ヘルパー
// ============================================================

template<typename T>
std::vector<T> extractCommands(const std::vector<DrawCommand*>& ptrs) {
    std::vector<T> result;
    result.reserve(ptrs.size());
    for (auto* ptr : ptrs) {
        result.push_back(*static_cast<T*>(ptr));
    }
    return result;
}

auto frameStart = std::chrono::high_resolution_clock::now();
// ============================================================
// メイン描画関数
// ============================================================
void renderAllCommands(
    NewLocal* local,
    GroupedCommands& grouped,
    RenderResources& resources, ImageMaster& master)
{
    bgfx::FrameBufferHandle* currentFBO = nullptr;
    uint8_t currentViewId = 255;
    for (auto& pass : grouped.passes) {
        if (pass.fbo != currentFBO || pass.viewId != currentViewId) {
            if (pass.fbo && bgfx::isValid(*pass.fbo)) {
                bgfx::setViewFrameBuffer(pass.viewId, *pass.fbo);
                bgfx::setViewClear(pass.viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x808080ff, 1.0f, 0);
                bgfx::setViewRect(pass.viewId, 0, 0, pass.fbsize->x, pass.fbsize->y);
                // 射影行列設定（2D用）
                float orthoProj[16];
                bx::mtxOrtho(orthoProj, 0.0f, pass.fbsize->x, pass.fbsize->y, 0.0f, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
                bgfx::setViewTransform(pass.viewId, NULL, orthoProj);
            }
            else {
                bgfx::setViewFrameBuffer(pass.viewId, BGFX_INVALID_HANDLE);
                bgfx::setViewClear(pass.viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xa0a0a0ff, 1.0f, 0);
                bgfx::setViewRect(pass.viewId, 0, 0, 800, 600);
                // 射影行列設定（2D用）
                float orthoProj[16];
                bx::mtxOrtho(orthoProj, 0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
                bgfx::setViewTransform(pass.viewId, NULL, orthoProj);
            }
            currentFBO = pass.fbo;
            currentViewId = pass.viewId;
        }

        // 各バッチを描画
        for (auto& batch : pass.batches) {
            if (batch.commands.empty()) continue;
            bgfx::TextureHandle tex = BGFX_INVALID_HANDLE;
            bgfx::TextureHandle wid = BGFX_INVALID_HANDLE;
            if (DrawCommandType::Gradient <= pass.dctype && pass.dctype <= DrawCommandType::GradientChecker) {
                tex = grouped.patternAtlas->getPaletteTexture();
                wid = grouped.patternAtlas->getWidthsTexture();
            }
            drawUnifiedBatch(reinterpret_cast<std::vector<UnifiedDrawCommand*>&>(batch.commands), resources, tex, wid, pass.viewId);
        }
    }
}

// ============================================================
// LayerInfo用の描画エントリポイント
// ============================================================

void renderLayer(NewLocal* local, LayerInfo& layer,
    RenderResources& resources, ImageMaster& master,
    PatternAtlasBuilder& patternAtlas)
{
    if (layer.cmds.empty()) return;

    // コマンドポインタのベクタを作成
    std::vector<DrawCommand*> cmdPtrs;
    cmdPtrs.reserve(layer.cmds.size());
    for (auto cmd : layer.cmds) {
        cmdPtrs.push_back(cmd);
    }

    // グループ化
    auto grouped = groupDrawCommands(cmdPtrs, patternAtlas);
    // 描画
    renderAllCommands(local, grouped, resources, master);
}

// ============================================================
// 複数レイヤーの描画
// ============================================================

void renderAllLayers(NewLocal* local, std::vector<LayerInfo>& layers,
    RenderResources& resources, ImageMaster& master,
    PatternAtlasBuilder& patternAtlas)
{
    // 全レイヤーのコマンドを収集
    std::vector<DrawCommand*> allCommands;

    for (auto& layer : layers) {
        if (!layer.visible) continue;

        for (auto cmd : layer.cmds) {
            allCommands.push_back(cmd);
        }
    }

    if (allCommands.empty()) return;
    // グループ化
    auto grouped = groupDrawCommands(allCommands, patternAtlas);

    // 描画
    renderAllCommands(local, grouped, resources, master);
}

// ============================================================
// 初期化・終了処理
// ============================================================
// シェーダーバイナリ読み込み
bgfx::ShaderHandle loadShader(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return BGFX_INVALID_HANDLE;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        return BGFX_INVALID_HANDLE;
    }

    const bgfx::Memory* mem = bgfx::copy(buffer.data(), static_cast<uint32_t>(size));
    return bgfx::createShader(mem);
}
void initRenderResources(RenderResources& resources) {
    createUnitQuad(resources.quadVB, resources.quadIB);
    resources.paletteUniform = bgfx::createUniform("s_palette", bgfx::UniformType::Sampler);
    resources.widthsUniform = bgfx::createUniform("s_widths", bgfx::UniformType::Sampler);
    resources.param1Uniform = bgfx::createUniform("u_param1", bgfx::UniformType::Vec4);
    bgfx::ShaderHandle vsu = loadShader("vs_unite2.bin");
    bgfx::ShaderHandle fsu = loadShader("fs_unite2.bin");
    resources.uniteProgram = bgfx::createProgram(vsu, fsu, true);
    bgfx::ShaderHandle vsp = loadShader("vs_page.bin");
    bgfx::ShaderHandle fsp = loadShader("fs_page.bin");
    resources.pageProgram = bgfx::createProgram(vsp, fsp, true);
    // シェーダープログラムの登録は外部で行う
    // resources.programs[DrawCommandType::Rectangle] = loadProgram(...);
}

void destroyRenderResources(RenderResources& resources) {
    if (bgfx::isValid(resources.quadVB)) bgfx::destroy(resources.quadVB);
    if (bgfx::isValid(resources.quadIB)) bgfx::destroy(resources.quadIB);
    if (bgfx::isValid(resources.paletteUniform)) bgfx::destroy(resources.paletteUniform);
    if (bgfx::isValid(resources.widthsUniform)) bgfx::destroy(resources.widthsUniform);
    if (bgfx::isValid(resources.param1Uniform)) bgfx::destroy(resources.param1Uniform);
    if (bgfx::isValid(resources.uniteProgram)) bgfx::destroy(resources.uniteProgram);
    if (bgfx::isValid(resources.pageProgram)) bgfx::destroy(resources.pageProgram);
}