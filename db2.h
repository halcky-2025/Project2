/*=====================================================================
   tx_sqlite_pool.hpp  (libcoro 0.15.0 専用)
   ©2025  MIT
 =====================================================================*/
#pragma once

#include <memory>
namespace std {
template<class T, class... Args>
constexpr T* construct_at(T* p, Args&&... args)
noexcept(std::is_nothrow_constructible_v<T, Args...>)
{
    return ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
}
}
#include <coroutine>

using json = nlohmann::json;
struct Cell {
    union {
        sqlite3_int64      i64;
        double             dbl;
        String*            str;
    };
};
enum class ColType : uint8_t { Null, Int, Real, Text, Blob };

struct ColumnMeta {
    String* name;
    ColType type;
};
struct RowSet {
    Map* columns;
    Map* rows;
    // 非SELECT文の補助情報
    sqlite3_int64 last_rowid = 0;          // INSERT → rowid
    int           changes = 0;          // UPDATE/DELETE → 変更行数
    bool          has_data = false;
};
enum SqlParamKind : int {
    SQL_NONE = 0,
    SQL_NULL = 1,
    SQL_INT = 2,
    SQL_I64 = 3,
    SQL_REAL = 4,
    SQL_TEXT = 5,
    SQL_BLOB = 6,
};

struct SqlParam {
    SqlParamKind kind;  // 4 bytes
    union {
        int           ival;
        long long     i64val;
        double        dval;
        struct { const char* ptr; int len; } text;
        struct { const void* ptr; int len; } blob;
    } data;
};
Generator RunGcAndNotify(ThreadGC* thgc,
    std::mutex* mu,
    std::condition_variable* cv,
    bool* done)
{
    // exegc は「ロック済みラッパ」である前提（= 中で thgc->m を取る）
    if (thgc->waitgc) exegc(thgc);

    { std::lock_guard lk(*mu); *done = true; }
    cv->notify_one();
    co_return (char*)0;
}
void CallGCant(ThreadGC* thgc, CoroutineQueue* queue, std::unique_lock<std::mutex>* lock) {
    lock->unlock();
    std::mutex mu;
    std::condition_variable cv;
    bool done = false;
    thgc->waitgc = true;

    // 一発コルーチンを作ってハンドルをキューに投げる
    Generator g = RunGcAndNotify(thgc, &mu, &cv, &done);
    auto h = g.coro;           // ハンドル抽出
    g.coro = {};               // 以後の破棄はキュー側で（※後述参照）
    queue->push(h);      // メインスレッドが resume_all() で実行する

    // ここでブロックして GC 完了を待つ（ロックは取らない）
    std::unique_lock lk(mu);
    cv.wait(lk, [&] { return done; });
    lock->lock();
}
class Worker {
public:
    sqlite3* db() noexcept { return db_; }
    void reset_db(sqlite3* newdb) noexcept { db_ = newdb; }
    explicit Worker(const std::string& db_path) {
        if (sqlite3_open_v2(db_path.c_str(), &db_,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
            SQLITE_OPEN_FULLMUTEX,
            nullptr) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db_));
        }
        exec_simple("PRAGMA journal_mode=WAL;");     // 読み取りは並行、書き込みは1人
        exec_simple("PRAGMA synchronous=NORMAL;");   // お好みで（耐障害性と速度のバランス）
        sqlite3_busy_timeout(db_, 5000);

        th_ = std::thread([this] { loop(); });
    }
    ~Worker() {
        { std::lock_guard lk(mu_); stop_ = true; cv_.notify_all(); }
        if (th_.joinable()) th_.join();
        if (db_) sqlite3_close_v2(db_);
    }

    struct Job {
        enum Kind { Exec, Commit, Rollback } kind{};
        ThreadGC* thgc;
		CoroutineQueue* queue{ nullptr }; 
        char* (*func)(ThreadGC*, CoroutineQueue*, std::unique_lock<std::mutex>*, sqlite3_stmt*, char*);
        char* obj;
        char* sql;
        std::vector<SqlParam> params;
        RowSet* result = {nullptr};
        std::exception_ptr* eptr{ nullptr };
        std::atomic_bool* busy_flag{ nullptr };
        std::coroutine_handle<Generator::promise_type> h;
    };

    void post(Job j) {
        std::lock_guard lk(mu_);
        q_.push_back(std::move(j));
        cv_.notify_one();
    }

private:
    void loop() {
        for (;;) {
            Job j;
            {
                std::unique_lock lk(mu_);
                cv_.wait(lk, [&] { return stop_ || !q_.empty(); });
                if (stop_) return;
                j = std::move(q_.front());
                q_.pop_front();
            }

            try {
                if (j.kind == Job::Exec)     run_sql(j.thgc, j.queue, j.sql, j.params, j.result, j.func, j.obj);
                else if (j.kind == Job::Commit)   exec_simple("COMMIT;");
                else                              exec_simple("ROLLBACK;");
            }
            catch (...) { if (j.eptr) *j.eptr = std::current_exception(); }

            /*if (j.kind != Job::Exec) {
                if (j.busy_flag) j.busy_flag->store(false, std::memory_order_release);
            }*/
            j.queue->push(j.h);      // <- ここで Awaiter を再開
        }
    }

    RowSet* run_sql(ThreadGC* thgc, CoroutineQueue* queue, char* sql, const std::vector<SqlParam>& params, RowSet* rs, char* (*func)(ThreadGC*, CoroutineQueue*, std::unique_lock<std::mutex>*, sqlite3_stmt*, char*), char* obj)
    {
        sqlite3_stmt* st = nullptr;
        if (sqlite3_prepare_v2(db_, sql, -1, &st, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db_));

        // 追加：パラメータをバインド
        for (int i = 0; i < params.size(); ++i) {
            switch (params[i].kind) {
            case SQL_NULL:
                sqlite3_bind_null(st, int(i + 1));
                break;
            case SQL_INT:
                sqlite3_bind_int(st, int(i + 1), params[i].data.ival);
                break;
            case SQL_I64:
                sqlite3_bind_int64(st, int(i + 1), params[i].data.i64val);
                break;
            case SQL_REAL:
                sqlite3_bind_double(st, int(i + 1), params[i].data.dval);
                break;
            case SQL_TEXT:
                sqlite3_bind_text(st, int(i + 1), params[i].data.text.ptr, params[i].data.text.len, SQLITE_TRANSIENT);
                break;
            case SQL_BLOB:
                sqlite3_bind_blob(st, int(i + 1), params[i].data.blob.ptr, params[i].data.blob.len, SQLITE_TRANSIENT);
                break;
            default:
                throw std::runtime_error("Unknown SqlParamC kind");
            }
        }

        const int ncol = sqlite3_column_count(st);

        if (ncol == 0) {
            if (sqlite3_step(st) != SQLITE_DONE)
                throw std::runtime_error(sqlite3_errmsg(db_));
            rs->has_data = false;
            rs->last_rowid = sqlite3_last_insert_rowid(db_);
            rs->changes = sqlite3_changes(db_);
            sqlite3_finalize(st);
            return rs;
        }
        rs->columns = rs->rows = NULL;
        {
            std::unique_lock lock(thgc->m);
        head99:
            rs->columns = create_mapy_ant(thgc, true);
            if (rs->columns == NULL) {
                CallGCant(thgc, queue, &lock);
                goto head99;
            }
        head88:
            rs->rows = create_mapy_ant(thgc, true);
            if (rs->rows == NULL) {
                goto head88;
            }
            for (int i = 0; i < ncol; ++i) {
            head77:
                ColumnMeta* column = (ColumnMeta*)GC_malloc_ant(thgc, _ColumnMeta);
                if (column == NULL) {
                    CallGCant(thgc, queue, &lock);
                    goto head77;
                }
            head55:
                char* column_name = (char*)sqlite3_column_name(st, i);
                column->name = createStringant(thgc, column_name, std::strlen(column_name), 1);
                if (column->name == NULL) {
					CallGCant(thgc, queue, &lock);
                    goto head55;
                }
                column->type = ColType::Null;
            head66:
                add_mapy_ant(thgc, rs->columns, NULL, (char*)column);
                if (column == NULL) {
                    CallGCant(thgc, queue, &lock);
                    goto head66;
                }
            }
            rs->has_data = true;
        }
        while (sqlite3_step(st) == SQLITE_ROW) {
            {
                std::unique_lock lock(thgc->m);
                char *o = func(thgc, queue, &lock, st, obj);
            head0:
                char *l = add_mapy_ant(thgc, rs->rows, NULL, o);
                if (l == NULL) {
                    CallGCant(thgc, queue, &lock);
                    goto head0;
                }
            }
        }

        sqlite3_finalize(st);
        return rs;
    }
    void exec_simple(const char* sql) {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK)
            throw std::runtime_error(err ? err : sqlite3_errmsg(db_));
    }

    sqlite3* db_{ nullptr }; // ThreadGC へのポインタ
    std::thread th_;
    std::mutex mu_;
    std::condition_variable cv_;
    std::deque<Job> q_;
    bool stop_{ false };
};
class SqlPool;   // forward

class Transaction {
public:
    Generator      commit();
    Generator      rollback();
    ~Transaction() {
        if (active_) {                        // コミットもロールバックもまだ
            try {
                // 非 coroutine 環境なので sync_wait でブロック
                //coro::sync_wait(rollback());
            }
            catch (...) {
                // デストラクタでは例外を伝播させない
            }
        }
    }
    Transaction(Transaction&& other) noexcept
        : thgc_(other.thgc_), pool_(other.pool_), idx_(other.idx_), w_(other.w_),
        busy_(other.busy_), active_(other.active_)
    {
        other.active_ = false;    // 所有権を奪ったので二重解放を防止
    }
    friend class SqlPool;
    Transaction(ThreadGC* thgc, SqlPool* p, CoroutineQueue* q, size_t idx, Worker* w,
        std::atomic_bool& busy)
        : thgc_(thgc), pool_(p),queue(q), idx_(idx), w_(w),  busy_(busy) {
    }
    void finish() {
        busy_.store(false, std::memory_order_release);
        active_ = false;
    }
    ThreadGC* thgc_;
    SqlPool* pool_;
    CoroutineQueue* queue;
    size_t                         idx_;
    Worker* w_;
    std::atomic_bool& busy_;
    bool                           active_{ true };
};

class SqlPool {
public:
    SqlPool(const std::string& db_path, size_t workers)
        : busy_(workers)
    {
        workers_.reserve(workers);
        for (size_t i = 0; i < workers; ++i)
            workers_.push_back(std::make_unique<Worker>(db_path));
    }

        // --- 追加: DB を別ファイルへスワップ -----------------
    Generator swap_database(const std::string& new_path,
        bool copy_current = false);
    std::atomic_bool accepting_{ true };
    friend class Transaction;

    Generator pick_idle(CoroutineQueue* queue) {
        for (;;) {
            for (size_t pass = 0; pass < busy_.size(); ++pass) {
                size_t i = pass; // 必要なら後述のラウンドロビンに置換
                bool expected = false;
                if (busy_[i].compare_exchange_strong(expected, true,
                    std::memory_order_acq_rel)) {
                    co_return (char*)i; // ここで確保済み
                }
            }
            co_await LoopAwaiter{ queue };
        }
    }

    struct LoopAwaiter {
        CoroutineQueue* queue;
        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<Generator::promise_type> h) {
            queue->push(h);
        }
        void await_resume() { }
    };
    struct detail {
        struct begin_awaiter {
			CoroutineQueue* queue;
            Worker* w;
            RowSet* result;
            std::exception_ptr ep;
            bool await_ready() const noexcept { return false; }
            void  await_suspend(std::coroutine_handle<Generator::promise_type> h) {
                Worker::Job j;
                j.kind = Worker::Job::Exec;
				j.queue = queue; // コルーチンキューへのポインタ
                j.sql = (char*)"BEGIN DEFERRED;";
                j.result = result = new RowSet();
                j.eptr = &ep;
                j.h = h;
                w->post(std::move(j));
            }
            void await_resume() {
                if (ep) std::rethrow_exception(ep);
            }
        };
    };
    std::vector<std::unique_ptr<Worker>> workers_;
    std::vector<std::atomic_bool>        busy_;
};

namespace detail {
    struct exec_awaiter {
        ThreadGC* thgc;
		CoroutineQueue* queue;
        char* (*func)(ThreadGC*, CoroutineQueue*, std::unique_lock<std::mutex>*, sqlite3_stmt*, char*);
        char* obj;
        Worker* w;
        char* sql;              // SQL 本体には必ず ? プレースホルダ
        std::vector<SqlParam> params;
        RowSet* result;
        std::exception_ptr ep;
        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<Generator::promise_type> h) {
            Worker::Job j;
            j.kind = Worker::Job::Exec;
			j.queue = queue; // コルーチンキューへのポインタ
            j.sql = std::move(sql);
            j.func = func;
            j.obj = obj;
            j.params = std::move(params);
            j.thgc = thgc;
            j.result = result = new RowSet();
            j.eptr = &ep;
            j.h = h;
            w->post(std::move(j));
        }
        RowSet* await_resume() {
            if (ep) std::rethrow_exception(ep);
            return std::move(result);
        }
    };

    struct end_awaiter {
        CoroutineQueue* queue;
        Worker* w; bool commit;
        std::atomic_bool& busy;
        std::exception_ptr ep;
        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<Generator::promise_type> h) {
            Worker::Job j;
            j.kind = commit ? Worker::Job::Commit : Worker::Job::Rollback;
            j.queue = queue;
            j.busy_flag = &busy;
            j.eptr = &ep;
            j.h = h;
            w->post(std::move(j));
        }
        void await_resume() { if (ep) std::rethrow_exception(ep); }
    };
} // namespace detail

inline Generator Transaction::commit() {
    if (!active_) co_return (char*)0;
    co_await detail::end_awaiter{queue, w_, true, busy_};
    finish();
    co_return (char*)0;
}
inline Generator Transaction::rollback() {
    if (!active_) co_return (char*)0;
    co_await detail::end_awaiter{queue, w_, false, busy_};
    finish();
	co_return (char*)0;
}
Generator execf_generic(Transaction* tx, const char* sql, const SqlParam params[], int param_count, char* (*func)(ThreadGC*, CoroutineQueue*, std::unique_lock<std::mutex>*, sqlite3_stmt*, char*), char *obj) {
    if (!tx->active_) throw std::runtime_error("transaction closed");

    // C の union から std::variant<SqlParam> に変換
    std::vector<SqlParam> vec;
    vec.reserve(param_count);
    for (int i = 0; i < param_count; ++i) {
        const auto& p = params[i];
        vec.emplace_back(p);
    }

    // 実行
    co_return (char*)co_await detail::exec_awaiter{
        tx->thgc_,
        tx->queue,
        func,
        obj,
        tx->w_,
        (char*)sql,
        std::move(vec)
    };
}
inline Generator execf(Transaction* tx,
    const char* sql,
    std::initializer_list<SqlParam> params, char* (*func)(ThreadGC*, CoroutineQueue*, std::unique_lock<std::mutex>*, sqlite3_stmt*, char*), char* obj)
{
    return execf_generic(tx, sql, params.begin(), params.size(), func, obj);
}
struct Version {
    int v1, v2;
};

Generator begin_tx(ThreadGC* thgc, SqlPool* sql, CoroutineQueue* queue) {
    while (!sql) {
        struct RequeueAwaiter {
            CoroutineQueue* queue;
            bool await_ready() noexcept { return false; }
            void await_suspend(std::coroutine_handle<Generator::promise_type> h) {
                queue->push(h);  // 再スケジュール
            }
            void await_resume() noexcept {}
        };

        co_await RequeueAwaiter{ queue };  // 再登録して待機
    }
    size_t idx = (size_t)co_await sql->pick_idle(queue);

    co_await SqlPool::detail::begin_awaiter(queue, sql->workers_[idx].get());

    co_return (char*)new Transaction(thgc, sql, queue, idx, sql->workers_[idx].get(), sql->busy_[idx]);
}
std::coroutine_handle<Generator::promise_type> beginTransaction(ThreadGC* thgc, SqlPool* sql, CoroutineQueue* queue) {
    Generator gen = begin_tx(thgc, sql, queue);
    auto coro = gen.coro;
    gen.coro = {};
    return coro;
}
std::coroutine_handle<Generator::promise_type> execSql(Transaction* tx, String* sql, SqlParam params[], int pcount, char* (*func)(ThreadGC*, CoroutineQueue*, std::unique_lock<std::mutex>*, sqlite3_stmt*, char*), char* obj) {
    int x = 0;
    Generator gen = execf_generic(tx, StringUTF8(sql, &x), params, pcount, func, obj);
    auto coro = gen.coro;
    gen.coro = {};
    return coro;
}
std::coroutine_handle<Generator::promise_type> TxCommit(Transaction* tx) {
    int x = 0;
    Generator gen = tx->commit();
    auto coro = gen.coro;
    gen.coro = {};
    return coro;
}
std::coroutine_handle<Generator::promise_type> TxRollback(Transaction* tx) {
    int x = 0;
    Generator gen = tx->rollback();
    auto coro = gen.coro;
    gen.coro = {};
    return coro;
}
String* LetterGetter(ThreadGC* thgc, String* text, int* i) {
	int start = *i;
    for (;*i < text->size;) {
        if (('a' <= text->data[*i] && text->data[*i] <= 'z') || ('A' <= text->data[*i] && text->data[*i] <= 'Z') || text->data[*i] == '_') {
            (*i)++;
        }
        else break;
    }
	return SubString(thgc, text, start, *i - start);
}
Version VersionGetter(ThreadGC* thgc, String* text, int* i) {
	Version v = { 0, 0 };
	int start = *i;
	for (; *i < text->size; ) {
		if (text->data[*i] == '.') {
			(*i)++;
			v.v1 = StringNumber(SubString(thgc, text, start, *i - start));
			start = *i;
		}
		else if (('0' <= text->data[*i] && text->data[*i] <= '9')) {
			(*i)++;
		}
		else break;
	}
	v.v2 = StringNumber(SubString(thgc, text, start, *i - start));
	return v;
}
String* NameVerVer(ThreadGC* thgc, String* text, int v1, int v2) {
	return StringAdd2(thgc, StringAdd(thgc, StringAdd2(thgc, StringAdd(thgc, text, (char*)"_", 1, 1), NumberString(thgc, v1)), (char*)"_", 1, 1), NumberString(thgc, v2));
}
struct Table {
    int id;
    String* name;
    String* sqlname;
    int version1;
    int version2;
    int taget_table_id;
};
struct Column {
    int id;
    String* name;
    String* sqlname;
    String* type;
    int table_id;
};
char* SetTable(ThreadGC* thgc, CoroutineQueue* queue, std::unique_lock<std::mutex>* lock, sqlite3_stmt* st, char *obj) {
    Table* t;
    head0:
    t = (Table*)GC_malloc_ant(thgc, _Table);
	if (t == NULL) {
		CallGCant(thgc, queue, lock);
        goto head0;
	}
	t->id = sqlite3_column_int(st, 0);
    head1:
	t->name = createStringant(thgc, (char*)sqlite3_column_text(st, 1), sqlite3_column_bytes(st, 1), 1);
	if (t->name == NULL) {
		CallGCant(thgc, queue, lock);
        goto head1;
	}
    head2:
	t->sqlname = createStringant(thgc, (char*)sqlite3_column_text(st, 2), sqlite3_column_bytes(st, 2), 1);
	if (t->sqlname == NULL) {
		CallGCant(thgc, queue, lock);
        goto head2;
	}
	t->version1 = sqlite3_column_int(st, 3);
	t->version2 = sqlite3_column_int(st, 4);
	t->taget_table_id = sqlite3_column_int(st, 5);
    return (char*)t;
}
char* SetColumn(ThreadGC* thgc, CoroutineQueue* queue, std::unique_lock<std::mutex>* lock, sqlite3_stmt* st, char *obj) {
    Column* c;
    head0:
    c = (Column*)GC_malloc_ant(thgc, _Column);
	if (c == NULL) {
		CallGCant(thgc, queue, lock);
        goto head0;
	}
	c->id = sqlite3_column_int(st, 0);
    head1:
	c->name = createStringant(thgc, (char*)sqlite3_column_text(st, 1), sqlite3_column_bytes(st, 1), 1);
	if (c->name == NULL) {
		CallGCant(thgc, queue, lock);
        goto head1;
	}
    head2:
	c->sqlname = createStringant(thgc, (char*)sqlite3_column_text(st, 2), sqlite3_column_bytes(st, 2), 1);
	if (c->sqlname == NULL) {
		CallGCant(thgc, queue, lock);
        goto head2;
	}
    head3:
	c->type = createStringant(thgc, (char*)sqlite3_column_text(st, 3), sqlite3_column_bytes(st, 3), 1);
	if (c->type == NULL) {
		CallGCant(thgc, queue, lock);
        goto head3;
	}
	c->table_id = sqlite3_column_int(st, 4);
    return (char*)c;
}
Generator MigrateSystem(ThreadGC* thgc, SqlPool* pool, String* text) {
	auto tx = (Transaction*)co_await begin_tx(thgc, pool, thgc->first);
	std::exception_ptr ep;
	try {
        co_await execf(tx, "create table if not exists tables(table_id Integer Primary key autoincrement, name Text, tablename Text, version1 Integer, version2 Integer, target_table_id Integer);", {}, NULL, NULL);
        co_await execf(tx, "create table if not exists columns(column_id Integer Primary key autoincrement, name Text, columnname Text, type Text, table_id Integer);", {}, NULL, NULL);
        int i = 0;
        String* table_name = NULL;
        int target_table_id = -1;
        for (; ;) {
            if (i >= text->size) {
                co_await tx->commit();
                break;
            }
            else if (text->data[i] == '@') {
                i++;
				String* name = LetterGetter(thgc, text, &i);
                if (text->data[i] == '>') {
                    i++;
                    String* table = LetterGetter(thgc, text, &i);
                    if (text->data[i] == '^') {
                        i++;
						Version v = VersionGetter(thgc, text, &i);
						table_name = NameVerVer(thgc, table, v.v1, v.v2);
                        std::string table_name_str(table_name->data, table_name->size);
                        if (text->data[i] == '\n' || i == text->size) {
                            i++;
                            auto rows = (RowSet*)co_await execf(tx, "Select * from tables where (name = ?1 or tablename = ?2 and version1 = ?3 and version2 = ?4);", { { SQL_TEXT, { .text = {name->data, name->size} } }, { SQL_TEXT, {.text = { table->data, table->size} } }, { SQL_INT, {.ival = v.v1} }, { SQL_INT, {.ival = v.v2} } }, SetTable, NULL);
                            if (((List*)rows->rows->kvs)->size == 0) {
                                rows = (RowSet*)co_await execf(tx, "Insert into tables(name, tablename, version1, version2, target_table_id) values(?1, ?2, ?3, ?4, -1);", { { SQL_TEXT, {.text = { name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } }, { SQL_INT, {.ival = v.v1} }, { SQL_INT, {.ival = v.v2} } }, NULL, NULL);
                                target_table_id = rows->last_rowid;
                                rows = (RowSet*)co_await execf(tx, "Select * from tables where (name = ?1 or tablename = ?2) and version1 = ?3 and version2 = -1;", { { SQL_TEXT, {.text = { name->data, name->size} } }, { SQL_TEXT, {.text = { table->data, table->size } } }, { SQL_INT, {.ival = v.v1} } }, SetTable, NULL);
                                if (((List*)rows->rows->kvs)->size == 0) {
                                    rows = (RowSet*)co_await execf(tx, "Select * from tables where (name = ?1 or tablename = ?2) and version1 = -1 and version2 = -1;", { { SQL_TEXT, {.text = { name->data, name->size} } }, { SQL_TEXT, {.text = { table->data, table->size} } } }, SetTable, NULL);
                                    if (((List*)rows->rows->kvs)->size == 0) {
                                        co_await execf(tx, "Insert into tables(name, tablename, version1, version2, target_table_id) values(?1, ?2, ?3, -1, ?4);", { { SQL_TEXT, {.text = { name->data, name->size} } }, { SQL_TEXT, {.text = { table->data, table->size} } }, { SQL_INT, {.ival = v.v1} }, { SQL_INT, {.ival = target_table_id} } }, NULL, NULL);
                                        co_await execf(tx, "Insert into tables(name, tablename, version1, version2, target_table_id) values(?1, ?2, -1, -1, ?3);", { { SQL_TEXT, {.text = { name->data, name->size} } }, { SQL_TEXT, {.text = { table->data, table->size} } }, { SQL_INT, {.ival = target_table_id} } }, NULL, NULL);
                                        co_await execf(tx, std::format("Create table \"{0:.{1}}\" (id INTEGER PRIMARY KEY AUTOINCREMENT );", table_name->data, table_name->size).c_str(), {}, NULL, NULL);
                                    }
                                    else {
                                        Table* t = (Table*)((KV*)*get_list(rows->rows->kvs, 0))->value;
                                        co_await execf(tx, "Insert into tables(name, tablename, version1, version2, target_table_id) values(?1, ?2, ?3, -1, ?4);", { { SQL_TEXT, {.text = { name->data,name->size} } }, { SQL_TEXT, {.text = { table->data, table->size} } }, { SQL_INT, {.ival = v.v1} }, { SQL_INT, {.ival = target_table_id} } }, NULL, NULL);
                                        co_await execf(tx, "Update tables set name=?1, tablename=?2, target_table_id=?3 where table_id=?4;", { { SQL_TEXT, {.text = { name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } }, { SQL_INT, {.ival = target_table_id} }, { SQL_INT, {.ival = (int)t->id} } }, NULL, NULL);
                                        co_await execf(tx, std::format("Create table \"{0:.{1}}\" (id INTEGER PRIMARY KEY AUTOINCREMENT); ", table_name->data, table_name->size).c_str(), {}, NULL, NULL);
                                    }
                                }
                                else {
                                    Table* t = (Table*)((KV*)*get_list(rows->rows->kvs, 0))->value;
                                    co_await execf(tx, "Update tables set name=?1, tablename=?2, version1=?3 where table_id=?4;", { { SQL_TEXT, {.text = { name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = v.v1} }, {SQL_INT, {.ival = (int)t->id} } }, NULL, NULL);
                                    rows = (RowSet*)co_await execf(tx, "Select * from tables where(name = ?1 or tablename = ?2) and version1 = -1 and version2 = -1; ", { {SQL_TEXT, {.text = {name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } } }, SetTable, NULL);
                                    if (((List*)rows->rows->kvs)->size == 0) {
                                        co_await execf(tx, "Update into tables(name, tablename, version1, version2) values(?1, ?2, -1, -1);", { { SQL_TEXT, {.text = { name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size} } } }, NULL, NULL);
                                    }
                                    else {
                                        Table* t2 = (Table*)((KV*)*get_list(rows->rows->kvs, 0))->value;
                                        co_await execf(tx, "Update tables set name=?1, tablename=?2 where table_id=?3;", { { SQL_TEXT, {.text = { name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } }, { SQL_INT, {.ival = (int)t2->id} } }, NULL, NULL);
                                    }
                                    rows = (RowSet*)co_await execf(tx, "select * from tables where table_id = ?1;", { {SQL_INT, {.ival = (int)t->taget_table_id} } }, SetTable, NULL);
                                    t = (Table*)((KV*)*get_list(rows->rows->kvs, 0))->value;
									String* oldname = NameVerVer(thgc, createString(thgc, (char*)t->sqlname->data, t->sqlname->size, 1), v.v1, v.v2);
                                    std::string old_name_str(oldname->data, oldname->size);
                                    co_await execf(tx, std::format("CREATE TABLE \"{0}\" AS SELECT * FROM \"{1}\";", table_name_str, old_name_str).c_str(), {}, NULL, NULL);
                                }
                            }
                            else {
								Table* t = (Table*)((KV*)*get_list(rows->rows->kvs, 0))->value;
                                if (!StringEqual2(table, t->sqlname)) {
                                    String* oldname = NameVerVer(thgc, createString(thgc, (char*)t->sqlname->data, t->sqlname->size, 1), v.v1, v.v2);
                                    std::string old_name_str(oldname->data, oldname->size);
                                    co_await execf(tx, std::format("Alter table \"{0}\" rename to \"{1}\";", old_name_str, table_name_str).c_str(), {}, NULL, NULL);
                                }
                                target_table_id = t->id;
                                rows = (RowSet*)co_await execf(tx, "Select * from tables where (name = ?1 or tablename = ?2) and version1 = ?3 and version2 = -1;", { { SQL_TEXT, {.text = { name->data, name->size } } }, {SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = v.v1} } }, SetTable, NULL);
                                if (((List*)rows->rows->kvs)->size == 0) {
                                    co_await execf(tx, "Insert into tables(name, tablename, version1, version2, target_table_id) values(?1, ?2, ?3, -1, ?4);", { {SQL_TEXT, {.text = { name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = v.v1} }, {SQL_INT, {.ival = (int)t->taget_table_id} } }, NULL, NULL);
                                }
                                else {
                                    t = (Table*)((KV*)*get_list(rows->rows->kvs, 0))->value;
                                    co_await execf(tx, "Update tables set name=?1, tablename=?2, target_table_id=?3 where table_id = ?4;", { { SQL_TEXT, {.text = { name->data, name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = target_table_id} }, {SQL_INT, {.ival = (int)t->id} } }, NULL, NULL);
                                }
                                rows = (RowSet*)co_await execf(tx, "Select * from tables where (name = ?1 or tablename = ?2) and version1 = -1 and version2 = -1;", { { SQL_TEXT, {.text = { name->data, name->size } } }, {SQL_TEXT, {.text = { table->data, table->size } } } }, SetTable, NULL);
                                if (((List*)rows->rows->kvs)->size == 0) {
                                    co_await execf(tx, "Insert into tables(name, tablename, version1, version2, target_table_id) values(?1, ?2, -1, -1, ?3);", { { SQL_TEXT, {.text = { name->data,name->size } } }, { SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = target_table_id} } }, NULL, NULL);
                                }
                                else {
                                    t = (Table*)((KV*)*get_list(rows->rows->kvs, 0))->value;
                                    co_await execf(tx, "Update tables set name=?1, tablename=?2, target_table_id=?3 where table_id = ?4;", { {SQL_TEXT, {.text = { name->data, name->size } } }, {SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = target_table_id} }, {SQL_INT, {.ival = (int)t->id } } }, NULL, NULL);

                                }
                            }
                        }
                    }
                }
            }
            else if (text->data[i] == ' ') {
                i++;
                if (text->data[i] == '@') {
                    i++;
					String* type = LetterGetter(thgc, text, &i);
                    std::string type_str(type->data, type->size);
                    if (text->data[i] == ' ') {
                        i++;
						String* name = LetterGetter(thgc, text, &i);
                        std::string name_str(name->data, name->size);
                        if (text->data[i] == '>') {
                            i++;
							String* table = LetterGetter(thgc, text, &i);
                            std::string table_str(table->data, table->size);
                            std::string table_name_str(table_name->data, table_name->size);
                            if (text->data[i] == '\n' || i == text->size) {
                                i++;
                                auto rows = (RowSet*)co_await execf(tx, "Select * from columns where table_id = ?3 and (name = ?1 or columnname = ?2);", { {SQL_TEXT, {.text = {name->data, name->size} } }, {SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = target_table_id} } }, SetColumn, NULL);
                                if (((List*)rows->rows->kvs)->size == 0) {
                                    co_await execf(tx, "Insert into columns(name, columnname, type, table_id) values(?1, ?2, ?3, ?4);", { {SQL_TEXT, {.text = { name->data, name->size } } }, {SQL_TEXT, {.text = { table->data, table->size } } }, { SQL_TEXT, {.text = { type->data, type->size } } }, { SQL_INT, {.ival = target_table_id} } }, NULL, NULL);
                                    co_await execf(tx, std::format("ALTER TABLE \"{0}\" ADD COLUMN \"{1}\" {2};", table_name_str, table_str, type_str).c_str(), {}, NULL, NULL);
								}
                                else {
									Column* c = (Column*)((KV*)*get_list(rows->rows->kvs, 0))->value;
                                    std::string sqlname_str(c->sqlname->data, c->sqlname->size);
                                    int column_id = c->id;
                                    if (!StringEqual2(table, c->sqlname)) {
                                        if (!StringEqual2(type, c->type)) {
                                            String* column_name = c->sqlname;
											String* type_name = c->type;
                                            auto rows = (RowSet*)co_await execf(tx, "Select * from columns where table_id=?1;", { {SQL_INT, {.ival = target_table_id} } }, SetColumn, NULL);

                                            String* columns = NULL;
                                            String* values = NULL;
                                            columns = createString(thgc, (char*)"id Integer Primary Key Autoincrement", 36, 1);
                                            values = createString(thgc, (char*)"id", 2, 1);
                                            for (int i = 0; i < ((List*)rows->rows->kvs)->size; i++) {
                                                columns = StringAdd(thgc, columns, (char*)',', 1, 1);
                                                values = StringAdd(thgc, columns, (char*)',', 1, 1);
                                                Column* c = (Column*)((KV*)*get_list(rows->rows->kvs, i))->value;
                                                columns = StringAdd2(thgc, columns, c->sqlname);
                                                if (!StringEqual2(column_name, c->sqlname)) {
                                                    columns = StringAdd2(thgc, columns, table);
                                                    values = StringAdd(thgc, values, (char*)"cast(", 5, 1);
                                                    values = StringAdd2(thgc, values, c->sqlname);
                                                    values = StringAdd(thgc, values, (char*)" as ", 4, 1);
                                                    values = StringAdd2(thgc, values, c->type);
                                                    values = StringAdd(thgc, values, (char*)")", 1, 1);
                                                }
                                                else {
                                                    columns = StringAdd2(thgc, columns, c->sqlname);
                                                    values = StringAdd2(thgc, values, c->sqlname);
                                                }
                                                columns = StringAdd(thgc, columns, (char*)" ", 1, 1);
                                                columns = StringAdd2(thgc, columns, type);
                                            }
                                            std::string columns_str(columns->data, columns->size);
                                            std::string values_str(values->data, values->size);
                                            co_await execf(tx, std::format("ALTER TABLE \"{0}\" RENAME TO \"{0}_\";", table_name_str).c_str(), {}, NULL, NULL);
                                            co_await execf(tx, std::format("CREATE TABLE {0} ({1});", table_name_str, columns_str).c_str(), {}, NULL, NULL);
                                            co_await execf(tx, std::format("INSERT INTO {0}({1}) SELECT * FROM {0}_;", table_name_str, values_str).c_str(), {}, NULL, NULL);
                                            co_await execf(tx, std::format("DROP TABLE {0}_;", table_name_str).c_str(), {}, NULL, NULL);
                                        }
                                        else co_await execf(tx, std::format("ALTER TABLE {2} RENAME Column {0} TO {1};", sqlname_str, table_str, table_name_str).c_str(), {}, NULL, NULL);
                                        co_await execf(tx, "UPDATE columns SET name = ?1, columnname = ?2 WHERE column_id=?3;", { { SQL_TEXT, {.text = { name->data, name->size } } }, {SQL_TEXT, {.text = { table->data, table->size } } }, {SQL_INT, {.ival = column_id } } }, NULL, NULL);
                                    }
                                    else if (!StringEqual2(type, c->type)) {
                                        String* type_name = createString(thgc, (char*)c->type->data, c->type->size, 1);
                                        auto rows = (RowSet*)co_await execf(tx, "Select * from columns where table_id=?1;", { {SQL_INT, {.ival = target_table_id} } }, SetColumn, NULL);

                                        String* columns = NULL;
                                        String* values = NULL;
                                        columns = createString(thgc, (char*)"id Integer Primary Key Autoincrement", 36, 1);
                                        values = createString(thgc, (char*)"id", 2, 1);
                                        for (int i = 0; i < ((List*)rows->rows->kvs)->size; i++) {
                                            columns = StringAdd(thgc, columns, (char*)',', 1, 1);
                                            values = StringAdd(thgc, columns, (char*)',', 1, 1);
                                            Column* c = (Column*)((KV*)*get_list(rows->rows->kvs, i))->value;
                                            columns = StringAdd2(thgc, columns, c->sqlname);
                                            if (!StringEqual2(table, c->sqlname)) {
                                                columns = StringAdd2(thgc, columns, table);
                                                values = StringAdd(thgc, values, (char*)"cast(", 5, 1);
                                                values = StringAdd2(thgc, values, c->sqlname);
                                                values = StringAdd(thgc, values, (char*)" as ", 4, 1);
                                                values = StringAdd2(thgc, values, c->type);
                                                values = StringAdd(thgc, values, (char*)")", 1, 1);
                                            }
                                            else {
                                                columns = StringAdd2(thgc, columns, c->sqlname);
                                                values = StringAdd2(thgc, values, c->sqlname);
                                            }
                                            columns = StringAdd(thgc, columns, (char*)" ", 1, 1);
                                            columns = StringAdd2(thgc, columns, type);
                                        }
                                        std::string columns_str(columns->data, columns->size);
                                        std::string values_str(values->data, values->size);
                                        co_await execf(tx, std::format("ALTER TABLE {0}_ RENAME TO {0}_;", table_name->data, table_name->size).c_str(), {}, NULL, NULL);
                                        co_await execf(tx, std::format("CREATE TABLE {0} ({1});", table_name_str, columns_str).c_str(), {}, NULL, NULL);
                                        co_await execf(tx, std::format("INSERT INTO {0}({1}) SELECT * FROM {0}_;", table_name_str, values_str).c_str(), {}, NULL, NULL);
                                        co_await execf(tx, std::format("DROP TABLE {0}_;", table_name_str).c_str(), {}, NULL, NULL);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                co_await tx->rollback();
                break;
            }
        }
	}
	catch (const std::exception& e) {
		std::cerr << "Tx aborted: " << e.what() << '\n';
		ep = std::current_exception();
	}
	if (ep) {
		try { co_await tx->rollback(); }  // まだ active_ なら ROLLBACK
		catch (...) {}                   // 失敗しても握りつぶす
		std::rethrow_exception(ep);      // 呼び出し元に元の例外を再送出
	}
    delete thgc->first;
    thgc->first = NULL;
	co_return (char*)0;
}
static std::string slurp(const std::filesystem::path& p)
{
    std::ifstream ifs(p, std::ios::binary);
    return { std::istreambuf_iterator<char>(ifs),
             std::istreambuf_iterator<char>() };
}

// data.json → json::array（失敗・非配列なら空配列）
static json load_array(const std::filesystem::path& file)
{
    if (!std::filesystem::exists(file))
        return json::array();                    // 空配列

    try {
        json v = json::parse(slurp(file));
        if (v.is_array()) return v;
    }
    catch (const json::parse_error&) { /* 壊れていたら空配列 */ }
    return json::array();
}

// 書き込み（空ならファイル削除）
static void save_array(const std::filesystem::path& file, const json& arr)
{
    if (arr.empty()) {
        std::error_code ec;
        std::filesystem::remove(file, ec);
        return;
    }
    //std::ofstream(file, std::ios::binary | std::ios::trunc) << arr.dump(2);                          // pretty-print 2space
}
static void add_entry(json& arr, std::string name, int v1, int v2, int migrate)
{
    arr.push_back(json{ {"name",  std::move(name)}, {"v1", v1}, {"v2", v2}, {"used", 1}, {"migrate", migrate} });
}
/* ---------- 配列処理ロジック ---------- */

static bool process_array(ThreadGC* thgc, json& arr, String*db, int version1, int version2, std::filesystem::path file, int migrate)
{
    json out = json::array();
    bool modified = false;
    json elem2 = NULL;
    Version ver;
    json ok = NULL;
    for (auto& elem : arr) {
        if (!elem.is_object()) { modified = true; continue; }
        std::string str = elem["name"].get<std::string>();
		int v1 = elem["v1"].get<int>();
		int v2 = elem["v2"].get<int>();
        if (StringEqual(db->data, db->size, db->esize, str.data(), str.size(), 1) && v1 == version1) {
            if (v2 == version2) {
                ok = elem;
            }
            if (elem["used"].get<int>() == 1) {
                elem2 = elem;
                ver.v1 = v1;
                ver.v2 = v2;
            }
        }
        out.push_back(std::move(elem));
    }

    if (ok != NULL) {
		std::string oks = ok["name"].get<std::string>();
		if (StringEqual(db->data, db->size, db->esize, oks.data(), oks.size(), 1) && ok["migrate"].get<int>() == migrate) {
            return false;
		}
        ok["used"] = 1;
        modified = true;
        if (elem2 != NULL) {
            elem2["used"] = 0;
        }
        save_array(file, out);
        return true;
    }
    add_entry(out, std::string(db->data, db->size), version1, version2, migrate);
    if (elem2 == NULL) {
        save_array(file, out);
    }
    else {
        elem2["used"] = 0;
        namespace fs = std::filesystem;
        String* db_str = StringAdd(thgc, NameVerVer(thgc, db, version1, version2), (char*)".db\0", 4, 1);
        std::string db_s(db_str->data, db_str->size);
        String* db_str_before = StringAdd(thgc, NameVerVer(thgc, db, elem2["v1"], elem2["v2"]), (char*)".db\0", 4, 1);
        std::string db_s_before(db_str_before->data, db_str_before->size);

        try {
            fs::copy_file(                           // 単一ファイルをコピー
                db_s_before,                      // 元ファイル
                db_s,                   // 先ファイル
                fs::copy_options::overwrite_existing // 既にあれば上書き
            );
            std::cout << "コピー完了\n";
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << e.what() << '\n';           // 例外でエラー判定
        }
        save_array(file, out);
    }
    return true;
}

/* ---------- データ追加の例 ---------- */


SqlPool* DbInit(ThreadGC* thgc, String* db, int version1, int version2, int migrate, String*dbtext) {
    const std::filesystem::path file = "data.json";

    json items = load_array(file);

    // 全要素を処理
    bool d = process_array(thgc, items, db, version1, version2, file, migrate);
    if (d) {
        String* db_str = StringAdd(thgc, NameVerVer(thgc, db, version1, version2), (char*)".db\0", 4, 1);
        std::string db_s(db_str->data, db_str->size);
        SqlPool* sql = new SqlPool(db_s, 2);
        Generator task = MigrateSystem(thgc, sql, dbtext);
        thgc->first = new CoroutineQueue();
        task.coro.promise().queue = thgc->first;
        task.coro.resume();
        task.coro = {};
        int x = 2;
        return sql;
    }
    return NULL;
}
void Check(Transaction* tx) {
    int n = 1;
}
/*int main()
{
    SqlPool pool("demo.db", 4);     // 4 スレッド / 4 接続

    std::vector<coro::task<>> tasks;
    for (int i = 0; i < 8; ++i) tasks.emplace_back(client(pool, i * 10));

    for (auto& t : tasks) coro::sync_wait(t);
}*/