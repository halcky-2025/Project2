#pragma once
//=============================================================================
// ImageId 名前空間
//=============================================================================



//=============================================================================
// 解決結果
//=============================================================================


enum class ResolveStatus {
    Success,        // 解決成功
    Pending,        // ロード中（placeholder使用可）
    NotFound,       // 見つからない
    Evicted,        // eviction済み（再ロード可能かも）
};

struct ResolveResult {
    ResolveStatus status = ResolveStatus::NotFound;
    ResolvedTexture resolved;
    ResolvedTexture placeholder;  // Pending時のフォールバック

    bool isReady() const { return status == ResolveStatus::Success; }

    // 描画用（成功ならresolved、Pendingならplaceholder）
    const ResolvedTexture& forDraw() const {
        return (status == ResolveStatus::Success) ? resolved : placeholder;
    }
};

//=============================================================================
// ImageLocation - ImageIdの配置情報
//=============================================================================

struct ImageLocation {
    enum class Type : uint8_t {
        None,
        Standalone,   // standaloneTextures_ に存在
        FontAtlas,    // FontAtlas に存在
        GridAtlas,    // ThumbnailGridAtlas に存在
        ShelfAtlas,   // ThumbnailShelfAtlas に存在
    };

    Type type = Type::None;

    // 各Atlas用の内部キー
    union {
        struct { uint64_t glyphKey; } font;           // FontId + codepoint
        struct { ThumbnailHandle handle; } thumbnail; // Grid/Shelf
    };

    uint16_t width = 0;
    uint16_t height = 0;
    bool isPending = false;

    ImageLocation() : font{ 0 } {}
};

//=============================================================================
// RenderGroup - 基本グループ
//=============================================================================


//=============================================================================
// ExtendedRenderGroup - ImageMaster 用拡張グループ
//=============================================================================
struct ExtendedRenderGroup : RenderGroup {
    std::unordered_set<uint64_t> usedGridThumbnails;
    std::unordered_set<uint64_t> usedShelfThumbnails;
    std::unordered_set<ImageId> usedStandaloneTextures;

    void trackGridThumbnail(uint64_t id) { usedGridThumbnails.insert(id); }
    void trackShelfThumbnail(uint64_t id) { usedShelfThumbnails.insert(id); }
    void trackStandaloneTexture(ImageId id) { usedStandaloneTextures.insert(id); }

    void clearAll() {
        clear();
        usedGridThumbnails.clear();
        usedShelfThumbnails.clear();
        usedStandaloneTextures.clear();
    }
};

//=============================================================================
// ImageMasterGroupManager - 前方宣言
//=============================================================================
class ImageMaster;  // 前方宣言

class ImageMasterGroupManager {
public:
    explicit ImageMasterGroupManager(ImageMaster& master) : master_(master) {}

    ExtendedRenderGroup& createGroup(SurfaceId surfaceId,
        RenderGroupFlags flags = RenderGroupFlags::None) {
        RenderGroupId id = nextId_++;
        auto& g = groups_[id];
        g.id = id;
        g.surfaceId = surfaceId;
        g.flags = flags;
        g.bornFrame = currentFrame_;
        g.lastUsedFrame = currentFrame_;
        surfaceGroups_[surfaceId].push_back(id);
        return g;
    }

    ExtendedRenderGroup* getGroup(RenderGroupId id) {
        auto it = groups_.find(id);
        return (it != groups_.end()) ? &it->second : nullptr;
    }

    void beginFrame() { ++currentFrame_; }
    uint32_t currentFrame() const { return currentFrame_; }

    size_t groupCount() const { return groups_.size(); }
    size_t surfaceCount() const { return surfaceGroups_.size(); }

private:
    friend class ImageMaster;

    ImageMaster& master_;
    RenderGroupId nextId_ = 1;
    uint32_t currentFrame_ = 0;
    std::unordered_map<RenderGroupId, ExtendedRenderGroup> groups_;
    std::unordered_map<SurfaceId, std::vector<RenderGroupId>> surfaceGroups_;
};

//=============================================================================
// ImageMaster
//=============================================================================

class ImageMaster {
public:
    struct Config {
        FontAtlas::Config fontConfig = {};
        ThumbnailGridAtlasConfig gridConfig = {};
        ThumbnailShelfAtlasConfig shelfConfig = {};
        uint32_t standaloneExpireFrames = 600;
        uint32_t groupExpireFrames = 300;
    };

    explicit ImageMaster(const Config& cfg = {})
        : config_(cfg)
        , fontAtlas_(cfg.fontConfig)
        , gridAtlas_(cfg.gridConfig)
        , shelfAtlas_(cfg.shelfConfig)
        , groupManager_(*this) {
    }

    ~ImageMaster() { shutdown(); }

    bool initialize() {
        fontAtlas_.init();
        if (!gridAtlas_.initialize()) return false;
        if (!shelfAtlas_.initialize()) return false;
        return true;
    }

    void shutdown() {
        // Standalone textures 解放
        {
            std::lock_guard lock(standaloneMutex_);
            for (auto& [id, info] : standaloneTextures_) {
                if (bgfx::isValid(info.fbo)) {
                    bgfx::destroy(info.fbo);
                }
                else if (bgfx::isValid(info.handle)) {
                    bgfx::destroy(info.handle);
                }
            }
            standaloneTextures_.clear();
        }

        // 配置情報クリア
        {
            std::lock_guard lock(locationMutex_);
            imageLocations_.clear();
        }

        gridAtlas_.shutdown();
        shelfAtlas_.shutdown();
    }

    void beginFrame() {
        ++currentFrame_;
        fontAtlas_.beginFrame();
        gridAtlas_.beginFrame();
        shelfAtlas_.beginFrame();
        groupManager_.beginFrame();
        processOffscreenQueue();
    }

    void collectGarbage() {
        // 期限切れグループの回収
        collectExpiredGroups(config_.groupExpireFrames);

        // FontAtlas GC
        fontAtlas_.collectGarbage();

        // ThumbnailAtlas は内部LRUで管理
        gridAtlas_.evictLRU(16);
        shelfAtlas_.evictLRU(16);

        // Standalone texture GC
        collectStandaloneGarbage();
    }

    void collectStandaloneGarbage() {
        std::lock_guard lock(standaloneMutex_);

        std::vector<ImageId> toRemove;
        for (auto& [id, info] : standaloneTextures_) {
            if (info.persistent || info.refCount > 0) continue;

            if (currentFrame_ - info.lastUsedFrame > config_.standaloneExpireFrames) {
                toRemove.push_back(id);
            }
        }

        for (ImageId id : toRemove) {
            auto it = standaloneTextures_.find(id);
            if (it != standaloneTextures_.end()) {
                if (bgfx::isValid(it->second.fbo)) {
                    bgfx::destroy(it->second.fbo);
                }
                else if (bgfx::isValid(it->second.handle)) {
                    bgfx::destroy(it->second.handle);
                }
                standaloneTextures_.erase(it);

                // 配置情報も削除
                std::lock_guard locLock(locationMutex_);
                imageLocations_.erase(id);
            }
        }
    }

    uint64_t currentFrame() const { return currentFrame_; }

    //=========================================================================
    // 統計情報
    //=========================================================================
    struct Stats {
        // FontAtlas
        size_t fontPageCount;
        size_t glyphCount;
        size_t imageCount;
        float fontAvgUtilization;

        // ThumbnailGridAtlas
        uint32_t gridPageCount;
        uint32_t gridUsedRects;
        uint32_t gridPendingUploads;

        // ThumbnailShelfAtlas
        uint32_t shelfPageCount;
        uint32_t shelfUsedShelves;
        uint32_t shelfPendingUploads;

        // Standalone Textures
        size_t standaloneCount;
        size_t standalonePersistentCount;
        uint64_t standaloneTotalBytes;

        // ImageLocations
        size_t totalRegisteredImages;
        size_t pendingImages;

        uint64_t currentFrame;
    };

    Stats getStats() const {
        Stats s{};

        auto fontStats = fontAtlas_.getStats();
        s.fontPageCount = fontStats.pageCount;
        s.glyphCount = fontStats.glyphCount;
        s.imageCount = fontStats.imageCount;
        s.fontAvgUtilization = fontStats.avgUtilization;

        auto gridStats = gridAtlas_.getStats();
        s.gridPageCount = gridStats.pageCount;
        s.gridUsedRects = gridStats.usedRects;
        s.gridPendingUploads = gridStats.pendingUploads;

        auto shelfStats = shelfAtlas_.getStats();
        s.shelfPageCount = shelfStats.pageCount;
        s.shelfUsedShelves = shelfStats.usedShelves;
        s.shelfPendingUploads = shelfStats.pendingUploads;

        {
            std::lock_guard lock(standaloneMutex_);
            s.standaloneCount = standaloneTextures_.size();
            s.standaloneTotalBytes = 0;
            s.standalonePersistentCount = 0;
            for (const auto& [id, info] : standaloneTextures_) {
                s.standaloneTotalBytes += (uint64_t)info.size.x * info.size.y * 4;
                if (info.persistent) ++s.standalonePersistentCount;
            }
        }

        {
            std::lock_guard lock(locationMutex_);
            s.totalRegisteredImages = imageLocations_.size();
            s.pendingImages = 0;
            for (const auto& [id, loc] : imageLocations_) {
                if (loc.isPending) ++s.pendingImages;
            }
        }

        s.currentFrame = currentFrame_;
        return s;
    }

    //=========================================================================
    // 【Layer A】統一解決API（UI/Scene向け）
    //=========================================================================
    ResolveResult resolveFontGlyph(ImageId id) {
        ResolveResult result;

        const GlyphInfo* gi = fontAtlas_.findByImageId(id);
        if (!gi || gi->width == 0) {
            result.status = ResolveStatus::NotFound;
            return result;
        }

        bgfx::TextureHandle tex = fontAtlas_.getPageTexture(gi->pageIndex);
        if (!bgfx::isValid(tex)) {
            result.status = ResolveStatus::Evicted;
            return result;
        }

        result.status = ResolveStatus::Success;
        result.resolved.texture = tex;
        result.resolved.u0 = gi->u0;
        result.resolved.v0 = gi->v0;
        result.resolved.u1 = gi->u1;
        result.resolved.v1 = gi->v1;
        result.resolved.width = gi->width;
        result.resolved.height = gi->height;
        return result;
    }
    ResolveResult resolveThumbnail(ImageId id) {
        ResolveResult result;
        uint64_t contentId = getImageIdLocal(id);

        // まず Grid で探す
        float u0, v0, u1, v1;
        auto gridHandle = gridAtlas_.find(contentId);
        if (gridHandle) {
            float u0, v0, u1, v1;
            if (gridAtlas_.getUV(*gridHandle, u0, v0, u1, v1)) {
                result.status = ResolveStatus::Success;
                result.resolved.texture = gridAtlas_.getTexture(*gridHandle);
                result.resolved.u0 = u0;
                result.resolved.v0 = v0;
                result.resolved.u1 = u1;
                result.resolved.v1 = v1;
                return result;
            }
        }

        // 次に Shelf で探す
        auto shelfHandle = shelfAtlas_.find(contentId);
        if (shelfHandle) {
            float u0, v0, u1, v1;
            if (shelfAtlas_.getUV(*shelfHandle, u0, v0, u1, v1)) {
                result.status = ResolveStatus::Success;
                result.resolved.texture = shelfAtlas_.getTexture(*shelfHandle);
                result.resolved.u0 = u0;
                result.resolved.v0 = v0;
                result.resolved.u1 = u1;
                result.resolved.v1 = v1;
                return result;
            }
        }

        result.status = ResolveStatus::NotFound;
        return result;
    }
    // ImageId → 描画可能なテクスチャを解決
    ResolveResult resolve(ImageId id) {
        ResolveResult result;

        if (id == 0) {
            result.status = ResolveStatus::NotFound;
            return result;
        }

        ImageIdDomain domain = getImageIdDomain(id);

        switch (domain) {
        case ImageIdDomain::FontGlyph:
            return resolveFontGlyph(id);

        case ImageIdDomain::Thumbnail:
            return resolveThumbnail(id);  // Grid/Shelf 両方

        case ImageIdDomain::Offscreen:
        case ImageIdDomain::Memory:
        case ImageIdDomain::Generated:
        case ImageIdDomain::File:
            return resolveStandalone(id);

        default:
            result.status = ResolveStatus::NotFound;
            return result;
        }
    }

    // 簡易版（成功時のみテクスチャ情報を返す）
    bool resolve(ImageId id, bgfx::TextureHandle& outTex,
        float& u0, float& v0, float& u1, float& v1) {
        auto result = resolve(id);
        if (!result.isReady()) return false;

        outTex = result.resolved.texture;
        u0 = result.resolved.u0;
        v0 = result.resolved.v0;
        u1 = result.resolved.u1;
        v1 = result.resolved.v1;
        return true;
    }

    // 描画用（Pending時はplaceholder）
    ResolvedTexture resolveForDraw(ImageId id) {
        return resolve(id).forDraw();
    }

    //=========================================================================
    // 【Layer A】ImageId 登録/管理
    //=========================================================================

    // ImageIdの状態を確認
    bool exists(ImageId id) const {
        std::lock_guard lock(locationMutex_);
        return imageLocations_.find(id) != imageLocations_.end();
    }

    bool isPending(ImageId id) const {
        std::lock_guard lock(locationMutex_);
        auto it = imageLocations_.find(id);
        return it != imageLocations_.end() && it->second.isPending;
    }

    // Pending状態で登録（ロード開始時）
    void registerPending(ImageId id) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::None;
        loc.isPending = true;
    }

    // touch（使用マーク）
    void touch(ImageId id) {
        ImageIdDomain domain = getImageIdDomain(id);

        // Standalone系
        if (domain == ImageIdDomain::Offscreen ||
            domain == ImageIdDomain::Memory ||
            domain == ImageIdDomain::Generated ||
            domain == ImageIdDomain::File) {
            std::lock_guard lock(standaloneMutex_);
            auto it = standaloneTextures_.find(id);
            if (it != standaloneTextures_.end()) {
                it->second.lastUsedFrame = currentFrame_;
            }
        }

        // Grid/Shelf は内部ハンドル経由で touch
        std::lock_guard lock(locationMutex_);
        auto it = imageLocations_.find(id);
        if (it != imageLocations_.end()) {
            if (it->second.type == ImageLocation::Type::GridAtlas) {
                gridAtlas_.touch(it->second.thumbnail.handle);
            }
            else if (it->second.type == ImageLocation::Type::ShelfAtlas) {
                shelfAtlas_.touch(it->second.thumbnail.handle);
            }
        }
    }

    // 参照カウント
    void retain(ImageId id) {
        ImageIdDomain domain = getImageIdDomain(id);
        if (domain == ImageIdDomain::Offscreen ||
            domain == ImageIdDomain::Memory ||
            domain == ImageIdDomain::Generated ||
            domain == ImageIdDomain::File) {
            std::lock_guard lock(standaloneMutex_);
            auto it = standaloneTextures_.find(id);
            if (it != standaloneTextures_.end()) {
                ++it->second.refCount;
                it->second.lastUsedFrame = currentFrame_;
            }
        }
        // Grid/Shelfは内部LRUで管理
    }

    void release(ImageId id) {
        ImageIdDomain domain = getImageIdDomain(id);
        if (domain == ImageIdDomain::Offscreen ||
            domain == ImageIdDomain::Memory ||
            domain == ImageIdDomain::Generated ||
            domain == ImageIdDomain::File) {
            std::lock_guard lock(standaloneMutex_);
            auto it = standaloneTextures_.find(id);
            if (it != standaloneTextures_.end() && it->second.refCount > 0) {
                --it->second.refCount;
            }
        }
    }

    //=========================================================================
    // 【Layer B】Standalone テクスチャ（低レベル）
    //=========================================================================

    // 作成（ピクセルデータから）
    ImageId createStandaloneTexture(const void* pixels, uint16_t w, uint16_t h,
        int pitch, bool persistent = false) {
        ImageId id = ImageIdGenerator::fromMemory();
        if (!createStandaloneInternal(id, pixels, w, h, pitch, persistent,
            ImageOrigin::Memory, false)) {
            return 0;
        }
        return id;
    }

    // 作成（SDL_Surface から）
    ImageId createStandaloneTexture(SDL_Surface* surface, bool persistent = false) {
        if (!surface) return 0;

        SDL_Surface* s = surface;
        SDL_Surface* conv = nullptr;
        if (s->format != SDL_PIXELFORMAT_RGBA8888) {
            conv = SDL_ConvertSurface(s, SDL_PIXELFORMAT_RGBA8888);
            s = conv;
        }

        ImageId id = createStandaloneTexture(s->pixels, s->w, s->h, s->pitch, persistent);

        if (conv) SDL_DestroySurface(conv);
        return id;
    }

    // 作成（空テクスチャ）
    ImageId createEmptyStandaloneTexture(uint16_t w, uint16_t h, bool persistent = false) {
        if (w == 0 || h == 0) return 0;

        ImageId id = ImageIdGenerator::fromMemory();

        std::lock_guard lock(standaloneMutex_);

        bgfx::TextureHandle handle = bgfx::createTexture2D(
            w, h, false, 1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            nullptr);

        if (!bgfx::isValid(handle)) return 0;

        auto& info = standaloneTextures_[id];
        info.handle = handle;
        info.size.x = w;
        info.size.y = h;
        info.refCount = 1;
        info.lastUsedFrame = currentFrame_;
        info.persistent = persistent;
        info.origin = ImageOrigin::Memory;
        info.isRenderTarget = false;

        // 配置情報を登録
        {
            std::lock_guard locLock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::Standalone;
            loc.width = w;
            loc.height = h;
            loc.isPending = false;
        }

        return id;
    }
    //=========================================================================
    // テクスチャポインタ取得API（追加）
    //=========================================================================

    // Standalone テクスチャへのポインタ取得
    bgfx::TextureHandle * getStandaloneTexturePtr(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it != standaloneTextures_.end()) {
            return &it->second.handle;
        }
        return nullptr;
    }

    // FBO へのポインタ取得
    bgfx::FrameBufferHandle* getOffscreenFBOPtr(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it != standaloneTextures_.end() && it->second.isRenderTarget) {
            return &it->second.fbo;
        }
        return nullptr;
    }

    // 旧API互換
    ImageId reserveOffscreenTexture(uint16_t w, uint16_t h,
        bool persistent = false) {
        ImageId id = ImageIdGenerator::forOffscreen();

        // 先にエントリを作成（テクスチャ未生成）
        {
            std::lock_guard lock(standaloneMutex_);
            auto [it, inserted] = standaloneTextures_.emplace(id, StandaloneTextureInfo{});
            auto& info = it->second;
            info.size.x = w;
            info.size.y = h;
            info.persistent = persistent;
            info.isRenderTarget = true;
            info.origin = ImageOrigin::Offscreen;
            info.refCount = 1;
            {
                std::lock_guard lock(locationMutex_);
                auto& loc = imageLocations_[id];
                loc.type = ImageLocation::Type::Standalone;
                loc.width = w;
                loc.height = h;
                loc.isPending = true;
            }

            // 作成キューに追加
            {
                std::lock_guard lock(offscreenQueueMutex_);
                offscreenCreateQueue_.push_back({ id, w, h, persistent});
            }

            return id;
        }

        return id;
    }
    void queueOffscreenResize(ImageId id, uint16_t newW, uint16_t newH) {
        std::lock_guard lock(offscreenQueueMutex_);
        offscreenResizeQueue_.push_back({ id, newW, newH });
    }
    // 通常テクスチャ予約（ロード前にポインタ取得）
    ImageId reserveTexture(bgfx::TextureHandle** outTexPtr, uint16_t w, uint16_t h) {
        ImageId id = ImageIdGenerator::fromMemory();

        {
            std::lock_guard lock(standaloneMutex_);
            auto [it, inserted] = standaloneTextures_.emplace(id, StandaloneTextureInfo{});
            auto& info = it->second;
            info.size.x = w;
            info.size.y = h;
            info.origin = ImageOrigin::Memory;

            if (outTexPtr) *outTexPtr = &info.handle;
        }

        {
            std::lock_guard lock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::Standalone;
            loc.width = w;
            loc.height = h;
            loc.isPending = true;
        }

        return id;
    }
    // テクスチャデータをアップロード（予約済みIDに対して）
    bool uploadTexture(ImageId id, const void* pixels, uint16_t w, uint16_t h, int pitch) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it == standaloneTextures_.end()) return false;

        auto& info = it->second;

        // テクスチャ作成
        const bgfx::Memory* mem = bgfx::copy(pixels, w * h * 4);
        info.handle = bgfx::createTexture2D(
            w, h, false, 1,
            bgfx::TextureFormat::RGBA8,
            BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            mem);

        if (!bgfx::isValid(info.handle)) return false;

        info.size.x = w;
        info.size.y = h;
        info.lastUsedFrame = currentFrame_;

        // Pending解除
        {
            std::lock_guard locLock(locationMutex_);
            auto locIt = imageLocations_.find(id);
            if (locIt != imageLocations_.end()) {
                locIt->second.isPending = false;
            }
        }

        return true;
    }

    //=========================================================================
    // FontAtlas テクスチャポインタ取得
    //=========================================================================

    bgfx::TextureHandle* getFontAtlasTexturePtr(uint16_t pageIndex) {
        return &fontAtlas_.getPageTexture(pageIndex);
    }

    // グリフ取得（ポインタ付き）
    struct GlyphResult {
        const GlyphInfo* info = nullptr;
        bgfx::TextureHandle* texturePtr = nullptr;
    };

    GlyphResult getOrAddGlyphWithPtr(FontId font, uint32_t codepoint,
        RenderGroup* group = nullptr,
        SDL_Color color = { 255,255,255,255 }) {
        GlyphResult result;
        const auto& gi = fontAtlas_.getOrAddGlyph(font, codepoint, group, color);
        result.info = &gi;
        if (gi.width > 0) {
            result.texturePtr = &fontAtlas_.getPageTexture(gi.pageIndex);
        }
        return result;
    }

    //=========================================================================
    // Thumbnail テクスチャポインタ取得
    //=========================================================================

    bgfx::TextureHandle* getGridAtlasTexturePtr(uint16_t pageIndex) {
        return (bgfx::TextureHandle*)(&gridAtlas_.getTexture(pageIndex));
    }

    bgfx::TextureHandle* getShelfAtlasTexturePtr(uint16_t pageIndex) {
        return (bgfx::TextureHandle*)(&shelfAtlas_.getTexture(pageIndex));
    }

    // Thumbnail 取得（ポインタ付き）
    struct ThumbnailResult {
        ThumbnailHandle handle;
        bgfx::TextureHandle* texturePtr = nullptr;
        float u0 = 0, v0 = 0, u1 = 1, v1 = 1;
        bool needsLoad = false;
        bool found = false;
    };

    ThumbnailResult acquireGridThumbnailWithPtr(uint64_t contentId) {
        ThumbnailResult result;
        result.handle = gridAtlas_.acquire(contentId, result.needsLoad);
        if (result.handle.isValid()) {
            result.found = true;
            result.texturePtr = getGridAtlasTexturePtr(result.handle.pageIndex);
            gridAtlas_.getUV(result.handle, result.u0, result.v0, result.u1, result.v1);
        }
        return result;
    }

    ThumbnailResult acquireShelfThumbnailWithPtr(uint64_t contentId, uint16_t w, uint16_t h) {
        ThumbnailResult result;
        result.handle = shelfAtlas_.acquire(contentId, w, h, result.needsLoad);
        if (result.handle.isValid()) {
            result.found = true;
            result.texturePtr = getShelfAtlasTexturePtr(result.handle.pageIndex);
            shelfAtlas_.getUV(result.handle, result.u0, result.v0, result.u1, result.v1);
        }
        return result;
    }

    //=========================================================================
    // Placeholder テクスチャポインタ
    //=========================================================================

    bgfx::TextureHandle* getPlaceholderTexturePtr() {
        const auto& white = fontAtlas_.getWhitePixel();
        return &fontAtlas_.getPageTexture(white.pageIndex);
    }


    bool resizeOffscreenInternal(ImageId id, uint16_t newW, uint16_t newH) {
        if (newW == 0 || newH == 0) return false;

        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it == standaloneTextures_.end() || !it->second.isRenderTarget) {
            return false;
        }

        auto& info = it->second;

        // サイズ変更なしなら何もしない
        if (info.size.x == newW && info.size.y == newH) {
            return true;
        }

        // 古いリソース破棄
        if (bgfx::isValid(info.fbo)) {
            bgfx::destroy(info.fbo);
        }

        // 新しいテクスチャ作成
        bgfx::TextureHandle newHandle = bgfx::createTexture2D(
            newW, newH, false, 1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            nullptr);

        if (!bgfx::isValid(newHandle)) return false;

        bgfx::FrameBufferHandle newFbo = bgfx::createFrameBuffer(1, &newHandle, true);
        if (!bgfx::isValid(newFbo)) {
            bgfx::destroy(newHandle);
            return false;
        }

        info.handle = newHandle;
        info.fbo = newFbo;
        info.size.x = newW;
        info.size.y = newH;
        info.lastUsedFrame = currentFrame_;

        // 配置情報も更新
        {
            std::lock_guard locLock(locationMutex_);
            auto locIt = imageLocations_.find(id);
            if (locIt != imageLocations_.end()) {
                locIt->second.width = newW;
                locIt->second.height = newH;
            }
        }

        return true;
    }
    struct OffscreenCreateRequest {
        ImageId id;
        uint16_t width, height;
        bool persistent;
        bgfx::FrameBufferHandle* dest;
        PointI* fbsize = nullptr;
    };

    struct OffscreenResizeRequest {
        ImageId id;
        uint16_t width, height;
        bgfx::FrameBufferHandle* dest;
        PointI* fbsize = nullptr;
    };

    std::mutex offscreenQueueMutex_;
    std::vector<OffscreenCreateRequest> offscreenCreateQueue_;
    std::vector<OffscreenResizeRequest> offscreenResizeQueue_;

    void processOffscreenQueue() {
        std::vector<OffscreenCreateRequest> creates;
        std::vector<OffscreenResizeRequest> resizes;
        {
            std::lock_guard lock(offscreenQueueMutex_);
            creates.swap(offscreenCreateQueue_);
            resizes.swap(offscreenResizeQueue_);
        }
        int size = creates.size() + resizes.size();
        // 作成
        for (auto& req : creates) {
            if (createOffscreenInternal(req.id, req.width, req.height, req.persistent)) {
                if (req.dest) {
                    auto textureInfo = getStandaloneTexture(req.id);
                }
            }
        }

        for (auto& req : resizes) {
            if (resizeOffscreenInternal(req.id, req.width, req.height)) {
                if (req.dest) {
                    auto textureInfo = getStandaloneTexture(req.id);
                }
            }
        }
        if (size != 0)bgfx::frame();
    }
    void destroyStandalone(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it != standaloneTextures_.end()) {
            if (bgfx::isValid(it->second.fbo)) {
                bgfx::destroy(it->second.fbo);
            }
            else if (bgfx::isValid(it->second.handle)) {
                bgfx::destroy(it->second.handle);
            }
            standaloneTextures_.erase(it);

            // 配置情報も削除
            std::lock_guard locLock(locationMutex_);
            imageLocations_.erase(id);
        }
    }

    // 旧API互換
    void touchStandaloneTexture(ImageId id) {
        touch(id);
    }

    void retainStandaloneTexture(ImageId id) {
        retain(id);
    }

    void releaseStandaloneTexture(ImageId id) {
        release(id);
    }

    void destroyStandaloneTexture(ImageId id) {
        destroyStandalone(id);
    }

    // Standalone テクスチャ情報取得
    StandaloneTextureInfo* getStandaloneTexture(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        return (it != standaloneTextures_.end()) ? &it->second : nullptr;
    }

    // Standalone テクスチャ更新
    bool updateStandaloneTexture(ImageId id, const void* pixels,
        uint16_t x, uint16_t y, uint16_t w, uint16_t h, int pitch) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it == standaloneTextures_.end()) return false;
        if (!bgfx::isValid(it->second.handle)) return false;

        std::vector<uint8_t> data(w * h * 4);
        const uint8_t* src = (const uint8_t*)pixels;
        for (uint16_t row = 0; row < h; ++row) {
            std::memcpy(data.data() + row * w * 4, src + row * pitch, w * 4);
        }

        bgfx::updateTexture2D(it->second.handle, 0, 0, x, y, w, h,
            bgfx::copy(data.data(), (uint32_t)data.size()));

        it->second.lastUsedFrame = currentFrame_;
        return true;
    }

    //=========================================================================
    // 【Layer B】FontAtlas（低レベル）
    //=========================================================================

    FontAtlas& fontAtlas() { return fontAtlas_; }
    const FontAtlas& fontAtlas() const { return fontAtlas_; }

    // FontAtlas 便利メソッド（委譲）
    FontId registerFont(const char* name, const std::string& fontPath, int size) {
        return fontAtlas_.registerFont(name, fontPath, size);
    }

    void setFallbackChain(FontId primary, std::initializer_list<FontId> fallbacks) {
        fontAtlas_.setFallbackChain(primary, fallbacks);
    }

    const GlyphInfo& getWhitePixel() const {
        return fontAtlas_.getWhitePixel();
    }

    const GlyphInfo& getOrAddGlyph(FontId font, uint32_t codepoint,
        RenderGroup* group = nullptr,
        SDL_Color color = { 255, 255, 255, 255 }) {
        return fontAtlas_.getOrAddGlyph(font, codepoint, group, color);
    }

    const GlyphInfo* getImage(ImageId id) const {
        return fontAtlas_.getImage(id);
    }

    bgfx::TextureHandle getFontTexture(uint16_t pageIndex) {
        return fontAtlas_.getPageTexture(pageIndex);
    }

    // グリフ追加してImageIdを返す
    ImageId addGlyph(FontId font, uint32_t codepoint, SDL_Color color = { 255,255,255,255 }) {
        // FontIndex を取得（衝突なしの連番）
        uint32_t fontIndex = fontAtlas_.getFontIndex(font);
        if (fontIndex == 0) return 0;  // 未登録フォント

        const auto& gi = fontAtlas_.getOrAddGlyph(font, codepoint, nullptr, color);
        if (gi.width == 0) return 0;

        // localId = fontIndex(24bit) + codepoint(24bit) で一意
        // codepoint は Unicode で最大 0x10FFFF (21bit) なので 24bit で十分
        uint64_t localId = (uint64_t(fontIndex) << 24) | (codepoint & 0x00FFFFFF);
        ImageId id = ImageIdGenerator::forFontGlyph(localId);

        // 配置情報を登録
        {
            std::lock_guard lock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::FontAtlas;
            loc.font.glyphKey = localId;
            loc.width = gi.width;
            loc.height = gi.height;
            loc.isPending = false;
        }

        return id;
    }

    // 画像追加してImageIdを返す
    ImageId addImageToFontAtlas(SDL_Surface* surface, bool pinned = false) {
        ImageId id = ImageIdGenerator::forFontGlyph(nextFontAtlasImageId_++);
        const auto& gi = fontAtlas_.addImage(id, surface, nullptr, pinned);
        if (gi.width == 0) return 0;

        {
            std::lock_guard lock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::FontAtlas;
            loc.font.glyphKey = id;
            loc.width = gi.width;
            loc.height = gi.height;
            loc.isPending = false;
        }

        return id;
    }

    // 直接アクセス
    bgfx::TextureHandle getFontAtlasTexture(uint16_t pageIndex) {
        return fontAtlas_.getPageTexture(pageIndex);
    }

    //=========================================================================
    // 【Layer B】ThumbnailGridAtlas（低レベル）
    //=========================================================================

    ThumbnailGridAtlas& gridAtlas() { return gridAtlas_; }
    const ThumbnailGridAtlas& gridAtlas() const { return gridAtlas_; }

    // Grid 便利メソッド（委譲）
    bool getGridUV(ThumbnailHandle handle, float& u0, float& v0, float& u1, float& v1) const {
        return gridAtlas_.getUV(handle, u0, v0, u1, v1);
    }

    void getGridPlaceholderUV(float& u0, float& v0, float& u1, float& v1) const {
        gridAtlas_.getPlaceholderUV(u0, v0, u1, v1);
    }

    bgfx::TextureHandle getGridTexture(ThumbnailHandle handle) {
        return gridAtlas_.getTexture(handle);
    }

    bgfx::TextureHandle getGridTexture(uint16_t pageIndex) {
        return gridAtlas_.getTexture(pageIndex);
    }

    void touchGrid(ThumbnailHandle handle) {
        gridAtlas_.touch(handle);
    }

    bool isGridValid(ThumbnailHandle handle) const {
        return gridAtlas_.isValid(handle);
    }

    // サムネイル取得/作成してImageIdを返す
    ImageId acquireGridThumbnail(uint64_t contentId, bool& outNeedsLoad) {
        ImageId id = ImageIdGenerator::forThumbnail(contentId);
        ThumbnailHandle handle = gridAtlas_.acquire(contentId, outNeedsLoad);

        if (!handle.isValid()) return 0;

        {
            std::lock_guard lock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::GridAtlas;
            loc.thumbnail.handle = handle;
            loc.isPending = outNeedsLoad;
        }

        return id;
    }

    // アップロード完了通知
    void onGridUploadComplete(ImageId id) {
        std::lock_guard lock(locationMutex_);
        auto it = imageLocations_.find(id);
        if (it != imageLocations_.end()) {
            it->second.isPending = false;
        }
    }

    // 直接アクセス
    bool queueGridUpload(ImageId id, const void* data, uint16_t w, uint16_t h) {
        std::lock_guard lock(locationMutex_);
        auto it = imageLocations_.find(id);
        if (it == imageLocations_.end() || it->second.type != ImageLocation::Type::GridAtlas) {
            return false;
        }
        bool result = gridAtlas_.queueUpload(it->second.thumbnail.handle, data, w, h);
        if (result) {
            it->second.isPending = false;
            it->second.width = w;
            it->second.height = h;
        }
        return result;
    }

    bgfx::TextureHandle getGridAtlasTexture(uint16_t pageIndex) {
        return gridAtlas_.getTexture(pageIndex);
    }

    //=========================================================================
    // 【Layer B】ThumbnailShelfAtlas（低レベル）
    //=========================================================================

    ThumbnailShelfAtlas& shelfAtlas() { return shelfAtlas_; }
    const ThumbnailShelfAtlas& shelfAtlas() const { return shelfAtlas_; }

    // Shelf 便利メソッド（委譲）
    bool getShelfUV(ThumbnailHandle handle, float& u0, float& v0, float& u1, float& v1) const {
        return shelfAtlas_.getUV(handle, u0, v0, u1, v1);
    }

    void getShelfPlaceholderUV(float& u0, float& v0, float& u1, float& v1) const {
        shelfAtlas_.getPlaceholderUV(u0, v0, u1, v1);
    }

    bgfx::TextureHandle getShelfTexture(ThumbnailHandle handle) {
        return shelfAtlas_.getTexture(handle);
    }

    bgfx::TextureHandle getShelfTexture(uint16_t pageIndex) {
        return shelfAtlas_.getTexture(pageIndex);
    }

    void touchShelf(ThumbnailHandle handle) {
        shelfAtlas_.touch(handle);
    }

    bool isShelfValid(ThumbnailHandle handle) const {
        return shelfAtlas_.isValid(handle);
    }

    ImageId acquireShelfThumbnail(uint64_t contentId, uint16_t w, uint16_t h,
        bool& outNeedsLoad) {
        ImageId id = ImageIdGenerator::forThumbnail(contentId);
        ThumbnailHandle handle = shelfAtlas_.acquire(contentId, w, h, outNeedsLoad);

        if (!handle.isValid()) return 0;

        {
            std::lock_guard lock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::ShelfAtlas;
            loc.thumbnail.handle = handle;
            loc.width = w;
            loc.height = h;
            loc.isPending = outNeedsLoad;
        }

        return id;
    }

    bool queueShelfUpload(ImageId id, const void* data, uint16_t w, uint16_t h) {
        std::lock_guard lock(locationMutex_);
        auto it = imageLocations_.find(id);
        if (it == imageLocations_.end() || it->second.type != ImageLocation::Type::ShelfAtlas) {
            return false;
        }
        bool result = shelfAtlas_.queueUpload(it->second.thumbnail.handle, data, w, h);
        if (result) {
            it->second.isPending = false;
        }
        return result;
    }

    bgfx::TextureHandle getShelfAtlasTexture(uint16_t pageIndex) {
        return shelfAtlas_.getTexture(pageIndex);
    }

    //=========================================================================
    // ImageLoader から呼ばれる登録メソッド（ID指定版）
    //=========================================================================

    // FontAtlas画像を登録（ImageLoader用）
    void registerFontAtlasImage(ImageId id, uint16_t width, uint16_t height) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::FontAtlas;
        loc.font.glyphKey = id;
        loc.width = width;
        loc.height = height;
        loc.isPending = false;
    }

    // Grid画像を登録（ImageLoader用）
    void registerGridImage(ImageId id, ThumbnailHandle handle, uint16_t width, uint16_t height) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::GridAtlas;
        loc.thumbnail.handle = handle;
        loc.width = width;
        loc.height = height;
        loc.isPending = false;
    }

    // Shelf画像を登録（ImageLoader用）
    void registerShelfImage(ImageId id, ThumbnailHandle handle, uint16_t width, uint16_t height) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::ShelfAtlas;
        loc.thumbnail.handle = handle;
        loc.width = width;
        loc.height = height;
        loc.isPending = false;
    }

    // Standalone作成（ID指定版、ImageLoader用）
    bool createStandaloneWithId(ImageId id, const void* pixels, uint16_t w, uint16_t h,
        int pitch, bool persistent) {
        return createStandaloneInternal(id, pixels, w, h, pitch, persistent,
            ImageOrigin::File, false);
    }

    //=========================================================================
    // Placeholder
    //=========================================================================

    ResolvedTexture getPlaceholder() {
        const auto& white = fontAtlas_.getWhitePixel();
        ResolvedTexture p;
        p.texture = fontAtlas_.getPageTexture(white.pageIndex);
        p.u0 = white.u0; p.v0 = white.v0;
        p.u1 = white.u1; p.v1 = white.v1;
        p.width = white.width;
        p.height = white.height;
        return p;
    }
public:
    void destroyOffscreenInternal(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it != standaloneTextures_.end()) {
            if (bgfx::isValid(it->second.fbo)) {
                bgfx::destroy(it->second.fbo);
            }
            else if (bgfx::isValid(it->second.handle)) {
                bgfx::destroy(it->second.handle);
            }
            standaloneTextures_.erase(it);
        }

        std::lock_guard locLock(locationMutex_);
        imageLocations_.erase(id);
    }
private:
    //=========================================================================
    // 内部resolve関数
    //=========================================================================

    ResolveResult resolveStandalone(ImageId id) {
        ResolveResult result;
        std::lock_guard lock(standaloneMutex_);

        auto it = standaloneTextures_.find(id);
        if (it == standaloneTextures_.end() || !bgfx::isValid(it->second.handle)) {
            result.status = ResolveStatus::NotFound;
            return result;
        }

        it->second.lastUsedFrame = currentFrame_;

        result.status = ResolveStatus::Success;
        result.resolved.texture = it->second.handle;
        result.resolved.u0 = 0; result.resolved.v0 = 0;
        result.resolved.u1 = 1; result.resolved.v1 = 1;
        result.resolved.width = it->second.size.x;
        result.resolved.height = it->second.size.y;
        return result;
    }

    ResolveResult resolveGridAtlas(ImageId id, const ImageLocation& loc) {
        ResolveResult result;

        float u0, v0, u1, v1;
        if (!gridAtlas_.getUV(loc.thumbnail.handle, u0, v0, u1, v1)) {
            // eviction されている可能性
            result.status = ResolveStatus::Evicted;
            return result;
        }

        gridAtlas_.touch(loc.thumbnail.handle);

        result.status = ResolveStatus::Success;
        result.resolved.texture = gridAtlas_.getTexture(loc.thumbnail.handle);
        result.resolved.u0 = u0; result.resolved.v0 = v0;
        result.resolved.u1 = u1; result.resolved.v1 = v1;
        result.resolved.width = loc.width;
        result.resolved.height = loc.height;
        return result;
    }

    ResolveResult resolveShelfAtlas(ImageId id, const ImageLocation& loc) {
        ResolveResult result;

        float u0, v0, u1, v1;
        if (!shelfAtlas_.getUV(loc.thumbnail.handle, u0, v0, u1, v1)) {
            result.status = ResolveStatus::Evicted;
            return result;
        }

        shelfAtlas_.touch(loc.thumbnail.handle);

        result.status = ResolveStatus::Success;
        result.resolved.texture = shelfAtlas_.getTexture(loc.thumbnail.handle);
        result.resolved.u0 = u0; result.resolved.v0 = v0;
        result.resolved.u1 = u1; result.resolved.v1 = v1;
        result.resolved.width = loc.width;
        result.resolved.height = loc.height;
        return result;
    }

    //=========================================================================
    // 内部作成関数
    //=========================================================================

    bool createStandaloneInternal(ImageId id, const void* pixels, uint16_t w, uint16_t h,
        int pitch, bool persistent, ImageOrigin origin,
        bool isRenderTarget) {
        std::lock_guard lock(standaloneMutex_);

        std::vector<uint8_t> data(w * h * 4);
        const uint8_t* src = (const uint8_t*)pixels;
        for (uint16_t y = 0; y < h; ++y) {
            std::memcpy(data.data() + y * w * 4, src + y * pitch, w * 4);
        }

        uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
        if (isRenderTarget) flags |= BGFX_TEXTURE_RT;

        bgfx::TextureHandle handle = bgfx::createTexture2D(
            w, h, false, 1, bgfx::TextureFormat::RGBA8, flags,
            bgfx::copy(data.data(), (uint32_t)data.size()));

        if (!bgfx::isValid(handle)) return false;

        auto& info = standaloneTextures_[id];
        info.handle = handle;
        info.size.x = w;
        info.size.y = h;
        info.refCount = 1;
        info.lastUsedFrame = currentFrame_;
        info.persistent = persistent;
        info.origin = origin;
        info.isRenderTarget = isRenderTarget;

        // 配置情報を登録
        {
            std::lock_guard locLock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::Standalone;
            loc.width = w;
            loc.height = h;
            loc.isPending = false;
        }

        return true;
    }
    bool createOffscreenInternal(ImageId id, uint16_t w, uint16_t h, bool persistent) {
        std::lock_guard lock(standaloneMutex_);

        bgfx::TextureHandle handle = bgfx::createTexture2D(
            w, h, false, 1, bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            nullptr);

        if (!bgfx::isValid(handle)) return false;

        bgfx::FrameBufferHandle fbo = bgfx::createFrameBuffer(1, &handle, true);
        if (!bgfx::isValid(fbo)) {
            bgfx::destroy(handle);
            return false;
        }

        auto& info = standaloneTextures_[id];
        info.handle = handle;
        info.fbo = fbo;
        info.size.x = w;
        info.size.y = h;
        info.refCount = 1;
        info.lastUsedFrame = currentFrame_;
        info.persistent = persistent;
        info.origin = ImageOrigin::Offscreen;
        info.isRenderTarget = true;

        {
            std::lock_guard locLock(locationMutex_);
            auto& loc = imageLocations_[id];
            loc.type = ImageLocation::Type::Standalone;
            loc.width = w;
            loc.height = h;
            loc.isPending = false;
        }

        return true;
    }

    //=========================================================================
    // メンバ
    //=========================================================================

    Config config_;
    uint64_t currentFrame_ = 0;

    // Atlas
    FontAtlas fontAtlas_;
    ThumbnailGridAtlas gridAtlas_;
    ThumbnailShelfAtlas shelfAtlas_;

    // Standalone textures
    mutable std::mutex standaloneMutex_;
    std::unordered_map<ImageId, StandaloneTextureInfo> standaloneTextures_;

    // Group Manager
    ImageMasterGroupManager groupManager_;

public:
    //=========================================================================
    // RenderGroup 管理（委譲 + 実装）
    //=========================================================================

    ExtendedRenderGroup& createGroup(SurfaceId surfaceId,
        RenderGroupFlags flags = RenderGroupFlags::None) {
        return groupManager_.createGroup(surfaceId, flags);
    }

    ExtendedRenderGroup* getGroup(RenderGroupId id) {
        return groupManager_.getGroup(id);
    }

    void destroyGroup(RenderGroupId id) {
        auto* g = groupManager_.getGroup(id);
        if (!g) return;

        // FontAtlas への通知
        fontAtlas_.onGroupDestroyed(*g);

        // Standalone texture の参照解放
        for (auto texId : g->usedStandaloneTextures) {
            release(texId);
        }

        // Surface リストから削除
        auto& sv = groupManager_.surfaceGroups_[g->surfaceId];
        sv.erase(std::remove(sv.begin(), sv.end(), id), sv.end());

        groupManager_.groups_.erase(id);
    }

    void destroySurfaceGroups(SurfaceId surfaceId) {
        auto it = groupManager_.surfaceGroups_.find(surfaceId);
        if (it == groupManager_.surfaceGroups_.end()) return;

        for (RenderGroupId gid : it->second) {
            auto git = groupManager_.groups_.find(gid);
            if (git != groupManager_.groups_.end()) {
                fontAtlas_.onGroupDestroyed(git->second);
                for (auto texId : git->second.usedStandaloneTextures) {
                    release(texId);
                }
                groupManager_.groups_.erase(git);
            }
        }
        groupManager_.surfaceGroups_.erase(it);
    }

    void collectExpiredGroups(uint32_t expireFrames = 300) {
        std::vector<RenderGroupId> expired;
        for (auto& [id, g] : groupManager_.groups_) {
            if (g.isPinned()) continue;
            if (groupManager_.currentFrame_ - g.lastUsedFrame > expireFrames) {
                expired.push_back(id);
            }
        }
        for (RenderGroupId id : expired) {
            destroyGroup(id);
        }
    }

    ImageMasterGroupManager& groupManager() { return groupManager_; }

    // ImageId → 配置情報のマッピング
    mutable std::mutex locationMutex_;
    std::unordered_map<ImageId, ImageLocation> imageLocations_;

    std::atomic<uint64_t> nextFontAtlasImageId_{ 0x1000000 };
};



//=============================================================================
// ImageSource - 画像ソース
//=============================================================================
struct ImageSource {
    enum class Type { File, Memory, SDL_Surface };

    Type type = Type::File;
    std::string path;
    const void* data = nullptr;
    size_t dataSize = 0;
    SDL_Surface* surface = nullptr;

    static ImageSource fromFile(const std::string& path) {
        return { Type::File, path, nullptr, 0, nullptr };
    }
    static ImageSource fromMemory(const void* data, size_t size) {
        return { Type::Memory, "", data, size, nullptr };
    }
    static ImageSource fromSurface(SDL_Surface* surface) {
        return { Type::SDL_Surface, "", nullptr, 0, surface };
    }
};

//=============================================================================
// ImageUsage - 配置ヒント
//=============================================================================
enum class ImageUsage : uint8_t {
    Auto,           // サイズから自動判断
    Icon,           // 小さな正方形 → Grid
    Thumbnail,      // 通常サムネ → サイズで Grid or Shelf
    VideoThumb,     // 横長サムネ → Shelf
    Background,     // 背景画像 → Standalone (persistent)
    UIImage,        // UI画像 → Standalone or FontAtlas
    Dynamic,        // 動的更新 → Standalone
};

//=============================================================================
// DecodedImage
//=============================================================================
struct DecodedImage {
    std::vector<uint8_t> pixels;  // RGBA8888
    uint16_t width = 0;
    uint16_t height = 0;

    bool isValid() const { return !pixels.empty() && width > 0 && height > 0; }
};

using FileDecoder = std::function<DecodedImage(const std::string& path)>;
using MemoryDecoder = std::function<DecodedImage(const void* data, size_t size)>;

//=============================================================================
// PlacementPolicy - 配置ポリシー
//=============================================================================
struct PlacementPolicy {
    uint16_t gridMaxSize = 128;
    uint16_t shelfMaxHeight = 160;
    uint16_t shelfMaxWidth = 512;
    float shelfMinAspect = 1.2f;
    uint16_t fontAtlasMaxSize = 64;
    size_t maxCpuCacheBytes = 128 * 1024 * 1024;  // 128MB
    uint32_t cacheExpireFrames = 600;
};

//=============================================================================
// CachedImage - CPUキャッシュエントリ
//=============================================================================
struct CachedImage {
    ImageId imageId = 0;
    std::vector<uint8_t> pixels;  // RGBA
    uint16_t width = 0;
    uint16_t height = 0;
    ImageUsage usage = ImageUsage::Auto;
    bool persistent = false;
    uint64_t lastUsedFrame = 0;
    uint32_t refCount = 0;
    bool isUploaded = false;

    size_t byteSize() const { return pixels.size(); }
};

//=============================================================================
// LoadRequest - 非同期ロードリクエスト
//=============================================================================
struct LoadRequest {
    ImageId imageId;              // ← 変更: 事前に決定されたID
    ImageSource source;
    ImageUsage usage;
    bool persistent;
    std::function<void(ImageId, bool success)> callback;  // ← 変更: ImageId を返す
};

//=============================================================================
// ImageLoader
//=============================================================================
class ImageLoader {
public:
    explicit ImageLoader(ImageMaster& master, const PlacementPolicy& policy = {})
        : master_(master), policy_(policy) {
        setDefaultDecoders();
    }

    ~ImageLoader() { shutdown(); }

    //=========================================================================
    // デコーダー設定
    //=========================================================================
    void setFileDecoder(FileDecoder decoder) {
        fileDecoder_ = std::move(decoder);
    }

    void setMemoryDecoder(MemoryDecoder decoder) {
        memoryDecoder_ = std::move(decoder);
    }

    //=========================================================================
    // 初期化・終了
    //=========================================================================
    bool initialize(int workerThreads = 2) {
        if (running_) return true;

        running_ = true;
        for (int i = 0; i < workerThreads; ++i) {
            workers_.emplace_back(&ImageLoader::workerThread, this);
        }
        return true;
    }

    void shutdown() {
        {
            std::lock_guard lock(queueMutex_);
            running_ = false;
        }
        queueCv_.notify_all();

        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
        workers_.clear();

        std::lock_guard lock(cacheMutex_);
        cache_.clear();
    }

    //=========================================================================
    // 同期ロード - ImageId を返す（変更）
    //=========================================================================
    ImageId loadSync(const ImageSource& source, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        // ImageId を事前に決定
        ImageId imageId = computeImageId(source);

        // 既にアップロード済みなら即返す
        if (master_.exists(imageId) && !master_.isPending(imageId)) {
            // touch して返す
            master_.touch(imageId);
            std::lock_guard lock(cacheMutex_);
            auto it = cache_.find(imageId);
            if (it != cache_.end()) {
                it->second.lastUsedFrame = currentFrame_;
            }
            return imageId;
        }

        // デコード
        auto decoded = decodeImage(source);
        if (!decoded.isValid()) {
            return 0;
        }

        // キャッシュに登録
        {
            std::lock_guard lock(cacheMutex_);
            auto& cached = cache_[imageId];
            cached.imageId = imageId;
            cached.pixels = std::move(decoded.pixels);
            cached.width = decoded.width;
            cached.height = decoded.height;
            cached.usage = usage;
            cached.persistent = persistent;
            cached.lastUsedFrame = currentFrame_;
            cached.refCount = 1;
        }

        // GPU にアップロード
        if (!uploadToGpu(imageId)) {
            return 0;
        }

        return imageId;
    }

    // ファイルから - ImageId を返す（変更）
    ImageId loadFromFile(const std::string& path, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        return loadSync(ImageSource::fromFile(path), usage, persistent);
    }

    // メモリから - ImageId を返す（変更）
    ImageId loadFromMemory(const void* data, size_t size,
        ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        return loadSync(ImageSource::fromMemory(data, size), usage, persistent);
    }

    // SDL_Surface から - ImageId を返す（変更）
    ImageId loadFromSurface(SDL_Surface* surface, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        return loadSync(ImageSource::fromSurface(surface), usage, persistent);
    }

    //=========================================================================
    // 非同期ロード - ImageId を返す（変更: Pending状態で即座に返す）
    //=========================================================================
    ImageId loadAsync(const ImageSource& source, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false,
        std::function<void(ImageId, bool success)> callback = nullptr) {
        // ImageId を事前に決定
        ImageId imageId = computeImageId(source);

        // 既にアップロード済みなら即完了
        if (master_.exists(imageId) && !master_.isPending(imageId)) {
            if (callback) callback(imageId, true);
            return imageId;
        }

        // 既に Pending なら重複ロードしない
        if (master_.isPending(imageId)) {
            // コールバックだけ登録
            if (callback) {
                std::lock_guard lock(pendingCallbacksMutex_);
                pendingCallbacks_[imageId].push_back(callback);
            }
            return imageId;
        }

        // Pending として ImageMaster に登録
        master_.registerPending(imageId);

        LoadRequest req;
        req.imageId = imageId;
        req.source = source;
        req.usage = usage;
        req.persistent = persistent;
        req.callback = callback;

        {
            std::lock_guard lock(queueMutex_);
            loadQueue_.push(std::move(req));
        }
        queueCv_.notify_one();

        return imageId;  // Pending 状態の ImageId を即座に返す
    }

    //=========================================================================
    // ImageId 事前計算（新規）
    //=========================================================================
    static ImageId computeImageId(const ImageSource& source) {
        switch (source.type) {
        case ImageSource::Type::File:
            return ImageIdGenerator::fromFile(source.path);
        case ImageSource::Type::Memory:
        case ImageSource::Type::SDL_Surface:
            return ImageIdGenerator::fromMemory();
        default:
            return 0;
        }
    }

    //=========================================================================
    // 状態確認（変更: ImageMaster に委譲）
    //=========================================================================
    bool isLoaded(ImageId imageId) const {
        return master_.exists(imageId) && !master_.isPending(imageId);
    }

    bool isPending(ImageId imageId) const {
        return master_.isPending(imageId);
    }

    // 旧API互換
    bool isUploaded(ImageId imageId) const {
        return isLoaded(imageId);
    }

    //=========================================================================
    // getDrawInfo は削除 - ImageMaster.resolve() を使用
    //=========================================================================
    // 
    // 【旧】
    // bgfx::TextureHandle tex;
    // float u0, v0, u1, v1;
    // if (loader.getDrawInfo(handle, tex, u0, v0, u1, v1)) { ... }
    //
    // 【新】
    // auto resolved = master.resolveForDraw(imageId);
    // if (resolved.isValid()) {
    //     // resolved.texture, resolved.u0, v0, u1, v1
    // }
    //

    //=========================================================================
    // 参照カウント
    //=========================================================================
    void retain(ImageId imageId) {
        master_.retain(imageId);
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(imageId);
        if (it != cache_.end()) {
            ++it->second.refCount;
        }
    }

    void release(ImageId imageId) {
        master_.release(imageId);
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(imageId);
        if (it != cache_.end() && it->second.refCount > 0) {
            --it->second.refCount;
        }
    }

    //=========================================================================
    // eviction 通知（ImageMaster から呼ばれる）
    //=========================================================================
    void onEvicted(ImageId id) {
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(id);
        if (it != cache_.end()) {
            it->second.isUploaded = false;
        }
    }

    //=========================================================================
    // 再アップロード（eviction 後に再度必要になった場合）
    //=========================================================================
    bool reupload(ImageId imageId) {
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(imageId);
        if (it == cache_.end() || it->second.pixels.empty()) {
            return false;
        }
        return uploadToGpuInternal(it->second);
    }

    //=========================================================================
    // フレーム処理
    //=========================================================================
    void beginFrame() {
        ++currentFrame_;
        processCompletedLoads();
    }

    //=========================================================================
    // CPUキャッシュ管理
    //=========================================================================
    void collectGarbage() {
        std::lock_guard lock(cacheMutex_);

        std::vector<ImageId> toRemove;
        size_t totalBytes = 0;

        for (auto& [id, cached] : cache_) {
            totalBytes += cached.byteSize();

            if (cached.persistent || cached.refCount > 0) continue;
            if (currentFrame_ - cached.lastUsedFrame > policy_.cacheExpireFrames) {
                toRemove.push_back(id);
            }
        }

        if (totalBytes > policy_.maxCpuCacheBytes) {
            std::vector<std::pair<uint64_t, ImageId>> byAge;
            for (auto& [id, cached] : cache_) {
                if (cached.persistent || cached.refCount > 0) continue;
                byAge.emplace_back(cached.lastUsedFrame, id);
            }
            std::sort(byAge.begin(), byAge.end());

            size_t freed = 0;
            for (auto& [_, id] : byAge) {
                if (totalBytes - freed <= policy_.maxCpuCacheBytes) break;
                auto it = cache_.find(id);
                if (it != cache_.end()) {
                    freed += it->second.byteSize();
                    toRemove.push_back(id);
                }
            }
        }

        for (ImageId id : toRemove) {
            cache_.erase(id);
        }
    }

    void evictFromCpuCache(ImageId imageId) {
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(imageId);
        if (it != cache_.end()) {
            it->second.pixels.clear();
            it->second.pixels.shrink_to_fit();
        }
    }

    //=========================================================================
    // 統計情報
    //=========================================================================
    struct Stats {
        size_t cacheEntries;
        size_t cpuCacheBytes;
        size_t uploadedCount;
        size_t pendingLoads;
        uint64_t currentFrame;
    };

    Stats getStats() const {
        std::lock_guard lock(cacheMutex_);
        Stats s{};
        s.cacheEntries = cache_.size();
        s.currentFrame = currentFrame_;

        for (const auto& [id, cached] : cache_) {
            s.cpuCacheBytes += cached.byteSize();
            if (cached.isUploaded) ++s.uploadedCount;
        }

        {
            std::lock_guard qlock(queueMutex_);
            s.pendingLoads = loadQueue_.size();
        }

        return s;
    }

    const PlacementPolicy& policy() const { return policy_; }
    PlacementPolicy& policy() { return policy_; }

private:
    //=========================================================================
    // 配置先決定
    //=========================================================================
    ImageLocation::Type decidePlacement(uint16_t w, uint16_t h, ImageUsage usage) {
        switch (usage) {
        case ImageUsage::Icon:
            return ImageLocation::Type::GridAtlas;

        case ImageUsage::VideoThumb:
            return ImageLocation::Type::ShelfAtlas;

        case ImageUsage::Background:
        case ImageUsage::Dynamic:
            return ImageLocation::Type::Standalone;

        case ImageUsage::UIImage:
            if (w <= policy_.fontAtlasMaxSize && h <= policy_.fontAtlasMaxSize) {
                return ImageLocation::Type::FontAtlas;
            }
            return ImageLocation::Type::Standalone;

        case ImageUsage::Thumbnail:
        case ImageUsage::Auto:
        default:
            break;
        }

        uint16_t maxDim = std::max(w, h);
        float aspect = (h > 0) ? (float)w / h : 1.0f;

        if (maxDim <= policy_.fontAtlasMaxSize) {
            return ImageLocation::Type::FontAtlas;
        }

        if (maxDim <= policy_.gridMaxSize && aspect >= 0.8f && aspect <= 1.25f) {
            return ImageLocation::Type::GridAtlas;
        }

        if (h <= policy_.shelfMaxHeight && w <= policy_.shelfMaxWidth && aspect >= policy_.shelfMinAspect) {
            return ImageLocation::Type::ShelfAtlas;
        }

        return ImageLocation::Type::Standalone;
    }

    //=========================================================================
    // 画像デコード
    //=========================================================================
    DecodedImage decodeImage(const ImageSource& source) {
        DecodedImage decoded;

        switch (source.type) {
        case ImageSource::Type::File:
            if (fileDecoder_) {
                decoded = fileDecoder_(source.path);
            }
            break;

        case ImageSource::Type::Memory:
            if (memoryDecoder_ && source.data && source.dataSize > 0) {
                decoded = memoryDecoder_(source.data, source.dataSize);
            }
            break;

        case ImageSource::Type::SDL_Surface:
            if (source.surface) {
                decoded = decodeSdlSurface(source.surface);
            }
            break;
        }

        return decoded;
    }

    DecodedImage decodeSdlSurface(SDL_Surface* surface) {
        DecodedImage result;
        if (!surface) return result;

        SDL_Surface* rgba = surface;
        bool ownsRgba = false;
        if (surface->format != SDL_PIXELFORMAT_RGBA8888) {
            rgba = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
            ownsRgba = true;
        }

        if (!rgba) return result;

        result.width = (uint16_t)rgba->w;
        result.height = (uint16_t)rgba->h;

        size_t rowBytes = (size_t)rgba->w * 4;
        result.pixels.resize(rowBytes * rgba->h);
        const uint8_t* src = (const uint8_t*)rgba->pixels;
        for (int y = 0; y < rgba->h; ++y) {
            std::memcpy(result.pixels.data() + y * rowBytes,
                src + y * rgba->pitch, rowBytes);
        }

        if (ownsRgba) SDL_DestroySurface(rgba);
        return result;
    }

    void setDefaultDecoders() {
        fileDecoder_ = [](const std::string& path) -> DecodedImage {
            int w, h, channels;
            unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
            if (!pixels) return DecodedImage{};

            DecodedImage result;
            result.width = (uint16_t)w;
            result.height = (uint16_t)h;
            result.pixels.assign(pixels, pixels + (size_t)w * h * 4);
            stbi_image_free(pixels);
            return result;
            };

        memoryDecoder_ = [](const void* data, size_t size) -> DecodedImage {
            int w, h, channels;
            unsigned char* pixels = stbi_load_from_memory(
                (const stbi_uc*)data, (int)size, &w, &h, &channels, 4);
            if (!pixels) return DecodedImage{};

            DecodedImage result;
            result.width = (uint16_t)w;
            result.height = (uint16_t)h;
            result.pixels.assign(pixels, pixels + (size_t)w * h * 4);
            stbi_image_free(pixels);
            return result;
            };
    }

    //=========================================================================
    // GPU アップロード - ImageMaster に登録
    //=========================================================================
    bool uploadToGpu(ImageId imageId) {
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(imageId);
        if (it == cache_.end()) return false;
        return uploadToGpuInternal(it->second);
    }

    bool uploadToGpuInternal(CachedImage& cached) {
        if (cached.pixels.empty()) return false;

        auto placement = decidePlacement(cached.width, cached.height, cached.usage);
        bool success = false;

        switch (placement) {
        case ImageLocation::Type::FontAtlas: {
            SDL_Surface* temp = SDL_CreateSurfaceFrom(
                cached.width, cached.height,
                SDL_PIXELFORMAT_RGBA8888,
                cached.pixels.data(), cached.width * 4);
            if (temp) {
                const auto& gi = master_.fontAtlas().addImage(
                    cached.imageId, temp, nullptr, cached.persistent);
                SDL_DestroySurface(temp);
                if (gi.width > 0) {
                    master_.registerFontAtlasImage(cached.imageId, cached.width, cached.height);
                    success = true;
                }
            }
            break;
        }

        case ImageLocation::Type::GridAtlas: {
            bool needsLoad = false;
            uint64_t contentId = getImageIdLocal(cached.imageId);
            auto handle = master_.gridAtlas().acquire(contentId, needsLoad);
            if (handle.isValid()) {
                if (needsLoad) {
                    master_.gridAtlas().queueUpload(handle, cached.pixels.data(),
                        cached.width, cached.height);
                }
                master_.registerGridImage(cached.imageId, handle, cached.width, cached.height);
                success = true;
            }
            break;
        }

        case ImageLocation::Type::ShelfAtlas: {
            bool needsLoad = false;
            uint64_t contentId = getImageIdLocal(cached.imageId);
            auto handle = master_.shelfAtlas().acquire(contentId, cached.width, cached.height, needsLoad);
            if (handle.isValid()) {
                if (needsLoad) {
                    master_.shelfAtlas().queueUpload(handle, cached.pixels.data(),
                        cached.width, cached.height);
                }
                master_.registerShelfImage(cached.imageId, handle, cached.width, cached.height);
                success = true;
            }
            break;
        }

        case ImageLocation::Type::Standalone: {
            success = master_.createStandaloneWithId(
                cached.imageId, cached.pixels.data(),
                cached.width, cached.height, cached.width * 4,
                cached.persistent);
            break;
        }

        default:
            break;
        }

        cached.isUploaded = success;
        cached.lastUsedFrame = currentFrame_;

        return success;
    }

    //=========================================================================
    // ワーカースレッド
    //=========================================================================
    void workerThread() {
        while (running_) {
            LoadRequest req;
            {
                std::unique_lock lock(queueMutex_);
                queueCv_.wait(lock, [this] {
                    return !running_ || !loadQueue_.empty();
                    });

                if (!running_ && loadQueue_.empty()) break;
                if (loadQueue_.empty()) continue;

                req = std::move(loadQueue_.front());
                loadQueue_.pop();
            }

            // デコード（ロック外）
            auto decoded = decodeImage(req.source);
            bool success = decoded.isValid();

            if (success) {
                // キャッシュに登録
                {
                    std::lock_guard lock(cacheMutex_);
                    auto& cached = cache_[req.imageId];
                    cached.imageId = req.imageId;
                    cached.pixels = std::move(decoded.pixels);
                    cached.width = decoded.width;
                    cached.height = decoded.height;
                    cached.usage = req.usage;
                    cached.persistent = req.persistent;
                    cached.lastUsedFrame = currentFrame_;
                    cached.refCount = 1;
                }
            }

            // 完了キューに追加
            {
                std::lock_guard lock(completedMutex_);
                completedLoads_.push_back({ req.imageId, req.callback, success });
            }
        }
    }

    //=========================================================================
    // 完了処理（メインスレッドで呼ばれる）
    //=========================================================================
    void processCompletedLoads() {
        std::vector<CompletedLoad> completed;
        {
            std::lock_guard lock(completedMutex_);
            completed.swap(completedLoads_);
        }

        for (auto& c : completed) {
            if (c.success && c.imageId != 0) {
                // GPUアップロード
                uploadToGpu(c.imageId);
            }

            // メインコールバック
            if (c.callback) {
                c.callback(c.imageId, c.success);
            }

            // 追加コールバック（重複ロード回避時に登録されたもの）
            {
                std::lock_guard lock(pendingCallbacksMutex_);
                auto it = pendingCallbacks_.find(c.imageId);
                if (it != pendingCallbacks_.end()) {
                    for (auto& cb : it->second) {
                        if (cb) cb(c.imageId, c.success);
                    }
                    pendingCallbacks_.erase(it);
                }
            }
        }
    }

    struct CompletedLoad {
        ImageId imageId;
        std::function<void(ImageId, bool)> callback;
        bool success;
    };

    //=========================================================================
    // メンバ
    //=========================================================================
    ImageMaster& master_;
    PlacementPolicy policy_;
    uint64_t currentFrame_ = 0;

    FileDecoder fileDecoder_;
    MemoryDecoder memoryDecoder_;

    mutable std::mutex cacheMutex_;
    std::unordered_map<ImageId, CachedImage> cache_;

    std::atomic<bool> running_{ false };
    std::vector<std::thread> workers_;

    mutable std::mutex queueMutex_;
    std::queue<LoadRequest> loadQueue_;
    std::condition_variable queueCv_;

    std::mutex completedMutex_;
    std::vector<CompletedLoad> completedLoads_;

    // 重複ロード回避用コールバック
    std::mutex pendingCallbacksMutex_;
    std::unordered_map<ImageId, std::vector<std::function<void(ImageId, bool)>>> pendingCallbacks_;
};