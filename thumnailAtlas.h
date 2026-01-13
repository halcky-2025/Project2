#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include <bgfx/bgfx.h>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <memory>
#include <cassert>

using ThumbnailId = uint64_t;
inline constexpr uint32_t kInvalidIndex = UINT32_MAX;
inline constexpr uint16_t kInvalidPageIndex = UINT16_MAX;

//=============================================================================
// ThumbnailHandle
//=============================================================================
struct ThumbnailHandle {
    uint16_t pageIndex = kInvalidPageIndex;
    uint32_t rectIndex = kInvalidIndex;
    uint32_t generation = 0;

    bool isValid() const {
        return pageIndex != kInvalidPageIndex && rectIndex != kInvalidIndex;
    }
    bool operator==(const ThumbnailHandle& o) const {
        return pageIndex == o.pageIndex &&
            rectIndex == o.rectIndex &&
            generation == o.generation;
    }
};


struct PendingUpload {
    ThumbnailHandle handle;
    uint16_t width, height;
    std::vector<uint8_t> data;
};
//=============================================================================
// ��`���
//=============================================================================
enum class RectState : uint8_t { Empty, Loading, Ready };

//=============================================================================
// Mipmap�����w���p�[
//=============================================================================
inline void generateMipmaps(bgfx::TextureHandle texture,
    const uint8_t* srcData,
    uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    uint8_t maxMipLevels) {
    if (maxMipLevels <= 1) return;

    std::vector<uint8_t> src(srcData, srcData + width * height * 4);
    uint16_t mw = width, mh = height;
    uint16_t mx = x, my = y;

    for (uint8_t level = 1; level < maxMipLevels; ++level) {
        uint16_t nw = std::max<uint16_t>(mw / 2, 1);
        uint16_t nh = std::max<uint16_t>(mh / 2, 1);
        std::vector<uint8_t> dst(nw * nh * 4);

        for (uint16_t py = 0; py < nh; ++py) {
            for (uint16_t px = 0; px < nw; ++px) {
                uint32_t sx = px * 2, sy = py * 2;
                uint32_t r = 0, g = 0, b = 0, a = 0;
                int cnt = 0;
                for (int dy = 0; dy < 2 && sy + dy < mh; ++dy) {
                    for (int dx = 0; dx < 2 && sx + dx < mw; ++dx) {
                        uint32_t i = ((sy + dy) * mw + (sx + dx)) * 4;
                        r += src[i]; g += src[i + 1]; b += src[i + 2]; a += src[i + 3];
                        cnt++;
                    }
                }
                uint32_t di = (py * nw + px) * 4;
                dst[di] = r / cnt; dst[di + 1] = g / cnt; dst[di + 2] = b / cnt; dst[di + 3] = a / cnt;
            }
        }

        bgfx::updateTexture2D(texture, 0, level, mx >> level, my >> level, nw, nh,
            bgfx::copy(dst.data(), (uint32_t)dst.size()));

        src = std::move(dst);
        mw = nw; mh = nh;
    }
}

//=============================================================================
// �v���[�X�z���_�[�����w���p�[
//=============================================================================
inline std::vector<uint8_t> createPlaceholderPixels(uint16_t size) {
    std::vector<uint8_t> gray(size * size * 4, 0x80);
    for (size_t i = 3; i < gray.size(); i += 4) {
        gray[i] = 0xFF;
    }
    return gray;
}

//=============================================================================
// Grid�ݒ�
//=============================================================================
struct GridPageConfig {
    uint16_t atlasWidth = 2048;
    uint16_t atlasHeight = 2048;
    uint16_t tileSize = 128;            // �^�C���T�C�Y�i�����`�j
    uint16_t contentSize = 124;         // ���ۂ̃R���e���c�T�C�Y
    uint8_t maxMipLevels = 4;
    uint64_t loadingTimeoutFrames = 300;
    bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA8;
};

//=============================================================================
// Grid��`���
//=============================================================================
struct GridRectInfo {
    ThumbnailId id = 0;
    uint32_t generation = 0;
    RectState state = RectState::Empty;
    uint16_t x = 0, y = 0;

    // LRU
    uint32_t lruPrev = kInvalidIndex;
    uint32_t lruNext = kInvalidIndex;
    uint64_t lastAccessFrame = 0;
};

//=============================================================================
// ThumbnailGridPage
//=============================================================================
class ThumbnailGridPage {
public:
    ThumbnailGridPage(uint16_t pageIndex, const GridPageConfig& config)
        : m_pageIndex(pageIndex), m_config(config)
    {
        m_tilesX = m_config.atlasWidth / m_config.tileSize;
        m_tilesY = m_config.atlasHeight / m_config.tileSize;

        uint8_t maxMip = static_cast<uint8_t>(std::log2(m_config.contentSize)) + 1;
        m_config.maxMipLevels = std::min(m_config.maxMipLevels, maxMip);
    }

    ~ThumbnailGridPage() { shutdown(); }

    ThumbnailGridPage(const ThumbnailGridPage&) = delete;
    ThumbnailGridPage& operator=(const ThumbnailGridPage&) = delete;

    bool initialize() {
        if (bgfx::isValid(m_texture)) return true;

        uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
        m_texture = bgfx::createTexture2D(
            m_config.atlasWidth, m_config.atlasHeight,
            m_config.maxMipLevels > 1, 1, m_config.format, flags, nullptr);

        if (!bgfx::isValid(m_texture)) return false;

        uint32_t total = m_tilesX * m_tilesY;
        m_rects.resize(total);
        m_freeList.reserve(total);

        uint16_t padding = (m_config.tileSize - m_config.contentSize) / 2;
        for (uint32_t i = 0; i < total; ++i) {
            m_rects[i] = GridRectInfo{};
            m_rects[i].x = (i % m_tilesX) * m_config.tileSize + padding;
            m_rects[i].y = (i / m_tilesX) * m_config.tileSize + padding;
            m_freeList.push_back(total - 1 - i);
        }

        return true;
    }

    void shutdown() {
        if (bgfx::isValid(m_texture)) {
            bgfx::destroy(m_texture);
            m_texture = BGFX_INVALID_HANDLE;
        }
        m_rects.clear();
        m_freeList.clear();
        m_lruHead = m_lruTail = kInvalidIndex;
        m_lruCount = 0;
    }

    void createPlaceholder() {
        if (m_freeList.empty()) return;

        m_freeList.pop_back();  // rect 0 �����O
        auto& r = m_rects[0];
        r.state = RectState::Ready;
        r.generation = 1;

        uint16_t size = m_config.contentSize;
        auto pixels = createPlaceholderPixels(size);
        bgfx::updateTexture2D(m_texture, 0, 0, r.x, r.y, size, size,
            bgfx::copy(pixels.data(), (uint32_t)pixels.size()));

        m_hasPlaceholder = true;
    }

    bool canAllocate() const { return !m_freeList.empty(); }

    bool allocate(ThumbnailId id, uint32_t& outRectIndex, uint32_t& outGeneration) {
        if (m_freeList.empty()) return false;

        uint32_t idx = m_freeList.back();
        m_freeList.pop_back();

        auto& r = m_rects[idx];
        r.id = id;
        r.generation++;
        r.state = RectState::Loading;
        r.lastAccessFrame = m_currentFrame;
        lruPushFront(idx);

        outRectIndex = idx;
        outGeneration = r.generation;
        return true;
    }

    bool isValid(uint32_t rectIndex, uint32_t generation) const {
        if (rectIndex >= m_rects.size()) return false;
        const auto& r = m_rects[rectIndex];
        return r.generation == generation && r.state == RectState::Ready;
    }

    bool isLoading(uint32_t rectIndex, uint32_t generation) const {
        if (rectIndex >= m_rects.size()) return false;
        const auto& r = m_rects[rectIndex];
        return r.generation == generation && r.state == RectState::Loading;
    }

    void markReady(uint32_t rectIndex, uint32_t generation) {
        if (rectIndex >= m_rects.size()) return;
        auto& r = m_rects[rectIndex];
        if (r.generation == generation && r.state == RectState::Loading) {
            r.state = RectState::Ready;
        }
    }

    void computeUV(uint32_t rectIndex, float& u0, float& v0, float& u1, float& v1) const {
        if (rectIndex >= m_rects.size()) return;
        const auto& r = m_rects[rectIndex];
        float invW = 1.0f / m_config.atlasWidth;
        float invH = 1.0f / m_config.atlasHeight;
        u0 = r.x * invW;
        v0 = r.y * invH;
        u1 = (r.x + m_config.contentSize) * invW;
        v1 = (r.y + m_config.contentSize) * invH;
    }

    void touch(uint32_t rectIndex, uint32_t generation) {
        if (rectIndex >= m_rects.size()) return;
        auto& r = m_rects[rectIndex];
        if (r.generation != generation) return;
        lruMoveToFront(rectIndex);
        r.lastAccessFrame = m_currentFrame;
    }

    bool getRectPosition(uint32_t rectIndex, uint16_t& x, uint16_t& y) const {
        if (rectIndex >= m_rects.size()) return false;
        x = m_rects[rectIndex].x;
        y = m_rects[rectIndex].y;
        return true;
    }

    uint32_t evictLRU(uint32_t maxToEvict, std::vector<ThumbnailId>* outEvictedIds = nullptr) {
        uint32_t evicted = 0;
        while (evicted < maxToEvict && m_lruTail != kInvalidIndex) {
            uint32_t victimIdx = m_lruTail;

            while (victimIdx != kInvalidIndex) {
                // �v���[�X�z���_�[�ی�
                if (victimIdx == 0 && m_hasPlaceholder) {
                    victimIdx = m_rects[victimIdx].lruPrev;
                    continue;
                }
                // Loading���Ń^�C���A�E�g���Ă��Ȃ����͔̂�΂�
                auto& r = m_rects[victimIdx];
                if (r.state == RectState::Loading) {
                    uint64_t elapsed = m_currentFrame - r.lastAccessFrame;
                    if (elapsed < m_config.loadingTimeoutFrames) {
                        victimIdx = r.lruPrev;
                        continue;
                    }
                }
                break;
            }
            if (victimIdx == kInvalidIndex) break;

            if (outEvictedIds && m_rects[victimIdx].id != 0) {
                outEvictedIds->push_back(m_rects[victimIdx].id);
            }

            lruRemove(victimIdx);
            freeRect(victimIdx);
            evicted++;
        }
        return evicted;
    }

    void setCurrentFrame(uint64_t frame) { m_currentFrame = frame; }
    bgfx::TextureHandle& getTexture() { return m_texture; }
    uint16_t pageIndex() const { return m_pageIndex; }
    uint16_t contentSize() const { return m_config.contentSize; }
    const GridPageConfig& config() const { return m_config; }
    bool isFull() const { return m_freeList.empty(); }
    uint32_t usedCount() const { return m_lruCount; }
    uint64_t lastUsedFrame() const { return m_lastUsedFrame; }
    void markUsed() { m_lastUsedFrame = m_currentFrame; }

private:
    void freeRect(uint32_t idx) {
        auto& r = m_rects[idx];
        r.state = RectState::Empty;
        r.id = 0;
        r.lruPrev = r.lruNext = kInvalidIndex;
        m_freeList.push_back(idx);
    }

    void lruRemove(uint32_t idx) {
        auto& r = m_rects[idx];
        if (r.lruPrev != kInvalidIndex) m_rects[r.lruPrev].lruNext = r.lruNext;
        else m_lruHead = r.lruNext;
        if (r.lruNext != kInvalidIndex) m_rects[r.lruNext].lruPrev = r.lruPrev;
        else m_lruTail = r.lruPrev;
        r.lruPrev = r.lruNext = kInvalidIndex;
        m_lruCount--;
    }

    void lruPushFront(uint32_t idx) {
        auto& r = m_rects[idx];
        r.lruPrev = kInvalidIndex;
        r.lruNext = m_lruHead;
        if (m_lruHead != kInvalidIndex) m_rects[m_lruHead].lruPrev = idx;
        m_lruHead = idx;
        if (m_lruTail == kInvalidIndex) m_lruTail = idx;
        m_lruCount++;
    }

    void lruMoveToFront(uint32_t idx) {
        if (idx == m_lruHead) return;
        lruRemove(idx);
        lruPushFront(idx);
    }

    uint16_t m_pageIndex;
    GridPageConfig m_config;
    uint16_t m_tilesX = 0, m_tilesY = 0;
    bool m_hasPlaceholder = false;

    bgfx::TextureHandle m_texture = BGFX_INVALID_HANDLE;
    std::vector<GridRectInfo> m_rects;
    std::vector<uint32_t> m_freeList;

    uint32_t m_lruHead = kInvalidIndex;
    uint32_t m_lruTail = kInvalidIndex;
    uint32_t m_lruCount = 0;
    uint64_t m_currentFrame = 0;
    uint64_t m_lastUsedFrame = 0;
};

//=============================================================================
// ThumbnailGridAtlas
//=============================================================================
struct ThumbnailGridAtlasConfig {
    uint16_t atlasWidth = 2048;
    uint16_t atlasHeight = 2048;
    uint16_t tileSize = 128;
    uint16_t contentSize = 124;
    uint8_t maxMipLevels = 4;
    uint64_t loadingTimeoutFrames = 300;
    uint16_t maxPages = 8;
    bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA8;
};

class ThumbnailGridAtlas {
public:
    explicit ThumbnailGridAtlas(const ThumbnailGridAtlasConfig& config = {})
        : m_config(config) {
    }

    ~ThumbnailGridAtlas() { shutdown(); }

    bool initialize() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pages.empty()) return true;

        GridPageConfig pageConfig;
        pageConfig.atlasWidth = m_config.atlasWidth;
        pageConfig.atlasHeight = m_config.atlasHeight;
        pageConfig.tileSize = m_config.tileSize;
        pageConfig.contentSize = m_config.contentSize;
        pageConfig.maxMipLevels = m_config.maxMipLevels;
        pageConfig.loadingTimeoutFrames = m_config.loadingTimeoutFrames;
        pageConfig.format = m_config.format;

        auto page = std::make_unique<ThumbnailGridPage>(0, pageConfig);
        if (!page->initialize()) return false;
        page->createPlaceholder();
        m_pages.push_back(std::move(page));
        return true;
    }

    void shutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pages.clear();
        m_idToHandle.clear();
        m_uploadQueue.clear();
    }

    std::optional<ThumbnailHandle> find(ThumbnailId id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_idToHandle.find(id);
        if (it != m_idToHandle.end()) {
            auto& h = it->second;
            if (h.pageIndex < m_pages.size()) {
                m_pages[h.pageIndex]->touch(h.rectIndex, h.generation);
                m_pages[h.pageIndex]->markUsed();
                return h;
            }
        }
        return std::nullopt;
    }

    ThumbnailHandle acquire(ThumbnailId id, bool& outNeedsLoad) {
        std::lock_guard<std::mutex> lock(m_mutex);
        outNeedsLoad = false;

        // �����`�F�b�N
        auto it = m_idToHandle.find(id);
        if (it != m_idToHandle.end()) {
            auto& h = it->second;
            if (h.pageIndex < m_pages.size()) {
                auto& page = m_pages[h.pageIndex];
                page->touch(h.rectIndex, h.generation);
                page->markUsed();
                outNeedsLoad = page->isLoading(h.rectIndex, h.generation);
                return h;
            }
        }

        // �󂫂�����y�[�W��T��
        ThumbnailGridPage* targetPage = nullptr;
        for (auto& page : m_pages) {
            if (page->canAllocate()) {
                targetPage = page.get();
                break;
            }
        }

        // �Ȃ���� eviction
        if (!targetPage) {
            for (auto& page : m_pages) {
                std::vector<ThumbnailId> evictedIds;
                if (page->evictLRU(1, &evictedIds) > 0) {
                    for (ThumbnailId evictedId : evictedIds) {
                        m_idToHandle.erase(evictedId);
                    }
                    if (page->canAllocate()) {
                        targetPage = page.get();
                        break;
                    }
                }
            }
        }

        // ����ł��Ȃ���ΐV�y�[�W
        if (!targetPage && m_pages.size() < m_config.maxPages) {
            targetPage = createNewPage();
        }

        if (!targetPage) return ThumbnailHandle{};

        uint32_t rectIdx, gen;
        if (targetPage->allocate(id, rectIdx, gen)) {
            ThumbnailHandle h{ targetPage->pageIndex(), rectIdx, gen };
            m_idToHandle[id] = h;
            targetPage->markUsed();
            outNeedsLoad = true;
            return h;
        }

        return ThumbnailHandle{};
    }

    bool queueUpload(ThumbnailHandle handle, const void* data, uint16_t w, uint16_t h) {
        if (!data || w == 0 || h == 0) return false;

        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex >= m_pages.size()) return false;

        auto& page = m_pages[handle.pageIndex];
        if (!page->isLoading(handle.rectIndex, handle.generation)) return false;

        // �T�C�Y�`�F�b�N
        uint16_t contentSize = m_config.contentSize;
        assert(w <= contentSize && h <= contentSize);

        PendingUpload upload;
        upload.handle = handle;
        upload.width = w;
        upload.height = h;
        upload.data.resize(w * h * 4);
        std::memcpy(upload.data.data(), data, upload.data.size());
        m_uploadQueue.push_back(std::move(upload));
        return true;
    }

    void beginFrame() {
        std::vector<PendingUpload> uploads;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_currentFrame++;
            for (auto& page : m_pages) {
                page->setCurrentFrame(m_currentFrame);
            }
            uploads.swap(m_uploadQueue);
        }

        for (auto& upload : uploads) {
            processUpload(upload);
        }
    }

    bool isValid(ThumbnailHandle handle) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex >= m_pages.size()) return false;
        return m_pages[handle.pageIndex]->isValid(handle.rectIndex, handle.generation);
    }

    bool getUV(ThumbnailHandle handle, float& u0, float& v0, float& u1, float& v1) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex >= m_pages.size()) return false;
        auto& page = m_pages[handle.pageIndex];
        if (!page->isValid(handle.rectIndex, handle.generation)) return false;
        page->computeUV(handle.rectIndex, u0, v0, u1, v1);
        return true;
    }

    void getPlaceholderUV(float& u0, float& v0, float& u1, float& v1) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pages.empty()) {
            m_pages[0]->computeUV(0, u0, v0, u1, v1);
        }
    }

    bgfx::TextureHandle& getTexture(uint16_t pageIndex) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (pageIndex < m_pages.size()) return m_pages[pageIndex]->getTexture();
        return nulltex;
    }

    bgfx::TextureHandle getTexture(ThumbnailHandle handle) {
        return getTexture(handle.pageIndex);
    }

    void touch(ThumbnailHandle handle) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex < m_pages.size()) {
            m_pages[handle.pageIndex]->touch(handle.rectIndex, handle.generation);
            m_pages[handle.pageIndex]->markUsed();
        }
    }

    uint32_t evictLRU(uint32_t maxToEvict = 16) {
        std::lock_guard<std::mutex> lock(m_mutex);
        uint32_t total = 0;
        for (auto& page : m_pages) {
            std::vector<ThumbnailId> evictedIds;
            uint32_t evicted = page->evictLRU(maxToEvict - total, &evictedIds);
            for (ThumbnailId id : evictedIds) {
                m_idToHandle.erase(id);
            }
            total += evicted;
            if (total >= maxToEvict) break;
        }
        return total;
    }

    struct Stats {
        uint32_t pageCount;
        uint32_t usedRects;
        uint32_t pendingUploads;
        uint64_t currentFrame;
    };

    Stats getStats() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        Stats s{};
        s.pageCount = (uint32_t)m_pages.size();
        s.pendingUploads = (uint32_t)m_uploadQueue.size();
        s.currentFrame = m_currentFrame;
        for (auto& page : m_pages) {
            s.usedRects += page->usedCount();
        }
        return s;
    }

    uint16_t contentSize() const { return m_config.contentSize; }

private:
    ThumbnailGridPage* createNewPage() {
        GridPageConfig pageConfig;
        pageConfig.atlasWidth = m_config.atlasWidth;
        pageConfig.atlasHeight = m_config.atlasHeight;
        pageConfig.tileSize = m_config.tileSize;
        pageConfig.contentSize = m_config.contentSize;
        pageConfig.maxMipLevels = m_config.maxMipLevels;
        pageConfig.loadingTimeoutFrames = m_config.loadingTimeoutFrames;
        pageConfig.format = m_config.format;

        auto page = std::make_unique<ThumbnailGridPage>((uint16_t)m_pages.size(), pageConfig);
        if (!page->initialize()) return nullptr;

        ThumbnailGridPage* ptr = page.get();
        m_pages.push_back(std::move(page));
        return ptr;
    }

    void processUpload(PendingUpload& upload) {
        uint16_t x, y;
        bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
        uint8_t maxMipLevels = 0;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (upload.handle.pageIndex >= m_pages.size()) return;
            auto& page = m_pages[upload.handle.pageIndex];
            if (!page->isLoading(upload.handle.rectIndex, upload.handle.generation)) return;
            if (!page->getRectPosition(upload.handle.rectIndex, x, y)) return;
            texture = page->getTexture();
            maxMipLevels = m_config.maxMipLevels;
        }

        if (!bgfx::isValid(texture)) return;

        bgfx::updateTexture2D(texture, 0, 0, x, y, upload.width, upload.height,
            bgfx::copy(upload.data.data(), upload.width * upload.height * 4));

        generateMipmaps(texture, upload.data.data(), x, y,
            upload.width, upload.height, maxMipLevels);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (upload.handle.pageIndex < m_pages.size()) {
                m_pages[upload.handle.pageIndex]->markReady(
                    upload.handle.rectIndex, upload.handle.generation);
            }
        }
    }

    ThumbnailGridAtlasConfig m_config;
    std::vector<std::unique_ptr<ThumbnailGridPage>> m_pages;
    std::unordered_map<ThumbnailId, ThumbnailHandle> m_idToHandle;
    std::vector<PendingUpload> m_uploadQueue;
    uint64_t m_currentFrame = 0;
    mutable std::mutex m_mutex;
};
struct ShelfPageConfig {
    uint16_t atlasWidth = 2048;
    uint16_t atlasHeight = 2048;
    uint16_t rowHeight = 160;           // �s�̍����i�p�f�B���O���݁j
    uint16_t contentHeight = 144;       // ���ۂ̃R���e���c����
    uint16_t maxContentWidth = 512;     // �ő�R���e���c��
    uint8_t maxMipLevels = 4;
    uint64_t loadingTimeoutFrames = 300;
    bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA8;
};

//=============================================================================
// Shelf��`���
//=============================================================================
struct ShelfRectInfo {
    ThumbnailId id = 0;
    uint32_t generation = 0;
    RectState state = RectState::Empty;
    uint16_t x = 0, y = 0;
    uint16_t contentWidth = 0, contentHeight = 0;
    uint32_t shelfIndex = kInvalidIndex;
    uint64_t lastAccessFrame = 0;
};

//=============================================================================
// Shelf�i�s�j���
//=============================================================================
struct ShelfInfo {
    uint16_t y = 0;
    uint16_t usedWidth = 0;
    uint32_t firstRectIndex = kInvalidIndex;
    uint32_t rectCount = 0;

    // �s�P��LRU
    uint32_t lruPrev = kInvalidIndex;
    uint32_t lruNext = kInvalidIndex;
    uint64_t lastAccessFrame = 0;
};

//=============================================================================
// ThumbnailShelfPage
//=============================================================================
class ThumbnailShelfPage {
public:
    ThumbnailShelfPage(uint16_t pageIndex, const ShelfPageConfig& config)
        : m_pageIndex(pageIndex), m_config(config)
    {
        m_maxShelves = m_config.atlasHeight / m_config.rowHeight;

        uint8_t maxMip = static_cast<uint8_t>(std::log2(m_config.contentHeight)) + 1;
        m_config.maxMipLevels = std::min(m_config.maxMipLevels, maxMip);
    }

    ~ThumbnailShelfPage() { shutdown(); }

    ThumbnailShelfPage(const ThumbnailShelfPage&) = delete;
    ThumbnailShelfPage& operator=(const ThumbnailShelfPage&) = delete;

    bool initialize() {
        if (bgfx::isValid(m_texture)) return true;

        uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
        m_texture = bgfx::createTexture2D(
            m_config.atlasWidth, m_config.atlasHeight,
            m_config.maxMipLevels > 1, 1, m_config.format, flags, nullptr);

        if (!bgfx::isValid(m_texture)) return false;

        m_rects.reserve(256);
        m_shelves.reserve(m_maxShelves);

        return true;
    }

    void shutdown() {
        if (bgfx::isValid(m_texture)) {
            bgfx::destroy(m_texture);
            m_texture = BGFX_INVALID_HANDLE;
        }
        m_rects.clear();
        m_shelves.clear();
        m_freeShelfList.clear();
        m_shelfLruHead = m_shelfLruTail = kInvalidIndex;
        m_shelfLruCount = 0;
    }

    void createPlaceholder() {
        uint16_t padding = (m_config.rowHeight - m_config.contentHeight) / 2;
        uint16_t size = m_config.contentHeight;

        // �ŏ��̍s�Ƀv���[�X�z���_�[
        uint32_t shelfIdx = allocateShelfSlot();
        auto& shelf = m_shelves[shelfIdx];
        shelf.y = 0;
        shelf.usedWidth = size + padding * 2;
        shelf.lastAccessFrame = m_currentFrame;
        shelfLruPushFront(shelfIdx);

        uint32_t rectIdx = (uint32_t)m_rects.size();
        m_rects.emplace_back();
        auto& r = m_rects.back();
        r.generation = 1;
        r.state = RectState::Ready;
        r.x = padding;
        r.y = padding;
        r.contentWidth = size;
        r.contentHeight = size;
        r.shelfIndex = shelfIdx;
        shelf.firstRectIndex = rectIdx;
        shelf.rectCount = 1;

        auto pixels = createPlaceholderPixels(size);
        bgfx::updateTexture2D(m_texture, 0, 0, r.x, r.y, size, size,
            bgfx::copy(pixels.data(), (uint32_t)pixels.size()));

        m_usedShelfCount = 1;
        m_hasPlaceholder = true;
    }

    bool canAllocate(uint16_t contentWidth, uint16_t contentHeight) const {
        if (contentWidth > m_config.maxContentWidth) return false;
        if (contentHeight > m_config.contentHeight) return false;

        uint16_t padding = (m_config.rowHeight - m_config.contentHeight) / 2;
        uint16_t cellWidth = contentWidth + padding * 2;

        // �����̍s�ɓ��邩
        for (const auto& shelf : m_shelves) {
            if (shelf.usedWidth > 0 && shelf.usedWidth + cellWidth <= m_config.atlasWidth) {
                return true;
            }
        }
        // �󂫍s�����邩
        if (!m_freeShelfList.empty()) return true;
        // �V�����s�����邩
        return m_usedShelfCount < m_maxShelves;
    }

    bool allocate(ThumbnailId id, uint16_t contentWidth, uint16_t contentHeight,
        uint32_t& outRectIndex, uint32_t& outGeneration) {
        contentWidth = std::min(contentWidth, m_config.maxContentWidth);
        contentHeight = std::min(contentHeight, m_config.contentHeight);

        uint16_t padding = (m_config.rowHeight - m_config.contentHeight) / 2;
        uint16_t cellWidth = contentWidth + padding * 2;

        // 1. �����̍s�ŋ󂫂����邩�T��
        for (uint32_t si = 0; si < m_shelves.size(); ++si) {
            auto& shelf = m_shelves[si];
            if (shelf.usedWidth > 0 && shelf.usedWidth + cellWidth <= m_config.atlasWidth) {
                return allocateInShelf(si, id, contentWidth, contentHeight,
                    cellWidth, padding, outRectIndex, outGeneration);
            }
        }

        // 2. �󂫍s������Ύg��
        if (!m_freeShelfList.empty()) {
            uint32_t si = m_freeShelfList.back();
            m_freeShelfList.pop_back();
            auto& shelf = m_shelves[si];
            shelf.usedWidth = 0;
            shelf.rectCount = 0;
            shelf.firstRectIndex = kInvalidIndex;
            shelfLruPushFront(si);
            return allocateInShelf(si, id, contentWidth, contentHeight,
                cellWidth, padding, outRectIndex, outGeneration);
        }

        // 3. �V�����s���쐬
        if (m_usedShelfCount >= m_maxShelves) return false;

        uint32_t si = allocateShelfSlot();
        auto& shelf = m_shelves[si];
        shelf.y = m_usedShelfCount * m_config.rowHeight;
        shelf.usedWidth = 0;
        shelf.rectCount = 0;
        shelf.firstRectIndex = kInvalidIndex;
        shelf.lastAccessFrame = m_currentFrame;
        m_usedShelfCount++;
        shelfLruPushFront(si);

        return allocateInShelf(si, id, contentWidth, contentHeight,
            cellWidth, padding, outRectIndex, outGeneration);
    }

    bool isValid(uint32_t rectIndex, uint32_t generation) const {
        if (rectIndex >= m_rects.size()) return false;
        const auto& r = m_rects[rectIndex];
        return r.generation == generation && r.state == RectState::Ready;
    }

    bool isLoading(uint32_t rectIndex, uint32_t generation) const {
        if (rectIndex >= m_rects.size()) return false;
        const auto& r = m_rects[rectIndex];
        return r.generation == generation && r.state == RectState::Loading;
    }

    void markReady(uint32_t rectIndex, uint32_t generation) {
        if (rectIndex >= m_rects.size()) return;
        auto& r = m_rects[rectIndex];
        if (r.generation == generation && r.state == RectState::Loading) {
            r.state = RectState::Ready;
        }
    }

    void computeUV(uint32_t rectIndex, float& u0, float& v0, float& u1, float& v1) const {
        if (rectIndex >= m_rects.size()) return;
        const auto& r = m_rects[rectIndex];
        float invW = 1.0f / m_config.atlasWidth;
        float invH = 1.0f / m_config.atlasHeight;
        u0 = r.x * invW;
        v0 = r.y * invH;
        u1 = (r.x + r.contentWidth) * invW;
        v1 = (r.y + r.contentHeight) * invH;
    }

    void touch(uint32_t rectIndex, uint32_t generation) {
        if (rectIndex >= m_rects.size()) return;
        auto& r = m_rects[rectIndex];
        if (r.generation != generation) return;

        // �s�P��LRU
        if (r.shelfIndex != kInvalidIndex && r.shelfIndex < m_shelves.size()) {
            shelfLruMoveToFront(r.shelfIndex);
            m_shelves[r.shelfIndex].lastAccessFrame = m_currentFrame;
        }
    }

    bool getRectInfo(uint32_t rectIndex, uint16_t& x, uint16_t& y,
        uint16_t& w, uint16_t& h) const {
        if (rectIndex >= m_rects.size()) return false;
        const auto& r = m_rects[rectIndex];
        x = r.x; y = r.y;
        w = r.contentWidth;
        h = r.contentHeight;
        return true;
    }

    // �s�P��eviction
    uint32_t evictLRU(uint32_t maxToEvict, std::vector<ThumbnailId>* outEvictedIds = nullptr) {
        uint32_t evicted = 0;

        while (evicted < maxToEvict && m_shelfLruTail != kInvalidIndex) {
            uint32_t victimShelfIdx = m_shelfLruTail;

            // �K�؂�victim��T��
            while (victimShelfIdx != kInvalidIndex) {
                // �v���[�X�z���_�[�s�͔�΂�
                if (victimShelfIdx == 0 && m_hasPlaceholder) {
                    victimShelfIdx = m_shelves[victimShelfIdx].lruPrev;
                    continue;
                }
                // Loading���Ń^�C���A�E�g���Ă��Ȃ��s�͔�΂�
                auto& shelf = m_shelves[victimShelfIdx];
                bool hasActiveLoading = false;
                for (uint32_t i = shelf.firstRectIndex;
                    i < shelf.firstRectIndex + shelf.rectCount && i < m_rects.size(); ++i) {
                    if (m_rects[i].state == RectState::Loading) {
                        uint64_t elapsed = m_currentFrame - m_rects[i].lastAccessFrame;
                        if (elapsed < m_config.loadingTimeoutFrames) {
                            hasActiveLoading = true;
                            break;
                        }
                    }
                }
                if (hasActiveLoading) {
                    victimShelfIdx = shelf.lruPrev;
                    continue;
                }
                break;
            }
            if (victimShelfIdx == kInvalidIndex) break;

            // �s���ۂ��Ɣp��
            auto& shelf = m_shelves[victimShelfIdx];
            for (uint32_t i = shelf.firstRectIndex;
                i < shelf.firstRectIndex + shelf.rectCount && i < m_rects.size(); ++i) {
                auto& r = m_rects[i];
                if (r.shelfIndex == victimShelfIdx && r.id != 0) {
                    if (outEvictedIds) {
                        outEvictedIds->push_back(r.id);
                    }
                    r.state = RectState::Empty;
                    r.id = 0;
                    evicted++;
                }
            }

            // �s��LRU����O����free���X�g��
            shelfLruRemove(victimShelfIdx);
            shelf.usedWidth = 0;
            shelf.rectCount = 0;
            shelf.firstRectIndex = kInvalidIndex;
            m_freeShelfList.push_back(victimShelfIdx);
        }

        return evicted;
    }

    void setCurrentFrame(uint64_t frame) { m_currentFrame = frame; }
    bgfx::TextureHandle& getTexture() { return m_texture; }
    uint16_t pageIndex() const { return m_pageIndex; }
    uint16_t contentHeight() const { return m_config.contentHeight; }
    const ShelfPageConfig& config() const { return m_config; }
    bool isFull() const { return !canAllocate(1, m_config.contentHeight); }
    uint32_t usedCount() const { return m_shelfLruCount; }
    uint64_t lastUsedFrame() const { return m_lastUsedFrame; }
    void markUsed() { m_lastUsedFrame = m_currentFrame; }

private:
    uint32_t allocateShelfSlot() {
        if (!m_freeShelfList.empty()) {
            uint32_t idx = m_freeShelfList.back();
            m_freeShelfList.pop_back();
            return idx;
        }
        uint32_t idx = (uint32_t)m_shelves.size();
        m_shelves.emplace_back();
        return idx;
    }

    bool allocateInShelf(uint32_t shelfIdx, ThumbnailId id,
        uint16_t contentWidth, uint16_t contentHeight,
        uint16_t cellWidth, uint16_t padding,
        uint32_t& outRectIndex, uint32_t& outGeneration) {
        auto& shelf = m_shelves[shelfIdx];

        uint32_t rectIdx = (uint32_t)m_rects.size();
        m_rects.emplace_back();
        auto& r = m_rects.back();
        r.id = id;
        r.generation = 1;
        r.state = RectState::Loading;
        r.x = shelf.usedWidth + padding;
        r.y = shelf.y + padding;
        r.contentWidth = contentWidth;
        r.contentHeight = contentHeight;
        r.shelfIndex = shelfIdx;
        r.lastAccessFrame = m_currentFrame;

        shelf.usedWidth += cellWidth;
        if (shelf.firstRectIndex == kInvalidIndex) {
            shelf.firstRectIndex = rectIdx;
        }
        shelf.rectCount++;
        shelf.lastAccessFrame = m_currentFrame;
        shelfLruMoveToFront(shelfIdx);

        outRectIndex = rectIdx;
        outGeneration = r.generation;
        return true;
    }

    void shelfLruRemove(uint32_t idx) {
        auto& s = m_shelves[idx];
        if (s.lruPrev != kInvalidIndex) m_shelves[s.lruPrev].lruNext = s.lruNext;
        else m_shelfLruHead = s.lruNext;
        if (s.lruNext != kInvalidIndex) m_shelves[s.lruNext].lruPrev = s.lruPrev;
        else m_shelfLruTail = s.lruPrev;
        s.lruPrev = s.lruNext = kInvalidIndex;
        m_shelfLruCount--;
    }

    void shelfLruPushFront(uint32_t idx) {
        auto& s = m_shelves[idx];
        s.lruPrev = kInvalidIndex;
        s.lruNext = m_shelfLruHead;
        if (m_shelfLruHead != kInvalidIndex) m_shelves[m_shelfLruHead].lruPrev = idx;
        m_shelfLruHead = idx;
        if (m_shelfLruTail == kInvalidIndex) m_shelfLruTail = idx;
        m_shelfLruCount++;
    }

    void shelfLruMoveToFront(uint32_t idx) {
        if (idx == m_shelfLruHead) return;
        shelfLruRemove(idx);
        shelfLruPushFront(idx);
    }

    uint16_t m_pageIndex;
    ShelfPageConfig m_config;
    uint16_t m_maxShelves = 0;
    uint16_t m_usedShelfCount = 0;
    bool m_hasPlaceholder = false;

    bgfx::TextureHandle m_texture = BGFX_INVALID_HANDLE;
    std::vector<ShelfRectInfo> m_rects;
    std::vector<ShelfInfo> m_shelves;
    std::vector<uint32_t> m_freeShelfList;

    uint32_t m_shelfLruHead = kInvalidIndex;
    uint32_t m_shelfLruTail = kInvalidIndex;
    uint32_t m_shelfLruCount = 0;

    uint64_t m_currentFrame = 0;
    uint64_t m_lastUsedFrame = 0;
};

//=============================================================================
// ThumbnailShelfAtlas
//=============================================================================
struct ThumbnailShelfAtlasConfig {
    uint16_t atlasWidth = 2048;
    uint16_t atlasHeight = 2048;
    uint16_t rowHeight = 160;
    uint16_t contentHeight = 144;
    uint16_t maxContentWidth = 512;
    uint8_t maxMipLevels = 4;
    uint64_t loadingTimeoutFrames = 300;
    uint16_t maxPages = 8;
    bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA8;
};

class ThumbnailShelfAtlas {
public:
    explicit ThumbnailShelfAtlas(const ThumbnailShelfAtlasConfig& config = {})
        : m_config(config) {
    }

    ~ThumbnailShelfAtlas() { shutdown(); }

    bool initialize() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pages.empty()) return true;

        ShelfPageConfig pageConfig;
        pageConfig.atlasWidth = m_config.atlasWidth;
        pageConfig.atlasHeight = m_config.atlasHeight;
        pageConfig.rowHeight = m_config.rowHeight;
        pageConfig.contentHeight = m_config.contentHeight;
        pageConfig.maxContentWidth = m_config.maxContentWidth;
        pageConfig.maxMipLevels = m_config.maxMipLevels;
        pageConfig.loadingTimeoutFrames = m_config.loadingTimeoutFrames;
        pageConfig.format = m_config.format;

        auto page = std::make_unique<ThumbnailShelfPage>(0, pageConfig);
        if (!page->initialize()) return false;
        page->createPlaceholder();
        m_pages.push_back(std::move(page));
        return true;
    }

    void shutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pages.clear();
        m_idToHandle.clear();
        m_uploadQueue.clear();
    }

    std::optional<ThumbnailHandle> find(ThumbnailId id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_idToHandle.find(id);
        if (it != m_idToHandle.end()) {
            auto& h = it->second;
            if (h.pageIndex < m_pages.size()) {
                m_pages[h.pageIndex]->touch(h.rectIndex, h.generation);
                m_pages[h.pageIndex]->markUsed();
                return h;
            }
        }
        return std::nullopt;
    }

    ThumbnailHandle acquire(ThumbnailId id, uint16_t contentWidth, uint16_t contentHeight,
        bool& outNeedsLoad) {
        std::lock_guard<std::mutex> lock(m_mutex);
        outNeedsLoad = false;

        // �����`�F�b�N
        auto it = m_idToHandle.find(id);
        if (it != m_idToHandle.end()) {
            auto& h = it->second;
            if (h.pageIndex < m_pages.size()) {
                auto& page = m_pages[h.pageIndex];
                page->touch(h.rectIndex, h.generation);
                page->markUsed();
                outNeedsLoad = page->isLoading(h.rectIndex, h.generation);
                return h;
            }
        }

        // �󂫂�����y�[�W��T��
        ThumbnailShelfPage* targetPage = nullptr;
        for (auto& page : m_pages) {
            if (page->canAllocate(contentWidth, contentHeight)) {
                targetPage = page.get();
                break;
            }
        }

        // �Ȃ���� eviction
        if (!targetPage) {
            for (auto& page : m_pages) {
                std::vector<ThumbnailId> evictedIds;
                if (page->evictLRU(1, &evictedIds) > 0) {
                    for (ThumbnailId evictedId : evictedIds) {
                        m_idToHandle.erase(evictedId);
                    }
                    if (page->canAllocate(contentWidth, contentHeight)) {
                        targetPage = page.get();
                        break;
                    }
                }
            }
        }

        // ����ł��Ȃ���ΐV�y�[�W
        if (!targetPage && m_pages.size() < m_config.maxPages) {
            targetPage = createNewPage();
        }

        if (!targetPage) return ThumbnailHandle{};

        uint32_t rectIdx, gen;
        if (targetPage->allocate(id, contentWidth, contentHeight, rectIdx, gen)) {
            ThumbnailHandle h{ targetPage->pageIndex(), rectIdx, gen };
            m_idToHandle[id] = h;
            targetPage->markUsed();
            outNeedsLoad = true;
            return h;
        }

        return ThumbnailHandle{};
    }

    bool queueUpload(ThumbnailHandle handle, const void* data, uint16_t w, uint16_t h) {
        if (!data || w == 0 || h == 0) return false;

        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex >= m_pages.size()) return false;

        auto& page = m_pages[handle.pageIndex];
        if (!page->isLoading(handle.rectIndex, handle.generation)) return false;

        // �T�C�Y�`�F�b�N
        uint16_t rx, ry, rw, rh;
        if (!page->getRectInfo(handle.rectIndex, rx, ry, rw, rh)) return false;
        assert(w <= rw && h <= rh);

        PendingUpload upload;
        upload.handle = handle;
        upload.width = w;
        upload.height = h;
        upload.data.resize(w * h * 4);
        std::memcpy(upload.data.data(), data, upload.data.size());
        m_uploadQueue.push_back(std::move(upload));
        return true;
    }

    void beginFrame() {
        std::vector<PendingUpload> uploads;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_currentFrame++;
            for (auto& page : m_pages) {
                page->setCurrentFrame(m_currentFrame);
            }
            uploads.swap(m_uploadQueue);
        }

        for (auto& upload : uploads) {
            processUpload(upload);
        }
    }

    bool isValid(ThumbnailHandle handle) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex >= m_pages.size()) return false;
        return m_pages[handle.pageIndex]->isValid(handle.rectIndex, handle.generation);
    }

    bool getUV(ThumbnailHandle handle, float& u0, float& v0, float& u1, float& v1) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex >= m_pages.size()) return false;
        auto& page = m_pages[handle.pageIndex];
        if (!page->isValid(handle.rectIndex, handle.generation)) return false;
        page->computeUV(handle.rectIndex, u0, v0, u1, v1);
        return true;
    }

    void getPlaceholderUV(float& u0, float& v0, float& u1, float& v1) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pages.empty()) {
            m_pages[0]->computeUV(0, u0, v0, u1, v1);
        }
    }

    bgfx::TextureHandle& getTexture(uint16_t pageIndex) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (pageIndex < m_pages.size()) return m_pages[pageIndex]->getTexture();
        return nulltex;
    }

    bgfx::TextureHandle getTexture(ThumbnailHandle handle) {
        return getTexture(handle.pageIndex);
    }

    void touch(ThumbnailHandle handle) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (handle.pageIndex < m_pages.size()) {
            m_pages[handle.pageIndex]->touch(handle.rectIndex, handle.generation);
            m_pages[handle.pageIndex]->markUsed();
        }
    }

    uint32_t evictLRU(uint32_t maxToEvict = 16) {
        std::lock_guard<std::mutex> lock(m_mutex);
        uint32_t total = 0;
        for (auto& page : m_pages) {
            std::vector<ThumbnailId> evictedIds;
            uint32_t evicted = page->evictLRU(maxToEvict - total, &evictedIds);
            for (ThumbnailId id : evictedIds) {
                m_idToHandle.erase(id);
            }
            total += evicted;
            if (total >= maxToEvict) break;
        }
        return total;
    }

    struct Stats {
        uint32_t pageCount;
        uint32_t usedShelves;
        uint32_t pendingUploads;
        uint64_t currentFrame;
    };

    Stats getStats() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        Stats s{};
        s.pageCount = (uint32_t)m_pages.size();
        s.pendingUploads = (uint32_t)m_uploadQueue.size();
        s.currentFrame = m_currentFrame;
        for (auto& page : m_pages) {
            s.usedShelves += page->usedCount();
        }
        return s;
    }

    uint16_t contentHeight() const { return m_config.contentHeight; }
    uint16_t maxContentWidth() const { return m_config.maxContentWidth; }

private:
    ThumbnailShelfPage* createNewPage() {
        ShelfPageConfig pageConfig;
        pageConfig.atlasWidth = m_config.atlasWidth;
        pageConfig.atlasHeight = m_config.atlasHeight;
        pageConfig.rowHeight = m_config.rowHeight;
        pageConfig.contentHeight = m_config.contentHeight;
        pageConfig.maxContentWidth = m_config.maxContentWidth;
        pageConfig.maxMipLevels = m_config.maxMipLevels;
        pageConfig.loadingTimeoutFrames = m_config.loadingTimeoutFrames;
        pageConfig.format = m_config.format;

        auto page = std::make_unique<ThumbnailShelfPage>((uint16_t)m_pages.size(), pageConfig);
        if (!page->initialize()) return nullptr;

        ThumbnailShelfPage* ptr = page.get();
        m_pages.push_back(std::move(page));
        return ptr;
    }

    void processUpload(PendingUpload& upload) {
        uint16_t x, y, w, h;
        bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
        uint8_t maxMipLevels = 0;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (upload.handle.pageIndex >= m_pages.size()) return;
            auto& page = m_pages[upload.handle.pageIndex];
            if (!page->isLoading(upload.handle.rectIndex, upload.handle.generation)) return;
            if (!page->getRectInfo(upload.handle.rectIndex, x, y, w, h)) return;
            texture = page->getTexture();
            maxMipLevels = m_config.maxMipLevels;
        }

        if (!bgfx::isValid(texture)) return;
        assert(upload.width <= w && upload.height <= h);

        bgfx::updateTexture2D(texture, 0, 0, x, y, upload.width, upload.height,
            bgfx::copy(upload.data.data(), upload.width * upload.height * 4));

        generateMipmaps(texture, upload.data.data(), x, y,
            upload.width, upload.height, maxMipLevels);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (upload.handle.pageIndex < m_pages.size()) {
                m_pages[upload.handle.pageIndex]->markReady(
                    upload.handle.rectIndex, upload.handle.generation);
            }
        }
    }

    ThumbnailShelfAtlasConfig m_config;
    std::vector<std::unique_ptr<ThumbnailShelfPage>> m_pages;
    std::unordered_map<ThumbnailId, ThumbnailHandle> m_idToHandle;
    std::vector<PendingUpload> m_uploadQueue;
    uint64_t m_currentFrame = 0;
    mutable std::mutex m_mutex;
};
using RenderGroupId = uint32_t;
using SurfaceId = uint32_t;

constexpr ImageId WHITE_PIXEL_ID = 0xFFFFFFFF;
constexpr RenderGroupId INVALID_GROUP = 0;

//=============================================================================
// GlyphInfo / GlyphKey
//=============================================================================
enum class GlyphFlags : uint8_t {
    None = 0,
    Pinned = 1 << 0,
};
inline GlyphFlags operator|(GlyphFlags a, GlyphFlags b) {
    return static_cast<GlyphFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
inline bool hasFlag(GlyphFlags f, GlyphFlags t) {
    return (static_cast<uint8_t>(f) & static_cast<uint8_t>(t)) != 0;
}

struct GlyphInfo {
    int advance = 0;
    int bearingX = 0;
    int bearingY = 0;
    int width = 0;
    int height = 0;
    int atlasX = 0;
    int atlasY = 0;
    float u0 = 0, v0 = 0, u1 = 0, v1 = 0;
    uint16_t pageIndex = 0;
    uint32_t lastUsedFrame = 0;
    GlyphFlags flags = GlyphFlags::None;
	ImageId imageId = 0;
};

struct GlyphKey {
    FontId fontId;
    uint32_t codepoint;
    bool operator==(const GlyphKey& o) const {
        return fontId == o.fontId && codepoint == o.codepoint;
    }
};
struct GlyphKeyHash {
    size_t operator()(const GlyphKey& k) const {
        return std::hash<uint64_t>{}(((uint64_t)k.fontId.computeHash() << 32) | k.codepoint);
    }
};


//=============================================================================
// PackHint / AtlasAffinity
//=============================================================================
enum class AtlasAffinity : uint8_t {
    Default,
    LongLived,
    Transient,
};

struct PackHint {
    AtlasAffinity affinity = AtlasAffinity::Default;
    RenderGroupId groupId = INVALID_GROUP;

    static PackHint longLived() { return { AtlasAffinity::LongLived, INVALID_GROUP }; }
    static PackHint transient() { return { AtlasAffinity::Transient, INVALID_GROUP }; }
    static PackHint forGroup(RenderGroupId g, AtlasAffinity a = AtlasAffinity::Default) {
        return { a, g };
    }
};

//=============================================================================
// AtlasPageStats: �y�[�W�̓��v���i�����̂݁j
//=============================================================================
struct AtlasPageStats {
    int liveCount = 0;
    int deadCount = 0;

    // Boyer-Moore majority vote
    struct GroupDominance {
        RenderGroupId id = INVALID_GROUP;
        int count = 0;
    } dominant;

    void incrementLive(RenderGroupId gid) {
        ++liveCount;
        trackGroup(gid);
    }

    void decrementLive() {
        if (liveCount > 0) --liveCount;
        ++deadCount;
    }

    float utilization() const {
        int total = liveCount + deadCount;
        return total > 0 ? float(liveCount) / float(total) : 1.0f;
    }

    void reset() {
        liveCount = 0;
        deadCount = 0;
        dominant = {};
    }

private:
    // Boyer-Moore majority vote algorithm
    void trackGroup(RenderGroupId gid) {
        if (gid == INVALID_GROUP) return;

        if (dominant.id == gid) {
            ++dominant.count;
        }
        else if (dominant.count == 0) {
            dominant.id = gid;
            dominant.count = 1;
        }
        else {
            --dominant.count;
        }
    }
};

//=============================================================================
// AtlasPage: �L�^�W�i���f���Ȃ��j
//=============================================================================
class FontAtlasPage {
public:
    struct Shelf { int x, y, h; };

    FontAtlasPage(int w, int h, int padding, uint16_t index, AtlasAffinity affinity)
        : W_(w), H_(h), pad_(padding), pageIndex_(index), affinity_(affinity)
    {
        pixels_.resize((size_t)W_ * H_ * 4, 0);
        shelves_.push_back({ 0, 0, 0 });
        tex_ = bgfx::createTexture2D(
            (uint16_t)W_, (uint16_t)H_, false, 1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
            nullptr);
    }
    ~FontAtlasPage() {
        if (bgfx::isValid(tex_)) bgfx::destroy(tex_);
    }

    FontAtlasPage(FontAtlasPage&& o) noexcept
        : W_(o.W_), H_(o.H_), pad_(o.pad_), pageIndex_(o.pageIndex_),
        affinity_(o.affinity_), pixels_(std::move(o.pixels_)),
        shelves_(std::move(o.shelves_)), tex_(o.tex_),
        stats_(o.stats_), pendingUploads_(std::move(o.pendingUploads_))
    {
        o.tex_ = BGFX_INVALID_HANDLE;
    }

    FontAtlasPage(const FontAtlasPage&) = delete;
    FontAtlasPage& operator=(const FontAtlasPage&) = delete;
    FontAtlasPage& operator=(FontAtlasPage&&) = delete;

    // �A�N�Z�T�i�ǂݎ���p�j
    bgfx::TextureHandle& handle() { return tex_; }
    uint16_t pageIndex() const { return pageIndex_; }
    AtlasAffinity affinity() const { return affinity_; }
    const AtlasPageStats& stats() const { return stats_; }
    AtlasPageStats& stats() { return stats_; }  // �O������X�V�p
    int width() const { return W_; }
    int height() const { return H_; }

    // �q���g�Ƃ̓K������i�����x�[�X�j
    bool matchesHint(const PackHint& hint) const {
        if (affinity_ != hint.affinity) return false;
        if (hint.groupId != INVALID_GROUP &&
            stats_.dominant.id != INVALID_GROUP &&
            stats_.dominant.id != hint.groupId) {
            return false;
        }
        return true;
    }

    // �p�b�L���O�i�����̋L�^�̂݁j
    GlyphInfo tryPack(int w, int h, const uint8_t* srcPixels,
        int pitch, bool immediate, RenderGroupId groupId) {
        GlyphInfo gi{};
        if (w == 0 || h == 0) return gi;

        const int tw = w + pad_ * 2;
        const int th = h + pad_ * 2;
        int px = -1, py = -1;

        // Shelf-pack
        for (auto& sh : shelves_) {
            if (sh.x + tw <= W_ && sh.y + std::max(sh.h, th) <= H_) {
                px = sh.x; py = sh.y;
                sh.x += tw;
                sh.h = std::max(sh.h, th);
                break;
            }
        }
        if (px < 0) {
            int newY = shelves_.back().y + shelves_.back().h;
            if (newY + th > H_) return gi;
            shelves_.push_back({ 0, newY, 0 });
            auto& sh = shelves_.back();
            px = sh.x; py = sh.y;
            sh.x += tw;
            sh.h = std::max(sh.h, th);
        }

        const int dx = px + pad_;
        const int dy = py + pad_;

        // CPU�o�b�t�@�փR�s�[
        uint8_t* dstBase = pixels_.data() + (size_t)dy * W_ * 4 + (size_t)dx * 4;
        for (int row = 0; row < h; ++row) {
            std::memcpy(dstBase + (size_t)row * W_ * 4,
                srcPixels + (size_t)row * pitch, (size_t)w * 4);
        }

        gi.width = w;
        gi.height = h;
        gi.atlasX = dx;
        gi.atlasY = dy;
        gi.u0 = float(dx) / float(W_);
        gi.v0 = float(dy) / float(H_);
        gi.u1 = float(dx + w) / float(W_);
        gi.v1 = float(dy + h) / float(H_);
        gi.pageIndex = pageIndex_;

        // ���v�X�V�i�����̋L�^�j
        stats_.incrementLive(groupId);

        if (immediate) {
            uploadRect(dx, dy, w, h, srcPixels, pitch);
        }
        else {
            pendingUploads_.push_back(gi);
        }
        return gi;
    }
    std::vector<GlyphInfo> takePendingUploads() {
        std::vector<GlyphInfo> result;
        result.swap(pendingUploads_);
        return result;
    }
    void flushPendingUploads() {
        for (const auto& gi : pendingUploads_) {
            uploadFromCPU(gi);
        }
        pendingUploads_.clear();
    }

    void clear() {
        std::fill(pixels_.begin(), pixels_.end(), (uint8_t)0);
        shelves_.clear();
        shelves_.push_back({ 0, 0, 0 });
        pendingUploads_.clear();
        stats_.reset();
    }
    void uploadRect(int x, int y, int w, int h, const void* src, int pitch) {
        std::vector<uint8_t> staging((size_t)w * h * 4);
        const uint8_t* s = (const uint8_t*)src;
        for (int row = 0; row < h; ++row) {
            std::memcpy(staging.data() + (size_t)row * w * 4,
                s + (size_t)row * pitch, (size_t)w * 4);
        }
        const bgfx::Memory* mem = bgfx::copy(staging.data(), (uint32_t)staging.size());
        bgfx::updateTexture2D(tex_, 0, 0, (uint16_t)x, (uint16_t)y,
            (uint16_t)w, (uint16_t)h, mem);
    }

    void uploadFromCPU(const GlyphInfo& gi) {
        std::vector<uint8_t> data((size_t)gi.width * gi.height * 4);
        for (int y = 0; y < gi.height; ++y) {
            std::memcpy(data.data() + (size_t)y * gi.width * 4,
                pixels_.data() + ((size_t)(gi.atlasY + y) * W_ + gi.atlasX) * 4,
                (size_t)gi.width * 4);
        }
        const bgfx::Memory* mem = bgfx::copy(data.data(), (uint32_t)data.size());
        bgfx::updateTexture2D(tex_, 0, 0, (uint16_t)gi.atlasX, (uint16_t)gi.atlasY,
            (uint16_t)gi.width, (uint16_t)gi.height, mem);
    }

    int W_, H_, pad_;
    uint16_t pageIndex_;
    AtlasAffinity affinity_;
    std::vector<uint8_t> pixels_;
    std::vector<Shelf> shelves_;
    bgfx::TextureHandle tex_{ BGFX_INVALID_HANDLE };
    AtlasPageStats stats_;
    std::vector<GlyphInfo> pendingUploads_;
};

//=============================================================================
// RenderGroup: �����P�ʁi�����Ǘ��̎�́j
//=============================================================================

inline RenderGroupFlags operator|(RenderGroupFlags a, RenderGroupFlags b) {
    return static_cast<RenderGroupFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
inline bool hasFlag(RenderGroupFlags f, RenderGroupFlags t) {
    return (static_cast<uint8_t>(f) & static_cast<uint8_t>(t)) != 0;
}
using RenderGroupId = uint32_t;
using SurfaceId = uint32_t;
constexpr RenderGroupId INVALID_RENDER_GROUP = 0;
struct RenderGroup {
    RenderGroupId id = INVALID_GROUP;
    SurfaceId surfaceId = 0;
    RenderGroupFlags flags = RenderGroupFlags::None;

    uint32_t bornFrame = 0;
    uint32_t lastUsedFrame = 0;

    std::unordered_set<GlyphKey, GlyphKeyHash> usedGlyphs;
    std::unordered_set<ImageId> usedImages;

    bool isPinned() const { return hasFlag(flags, RenderGroupFlags::Pinned); }
    bool isTransient() const { return hasFlag(flags, RenderGroupFlags::Transient); }

    void markUsed(uint32_t frame) { lastUsedFrame = frame; }
    void trackGlyph(const GlyphKey& key) { usedGlyphs.insert(key); }
    void trackImage(ImageId id) { usedImages.insert(id); }

    void clear() {
        usedGlyphs.clear();
        usedImages.clear();
    }

    PackHint getPackHint() const {
        AtlasAffinity a = AtlasAffinity::Default;
        if (isPinned()) a = AtlasAffinity::LongLived;
        else if (isTransient()) a = AtlasAffinity::Transient;
        return { a, id };
    }
};

// FontIdHash ��ǉ��iGlyphKeyHash �̋߂��ɔz�u�j
struct FontIdHash {
    size_t operator()(const FontId& id) const {
        return std::hash<uint64_t>{}(id.computeHash());
    }
};

//=============================================================================
// FontAtlas: �C���ŁifontIdToIndex_ �ǉ��j
//=============================================================================
class FontAtlas {
public:
    struct Config {
        int pageW = 2048;
        int pageH = 2048;
        int padding = 1;
        uint32_t expireFrames = 300;
        bool autoPinAscii = true;

        int dropDeadRatio = 4;
        int dropMinLive = 10;
    };

    FontAtlas() : config_() {
    }
    explicit FontAtlas(const Config & cfg) : config_(cfg) {
    }
    const GlyphInfo* findByImageId(ImageId id) const {
        std::lock_guard lock(mutex_);

        // imageMap_�iaddImage �Œǉ������摜�j
        auto it = imageMap_.find(id);
        if (it != imageMap_.end()) return &it->second;

        // �O���t
        auto it2 = imageIdToGlyph_.find(id);
        if (it2 != imageIdToGlyph_.end()) return it2->second;

        return nullptr;
    }
    void init() {
        addPage(AtlasAffinity::LongLived);
        addPage(AtlasAffinity::Default);
        addWhitePixel();
    }
    std::unordered_map<ImageId, GlyphInfo*> imageIdToGlyph_;
    //--- �t�H���g�Ǘ� ---
    FontId registerFont(const char* name, const std::string& font, int size) {
        std::lock_guard lock(mutex_);
        TTF_Font* f = nullptr;
#ifdef __ANDROID__
        // Android�ł̓A�Z�b�g����ǂݍ���
        SDL_IOStream* io = SDL_IOFromFile(font.c_str(), "rb");
        if (io) {
            f = TTF_OpenFontIO(io, true, (float)size);
            if (!f) {
                SDL_Log("registerFont: TTF_OpenFontIO failed for %s: %s", font.c_str(), SDL_GetError());
            }
            else {
                SDL_Log("registerFont: Loaded font %s size %d", font.c_str(), size);
            }
        }
        else {
            SDL_Log("registerFont: SDL_IOFromFile failed for %s: %s", font.c_str(), SDL_GetError());
        }
#elif defined(__APPLE__)
        // Apple (iOS/macOS)ではバンドルから読み込む
        std::string bundlePath = getBundlePath(font.c_str());
        SDL_Log("registerFont: Loading font from bundle: %s", bundlePath.c_str());
        f = TTF_OpenFont(bundlePath.c_str(), size);
        if (!f) {
            SDL_Log("registerFont: TTF_OpenFont failed for %s: %s", bundlePath.c_str(), SDL_GetError());
        }
        else {
            SDL_Log("registerFont: Loaded font %s size %d", bundlePath.c_str(), size);
        }
#else
        // Linux - getBundlePathを使用
        std::string bundlePath = getBundlePath(font.c_str());
        SDL_Log("registerFont: Loading font from: %s", bundlePath.c_str());
        f = TTF_OpenFont(bundlePath.c_str(), size);
        if (!f) {
            SDL_Log("registerFont: TTF_OpenFont failed for %s: %s", bundlePath.c_str(), SDL_GetError());
        }
        else {
            SDL_Log("registerFont: Loaded font %s size %d", bundlePath.c_str(), size);
        }
#endif
        FontId id = FontId(name, size);
        fonts_[id] = FontEntry{ f, name, {} };

        // �C���f�b�N�X���蓖��
        fontIdToIndex_[id] = nextFontIndex_++;

        return id;
    }

    // FontId �� �A�ԃC���f�b�N�X�擾�i�ǉ��j
    uint32_t getFontIndex(FontId font) const {
        std::lock_guard lock(mutex_);
        auto it = fontIdToIndex_.find(font);
        return (it != fontIdToIndex_.end()) ? it->second : 0;
    }

    void setFallbackChain(FontId primary, std::initializer_list<FontId> fallbacks) {
        std::lock_guard lock(mutex_);
        auto it = fonts_.find(primary);
        if (it != fonts_.end()) {
            it->second.fallbacks.clear();
            for (auto fb : fallbacks) it->second.fallbacks.push_back(fb);
        }
    }

    //--- �O���t�擾 ---
    const GlyphInfo& getOrAddGlyph(FontId font, uint32_t codepoint,
        RenderGroup* group = nullptr,
        SDL_Color color = { 255, 255, 255, 255 }) {
        GlyphKey key{ font, codepoint };
        {
            std::lock_guard lock(mutex_);
            if (auto it = glyphMap_.find(key); it != glyphMap_.end()) {
                if (group) group->trackGlyph(key);
                return it->second;
            }
        }
        PackHint hint = group ? group->getPackHint() : PackHint{};
        return addGlyph(font, codepoint, color, hint, group);
    }

    //--- �摜�ǉ� ---
    const GlyphInfo& addImage(ImageId id, SDL_Surface* surface,
        RenderGroup* group = nullptr, bool pinned = false) {
        std::lock_guard lock(mutex_);
        if (auto it = imageMap_.find(id); it != imageMap_.end()) {
            if (group) group->trackImage(id);
            return it->second;
        }

        SDL_Surface* s = surface;
        SDL_Surface* conv = nullptr;
        if (s->format != SDL_PIXELFORMAT_RGBA8888) {
            conv = SDL_ConvertSurface(s, SDL_PIXELFORMAT_RGBA8888);
            s = conv;
        }

        PackHint hint = group ? group->getPackHint() : PackHint{};
        if (pinned) hint.affinity = AtlasAffinity::LongLived;

        GlyphInfo gi = packWithHint(s->w, s->h, (const uint8_t*)s->pixels,
            s->pitch, true, hint);
        if (conv) SDL_DestroySurface(conv);

        if (gi.width == 0) {
            static GlyphInfo dummy{};
            return dummy;
        }

        if (pinned) gi.flags = gi.flags | GlyphFlags::Pinned;
        gi.lastUsedFrame = currentFrame_;
        gi.imageId = id;  // �ǉ�
        if (group) group->trackImage(id);

        auto [it, _] = imageMap_.emplace(id, gi);
        return it->second;
    }

    const GlyphInfo* getImage(ImageId id) const {
        std::lock_guard lock(mutex_);
        auto it = imageMap_.find(id);
        return (it != imageMap_.end()) ? &it->second : nullptr;
    }

    const GlyphInfo& getWhitePixel() const {
        auto it = imageMap_.find(WHITE_PIXEL_ID);
        return it->second;
    }

    void onGroupDestroyed(RenderGroup& group) {
        std::lock_guard lock(mutex_);

        for (const auto& key : group.usedGlyphs) {
            auto it = glyphMap_.find(key);
            if (it != glyphMap_.end()) {
                const auto& gi = it->second;
                if (gi.pageIndex < pages_.size()) {
                    pages_[gi.pageIndex].stats().decrementLive();
                }
            }
        }

        for (ImageId id : group.usedImages) {
            if (id == WHITE_PIXEL_ID) continue;
            auto it = imageMap_.find(id);
            if (it != imageMap_.end()) {
                const auto& gi = it->second;
                if (gi.pageIndex < pages_.size()) {
                    pages_[gi.pageIndex].stats().decrementLive();
                }
            }
        }
    }

    void beginFrame() {
        std::vector<std::pair<uint16_t, std::vector<GlyphInfo>>> uploads;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ++currentFrame_;
            for (uint16_t i = 0; i < pages_.size(); ++i) {
                auto pending = pages_[i].takePendingUploads();
                if (!pending.empty()) {
                    uploads.emplace_back(i, std::move(pending));
                }
            }
        }

        for (auto& [pageIdx, glyphs] : uploads) {
            for (const auto& gi : glyphs) {
                pages_[pageIdx].uploadFromCPU(gi);
            }
        }
    }

    bool shouldDropPage(uint16_t pageIndex) const {
        std::lock_guard lock(mutex_);
        if (pageIndex >= pages_.size()) return false;

        const auto& page = pages_[pageIndex];
        const auto& s = page.stats();

        if (page.affinity() == AtlasAffinity::LongLived) return false;
        if (pageIndex <= 1) return false;

        return s.deadCount > s.liveCount * config_.dropDeadRatio &&
            s.liveCount < config_.dropMinLive;
    }

    void collectGarbage() {
        std::lock_guard lock(mutex_);

        while (pages_.size() > 2) {
            uint16_t lastIdx = (uint16_t)(pages_.size() - 1);
            if (!shouldDropPageInternal(lastIdx)) break;

            std::erase_if(glyphMap_, [lastIdx](const auto& kv) {
                return kv.second.pageIndex == lastIdx;
                });
            std::erase_if(imageMap_, [lastIdx](const auto& kv) {
                return kv.first != WHITE_PIXEL_ID &&
                    kv.second.pageIndex == lastIdx;
                });

            pages_.pop_back();
        }
    }

    bgfx::TextureHandle& getPageTexture(uint16_t idx) {
        std::lock_guard lock(mutex_);
        if (idx < pages_.size()) return pages_[idx].handle();
        return nulltex;
    }

    size_t pageCount() const {
        std::lock_guard lock(mutex_);
        return pages_.size();
    }

    AtlasAffinity getPageAffinity(uint16_t idx) const {
        std::lock_guard lock(mutex_);
        return (idx < pages_.size()) ? pages_[idx].affinity() : AtlasAffinity::Default;
    }

    const AtlasPageStats* getPageStats(uint16_t idx) const {
        std::lock_guard lock(mutex_);
        return (idx < pages_.size()) ? &pages_[idx].stats() : nullptr;
    }

    uint32_t currentFrame() const { return currentFrame_; }

    struct Stats {
        size_t pageCount;
        size_t glyphCount;
        size_t imageCount;
        float avgUtilization;
    };

    Stats getStats() const {
        std::lock_guard lock(mutex_);
        Stats s{};
        s.pageCount = pages_.size();
        s.glyphCount = glyphMap_.size();
        s.imageCount = imageMap_.size();
        float totalUtil = 0;
        for (const auto& p : pages_) totalUtil += p.stats().utilization();
        s.avgUtilization = pages_.empty() ? 0 : totalUtil / pages_.size();
        return s;
    }

    struct FontEntry {
        TTF_Font* font;
        std::string name;
        std::vector<FontId> fallbacks;
    };

    void addPage(AtlasAffinity affinity) {
        pages_.emplace_back(config_.pageW, config_.pageH,
            config_.padding, (uint16_t)pages_.size(), affinity);
    }

    void addWhitePixel() {
        const int size = 1 + config_.padding * 2;
        std::vector<uint8_t> white((size_t)size * size * 4, 255);
        GlyphInfo gi = pages_[0].tryPack(size, size, white.data(),
            size * 4, true, INVALID_GROUP);
        gi.atlasX += config_.padding;
        gi.atlasY += config_.padding;
        gi.width = 1;
        gi.height = 1;
        gi.u0 = float(gi.atlasX) / float(config_.pageW);
        gi.v0 = float(gi.atlasY) / float(config_.pageH);
        gi.u1 = float(gi.atlasX + 1) / float(config_.pageW);
        gi.v1 = float(gi.atlasY + 1) / float(config_.pageH);
        gi.flags = GlyphFlags::Pinned;
        gi.imageId = WHITE_PIXEL_ID;  // �ǉ�
        imageMap_[WHITE_PIXEL_ID] = gi;
    }

    bool shouldDropPageInternal(uint16_t idx) const {
        if (idx >= pages_.size()) return false;
        const auto& page = pages_[idx];
        const auto& s = page.stats();
        if (page.affinity() == AtlasAffinity::LongLived) return false;
        if (idx <= 1) return false;
        return s.deadCount > s.liveCount * config_.dropDeadRatio &&
            s.liveCount < config_.dropMinLive;
    }

    GlyphInfo packWithHint(int w, int h, const uint8_t* px, int pitch,
        bool imm, const PackHint& hint) {
        RenderGroupId gid = hint.groupId;

        for (auto& page : pages_) {
            if (page.matchesHint(hint)) {
                GlyphInfo gi = page.tryPack(w, h, px, pitch, imm, gid);
                if (gi.width > 0) return gi;
            }
        }

        for (auto& page : pages_) {
            if (page.affinity() == hint.affinity) {
                GlyphInfo gi = page.tryPack(w, h, px, pitch, imm, gid);
                if (gi.width > 0) return gi;
            }
        }

        addPage(hint.affinity);
        return pages_.back().tryPack(w, h, px, pitch, imm, gid);
    }

    const GlyphInfo& addGlyph(FontId font, uint32_t cp, SDL_Color color,
        const PackHint& hint, RenderGroup* group) {
        std::lock_guard lock(mutex_);
        GlyphKey key{ font, cp };

        std::vector<FontId> chain = { font };
        auto fontIt = fonts_.find(font);
        if (fontIt != fonts_.end()) {
            chain.insert(chain.end(),
                fontIt->second.fallbacks.begin(),
                fontIt->second.fallbacks.end());
        }

        TTF_Font* ttf = nullptr;
        for (FontId fid : chain) {
            auto it = fonts_.find(fid);
            if (it == fonts_.end()) continue;
            if (TTF_FontHasGlyph(it->second.font, cp)) {
                ttf = it->second.font;
                break;
            }
        }
        if (!ttf && fontIt != fonts_.end()) {
            ttf = fontIt->second.font;
        }
        if (!ttf) {
            static GlyphInfo dummy{};
            return dummy;
        }

        int minx, maxx, miny, maxy, advance;
        if (TTF_GetGlyphMetrics(ttf, cp, &minx, &maxx, &miny, &maxy, &advance) == 0) {
            static GlyphInfo dummy{};
            return dummy;
        }

        const int gw = std::max(0, maxx - minx);
        const int gh = std::max(0, maxy - miny);

        if (gw == 0 || gh == 0) {
            GlyphInfo gi{};
            gi.advance = advance;
            gi.bearingX = minx;
            gi.bearingY = maxy;
            gi.lastUsedFrame = currentFrame_;
            // ���[���ł� ImageId ��ݒ�
            uint32_t fontIndex = fontIdToIndex_[font];
            uint64_t localId = (uint64_t(fontIndex) << 24) | (cp & 0x00FFFFFF);
            gi.imageId = ImageIdGenerator::forFontGlyph(localId);
            glyphMap_[key] = gi;
            if (group) group->trackGlyph(key);
            return glyphMap_[key];
        }

        SDL_Surface* s = TTF_RenderGlyph_Blended(ttf, cp, color);
        if (!s) {
            static GlyphInfo dummy{};
            return dummy;
        }
        if (s->format != SDL_PIXELFORMAT_RGBA32) {
            SDL_Surface* conv = SDL_ConvertSurface(s, SDL_PIXELFORMAT_RGBA32);
            SDL_DestroySurface(s);
            s = conv;
        }

        GlyphInfo gi = packWithHint(s->w, s->h, (const uint8_t*)s->pixels,
            s->pitch, false, hint);
        SDL_DestroySurface(s);

        if (gi.width == 0) {
            static GlyphInfo dummy{};
            return dummy;
        }

        gi.advance = advance;
        gi.bearingX = minx;
        gi.bearingY = 0;
        gi.lastUsedFrame = currentFrame_;

        // ImageId ��ݒ�
        uint32_t fontIndex = fontIdToIndex_[font];
        uint64_t localId = (uint64_t(fontIndex) << 24) | (cp & 0x00FFFFFF);
        gi.imageId = ImageIdGenerator::forFontGlyph(localId);

        if (config_.autoPinAscii && cp >= 0x20 && cp <= 0x7E) {
            gi.flags = gi.flags | GlyphFlags::Pinned;
        }

        auto [it, _] = glyphMap_.emplace(key, gi);
        imageIdToGlyph_[it->second.imageId] = &it->second;
        if (group) group->trackGlyph(key);
        return it->second;
    }

    Config config_;
    uint32_t currentFrame_ = 0;

    mutable std::mutex mutex_;
    std::vector<FontAtlasPage> pages_;
    std::unordered_map<FontId, FontEntry, FontIdHash> fonts_;
    std::unordered_map<FontId, uint32_t, FontIdHash> fontIdToIndex_;  // �ǉ�
    uint32_t nextFontIndex_ = 1;  // �ǉ�: 0�͖����l
    std::unordered_map<GlyphKey, GlyphInfo, GlyphKeyHash> glyphMap_;
    std::unordered_map<ImageId, GlyphInfo> imageMap_;

    bgfx::TextureHandle nulltex = BGFX_INVALID_HANDLE;
};

class RenderGroupManager {
public:
    explicit RenderGroupManager(FontAtlas& atlas) : atlas_(atlas) {}

    RenderGroup& createGroup(SurfaceId surfaceId,
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

    RenderGroup* getGroup(RenderGroupId id) {
        auto it = groups_.find(id);
        return (it != groups_.end()) ? &it->second : nullptr;
    }

    void destroyGroup(RenderGroupId id) {
        auto it = groups_.find(id);
        if (it == groups_.end()) return;

        // Atlas��dead�ʒm
        atlas_.onGroupDestroyed(it->second);

        // Surface���X�g����폜
        auto& sv = surfaceGroups_[it->second.surfaceId];
        sv.erase(std::remove(sv.begin(), sv.end(), id), sv.end());

        groups_.erase(it);
    }

    void destroySurfaceGroups(SurfaceId surfaceId) {
        auto it = surfaceGroups_.find(surfaceId);
        if (it == surfaceGroups_.end()) return;

        for (RenderGroupId gid : it->second) {
            auto git = groups_.find(gid);
            if (git != groups_.end()) {
                atlas_.onGroupDestroyed(git->second);
                groups_.erase(git);
            }
        }
        surfaceGroups_.erase(it);
    }

    void collectExpiredGroups(uint32_t expireFrames) {
        std::vector<RenderGroupId> expired;
        for (auto& [id, g] : groups_) {
            if (g.isPinned()) continue;
            if (currentFrame_ - g.lastUsedFrame > expireFrames) {
                expired.push_back(id);
            }
        }
        for (RenderGroupId id : expired) {
            destroyGroup(id);
        }
    }

    void beginFrame() { ++currentFrame_; }
    uint32_t currentFrame() const { return currentFrame_; }

private:
    FontAtlas& atlas_;
    RenderGroupId nextId_ = 1;
    uint32_t currentFrame_ = 0;
    std::unordered_map<RenderGroupId, RenderGroup> groups_;
    std::unordered_map<SurfaceId, std::vector<RenderGroupId>> surfaceGroups_;
}; using StandaloneTextureId = uint64_t;

