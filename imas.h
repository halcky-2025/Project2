#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include <algorithm>

#ifdef __ANDROID__
#include <SDL3/SDL.h>
#endif

//=============================================================================
// ImageId
//=============================================================================



//=============================================================================
// ResolveStatus
//=============================================================================


enum class ResolveStatus {
    Success,        // ��������
    Pending,        // ���[�h���iplaceholder�g�p�j
    NotFound,       // ������Ȃ�
    Evicted,        // eviction�ς݁i�ă��[�h�\�����j
};

struct ResolveResult {
    ResolveStatus status = ResolveStatus::NotFound;
    ResolvedTexture resolved;
    ResolvedTexture placeholder;  // Pending���̃t�H�[���o�b�N

    bool isReady() const { return status == ResolveStatus::Success; }

    // �`��p�i�����Ȃ�resolved�APending�Ȃ�placeholder�j
    const ResolvedTexture& forDraw() const {
        return (status == ResolveStatus::Success) ? resolved : placeholder;
    }
};

//=============================================================================
// ImageLocation - ImageId�̔z�u���
//=============================================================================

struct ImageLocation {
    enum class Type : uint8_t {
        None,
        Standalone,   // standaloneTextures_ �ɑ���
        FontAtlas,    // FontAtlas �ɑ���
        GridAtlas,    // ThumbnailGridAtlas �ɑ���
        ShelfAtlas,   // ThumbnailShelfAtlas �ɑ���
    };

    Type type = Type::None;

    // �eAtlas�p�̓����L�[
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
// RenderGroup - ��{�O���[�v
//=============================================================================


//=============================================================================
// ExtendedRenderGroup - ImageMaster �p�g���O���[�v
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
// ImageMasterGroupManager - �O���錾
//=============================================================================
class ImageMaster;  // �O���錾

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

    ImageMaster()
        : config_()
        , fontAtlas_(config_.fontConfig)
        , gridAtlas_(config_.gridConfig)
        , shelfAtlas_(config_.shelfConfig)
        , groupManager_(*this) {
    }
    explicit ImageMaster(const Config& cfg)
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
        // Standalone textures ���
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

        // �z�u���N���A
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
        // �����؂�O���[�v�̉��
        collectExpiredGroups(config_.groupExpireFrames);

        // FontAtlas GC
        fontAtlas_.collectGarbage();

        // ThumbnailAtlas �͓���LRU�ŊǗ�
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

                // �z�u�����폜
                std::lock_guard locLock(locationMutex_);
                imageLocations_.erase(id);
            }
        }
    }

    uint64_t currentFrame() const { return currentFrame_; }

    //=========================================================================
    // ���v���
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
    // �yLayer A�z�������API�iUI/Scene�����j
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

        // �܂� Grid �ŒT��
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

        // ���� Shelf �ŒT��
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
    // ImageId �� �`��\�ȃe�N�X�`��������
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
            return resolveThumbnail(id);  // Grid/Shelf ����

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

    // �ȈՔŁi�������̂݃e�N�X�`������Ԃ��j
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

    // �`��p�iPending����placeholder�j
    ResolvedTexture resolveForDraw(ImageId id) {
        return resolve(id).forDraw();
    }

    //=========================================================================
    // �yLayer A�zImageId �o�^/�Ǘ�
    //=========================================================================

    // ImageId�̏�Ԃ��m�F
    bool exists(ImageId id) const {
        std::lock_guard lock(locationMutex_);
        return imageLocations_.find(id) != imageLocations_.end();
    }

    bool isPending(ImageId id) const {
        std::lock_guard lock(locationMutex_);
        auto it = imageLocations_.find(id);
        return it != imageLocations_.end() && it->second.isPending;
    }

    // Pending��Ԃœo�^�i���[�h�J�n���j
    void registerPending(ImageId id) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::None;
        loc.isPending = true;
    }

    // touch�i�g�p�}�[�N�j
    void touch(ImageId id) {
        ImageIdDomain domain = getImageIdDomain(id);

        // Standalone�n
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

        // Grid/Shelf �͓����n���h���o�R�� touch
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

    // �Q�ƃJ�E���g
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
        // Grid/Shelf�͓���LRU�ŊǗ�
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
    // �yLayer B�zStandalone �e�N�X�`���i�჌�x���j
    //=========================================================================

    // �쐬�i�s�N�Z���f�[�^����j
    ImageId createStandaloneTexture(const void* pixels, uint16_t w, uint16_t h,
        int pitch, bool persistent = false) {
        ImageId id = ImageIdGenerator::fromMemory();
        if (!createStandaloneInternal(id, pixels, w, h, pitch, persistent,
            ImageOrigin::Memory, false)) {
            return 0;
        }
        return id;
    }

    // �쐬�iSDL_Surface ����j
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

    // �쐬�i��e�N�X�`���j
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

        // �z�u����o�^
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
    // �e�N�X�`���|�C���^�擾API�i�ǉ��j
    //=========================================================================

    // Standalone �e�N�X�`���ւ̃|�C���^�擾
    bgfx::TextureHandle* getStandaloneTexturePtr(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it != standaloneTextures_.end()) {
            return &it->second.handle;
        }
        return nullptr;
    }

    // FBO �ւ̃|�C���^�擾
    bgfx::FrameBufferHandle* getOffscreenFBOPtr(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it != standaloneTextures_.end() && it->second.isRenderTarget) {
            return &it->second.fbo;
        }
        return nullptr;
    }

    // ��API�݊�
    ImageId reserveOffscreenTexture(uint16_t w, uint16_t h,
        bool persistent = false) {
        ImageId id = ImageIdGenerator::forOffscreen();

        // ��ɃG���g�����쐬�i�e�N�X�`���������j
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

            // �쐬�L���[�ɒǉ�
            {
                std::lock_guard lock(offscreenQueueMutex_);
                offscreenCreateQueue_.push_back({ id, w, h, persistent });
            }

            return id;
        }

        return id;
    }
    void queueOffscreenResize(ImageId id, uint16_t newW, uint16_t newH) {
        std::lock_guard lock(offscreenQueueMutex_);
        offscreenResizeQueue_.push_back({ id, newW, newH });
    }
    // �ʏ�e�N�X�`���\��i���[�h�O�Ƀ|�C���^�擾�j
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
    // �e�N�X�`���f�[�^���A�b�v���[�h�i�\��ς�ID�ɑ΂��āj
    bool uploadTexture(ImageId id, const void* pixels, uint16_t w, uint16_t h, int pitch) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        if (it == standaloneTextures_.end()) return false;

        auto& info = it->second;

        // �e�N�X�`���쐬
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

        // Pending����
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
    // FontAtlas �e�N�X�`���|�C���^�擾
    //=========================================================================

    bgfx::TextureHandle* getFontAtlasTexturePtr(uint16_t pageIndex) {
        return &fontAtlas_.getPageTexture(pageIndex);
    }

    // �O���t�擾�i�|�C���^�t���j
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
    // Thumbnail �e�N�X�`���|�C���^�擾
    //=========================================================================

    bgfx::TextureHandle* getGridAtlasTexturePtr(uint16_t pageIndex) {
        return (bgfx::TextureHandle*)(&gridAtlas_.getTexture(pageIndex));
    }

    bgfx::TextureHandle* getShelfAtlasTexturePtr(uint16_t pageIndex) {
        return (bgfx::TextureHandle*)(&shelfAtlas_.getTexture(pageIndex));
    }

    // Thumbnail �擾�i�|�C���^�t���j
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
    // Placeholder �e�N�X�`���|�C���^
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

        // �T�C�Y�ύX�Ȃ��Ȃ牽�����Ȃ�
        if (info.size.x == newW && info.size.y == newH) {
            return true;
        }

        // �Â����\�[�X�j��
        if (bgfx::isValid(info.fbo)) {
            bgfx::destroy(info.fbo);
        }

        // 新しいテクスチャ作成 (BGRA8 for Metal compatibility)
        bgfx::TextureHandle newHandle = bgfx::createTexture2D(
            newW, newH, false, 1,
            bgfx::TextureFormat::BGRA8,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            nullptr);

        if (!bgfx::isValid(newHandle)) return false;

        // Create depth texture for offscreen framebuffer (required for Metal depth testing)
        bgfx::TextureHandle newDepthHandle = bgfx::createTexture2D(
            newW, newH, false, 1,
            bgfx::TextureFormat::D24S8,
            BGFX_TEXTURE_RT, nullptr);

        if (!bgfx::isValid(newDepthHandle)) {
            bgfx::destroy(newHandle);
            return false;
        }

        // Use bgfx::Attachment to properly attach color and depth
        bgfx::Attachment att[2];
        att[0].init(newHandle);      // color attachment
        att[1].init(newDepthHandle); // depth attachment

        bgfx::FrameBufferHandle newFbo = bgfx::createFrameBuffer(2, att, true);
        if (!bgfx::isValid(newFbo)) {
            bgfx::destroy(newHandle);
            bgfx::destroy(newDepthHandle);
            return false;
        }

        info.handle = newHandle;
        info.fbo = newFbo;
        info.size.x = newW;
        info.size.y = newH;
        info.lastUsedFrame = currentFrame_;

        // �z�u�����X�V
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
        // �쐬
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

            // �z�u�����폜
            std::lock_guard locLock(locationMutex_);
            imageLocations_.erase(id);
        }
    }

    // ��API�݊�
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

    // Standalone �e�N�X�`�����擾
    StandaloneTextureInfo* getStandaloneTexture(ImageId id) {
        std::lock_guard lock(standaloneMutex_);
        auto it = standaloneTextures_.find(id);
        return (it != standaloneTextures_.end()) ? &it->second : nullptr;
    }

    // Standalone �e�N�X�`���X�V
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
    // �yLayer B�zFontAtlas�i�჌�x���j
    //=========================================================================

    FontAtlas& fontAtlas() { return fontAtlas_; }
    const FontAtlas& fontAtlas() const { return fontAtlas_; }

    // FontAtlas �֗����\�b�h�i�Ϗ��j
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

    // �O���t�ǉ�����ImageId��Ԃ�
    ImageId addGlyph(FontId font, uint32_t codepoint, SDL_Color color = { 255,255,255,255 }) {
        // FontIndex ���擾�i�Փ˂Ȃ��̘A�ԁj
        uint32_t fontIndex = fontAtlas_.getFontIndex(font);
        if (fontIndex == 0) return 0;  // ���o�^�t�H���g

        const auto& gi = fontAtlas_.getOrAddGlyph(font, codepoint, nullptr, color);
        if (gi.width == 0) return 0;

        // localId = fontIndex(24bit) + codepoint(24bit) �ň��
        // codepoint �� Unicode �ōő� 0x10FFFF (21bit) �Ȃ̂� 24bit �ŏ\��
        uint64_t localId = (uint64_t(fontIndex) << 24) | (codepoint & 0x00FFFFFF);
        ImageId id = ImageIdGenerator::forFontGlyph(localId);

        // �z�u����o�^
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

    // �摜�ǉ�����ImageId��Ԃ�
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

    // ���ڃA�N�Z�X
    bgfx::TextureHandle getFontAtlasTexture(uint16_t pageIndex) {
        return fontAtlas_.getPageTexture(pageIndex);
    }

    //=========================================================================
    // �yLayer B�zThumbnailGridAtlas�i�჌�x���j
    //=========================================================================

    ThumbnailGridAtlas& gridAtlas() { return gridAtlas_; }
    const ThumbnailGridAtlas& gridAtlas() const { return gridAtlas_; }

    // Grid �֗����\�b�h�i�Ϗ��j
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

    // �T���l�C���擾/�쐬����ImageId��Ԃ�
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

    // �A�b�v���[�h�����ʒm
    void onGridUploadComplete(ImageId id) {
        std::lock_guard lock(locationMutex_);
        auto it = imageLocations_.find(id);
        if (it != imageLocations_.end()) {
            it->second.isPending = false;
        }
    }

    // ���ڃA�N�Z�X
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
    // �yLayer B�zThumbnailShelfAtlas�i�჌�x���j
    //=========================================================================

    ThumbnailShelfAtlas& shelfAtlas() { return shelfAtlas_; }
    const ThumbnailShelfAtlas& shelfAtlas() const { return shelfAtlas_; }

    // Shelf �֗����\�b�h�i�Ϗ��j
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
    // ImageLoader ����Ă΂��o�^���\�b�h�iID�w��Łj
    //=========================================================================

    // FontAtlas�摜��o�^�iImageLoader�p�j
    void registerFontAtlasImage(ImageId id, uint16_t width, uint16_t height) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::FontAtlas;
        loc.font.glyphKey = id;
        loc.width = width;
        loc.height = height;
        loc.isPending = false;
    }

    // Grid�摜��o�^�iImageLoader�p�j
    void registerGridImage(ImageId id, ThumbnailHandle handle, uint16_t width, uint16_t height) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::GridAtlas;
        loc.thumbnail.handle = handle;
        loc.width = width;
        loc.height = height;
        loc.isPending = false;
    }

    // Shelf�摜��o�^�iImageLoader�p�j
    void registerShelfImage(ImageId id, ThumbnailHandle handle, uint16_t width, uint16_t height) {
        std::lock_guard lock(locationMutex_);
        auto& loc = imageLocations_[id];
        loc.type = ImageLocation::Type::ShelfAtlas;
        loc.thumbnail.handle = handle;
        loc.width = width;
        loc.height = height;
        loc.isPending = false;
    }

    // Standalone�쐬�iID�w��ŁAImageLoader�p�j
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
    // ����resolve�֐�
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
            // eviction ����Ă���\��
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
    // �����쐬�֐�
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

        // �z�u����o�^
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

        // Use BGRA8 for Metal compatibility
        bgfx::TextureHandle handle = bgfx::createTexture2D(
            w, h, false, 1, bgfx::TextureFormat::BGRA8,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            nullptr);

        if (!bgfx::isValid(handle)) return false;

        // Create depth texture for offscreen framebuffer (required for Metal depth testing)
        bgfx::TextureHandle depthHandle = bgfx::createTexture2D(
            w, h, false, 1, bgfx::TextureFormat::D24S8,
            BGFX_TEXTURE_RT, nullptr);

        if (!bgfx::isValid(depthHandle)) {
            bgfx::destroy(handle);
            return false;
        }

        // Use bgfx::Attachment to properly attach color and depth
        bgfx::Attachment att[2];
        att[0].init(handle);      // color attachment
        att[1].init(depthHandle); // depth attachment

        bgfx::FrameBufferHandle fbo = bgfx::createFrameBuffer(2, att, true);
        if (!bgfx::isValid(fbo)) {
            bgfx::destroy(handle);
            bgfx::destroy(depthHandle);
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
    // �����o
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
    // RenderGroup �Ǘ��i�Ϗ� + �����j
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

        // FontAtlas �ւ̒ʒm
        fontAtlas_.onGroupDestroyed(*g);

        // Standalone texture �̎Q�Ɖ��
        for (auto texId : g->usedStandaloneTextures) {
            release(texId);
        }

        // Surface ���X�g����폜
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

    // ImageId �� �z�u���̃}�b�s���O
    mutable std::mutex locationMutex_;
    std::unordered_map<ImageId, ImageLocation> imageLocations_;

    std::atomic<uint64_t> nextFontAtlasImageId_{ 0x1000000 };
};



//=============================================================================
// ImageSource - �摜�\�[�X
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
// ImageUsage - �z�u�q���g
//=============================================================================
enum class ImageUsage : uint8_t {
    Auto,           // �T�C�Y���玩�����f
    Icon,           // �����Ȑ����` �� Grid
    Thumbnail,      // �ʏ�T���l �� �T�C�Y�� Grid or Shelf
    VideoThumb,     // �����T���l �� Shelf
    Background,     // �w�i�摜 �� Standalone (persistent)
    UIImage,        // UI�摜 �� Standalone or FontAtlas
    Dynamic,        // ���I�X�V �� Standalone
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
// PlacementPolicy - �z�u�|���V�[
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
// CachedImage - CPU�L���b�V���G���g��
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
// LoadRequest - �񓯊����[�h���N�G�X�g
//=============================================================================
struct LoadRequest {
    ImageId imageId;              // �� �ύX: ���O�Ɍ��肳�ꂽID
    ImageSource source;
    ImageUsage usage;
    bool persistent;
    std::function<void(ImageId, bool success)> callback;  // �� �ύX: ImageId ��Ԃ�
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
    // �f�R�[�_�[�ݒ�
    //=========================================================================
    void setFileDecoder(FileDecoder decoder) {
        fileDecoder_ = std::move(decoder);
    }

    void setMemoryDecoder(MemoryDecoder decoder) {
        memoryDecoder_ = std::move(decoder);
    }

    //=========================================================================
    // �������E�I��
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
    // �������[�h - ImageId ��Ԃ��i�ύX�j
    //=========================================================================
    ImageId loadSync(const ImageSource& source, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        // ImageId �����O�Ɍ���
        ImageId imageId = computeImageId(source);

        // ���ɃA�b�v���[�h�ς݂Ȃ瑦�Ԃ�
        if (master_.exists(imageId) && !master_.isPending(imageId)) {
            // touch ���ĕԂ�
            master_.touch(imageId);
            std::lock_guard lock(cacheMutex_);
            auto it = cache_.find(imageId);
            if (it != cache_.end()) {
                it->second.lastUsedFrame = currentFrame_;
            }
            return imageId;
        }

        // �f�R�[�h
        auto decoded = decodeImage(source);
        if (!decoded.isValid()) {
            return 0;
        }

        // �L���b�V���ɓo�^
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

        // GPU �ɃA�b�v���[�h
        if (!uploadToGpu(imageId)) {
            return 0;
        }

        return imageId;
    }

    // �t�@�C������ - ImageId ��Ԃ��i�ύX�j
    ImageId loadFromFile(const std::string& path, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        return loadSync(ImageSource::fromFile(path), usage, persistent);
    }

    // ���������� - ImageId ��Ԃ��i�ύX�j
    ImageId loadFromMemory(const void* data, size_t size,
        ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        return loadSync(ImageSource::fromMemory(data, size), usage, persistent);
    }

    // SDL_Surface ���� - ImageId ��Ԃ��i�ύX�j
    ImageId loadFromSurface(SDL_Surface* surface, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false) {
        return loadSync(ImageSource::fromSurface(surface), usage, persistent);
    }

    //=========================================================================
    // �񓯊����[�h - ImageId ��Ԃ��i�ύX: Pending��Ԃő����ɕԂ��j
    //=========================================================================
    ImageId loadAsync(const ImageSource& source, ImageUsage usage = ImageUsage::Auto,
        bool persistent = false,
        std::function<void(ImageId, bool success)> callback = nullptr) {
        // ImageId �����O�Ɍ���
        ImageId imageId = computeImageId(source);

        // ���ɃA�b�v���[�h�ς݂Ȃ瑦����
        if (master_.exists(imageId) && !master_.isPending(imageId)) {
            if (callback) callback(imageId, true);
            return imageId;
        }

        // ���� Pending �Ȃ�d�����[�h���Ȃ�
        if (master_.isPending(imageId)) {
            // �R�[���o�b�N�����o�^
            if (callback) {
                std::lock_guard lock(pendingCallbacksMutex_);
                pendingCallbacks_[imageId].push_back(callback);
            }
            return imageId;
        }

        // Pending �Ƃ��� ImageMaster �ɓo�^
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

        return imageId;  // Pending ��Ԃ� ImageId �𑦍��ɕԂ�
    }

    //=========================================================================
    // ImageId ���O�v�Z�i�V�K�j
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
    // ��Ԋm�F�i�ύX: ImageMaster �ɈϏ��j
    //=========================================================================
    bool isLoaded(ImageId imageId) const {
        return master_.exists(imageId) && !master_.isPending(imageId);
    }

    bool isPending(ImageId imageId) const {
        return master_.isPending(imageId);
    }

    // ��API�݊�
    bool isUploaded(ImageId imageId) const {
        return isLoaded(imageId);
    }

    //=========================================================================
    // getDrawInfo �͍폜 - ImageMaster.resolve() ���g�p
    //=========================================================================
    // 
    // �y���z
    // bgfx::TextureHandle tex;
    // float u0, v0, u1, v1;
    // if (loader.getDrawInfo(handle, tex, u0, v0, u1, v1)) { ... }
    //
    // �y�V�z
    // auto resolved = master.resolveForDraw(imageId);
    // if (resolved.isValid()) {
    //     // resolved.texture, resolved.u0, v0, u1, v1
    // }
    //

    //=========================================================================
    // �Q�ƃJ�E���g
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
    // eviction �ʒm�iImageMaster ����Ă΂��j
    //=========================================================================
    void onEvicted(ImageId id) {
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(id);
        if (it != cache_.end()) {
            it->second.isUploaded = false;
        }
    }

    //=========================================================================
    // �ăA�b�v���[�h�ieviction ��ɍēx�K�v�ɂȂ����ꍇ�j
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
    // �t���[������
    //=========================================================================
    void beginFrame() {
        ++currentFrame_;
        processCompletedLoads();
    }

    //=========================================================================
    // CPU�L���b�V���Ǘ�
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
    // ���v���
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

#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
    //=========================================================================
    // iOS: Process queued GPU uploads on main thread
    // Must be called from main thread in bgfx single-thread mode
    //=========================================================================
    void processGpuUploadsMainThread() {
        std::vector<ImageId> toProcess;
        {
            std::lock_guard lock(gpuUploadQueueMutex_);
            toProcess.swap(gpuUploadQueue_);
        }

        for (ImageId imageId : toProcess) {
            std::lock_guard lock(cacheMutex_);
            auto it = cache_.find(imageId);
            if (it != cache_.end()) {
                uploadToGpuInternal(it->second);
            }
        }
    }
#endif

private:
    //=========================================================================
    // �z�u�挈��
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
    // �摜�f�R�[�h
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
            unsigned char* pixels = nullptr;

#ifdef __ANDROID__
            // Android: Use SDL_IOFromFile to read from assets
            SDL_IOStream* io = SDL_IOFromFile(path.c_str(), "rb");
            if (!io) return DecodedImage{};

            Sint64 size = SDL_GetIOSize(io);
            if (size <= 0) {
                SDL_CloseIO(io);
                return DecodedImage{};
            }

            std::vector<uint8_t> fileData(static_cast<size_t>(size));
            size_t read = SDL_ReadIO(io, fileData.data(), fileData.size());
            SDL_CloseIO(io);

            if (read != fileData.size()) return DecodedImage{};

            pixels = stbi_load_from_memory(fileData.data(), (int)fileData.size(), &w, &h, &channels, 4);
#elif TARGET_OS_IOS || TARGET_OS_SIMULATOR || TARGET_OS_MAC
            // iOS/macOS: Use bundle path (macOS falls back to relative path)
            std::string bundlePath = getBundlePath(path.c_str());
            SDL_Log("loadImage: Trying bundle path: %s", bundlePath.c_str());
            pixels = stbi_load(bundlePath.c_str(), &w, &h, &channels, 4);
#if TARGET_OS_MAC && !TARGET_OS_IOS
            // macOS: If bundle path failed, try relative path (for command line execution)
            if (!pixels) {
                SDL_Log("loadImage: Bundle path failed, trying relative path: %s", path.c_str());
                pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
            }
#else
            // iOS only: Swap R and B channels (RGBA -> BGRA) for Metal compatibility
            if (pixels) {
                for (int i = 0; i < w * h; ++i) {
                    std::swap(pixels[i * 4 + 0], pixels[i * 4 + 2]);
                }
            }
#endif
#else
            // Linux: Use bundle path
            std::string bundlePath = getBundlePath(path.c_str());
            SDL_Log("loadImage: Trying path: %s", bundlePath.c_str());
            pixels = stbi_load(bundlePath.c_str(), &w, &h, &channels, 4);
            if (!pixels) {
                SDL_Log("loadImage: Failed to load: %s", bundlePath.c_str());
            }
#endif

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

#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
            // iOS only: Swap R and B channels (RGBA -> BGRA) for Metal compatibility
            for (int i = 0; i < w * h; ++i) {
                std::swap(pixels[i * 4 + 0], pixels[i * 4 + 2]);
            }
#endif

            DecodedImage result;
            result.width = (uint16_t)w;
            result.height = (uint16_t)h;
            result.pixels.assign(pixels, pixels + (size_t)w * h * 4);
            stbi_image_free(pixels);
            return result;
            };
    }

    //=========================================================================
    // GPU �A�b�v���[�h - ImageMaster �ɓo�^
    //=========================================================================
    bool uploadToGpu(ImageId imageId) {
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
        // iOS: Queue the upload for main thread processing
        // bgfx single-thread mode requires all API calls on main thread
        {
            std::lock_guard lock(gpuUploadQueueMutex_);
            gpuUploadQueue_.push_back(imageId);
        }
        return true;  // Will be processed on main thread
#else
        std::lock_guard lock(cacheMutex_);
        auto it = cache_.find(imageId);
        if (it == cache_.end()) return false;
        return uploadToGpuInternal(it->second);
#endif
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
    // ���[�J�[�X���b�h
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

            // �f�R�[�h�i���b�N�O�j
            auto decoded = decodeImage(req.source);
            bool success = decoded.isValid();

            if (success) {
                // �L���b�V���ɓo�^
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

            // �����L���[�ɒǉ�
            {
                std::lock_guard lock(completedMutex_);
                completedLoads_.push_back({ req.imageId, req.callback, success });
            }
        }
    }

    //=========================================================================
    // ���������i���C���X���b�h�ŌĂ΂��j
    //=========================================================================
    void processCompletedLoads() {
        std::vector<CompletedLoad> completed;
        {
            std::lock_guard lock(completedMutex_);
            completed.swap(completedLoads_);
        }

        for (auto& c : completed) {
            if (c.success && c.imageId != 0) {
                // GPU�A�b�v���[�h
                uploadToGpu(c.imageId);
            }

            // ���C���R�[���o�b�N
            if (c.callback) {
                c.callback(c.imageId, c.success);
            }

            // �ǉ��R�[���o�b�N�i�d�����[�h������ɓo�^���ꂽ���́j
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
    // �����o
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

    // �d�����[�h���p�R�[���o�b�N
    std::mutex pendingCallbacksMutex_;
    std::unordered_map<ImageId, std::vector<std::function<void(ImageId, bool)>>> pendingCallbacks_;

#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
    // iOS: Queue for deferred GPU uploads (must be processed on main thread)
    std::mutex gpuUploadQueueMutex_;
    std::vector<ImageId> gpuUploadQueue_;
#endif
};