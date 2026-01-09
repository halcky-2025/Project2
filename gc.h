
#include <coro/coro.hpp> 

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
typedef struct ThreadGC ThreadGC;
class Transaction;
typedef struct RootNode {
    ThreadGC* thgc;
    int rn;
    Transaction* tx;
    char*** root;
    RootNode* next;
    RootNode* before;
} RootNode;
typedef struct ClassType;
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
typedef struct ThreadGC {
    CoroutineQueue* queue;
    RootNode* roots;
    CoroutineQueue* first;
    int cn = 0;
    char* heap1;
    char* heap2;
    ClassType* class_table;

    char* from_space;
    char* to_space;
    char* alloc_ptr;
    char* to_alloc_ptr;
    int HEAP_SIZE;
    int gcc;
    std::mutex m;
    bool waitgc;
    int staticid;
    Map* map;
    RenderCommandQueue* commandQueue;
    HopStar* hoppy;
} ThreadGC;

void push_to_queue(CoroutineQueue* queue, char* raw) {
    queue->push(std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(raw)));  // グローバル or TLS な CoroutineQueue を使う
}
char* wait_handle(ThreadGC* thgc, char* raw) {
    auto h2 = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(raw));
	auto queue = new CoroutineQueue();
	queue->push(h2);  // グローバル or TLS な CoroutineQueue を使う
    h2.promise().queue = queue;
    for (;;) {
        if (thgc->first == NULL) break;
        else thgc->first->resume_all();
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
	_List, _String, _Element, _FRect, _LetterC, _Detection, _LocalC, _Select, _State, _LineC, _VLineC, _LetterPart,
    _MapData, _Map, _KV, _EndC, _CloneElemC, _HoppyWindow, _ColumnMeta, _Table, _Column, _TreeElement, _DivC, _SheetC,
    _MemTable, _MouseEvent, _KeyEvent, _MemFunc, _FuncType, _Offscreen
};
typedef struct ClassType{
    String* name;
    int size;
    void (*gccheck)(ThreadGC*, char *);
    void (*gcfinalize)(ThreadGC*, char *);
} ClassType;
typedef struct Object {
    enum CType type;  // 単純な1フィールドオブジェクト
    int forwarded;       // 既に移動済みなら1
    char* fwd;
} Object;
ThreadGC* magc = NULL;
void gc(ThreadGC* thgc);
Object* root = NULL;
ThreadGC* GC_init(int size) {
	ThreadGC* thgc = (ThreadGC*)malloc(sizeof(ThreadGC));
    thgc->cn = 32;
    thgc->queue = new CoroutineQueue();
    thgc->HEAP_SIZE = size;
	thgc->heap1 = (char*)malloc(thgc->HEAP_SIZE);
	thgc->heap2 = (char*)malloc(thgc->HEAP_SIZE);
	thgc->class_table = (ClassType*)malloc(sizeof(ClassType) * 64);
	thgc->roots = (RootNode*)malloc(sizeof(RootNode));
    thgc->roots->thgc = thgc;
    thgc->roots->next = thgc->roots->before = thgc->roots;
	thgc->roots->root = (char***)malloc(sizeof(char*) * 64);
    thgc->roots->rn = 0;
    thgc->waitgc = false;
    thgc->first = NULL;
	thgc->from_space = thgc->heap1;
	thgc->to_space = thgc->heap2;
	thgc->alloc_ptr = thgc->from_space;
	thgc->to_alloc_ptr = thgc->to_space;
    thgc->gcc = 0;
    thgc->staticid = -1;
    thgc->commandQueue = createRCQ();
    new (&thgc->m) std::mutex;
	root = NULL;
    return thgc;
}
String* createString(ThreadGC* thgc, char* tex, int size, int esize);
void add_mapy(ThreadGC* thgc, Map* map, String* key, char* value);
void GC_SetClass(ThreadGC* thgc, enum CType type, const char* text, int size, void (*gccheck)(ThreadGC*, char *), void (*gcfinalize)(ThreadGC*, char *)) {
	thgc->class_table[type].size = size;
	thgc->class_table[type].gccheck = gccheck;
	thgc->class_table[type].gcfinalize = gcfinalize;
    String* name = createString(thgc, (char *)text, strlen(text), 1);
	thgc->class_table[type].name = name;
}
int GC_AddClass(ThreadGC* thgc, int type, int size, const char * text, void (*gccheck)(ThreadGC*, char*), void (*gcfinalize)(ThreadGC*, char*)) {
    thgc->class_table[type].size = size;
    thgc->class_table[type].gccheck = gccheck;
    thgc->class_table[type].gcfinalize = gcfinalize;
    String* name = createString(thgc, (char*)text, strlen(text), 1);
    thgc->class_table[type].name = name;
    add_mapy(thgc, thgc->map, name, (char*)thgc->cn);
    return type++;
}
RootNode* GC_AddRoot(ThreadGC* thgc) {
    auto root = (RootNode*)malloc(sizeof(RootNode));
    root->rn = 0;
    root->root = (char***)malloc(sizeof(char*) * 64);
	thgc->roots->before->next = root;
	root->before = thgc->roots->before;
    thgc->roots->before = root;
    root->next = thgc->roots;
    root->thgc = thgc;
    return root;
}
void GC_ReleaseRoot(RootNode* node) {
    node->before->next = node->next;
	node->next->before = node->before;
    free(node->root);
    free(node);
}
void GC_SetRoot(RootNode* node, char** root) {
	node->root[node->rn] = root;
    node->rn++;
}
void GC_BackRoot(RootNode* node, int n) {
    node->rn -= n;
}
char* GC_malloc(ThreadGC* thgc, int type) {
    std::lock_guard lock(thgc->m);
    ClassType clstype = thgc->class_table[type];
    if (thgc->alloc_ptr + clstype.size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
        gc(thgc);
    }
    Object* obj = (Object*)(thgc->alloc_ptr);
    char* value = thgc->alloc_ptr + sizeof(Object);
    memset(value, 0, clstype.size);
    thgc->alloc_ptr += clstype.size + sizeof(Object);
    obj->type = (enum CType)type;
    obj->forwarded = thgc->gcc;
    obj->fwd = NULL;
    return value;
}
char* GC_malloc_size(ThreadGC* thgc, int size) {
    std::lock_guard lock(thgc->m);
    if (thgc->alloc_ptr + size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
        gc(thgc);
    }
    Object* obj = (Object*)(thgc->alloc_ptr);
    char* value = thgc->alloc_ptr + sizeof(Object);
    memset(value, 0, size);
    thgc->alloc_ptr += size + sizeof(Object);
    obj->type = (CType)(65536 + size);
    obj->forwarded = thgc->gcc;
    obj->fwd = NULL;
    return value;
}
char* GC_malloc_ant(ThreadGC* thgc, int type) {
    ClassType clstype = thgc->class_table[type];
    if (thgc->alloc_ptr + clstype.size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
        return NULL;
    }
    Object* obj = (Object*)(thgc->alloc_ptr);
    char* value = thgc->alloc_ptr + sizeof(Object);
    memset(value, 0, clstype.size);
    thgc->alloc_ptr += clstype.size + sizeof(Object);
    obj->type = (enum CType)type;
    obj->forwarded = thgc->gcc;
    obj->fwd = NULL;
    return value;
}
char* GC_malloc_size_ant(ThreadGC* thgc, int size) {
    if (thgc->alloc_ptr + size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
        return NULL;
    }
    Object* obj = (Object*)(thgc->alloc_ptr);
    char* value = thgc->alloc_ptr + sizeof(Object);
    memset(value, 0, size);
    thgc->alloc_ptr += size + sizeof(Object);
    obj->type = (CType)(65536 + size);
    obj->forwarded = thgc->gcc;
    obj->fwd = NULL;
    return value;
}
char* clone_object(ThreadGC* thgc, char* value) {
    std::lock_guard lock(thgc->m);
    Object* obj = (Object*)(value - sizeof(Object));
    if (obj->type >= 65536) {
        int size = obj->type - 65536;
        if (thgc->alloc_ptr + size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
            gc(thgc);
        }
        Object* newobj = (Object*)(thgc->alloc_ptr);
        thgc->alloc_ptr += size + sizeof(Object);
        memcpy(newobj, obj, sizeof(Object) + size);

        newobj->forwarded = thgc->gcc;
        char* newvalue = (char*)newobj + sizeof(Object);
        obj->fwd = NULL;
        return newvalue;
    }
    else {
        ClassType clstype = thgc->class_table[obj->type];
        if (thgc->alloc_ptr + clstype.size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
            gc(thgc);
        }
        Object* newobj = (Object*)(thgc->alloc_ptr);
        thgc->alloc_ptr += clstype.size + sizeof(Object);
        memcpy(newobj, obj, sizeof(Object) + clstype.size);

        newobj->forwarded = thgc->gcc;
        char* newvalue = (char*)newobj + sizeof(Object);
        newobj->fwd = NULL;
        return newvalue;
    }
}
char* clone_object_ant(ThreadGC* thgc, char* value) {
    Object* obj = (Object*)(value - sizeof(Object));
    if (obj->type >= 65536) {
        int size = obj->type - 65536;
        if (thgc->alloc_ptr + size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
            return NULL;
        }
        Object* newobj = (Object*)(thgc->alloc_ptr);
        thgc->alloc_ptr += size + sizeof(Object);
        memcpy(newobj, obj, sizeof(Object) + size);

        newobj->forwarded = thgc->gcc;
        char* newvalue = (char*)newobj + sizeof(Object);
        obj->fwd = NULL;
        return newvalue;
    }
    else {
        ClassType clstype = thgc->class_table[obj->type];
        if (thgc->alloc_ptr + clstype.size + sizeof(Object) > thgc->from_space + thgc->HEAP_SIZE) {
            return NULL;
        }
        Object* newobj = (Object*)(thgc->alloc_ptr);
        thgc->alloc_ptr += clstype.size + sizeof(Object);
        memcpy(newobj, obj, sizeof(Object) + clstype.size);

        newobj->forwarded = thgc->gcc;
        char* newvalue = (char*)newobj + sizeof(Object);
        newobj->fwd = NULL;
        return newvalue;
    }
}

char* copy_object(ThreadGC* thgc, char * value) {
    if (value == NULL) return NULL;
    Object* obj = (Object*)(value - sizeof(Object));

    if (obj->forwarded == thgc->gcc) {
        return obj->fwd; // すでに移動済み
    }

    Object* new_obj = (Object*)thgc->to_alloc_ptr;
    if (obj->type >= 65536) {
        int size = obj->type - 65536;
        thgc->to_alloc_ptr += size + sizeof(Object);
        memcpy(new_obj, obj, sizeof(Object) + size);

        obj->forwarded = thgc->gcc;
        char* newvalue = (char*)new_obj + sizeof(Object);
        obj->fwd = newvalue;
        return newvalue;
    }
    else {
        ClassType clstype = thgc->class_table[obj->type];
        thgc->to_alloc_ptr += clstype.size + sizeof(Object);
        memcpy(new_obj, obj, sizeof(Object) + clstype.size);

        obj->forwarded = thgc->gcc;
        char* newvalue = (char*)new_obj + sizeof(Object);
        obj->fwd = newvalue;
        if (clstype.gccheck != NULL) clstype.gccheck(thgc, newvalue);
        return newvalue;
    }
}
void gc(ThreadGC* thgc) {
	thgc->waitgc = false;
    thgc->to_alloc_ptr = thgc->to_space;
    thgc->gcc = (thgc->gcc + 1) % 2;
    auto first = thgc->roots;
    for (auto root = first; ; ) {
        for (int i = 0; i < root->rn; i++) {
            if (root->root[i] == NULL) continue;
            *root->root[i] = (char*)copy_object(thgc, *root->root[i]);
        }
		root = root->next;
		if (root == first) break;  // ループ終了
    }
    char* scan = thgc->from_space;
    while (scan < thgc->alloc_ptr) {
        Object* obj = (Object*)scan;
        char* value = scan + sizeof(Object);

        int size;
        if (obj->type >= 65536) {
            size = obj->type - 65536;
        }
        else {
            size = thgc->class_table[obj->type].size;
        }

        if (!obj->forwarded) {
            // 生存していない＝finalize対象
            if (obj->type < 65536 && thgc->class_table[obj->type].gcfinalize) {
                ClassType clstype = thgc->class_table[obj->type];
                if (clstype.gcfinalize != NULL) clstype.gcfinalize(thgc, value);
            }
        }

        scan += sizeof(Object) + size;
    }
    // 領域の入れ替え
    char* temp = thgc->from_space;
    thgc->from_space = thgc->to_space;
    thgc->to_space = temp;
    thgc->alloc_ptr = thgc->to_alloc_ptr;
}
void exegc(ThreadGC* thgc) {
	std::lock_guard lock(thgc->m);
	gc(thgc);
}
