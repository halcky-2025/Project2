#pragma once
// ============================================================
// lldb.h - LLDB Client using SB API (liblldb)
// ============================================================
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>

#include <lldb/API/LLDB.h>

// ============================================================
// Section A: Data structs (cs/lldb.cs互換)
// ============================================================
struct DAPStackFrame {
    int id = 0;
    std::string name;
    std::string source;
    int line = 0;
    int column = 0;
};

struct DAPScope {
    std::string name;
    int variablesReference = 0; // SBFrame index
    bool expensive = false;
};

struct DAPVariable {
    std::string head;
    std::string name;
    std::string value;
    std::string type;
    int variablesReference = 0; // children count
    std::vector<DAPVariable> children;
};

struct DAPBreakpoint {
    int id = 0;
    bool verified = false;
    int line = 0;
    std::string message;
    std::string source;
};

struct DAPStoppedEvent {
    std::string reason;
    int threadId = 1;
    bool allThreadsStopped = false;
    std::string text;
};

struct DAPEvaluateResult {
    std::string result;
    int variablesReference = 0;
};

struct DAPBreakpointChanged {
    std::string reason;
    int id = 0;
    bool verified = false;
    int line = 0;
    std::string source;
    std::string message;
};

struct DAPMemoryReadResult {
    std::string address;
    std::vector<uint8_t> data;
    std::string text;
};

// ============================================================
// Section B: LLDBClient (SB API)
// ============================================================
class LLDBClient {
public:
    // コールバック
    std::function<void(const std::string&)> onOutput;
    std::function<void(const DAPStoppedEvent&)> onStopped;
    std::function<void()> onInitialized;
    std::function<void()> onTerminated;
    std::function<void(const std::vector<DAPStackFrame>&)> onStackTrace;
    std::function<void(const std::vector<DAPVariable>&)> onVariables;
    std::function<void(const std::vector<DAPBreakpoint>&)> onBreakpointsSet;
    std::function<void(const std::vector<DAPScope>&)> onScopes;
    std::function<void(const DAPEvaluateResult&)> onEvaluate;
    std::function<void(const DAPBreakpointChanged&)> onBreakpointChanged;
    std::function<void(const DAPMemoryReadResult&)> onMemoryRead;

    LLDBClient() = default;
    ~LLDBClient() { dispose(); }

    // ============================================================
    // 起動・初期化
    // ============================================================
    void start() {
        lldb::SBDebugger::Initialize();
        debugger_ = lldb::SBDebugger::Create(false);
        debugger_.SetAsync(true);
        listener_ = debugger_.GetListener();
        isRunning_ = true;

        // イベントリスナースレッド
        eventThread_ = std::thread([this]() { eventLoop(); });

        if (onOutput) onOutput("LLDB initialized: " + std::string(lldb::SBDebugger::GetVersionString()));
        if (onInitialized) onInitialized();
    }

    // ============================================================
    // プログラムを起動
    // ============================================================
    void launch(const std::string& program, const std::vector<std::string>& args = {},
                const std::string& cwd = "", bool stopOnEntry = true) {
        // ターゲット作成
        target_ = debugger_.CreateTarget(program.c_str());
        if (!target_.IsValid()) {
            if (onOutput) onOutput("error: failed to create target: " + program);
            return;
        }
        if (onOutput) onOutput("target created: " + program);

        // 引数配列
        std::vector<const char*> argv;
        for (auto& a : args) argv.push_back(a.c_str());
        argv.push_back(nullptr);

        // 作業ディレクトリ
        std::string workDir = cwd;
        if (workDir.empty()) {
            auto pos = program.find_last_of("\\/");
            workDir = (pos != std::string::npos) ? program.substr(0, pos) : ".";
        }

        // Launch
        lldb::SBError error;
        process_ = target_.Launch(listener_,
            args.empty() ? nullptr : argv.data(),
            nullptr,       // envp
            nullptr,       // stdin
            nullptr,       // stdout
            nullptr,       // stderr
            workDir.c_str(),
            0,             // launch flags
            stopOnEntry,
            error);

        if (error.Fail()) {
            if (onOutput) onOutput("launch error: " + std::string(error.GetCString()));
            return;
        }
        if (onOutput) onOutput("process launched, pid=" + std::to_string(process_.GetProcessID()));
    }

    // ============================================================
    // ブレークポイント設定
    // ============================================================
    void setBreakpoints(const std::string& file, const std::vector<int>& lines) {
        if (!target_.IsValid()) return;

        // このファイルの既存ブレークポイントを削除
        auto it = fileBreakpoints_.find(file);
        if (it != fileBreakpoints_.end()) {
            for (auto bpId : it->second) {
                target_.BreakpointDelete(bpId);
            }
            it->second.clear();
        }

        // 新しいブレークポイントを設定
        std::vector<int>& ids = fileBreakpoints_[file];
        std::vector<DAPBreakpoint> results;

        for (int line : lines) {
            lldb::SBBreakpoint bp = target_.BreakpointCreateByLocation(file.c_str(), line);
            DAPBreakpoint dbp;
            dbp.id = bp.GetID();
            dbp.verified = bp.GetNumLocations() > 0;
            dbp.source = file;

            if (bp.GetNumLocations() > 0) {
                lldb::SBBreakpointLocation loc = bp.GetLocationAtIndex(0);
                lldb::SBLineEntry le = loc.GetAddress().GetLineEntry();
                dbp.line = le.GetLine();
            } else {
                dbp.line = line;
            }

            ids.push_back(bp.GetID());
            results.push_back(dbp);

            if (onOutput) onOutput("breakpoint #" + std::to_string(dbp.id) +
                " at " + file + ":" + std::to_string(dbp.line) +
                (dbp.verified ? " (verified)" : " (pending)"));
        }

        if (onBreakpointsSet) onBreakpointsSet(results);
    }

    // 関数ブレークポイント設定
    void setFunctionBreakpoint(const std::string& functionName) {
        if (!target_.IsValid()) return;

        lldb::SBBreakpoint bp = target_.BreakpointCreateByName(functionName.c_str());
        DAPBreakpoint dbp;
        dbp.id = bp.GetID();
        dbp.verified = bp.GetNumLocations() > 0;
        dbp.line = 0;
        dbp.source = functionName;

        if (onOutput) onOutput("function breakpoint #" + std::to_string(dbp.id) +
            " on " + functionName);

        std::vector<DAPBreakpoint> results = { dbp };
        if (onBreakpointsSet) onBreakpointsSet(results);
    }

    // ============================================================
    // 実行制御
    // ============================================================
    void continueExec() {
        if (!process_.IsValid()) return;
        lldb::SBError err = process_.Continue();
        if (err.Fail() && onOutput) onOutput("continue error: " + std::string(err.GetCString()));
    }

    void stepOver(int threadId = -1) {
        lldb::SBThread thread = getThread(threadId);
        if (!thread.IsValid()) return;
        thread.StepOver();
    }

    void stepIn(int threadId = -1) {
        lldb::SBThread thread = getThread(threadId);
        if (!thread.IsValid()) return;
        thread.StepInto();
    }

    void stepOut(int threadId = -1) {
        lldb::SBThread thread = getThread(threadId);
        if (!thread.IsValid()) return;
        thread.StepOut();
    }

    void pause() {
        if (!process_.IsValid()) return;
        lldb::SBError err = process_.Stop();
        if (err.Fail() && onOutput) onOutput("pause error: " + std::string(err.GetCString()));
    }

    // ============================================================
    // スタックトレース取得
    // ============================================================
    void getStackTrace(int threadId = -1) {
        lldb::SBThread thread = getThread(threadId);
        if (!thread.IsValid()) return;

        std::vector<DAPStackFrame> frames;
        uint32_t numFrames = thread.GetNumFrames();

        for (uint32_t i = 0; i < numFrames && i < 20; i++) {
            lldb::SBFrame frame = thread.GetFrameAtIndex(i);
            DAPStackFrame sf;
            sf.id = frame.GetFrameID();
            sf.name = frame.GetFunctionName() ? frame.GetFunctionName() : "??";

            lldb::SBLineEntry le = frame.GetLineEntry();
            if (le.IsValid()) {
                lldb::SBFileSpec fs = le.GetFileSpec();
                char path[1024];
                fs.GetPath(path, sizeof(path));
                sf.source = path;
                sf.line = le.GetLine();
                sf.column = le.GetColumn();
            }
            frames.push_back(sf);
        }

        if (onStackTrace) onStackTrace(frames);
    }

    // ============================================================
    // スコープ取得
    // ============================================================
    void getScopes(int frameIndex) {
        lldb::SBThread thread = process_.GetSelectedThread();
        if (!thread.IsValid()) return;

        lldb::SBFrame frame = thread.GetFrameAtIndex(frameIndex);
        if (!frame.IsValid()) return;

        // LLDB doesn't have "scopes" concept like DAP.
        // Locals = arguments + locals, Globals = statics
        std::vector<DAPScope> scopes;

        // Locals scope
        lldb::SBValueList locals = frame.GetVariables(true, true, false, true);
        if (locals.GetSize() > 0) {
            DAPScope sc;
            sc.name = "Locals";
            sc.variablesReference = frameIndex; // frame index as reference
            sc.expensive = false;
            scopes.push_back(sc);
        }

        // Statics/Globals scope
        lldb::SBValueList statics = frame.GetVariables(false, false, true, true);
        if (statics.GetSize() > 0) {
            DAPScope sc;
            sc.name = "Globals";
            sc.variablesReference = frameIndex | 0x10000; // upper bit = globals
            sc.expensive = true;
            scopes.push_back(sc);
        }

        if (onScopes) onScopes(scopes);
    }

    // ============================================================
    // 変数取得
    // ============================================================
    void getVariables(int variablesReference) {
        lldb::SBThread thread = process_.GetSelectedThread();
        if (!thread.IsValid()) return;

        bool isGlobals = (variablesReference & 0x10000) != 0;
        int frameIndex = variablesReference & 0xFFFF;

        lldb::SBFrame frame = thread.GetFrameAtIndex(frameIndex);
        if (!frame.IsValid()) return;

        lldb::SBValueList valueList = isGlobals
            ? frame.GetVariables(false, false, true, true)
            : frame.GetVariables(true, true, false, true);

        std::vector<DAPVariable> vars;
        for (uint32_t i = 0; i < valueList.GetSize(); i++) {
            lldb::SBValue val = valueList.GetValueAtIndex(i);
            vars.push_back(sbValueToVariable(val));
        }

        if (onVariables) onVariables(vars);
    }

    // 子変数を展開（SBValueのchildren）
    std::vector<DAPVariable> getChildren(lldb::SBValue parent) {
        std::vector<DAPVariable> children;
        uint32_t numChildren = parent.GetNumChildren();
        for (uint32_t i = 0; i < numChildren && i < 100; i++) {
            lldb::SBValue child = parent.GetChildAtIndex(i);
            children.push_back(sbValueToVariable(child));
        }
        return children;
    }

    // ============================================================
    // 式を評価
    // ============================================================
    void evaluate(const std::string& expression, int frameIndex = 0) {
        lldb::SBThread thread = process_.GetSelectedThread();
        if (!thread.IsValid()) return;

        lldb::SBFrame frame = thread.GetFrameAtIndex(frameIndex);
        if (!frame.IsValid()) return;

        lldb::SBValue result = frame.EvaluateExpression(expression.c_str());
        DAPEvaluateResult er;
        if (result.IsValid()) {
            er.result = result.GetValue() ? result.GetValue() : "";
            if (er.result.empty() && result.GetSummary()) {
                er.result = result.GetSummary();
            }
            er.variablesReference = result.GetNumChildren();
        } else {
            lldb::SBError err = result.GetError();
            er.result = err.GetCString() ? err.GetCString() : "error";
        }

        if (onEvaluate) onEvaluate(er);
    }

    // ============================================================
    // メモリ読み取り
    // ============================================================
    void readMemory(uint64_t address, int count) {
        if (!process_.IsValid()) return;

        std::vector<uint8_t> buf(count);
        lldb::SBError error;
        size_t bytesRead = process_.ReadMemory(address, buf.data(), count, error);

        if (error.Fail()) {
            if (onOutput) onOutput("readMemory error: " + std::string(error.GetCString()));
            return;
        }

        buf.resize(bytesRead);
        DAPMemoryReadResult r;
        char addrStr[32];
        snprintf(addrStr, sizeof(addrStr), "0x%llx", (unsigned long long)address);
        r.address = addrStr;
        r.data = buf;
        r.text = std::string(buf.begin(), buf.end());

        if (onMemoryRead) onMemoryRead(r);
    }

    // ============================================================
    // スレッド一覧取得
    // ============================================================
    void getThreads() {
        if (!process_.IsValid()) return;

        uint32_t numThreads = process_.GetNumThreads();
        if (onOutput) {
            for (uint32_t i = 0; i < numThreads; i++) {
                lldb::SBThread t = process_.GetThreadAtIndex(i);
                std::string name = t.GetName() ? t.GetName() : "";
                onOutput("thread #" + std::to_string(t.GetThreadID()) +
                    (name.empty() ? "" : " \"" + name + "\""));
            }
        }
    }

    // ============================================================
    // 切断・解放
    // ============================================================
    void disconnect() {
        if (process_.IsValid()) {
            process_.Kill();
        }
    }

    void dispose() {
        isRunning_ = false;

        if (process_.IsValid()) {
            process_.Kill();
        }

        if (eventThread_.joinable()) {
            eventThread_.join();
        }

        if (debugger_.IsValid()) {
            lldb::SBDebugger::Destroy(debugger_);
        }

        lldb::SBDebugger::Terminate();
    }

    // ============================================================
    // 直接アクセス（拡張用）
    // ============================================================
    lldb::SBDebugger& getDebugger() { return debugger_; }
    lldb::SBTarget& getTarget() { return target_; }
    lldb::SBProcess& getProcess() { return process_; }

private:
    lldb::SBDebugger debugger_;
    lldb::SBTarget target_;
    lldb::SBProcess process_;
    lldb::SBListener listener_;

    std::atomic<bool> isRunning_{false};
    std::thread eventThread_;

    // ファイル毎のブレークポイントID管理
    std::map<std::string, std::vector<int>> fileBreakpoints_;

    // ============================================================
    // スレッド取得ヘルパー
    // ============================================================
    lldb::SBThread getThread(int threadId) {
        if (!process_.IsValid()) return lldb::SBThread();
        if (threadId < 0) return process_.GetSelectedThread();

        uint32_t numThreads = process_.GetNumThreads();
        for (uint32_t i = 0; i < numThreads; i++) {
            lldb::SBThread t = process_.GetThreadAtIndex(i);
            if ((int)t.GetThreadID() == threadId) return t;
        }
        return process_.GetSelectedThread();
    }

    // ============================================================
    // SBValue → DAPVariable 変換
    // ============================================================
    DAPVariable sbValueToVariable(lldb::SBValue val) {
        DAPVariable var;
        var.name = val.GetName() ? val.GetName() : "";
        var.value = val.GetValue() ? val.GetValue() : "";
        if (var.value.empty() && val.GetSummary()) {
            var.value = val.GetSummary();
        }
        var.type = val.GetTypeName() ? val.GetTypeName() : "";
        var.variablesReference = val.GetNumChildren();
        return var;
    }

    // ============================================================
    // StopReason → 文字列
    // ============================================================
    static const char* stopReasonToString(lldb::StopReason reason) {
        switch (reason) {
        case lldb::eStopReasonBreakpoint:    return "breakpoint";
        case lldb::eStopReasonWatchpoint:    return "watchpoint";
        case lldb::eStopReasonSignal:        return "signal";
        case lldb::eStopReasonException:     return "exception";
        case lldb::eStopReasonTrace:         return "step";
        case lldb::eStopReasonPlanComplete:  return "step";
        case lldb::eStopReasonExec:          return "exec";
        default:                             return "pause";
        }
    }

    // ============================================================
    // イベントループ（別スレッド）
    // ============================================================
    void eventLoop() {
        while (isRunning_) {
            lldb::SBEvent event;
            if (!listener_.WaitForEvent(1, event)) continue;

            if (!lldb::SBProcess::EventIsProcessEvent(event)) continue;

            lldb::StateType state = lldb::SBProcess::GetStateFromEvent(event);

            switch (state) {
            case lldb::eStateStopped: {
                // stdout/stderr出力があれば取得
                flushProcessOutput();

                lldb::SBThread thread = process_.GetSelectedThread();
                DAPStoppedEvent evt;
                evt.threadId = thread.IsValid() ? (int)thread.GetThreadID() : 1;
                evt.allThreadsStopped = true;
                evt.reason = thread.IsValid()
                    ? stopReasonToString(thread.GetStopReason())
                    : "pause";

                if (onOutput) onOutput("stopped: " + evt.reason +
                    " (thread " + std::to_string(evt.threadId) + ")");
                if (onStopped) onStopped(evt);
                break;
            }

            case lldb::eStateExited: {
                flushProcessOutput();
                int exitStatus = process_.GetExitStatus();
                if (onOutput) onOutput("process exited with status " + std::to_string(exitStatus));
                if (onTerminated) onTerminated();
                break;
            }

            case lldb::eStateCrashed: {
                flushProcessOutput();
                if (onOutput) onOutput("process crashed");
                DAPStoppedEvent evt;
                evt.reason = "exception";
                evt.text = "Process crashed";
                evt.allThreadsStopped = true;
                if (onStopped) onStopped(evt);
                break;
            }

            case lldb::eStateRunning:
            case lldb::eStateStepping:
                break;

            default:
                if (onOutput) {
                    onOutput("process state: " +
                        std::string(lldb::SBDebugger::StateAsCString(state)));
                }
                break;
            }
        }
    }

    // ============================================================
    // プロセスのstdout/stderr出力を取得
    // ============================================================
    void flushProcessOutput() {
        if (!process_.IsValid()) return;

        char buf[4096];
        size_t n;

        // stdout
        while ((n = process_.GetSTDOUT(buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            if (onOutput) onOutput(std::string("[stdout] ") + buf);
        }

        // stderr
        while ((n = process_.GetSTDERR(buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            if (onOutput) onOutput(std::string("[stderr] ") + buf);
        }
    }
};
