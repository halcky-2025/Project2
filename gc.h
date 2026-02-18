
#if defined(__ANDROID__) || defined(__APPLE__)
#include <coroutine>
namespace coro = std;
#else
#include <coro/coro.hpp>
#endif

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <variant>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <functional>
// =============================================================================
// 定数
// =============================================================================
constexpr int MAX_CLASSES = 256;
constexpr int MAX_ROOTS_PER_NODE = 64;
constexpr int PROMOTION_THRESHOLD = 3;
struct ThreadGC;
class Transaction;
struct RootNode {
    ThreadGC* gc;
    int rn;
    Transaction* tx;
    char*** roots;
    RootNode* next;
    RootNode* prev;
    int count;
    int capacity;
};
struct ClassType;
class CoroutineQueue;
struct Frame;
// Generator の定義
class Generator {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    Generator(handle_type h) : coro(h) {}
    Generator(const Generator&) = delete;
    Generator(Generator&& other) noexcept : coro(other.coro) {
        other.coro = nullptr;
    }
    ~Generator() {
        if (coro) coro.destroy();
    }

    // ジェネレーターが await 可能になるために必要な 3つ
    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) {
        coro.promise().parent = h;  // 親のハンドルを保存
        coro.promise().queue = h.promise().queue;
        return coro;  // 子のコルーチンを実行
    }

    char* await_resume() noexcept {
        return coro.promise().current_value;
    }

    bool next() {
        if (!coro.done()) coro.resume();
        return !coro.done();
    }

    char* value() const {
        return coro.promise().current_value;
    }

    handle_type coro;

public:
    struct promise_type {
        void (*func)(Frame*);
        RootNode* root;
        CoroutineQueue* queue = nullptr;
        char* blockobj;
        std::coroutine_handle<Generator::promise_type> parent;
        int state;
        char* current_value;
        char* ret_value;
        int statefin;
		std::suspend_always yield_value(char* val) {
            current_value = val;
            return {};
        }

        void return_value(char* val) {
            current_value = val;
            if (parent != NULL) parent.promise().current_value = val;
        }

        std::suspend_always initial_suspend() { return {}; }

        // final_suspendの実装は後で定義（CoroutineQueueが完全型になってから）
        auto final_suspend() noexcept;

        void unhandled_exception() {
            auto eptr = std::current_exception();
            try {
                std::rethrow_exception(eptr); // 再スロー
            }
            catch (const std::exception& e) {
                std::cerr << "Exception caught in unhandled_exception: " << e.what() << "\n";
            }
            catch (...) {
                std::cerr << "Unknown exception caught in unhandled_exception\n";
            }
            current_value = 0;
            state = -5;
        }

        Generator get_return_object() {
            return Generator{ handle_type::from_promise(*this) };
        }
    };
};

// CoroutineQueue の完全定義
class CoroutineQueue {
public:
    std::queue<std::coroutine_handle<Generator::promise_type>> q;
    std::mutex m;

    void push(std::coroutine_handle<Generator::promise_type> h) {
        std::lock_guard lock(m);
        q.push(h);
    }

    void resume_all() {
        std::queue<std::coroutine_handle<Generator::promise_type>> local;
        {
            std::lock_guard lock(m);
            std::swap(local, q);
        }
        while (!local.empty()) {
            auto h = local.front();
            local.pop();
            if (h.promise().state == -1) {
                // 完了済みのコルーチンはスキップ
            }
            else {
                h.resume();
            }
        }
    }
};

struct Frame {
    void (*resume)(Frame*);
    void (*destory)(Frame*);
    Generator::promise_type promise;
};
void ResumeFrame(Frame* frame) {
    frame->promise.func(frame);
}
void DestroyFrame(Frame* frame) {
    free(frame);
}

Frame* MakeFrame(RootNode* root, void (*func)(Frame*), char* blockobj) {
    Frame* frame = (Frame*)malloc(sizeof(Frame));
    frame->resume = ResumeFrame;
    frame->destory = DestroyFrame;
    frame->promise.func = func;
    frame->promise.root = root;
    frame->promise.blockobj = blockobj;
    frame->promise.state = 0;
    frame->promise.parent = NULL;
    frame->promise.current_value = nullptr;
    frame->promise.ret_value = nullptr;
    frame->promise.statefin = -1;
    return frame;
}
struct FuncType {
    char* blk;
    char* (*drawcall)(RootNode*, char*);
	char* (*call)(RootNode*, char*);
    char* obj;
};
// final_suspendの実装（CoroutineQueueが完全型になってから）
inline auto Generator::promise_type::final_suspend() noexcept {
    struct awaiter {
        CoroutineQueue* q;
        std::coroutine_handle<Generator::promise_type> parent;

        bool await_ready() noexcept { return false; }        // ← 最終停止で“止まる”
        void await_suspend(std::coroutine_handle<>) const noexcept {
            if (q && parent) q->push(parent);                // ← 親をキューへ
        }
        void await_resume() noexcept {}
    };

    state = -1;
    return awaiter{ queue, parent };
}
class RenderCommandQueue; 
RenderCommandQueue* createRCQ();
class HopStar;
class SqlPool;
typedef struct Map Map;
typedef struct String String;
CoroutineQueue* queuefirst(ThreadGC* thgc);
void push_to_queue(CoroutineQueue* queue, char* raw) {
    queue->push(std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(raw)));  // グローバル or TLS な CoroutineQueue を使う
}
char* wait_handle(ThreadGC* thgc, char* raw) {
    auto h2 = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(raw));
	auto queue = new CoroutineQueue();
	queue->push(h2);  // グローバル or TLS な CoroutineQueue を使う
    h2.promise().queue = queue;
    for (;;) {
        if (queuefirst(thgc) == NULL) break;
        else queuefirst(thgc)->resume_all();
    }
    for (;;) {
        std::queue<std::coroutine_handle<Generator::promise_type>> local;

        {
            std::lock_guard lock(queue->m);
            std::swap(local, queue->q);  // 🔄 ロック中に中身を移す
        }

        while (!local.empty()) {
            auto h = local.front();
            local.pop();
            h.resume();
            if (h == h2 && h.promise().state == -1) {
                return  h.promise().ret_value;
            }
        }
    }
}
enum CType {
	_Struct,
	_List, _String, _Element, _FRect, _LetterC, _Detection, _LocalC, _Select, _State, _LineC, _VLineC, _LetterPart,
    _MapData, _Map, _KV, _EndC, _CloneElemC, _HoppyWindow, _ColumnMeta, _Table, _Column, _TreeElement, _DivC, _SheetC,
    _MemTable, _MouseEvent, _KeyEvent, _MemFunc, _FuncType, _Offscreen,
    _ATSSpan, _StyleSpan, _RenderSpan,
    _CObj, _Block, _CallBlock, _TagBlock,
    _Primary, _COperator, _Word, _CNumber,
    _FloatVal, _StrObj, _Address, _Comment,
    _Comment2, _HtmObj, _CMountain, _CQuestion,
    _CDolor, _PrimOp, _SingleOp, _Label,
    _Local, _Operator,
    _VariClass, _ArrType, _FuncType2, _Variable, _Function, _GenericFunction,
    _ClassObj, _ModelObj, _GeneObj, _GenericObj, _ObjBlock, _Generic
};
// =============================================================================
// クラス定義
// =============================================================================
using GCCheckFunc = void (*)(ThreadGC*, char*);
using GCFinalizeFunc = void (*)(ThreadGC*, char*);
struct ClassType {
    String* name;
    uint32_t size;
    GCCheckFunc gc_check;       // 子オブジェクトのスキャン
    GCFinalizeFunc gc_finalize; // ファイナライザ
    bool has_io;                // I/Oリソースを持つか（trueならOld直行）
};
ThreadGC* magc = NULL;
// gc.hpp
// 世代別GC - リアルタイムアプリケーション向け
// Young (Eden + Survivor0 + Survivor1) + Old + Finalizer Queue





// =============================================================================
// オブジェクトヘッダ
// =============================================================================
enum class Generation : uint8_t {
    Young = 0,
    Old = 1
};

struct Object {
    uint32_t type;              // クラスID (65536以上はサイズ埋め込み)
    uint8_t survival_count;     // 生存回数（昇格判定用）
    uint8_t generation;         // 0=Young, 1=Old
    uint8_t forwarded;          // GC世代カウンタと一致なら転送済み
    uint8_t flags;              // 予約
    char* fwd;                  // 転送先ポインタ
};

static_assert(sizeof(Object) == 16, "Object header should be 16 bytes");







// =============================================================================
// Finalizerキュー（非同期実行用）
// =============================================================================
class FinalizerQueue {
public:
    struct FinalizerTask {
        GCFinalizeFunc func;
        char* data;             // オブジェクトデータのコピー
        uint32_t size;
    };

    FinalizerQueue() : running_(true) {
        worker_ = std::thread([this]() { worker_loop(); });
    }

    ~FinalizerQueue() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_one();
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    void push(GCFinalizeFunc func, char* data, uint32_t size) {
        // データをコピー（元オブジェクトはGCで消える可能性）
        char* copy = (char*)malloc(size);
        memcpy(copy, data, size);

        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push({ func, copy, size });
        }
        cv_.notify_one();
    }

    void flush() {
        // 全Finalizerを同期実行（シャットダウン時用）
        std::unique_lock<std::mutex> lock(mutex_);
        while (!queue_.empty()) {
            auto task = queue_.front();
            queue_.pop();
            lock.unlock();

            task.func(nullptr, task.data);
            free(task.data);

            lock.lock();
        }
    }

private:
    void worker_loop() {
        while (true) {
            FinalizerTask task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() { return !queue_.empty() || !running_; });

                if (!running_ && queue_.empty()) return;

                task = queue_.front();
                queue_.pop();
            }

            // GC外で実行（I/O可能）
            task.func(nullptr, task.data);
            free(task.data);
        }
    }

    std::queue<FinalizerTask> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread worker_;
    bool running_;
};

// =============================================================================
// Young世代ヒープ（Eden + Survivor0 + Survivor1）
// =============================================================================
struct YoungHeap {
    char* eden;
    char* survivor0;
    char* survivor1;

    char* eden_ptr;         // Eden割り当てポインタ
    char* survivor_ptr;     // アクティブSurvivorの割り当てポインタ

    size_t eden_size;
    size_t survivor_size;

    int active_survivor;    // 0 or 1

    void init(size_t total_size) {
        // 50% Eden, 25% Survivor0, 25% Survivor1
        eden_size = total_size / 2;
        survivor_size = total_size / 4;

        eden = (char*)malloc(eden_size);
        survivor0 = (char*)malloc(survivor_size);
        survivor1 = (char*)malloc(survivor_size);

        eden_ptr = eden;
        survivor_ptr = survivor0;
        active_survivor = 0;
    }

    void destroy() {
        free(eden);
        free(survivor0);
        free(survivor1);
    }

    char* active_from() const {
        return active_survivor == 0 ? survivor0 : survivor1;
    }

    char* active_to() const {
        return active_survivor == 0 ? survivor1 : survivor0;
    }

    void swap_survivors() {
        active_survivor = 1 - active_survivor;
        survivor_ptr = active_to();
    }

    bool in_eden(char* ptr) const {
        return ptr >= eden && ptr < eden + eden_size;
    }

    bool in_survivor(char* ptr) const {
        char* from = active_from();
        return ptr >= from && ptr < from + survivor_size;
    }

    bool in_young(char* ptr) const {
        return in_eden(ptr) || in_survivor(ptr);
    }
};

// =============================================================================
// Old世代ヒープ（移動なし、リンクリスト管理）
// =============================================================================
struct OldBlock {
    OldBlock* next;
    OldBlock* prev;
    bool marked;
    // Object + data follows
};

struct OldHeap {
    OldBlock sentinel;      // ダミーヘッド
    size_t total_size;
    size_t used_size;

    void init() {
        sentinel.next = &sentinel;
        sentinel.prev = &sentinel;
        total_size = 0;
        used_size = 0;
    }

    void destroy() {
        OldBlock* block = sentinel.next;
        while (block != &sentinel) {
            OldBlock* next = block->next;
            free(block);
            block = next;
        }
    }

    char* alloc(uint32_t type, uint32_t size, uint8_t gc_epoch) {
        size_t block_size = sizeof(OldBlock) + sizeof(Object) + size;
        OldBlock* block = (OldBlock*)malloc(block_size);
        if (!block) return nullptr;

        // リンクリストに追加
        block->next = sentinel.next;
        block->prev = &sentinel;
        sentinel.next->prev = block;
        sentinel.next = block;
        block->marked = false;

        Object* obj = (Object*)(block + 1);
        obj->type = type;
        obj->survival_count = PROMOTION_THRESHOLD;  // 昇格済み
        obj->generation = (uint8_t)Generation::Old;
        obj->forwarded = gc_epoch;
        obj->flags = 0;
        obj->fwd = nullptr;

        char* data = (char*)(obj + 1);
        memset(data, 0, size);

        used_size += block_size;
        return data;
    }

    void sweep(ThreadGC* gc, FinalizerQueue* finalizer_queue);
};


struct ThreadGC {
    CoroutineQueue* queue;
    RootNode root_sentinel;
    CoroutineQueue* first;
    YoungHeap young;
    OldHeap old;
    ClassType class_table[MAX_CLASSES];
    int class_count;
    FinalizerQueue* finalizer_queue;
    uint8_t gc_epoch;
    uint64_t young_gc_count;
    uint64_t old_gc_count;
    uint64_t promoted_count;
    std::mutex mutex;
    RenderCommandQueue* commandQueue;
    HopStar* hoppy;
    bool waitgc = false;
    Map* map;
    int staticid = -1;
};

// =============================================================================
// 初期化・終了
// =============================================================================
inline ThreadGC* GC_init(size_t young_size) {
    ThreadGC* gc = new ThreadGC();
	gc->queue = new CoroutineQueue();
    gc->young.init(young_size);
    gc->old.init();

    gc->class_count = 32;
    gc->first = NULL;
    gc->commandQueue = createRCQ();

    // ルートセンチネル
    gc->root_sentinel.gc = gc;
    gc->root_sentinel.next = &gc->root_sentinel;
    gc->root_sentinel.prev = &gc->root_sentinel;
    gc->root_sentinel.roots = nullptr;
    gc->root_sentinel.count = 0;
    gc->root_sentinel.capacity = MAX_ROOTS_PER_NODE;

    gc->finalizer_queue = new FinalizerQueue();
    gc->gc_epoch = 0;

    gc->young_gc_count = 0;
    gc->old_gc_count = 0;
    gc->promoted_count = 0;

    return gc;
}

inline void GC_shutdown(ThreadGC* gc) {
    // Finalizerをフラッシュ
    gc->finalizer_queue->flush();
    delete gc->finalizer_queue;

    // ヒープ解放
    gc->young.destroy();
    gc->old.destroy();

    // ルート解放
    RootNode* node = gc->root_sentinel.next;
    while (node != &gc->root_sentinel) {
        RootNode* next = node->next;
        free(node->roots);
        free(node);
        node = next;
    }

    delete gc;
}

// =============================================================================
// クラス登録
// =============================================================================
String* createString(ThreadGC* thgc, char* tex, int size, int esize);
inline int GC_register_class(ThreadGC* gc, int type, const char* name, int size,
    GCCheckFunc check, GCFinalizeFunc finalize,
    bool has_io = false) {
    gc->class_table[type].size = size;
    gc->class_table[type].gc_check = check;
    gc->class_table[type].gc_finalize = finalize;
    gc->class_table[type].has_io = has_io;
	gc->class_table[type].name = createString(gc, (char*)name, sizeof(name), 1);
    return type;
}

// =============================================================================
// ルート管理
// =============================================================================
inline RootNode* GC_add_root_node(ThreadGC* gc) {
    RootNode* node = (RootNode*)malloc(sizeof(RootNode));
    node->gc = gc;
    node->roots = (char***)malloc(sizeof(char**) * MAX_ROOTS_PER_NODE);
    node->count = 0;
    node->capacity = MAX_ROOTS_PER_NODE;

    // リンクリストに追加
    node->next = gc->root_sentinel.next;
    node->prev = &gc->root_sentinel;
    gc->root_sentinel.next->prev = node;
    gc->root_sentinel.next = node;

    return node;
}

inline void GC_release_root_node(RootNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node->roots);
    free(node);
}

inline void GC_add_root(RootNode* node, char** root) {
    if (node->count < node->capacity) {
        node->roots[node->count++] = root;
    }
}

inline void GC_pop_roots(RootNode* node, int n) {
    node->count -= n;
    if (node->count < 0) node->count = 0;
}

// =============================================================================
// オブジェクトコピー（Young GC用）
// =============================================================================
inline char* copy_object_young(ThreadGC* gc, char* value) {
    if (value == nullptr) return nullptr;

    Object* obj = (Object*)(value - sizeof(Object));

    // Old世代は移動しない
    if (obj->generation == (uint8_t)Generation::Old) {
        return value;
    }

    // 既に転送済み
    if (obj->forwarded == gc->gc_epoch && obj->fwd != nullptr) {
        return obj->fwd;
    }

    // サイズ取得
    uint32_t size;
    ClassType* cls = nullptr;
    if (obj->type >= 65536) {
        size = obj->type - 65536;
    }
    else {
        cls = &gc->class_table[obj->type];
        size = cls->size;
    }

    // 生存回数インクリメント
    obj->survival_count++;

    // 昇格判定
    if (obj->survival_count >= PROMOTION_THRESHOLD) {
        // Oldへ昇格
        char* new_data = gc->old.alloc(obj->type, size, gc->gc_epoch);
        memcpy(new_data, value, size);

        obj->forwarded = gc->gc_epoch;
        obj->fwd = new_data;

        gc->promoted_count++;

        // 子オブジェクトをスキャン（新しい場所で）
        if (cls && cls->gc_check) {
            cls->gc_check(gc, new_data);
        }

        return new_data;
    }

    // Survivorにコピー
    char* to_space = gc->young.active_to();

    // オーバーフローチェック
    if (gc->young.survivor_ptr + sizeof(Object) + size > to_space + gc->young.survivor_size) {
        // Survivorが溢れたらOldへ強制昇格
        char* new_data = gc->old.alloc(obj->type, size, gc->gc_epoch);
        memcpy(new_data, value, size);

        obj->forwarded = gc->gc_epoch;
        obj->fwd = new_data;

        gc->promoted_count++;

        if (cls && cls->gc_check) {
            cls->gc_check(gc, new_data);
        }

        return new_data;
    }

    // Survivorにコピー
    Object* new_obj = (Object*)gc->young.survivor_ptr;
    gc->young.survivor_ptr += sizeof(Object) + size;

    memcpy(new_obj, obj, sizeof(Object) + size);
    new_obj->generation = (uint8_t)Generation::Young;
    new_obj->forwarded = gc->gc_epoch;
    new_obj->fwd = nullptr;

    char* new_data = (char*)(new_obj + 1);

    obj->forwarded = gc->gc_epoch;
    obj->fwd = new_data;

    // 子オブジェクトをスキャン
    if (cls && cls->gc_check) {
        cls->gc_check(gc, new_data);
    }

    return new_data;
}

// =============================================================================
// Young GC
// =============================================================================
inline void gc_young(ThreadGC* gc) {
    gc->gc_epoch = (gc->gc_epoch + 1) % 256;

    // To-Survivorをリセット
    gc->young.survivor_ptr = gc->young.active_to();

    // ルートからスキャン
    RootNode* node = gc->root_sentinel.next;
    while (node != &gc->root_sentinel) {
        for (int i = 0; i < node->count; i++) {
            if (node->roots[i] && *node->roots[i]) {
                *node->roots[i] = copy_object_young(gc, *node->roots[i]);
            }
        }
        node = node->next;
    }

    // Edenをクリア
    gc->young.eden_ptr = gc->young.eden;

    // Survivorをスワップ
    gc->young.swap_survivors();

    gc->young_gc_count++;
}

// =============================================================================
// Old GC（Mark-Sweep）
// =============================================================================
inline void mark_object(ThreadGC* gc, char* value) {
    if (value == nullptr) return;

    Object* obj = (Object*)(value - sizeof(Object));

    // Young世代はスキップ（Young GCで処理済み）
    if (obj->generation == (uint8_t)Generation::Young) {
        return;
    }

    // Old世代のマーク
    OldBlock* block = (OldBlock*)((char*)obj - sizeof(OldBlock));
    if (block->marked) return;  // 既にマーク済み

    block->marked = true;

    // 子オブジェクトをスキャン
    if (obj->type < 65536) {
        ClassType* cls = &gc->class_table[obj->type];
        if (cls->gc_check) {
            cls->gc_check(gc, value);
        }
    }
}

inline void OldHeap::sweep(ThreadGC* gc, FinalizerQueue* finalizer_queue) {
    OldBlock* block = sentinel.next;
    while (block != &sentinel) {
        OldBlock* next = block->next;

        if (!block->marked) {
            // 未マーク = 回収対象
            Object* obj = (Object*)(block + 1);
            char* data = (char*)(obj + 1);

            // Finalizerをキューに積む（非同期実行）
            if (obj->type < 65536) {
                ClassType* cls = &gc->class_table[obj->type];
                if (cls->gc_finalize) {
                    uint32_t size = cls->size;
                    finalizer_queue->push(cls->gc_finalize, data, size);
                }
            }

            // リンクリストから削除
            block->prev->next = block->next;
            block->next->prev = block->prev;

            // ブロック解放
            free(block);
        }
        else {
            // マークをクリア（次回GC用）
            block->marked = false;
        }

        block = next;
    }
}

inline void gc_old(ThreadGC* gc) {
    // まずYoung GC
    gc_young(gc);

    // Mark phase
    RootNode* node = gc->root_sentinel.next;
    while (node != &gc->root_sentinel) {
        for (int i = 0; i < node->count; i++) {
            if (node->roots[i] && *node->roots[i]) {
                mark_object(gc, *node->roots[i]);
            }
        }
        node = node->next;
    }

    // Sweep phase
    gc->old.sweep(gc, gc->finalizer_queue);

    gc->old_gc_count++;
}

// =============================================================================
// メモリ割り当て
// =============================================================================

// Young世代に割り当て（通常のオブジェクト）
inline char* GC_alloc(ThreadGC* gc, int type) {
    std::lock_guard<std::mutex> lock(gc->mutex);

    ClassType* cls = &gc->class_table[type];

    // I/Oリソースを持つクラスはOld直行
    if (cls->has_io) {
        return gc->old.alloc(type, cls->size, gc->gc_epoch);
    }

    uint32_t size = cls->size;

    // Edenに空きがあるか
    if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
        gc_young(gc);

        // それでも足りなければOld GC
        if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
            gc_old(gc);
        }
    }

    Object* obj = (Object*)gc->young.eden_ptr;
    gc->young.eden_ptr += sizeof(Object) + size;

    obj->type = type;
    obj->survival_count = 0;
    obj->generation = (uint8_t)Generation::Young;
    obj->forwarded = gc->gc_epoch;
    obj->flags = 0;
    obj->fwd = nullptr;

    char* data = (char*)(obj + 1);
    memset(data, 0, size);

    return data;
}

// 可変サイズ割り当て（Young）
inline char* GC_alloc_size(ThreadGC* gc, uint32_t size) {
    std::lock_guard<std::mutex> lock(gc->mutex);

    if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
        gc_young(gc);

        if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
            gc_old(gc);
        }
    }

    Object* obj = (Object*)gc->young.eden_ptr;
    gc->young.eden_ptr += sizeof(Object) + size;

    obj->type = 65536 + size;  // サイズ埋め込み
    obj->survival_count = 0;
    obj->generation = (uint8_t)Generation::Young;
    obj->forwarded = gc->gc_epoch;
    obj->flags = 0;
    obj->fwd = nullptr;

    char* data = (char*)(obj + 1);
    memset(data, 0, size);

    return data;
}

// Old世代に直接割り当て（I/Oリソース用）
inline char* GC_alloc_old(ThreadGC* gc, int type) {
    std::lock_guard<std::mutex> lock(gc->mutex);

    ClassType* cls = &gc->class_table[type];
    return gc->old.alloc(type, cls->size, gc->gc_epoch);
}

// Old世代に可変サイズ割り当て
inline char* GC_alloc_old_size(ThreadGC* gc, uint32_t size) {
    std::lock_guard<std::mutex> lock(gc->mutex);
    return gc->old.alloc(65536 + size, size, gc->gc_epoch);
}
// オブジェクトを複製（Young世代に作成）
inline char* GC_clone(ThreadGC* gc, char* value) {
    if (value == nullptr) return nullptr;

    std::lock_guard<std::mutex> lock(gc->mutex);

    Object* obj = (Object*)(value - sizeof(Object));

    uint32_t size;
    ClassType* cls = nullptr;
    if (obj->type >= 65536) {
        size = obj->type - 65536;
    }
    else {
        cls = &gc->class_table[obj->type];
        size = cls->size;
    }

    // I/Oリソースを持つならOldに複製
    if (cls && cls->has_io) {
        char* new_data = gc->old.alloc(obj->type, size, gc->gc_epoch);
        if (new_data) {
            memcpy(new_data, value, size);
        }
        return new_data;
    }

    // Edenに空きがあるか
    if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
        gc_young(gc);
    }

    // まだ足りなければOldに作成
    if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
        char* new_data = gc->old.alloc(obj->type, size, gc->gc_epoch);
        if (new_data) {
            memcpy(new_data, value, size);
        }
        return new_data;
    }

    // Edenに複製
    Object* new_obj = (Object*)gc->young.eden_ptr;
    gc->young.eden_ptr += sizeof(Object) + size;

    new_obj->type = obj->type;
    new_obj->survival_count = 0;
    new_obj->generation = (uint8_t)Generation::Young;
    new_obj->forwarded = gc->gc_epoch;
    new_obj->flags = 0;
    new_obj->fwd = nullptr;

    char* new_data = (char*)(new_obj + 1);
    memcpy(new_data, value, size);

    return new_data;
}
// オブジェクトを複製（Old世代に作成）
inline char* GC_clone_old(ThreadGC* gc, char* value) {
    if (value == nullptr) return nullptr;

    std::lock_guard<std::mutex> lock(gc->mutex);

    Object* obj = (Object*)(value - sizeof(Object));

    uint32_t size;
    if (obj->type >= 65536) {
        size = obj->type - 65536;
    }
    else {
        size = gc->class_table[obj->type].size;
    }

    char* new_data = gc->old.alloc(obj->type, size, gc->gc_epoch);
    if (new_data) {
        memcpy(new_data, value, size);
    }
    return new_data;
}

// ロックなし版（GC中やバッチ処理用）
inline char* GC_clone_unlocked(ThreadGC* gc, char* value) {
    if (value == nullptr) return nullptr;

    Object* obj = (Object*)(value - sizeof(Object));

    uint32_t size;
    ClassType* cls = nullptr;
    if (obj->type >= 65536) {
        size = obj->type - 65536;
    }
    else {
        cls = &gc->class_table[obj->type];
        size = cls->size;
    }

    // 空きチェック（足りなければnullptr）
    if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
        return nullptr;
    }

    Object* new_obj = (Object*)gc->young.eden_ptr;
    gc->young.eden_ptr += sizeof(Object) + size;

    new_obj->type = obj->type;
    new_obj->survival_count = 0;
    new_obj->generation = (uint8_t)Generation::Young;
    new_obj->forwarded = gc->gc_epoch;
    new_obj->flags = 0;
    new_obj->fwd = nullptr;

    char* new_data = (char*)(new_obj + 1);
    memcpy(new_data, value, size);

    return new_data;
}
// =============================================================================
// 明示的GC呼び出し
// =============================================================================
inline void GC_collect_young(ThreadGC* gc) {
    std::lock_guard<std::mutex> lock(gc->mutex);
    gc_young(gc);
}

inline void GC_collect_full(ThreadGC* gc) {
    std::lock_guard<std::mutex> lock(gc->mutex);
    gc_old(gc);
}

// =============================================================================
// ユーティリティ
// =============================================================================
inline Object* GC_get_header(char* data) {
    return (Object*)(data - sizeof(Object));
}

inline bool GC_is_young(char* data) {
    Object* obj = GC_get_header(data);
    return obj->generation == (uint8_t)Generation::Young;
}

inline bool GC_is_old(char* data) {
    Object* obj = GC_get_header(data);
    return obj->generation == (uint8_t)Generation::Old;
}


// =============================================================================
// ロックなし割り当て（別スレッド用 - オーディオコールバック等）
// GCを発動しない、空きがなければnullptrを返す
// =============================================================================

// ロックなしYoung割り当て
inline char* GC_alloc_ant(ThreadGC* gc, int type) {
    ClassType* cls = &gc->class_table[type];
    uint32_t size = cls->size;

    // 空きチェックのみ（GC発動しない）
    if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
        return nullptr;
    }

    Object* obj = (Object*)gc->young.eden_ptr;
    gc->young.eden_ptr += sizeof(Object) + size;

    obj->type = type;
    obj->survival_count = 0;
    obj->generation = (uint8_t)Generation::Young;
    obj->forwarded = gc->gc_epoch;
    obj->flags = 0;
    obj->fwd = nullptr;

    char* data = (char*)(obj + 1);
    memset(data, 0, size);

    return data;
}

// ロックなし可変サイズ割り当て
inline char* GC_alloc_size_ant(ThreadGC* gc, uint32_t size) {
    if (gc->young.eden_ptr + sizeof(Object) + size > gc->young.eden + gc->young.eden_size) {
        return nullptr;
    }

    Object* obj = (Object*)gc->young.eden_ptr;
    gc->young.eden_ptr += sizeof(Object) + size;

    obj->type = 65536 + size;
    obj->survival_count = 0;
    obj->generation = (uint8_t)Generation::Young;
    obj->forwarded = gc->gc_epoch;
    obj->flags = 0;
    obj->fwd = nullptr;

    char* data = (char*)(obj + 1);
    memset(data, 0, size);

    return data;
}

// ロックなしOld割り当て（mallocするので注意）
inline char* GC_alloc_old_ant(ThreadGC* gc, int type) {
    ClassType* cls = &gc->class_table[type];
    return gc->old.alloc(type, cls->size, gc->gc_epoch);
}

// ロックなしOld可変サイズ割り当て
inline char* GC_alloc_old_size_ant(ThreadGC* gc, uint32_t size) {
    return gc->old.alloc(65536 + size, size, gc->gc_epoch);
}
// 統計情報
inline void GC_print_stats(ThreadGC* gc) {
    std::cout << "=== GC Statistics ===" << std::endl;
    std::cout << "Young GC count: " << gc->young_gc_count << std::endl;
    std::cout << "Old GC count: " << gc->old_gc_count << std::endl;
    std::cout << "Promoted objects: " << gc->promoted_count << std::endl;
    std::cout << "Eden used: " << (gc->young.eden_ptr - gc->young.eden) << " / " << gc->young.eden_size << std::endl;
}

// =============================================================================
// ヘルパーマクロ
// =============================================================================
#define GC_UPDATE_PTR(gc, ptr) \
do { \
    if (ptr) ptr = copy_object_young(gc, ptr); \
} while(0)
#define GC_UPDATE_PTR_C(gc, ptr, type) \
do { \
    if (ptr) ptr = (type)copy_object_young(gc, (char*)(ptr)); \
} while(0)

CoroutineQueue* queuefirst(ThreadGC* thgc) {
    return thgc->first;
}
