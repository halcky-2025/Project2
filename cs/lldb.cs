using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Cyclon
{
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Text.Json;
    using System.Text.Json.Nodes;
    using System.Threading.Tasks;
    using static System.Formats.Asn1.AsnWriter;

    public class LLDBDAPClient : IDisposable
    {
        private Process process;
        private StreamWriter stdin;
        private StreamReader stdout;
        private int sequenceNumber = 1;
        private bool isRunning = false;

        // イベント
        public event EventHandler<string> OutputReceived;
        public event EventHandler<StoppedEventArgs> Stopped;
        public event EventHandler Initialized;
        public event EventHandler Terminated;
        public event EventHandler<StackFrame[]> StackTraceReceived;
        public event EventHandler<Variable2[]> VariablesReceived;
        public event EventHandler<Breakpoint[]> BreakpointsSet;
        public event EventHandler<Scope[]> ScopesReceived;
        public event EventHandler<EvaluateResult> EvaluateResult;
        public event EventHandler<BreakpointChangedEventArgs> BreakpointChanged;
        public event EventHandler<MemoryReadResult> MemoryRead;
        // 起動
        public void Start(string lldbDapPath = @"clang2\lldb-dap.exe")
        {
            process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = lldbDapPath,
                    Arguments = "",
                    RedirectStandardInput = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                }
            };
            process.ErrorDataReceived += (sender, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    OutputReceived?.Invoke(this, $"[stderr] {e.Data}");
                }
            };

            process.Start();
            stdin = process.StandardInput;
            stdout = process.StandardOutput;
            process.BeginErrorReadLine();
            isRunning = true;

            // 非同期でメッセージを受信
            Task.Run(() => ReadMessages());
        }

        // 初期化
        public int Initialize(bool isVsdbg = false)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "initialize",
                arguments = new
                {
                    clientID = "myCSharpDebugger",
                    clientName = "My C# Debugger",
                    adapterID = isVsdbg ? "cppvsdbg" : "lldb",
                    linesStartAt1 = true,
                    columnsStartAt1 = true,
                    pathFormat = "path",
                    supportsVariableType = true,
                    supportsVariablePaging = true,
                    supportsRunInTerminalRequest = false
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // プログラムを起動
        public int Launch(string program, string[] args = null, string cwd = null)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "launch",
                arguments = new
                {
                    program = program,
                    args = args ?? Array.Empty<string>(),
                    cwd = cwd ?? Path.GetDirectoryName(program),
                    stopOnEntry = true,
                    disableASLR = true,
                    disableSTDIO = false,
                    shellExpandArguments = false,
                    sourceMap = new Dictionary<string, string>
                    {
                        { ".", @"C:\Users\hayah\source\repos\Project2\Project2"}
                    }
                }
            };

            SendRequest(request);
            return request.seq;
        }
        public void Disconnect()
        {

            process.Close();
        }

        // 設定完了通知（初期化後に必須）
        public int ConfigurationDone()
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "configurationDone"
            };

            SendRequest(request);
            return request.seq;
        }
        // ブレークポイント設定
        public int SetBreakpoints(string file, int[] lines)
        {
            var breakpoints = lines.Select(line => new { line = line }).ToArray();

            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "setBreakpoints",
                arguments = new
                {
                    source = new { path = file },
                    breakpoints = breakpoints
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // 関数ブレークポイント設定
        public int SetFunctionBreakpoint(string functionName)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "setFunctionBreakpoints",
                arguments = new
                {
                    breakpoints = new[]
                    {
                    new { name = functionName }
                }
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // 例外ブレークポイント設定
        public int SetExceptionBreakpoints(bool breakOnThrow = true)
        {
            var filters = new List<string>();
            if (breakOnThrow) filters.Add("cpp_throw");

            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "setExceptionBreakpoints",
                arguments = new
                {
                    filters = filters.ToArray()
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // 実行継続
        public int Continue(int threadId = 1)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "continue",
                arguments = new
                {
                    threadId = threadId,
                    singleThread = false
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // Step Over（次の行へ）
        public int StepOver(int threadId = 1)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "next",
                arguments = new
                {
                    threadId = threadId,
                    granularity = "statement"
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // Step Into（関数内に入る）
        public int StepIn(int threadId = 1)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "stepIn",
                arguments = new
                {
                    threadId = threadId,
                    granularity = "statement"
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // Step Out（関数から抜ける）
        public int StepOut(int threadId = 1)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "stepOut",
                arguments = new
                {
                    threadId = threadId,
                    granularity = "statement"
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // 一時停止
        public int Pause(int threadId = 1)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "pause",
                arguments = new { threadId = threadId }
            };

            SendRequest(request);
            return request.seq;
        }

        // スレッド一覧取得
        public int GetThreads()
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "threads"
            };

            SendRequest(request);
            return request.seq;
        }

        // スタックトレース取得
        public int GetStackTrace(int threadId = 1)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "stackTrace",
                arguments = new
                {
                    threadId = threadId,
                    startFrame = 0,
                    levels = 20
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // スコープ取得（ローカル変数など）
        public int GetScopes(int frameId)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "scopes",
                arguments = new { frameId = frameId }
            };

            SendRequest(request);
            return request.seq;
        }

        // 変数取得
        public int GetVariables(int variablesReference)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "variables",
                arguments = new
                {
                    variablesReference = variablesReference,
                    format = new { hex = false }
                }
            };

            SendRequest(request);
            return request.seq;
        }
        // メモリ読み取り
        public int ReadMemory(string memoryReference, int count)
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "readMemory",
                arguments = new
                {
                    memoryReference = memoryReference,
                    count = count
                }
            };

            SendRequest(request);
            return request.seq;
        }
        // 式を評価
        public int Evaluate(string expression, int frameId = 0, string context = "watch")
        {
            var request = new
            {
                seq = sequenceNumber++,
                type = "request",
                command = "evaluate",
                arguments = new
                {
                    expression = expression,
                    frameId = frameId,
                    context = context // "watch", "repl", "hover"
                }
            };

            SendRequest(request);
            return request.seq;
        }

        // リクエスト送信
        private void SendRequest(object request)
        {
            try
            {
                string json = JsonSerializer.Serialize(request, new JsonSerializerOptions
                {
                    WriteIndented = false
                });

                byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
                string message = $"Content-Length: {jsonBytes.Length}\r\n\r\n{json}";

                stdin.Write(message);
                stdin.Flush();

                OutputReceived?.Invoke(this, $"→ {request.GetType().GetProperty("command")?.GetValue(request)}{request.GetType().GetProperty("seq")?.GetValue(request)}");
            }
            catch (Exception ex)
            {
                OutputReceived?.Invoke(this, $"エラー: {ex.Message}");
            }
        }

        // メッセージ受信ループ
        private void ReadMessages()
        {
            var baseStream = process.StandardOutput.BaseStream;

            while (isRunning && !process.HasExited)
            {
                try
                {
                    // ヘッダーを1行ずつ読む（ASCIIなので問題ない）
                    string header = ReadLineFromStream(baseStream);
                    if (string.IsNullOrEmpty(header)) continue;

                    if (!header.StartsWith("Content-Length:"))
                    {
                        continue;
                    }

                    int contentLength = int.Parse(header.Split(':')[1].Trim());

                    // 空行を読み飛ばす
                    ReadLineFromStream(baseStream);

                    // ★ バイト配列として正確に読み込む
                    byte[] jsonBytes = new byte[contentLength];
                    int totalRead = 0;

                    while (totalRead < contentLength)
                    {
                        int bytesRead = baseStream.Read(jsonBytes, totalRead, contentLength - totalRead);
                        if (bytesRead == 0)
                        {
                            throw new Exception("ストリームが予期せず終了しました");
                        }
                        totalRead += bytesRead;
                    }

                    // ★ UTF-8としてデコード
                    string json = Encoding.UTF8.GetString(jsonBytes);

                    var message = JsonNode.Parse(json)?.AsObject();
                    if (message == null) continue;

                    string type = message["type"]?.ToString();

                    if (type == "event")
                    {
                        HandleEvent(message);
                    }
                    else if (type == "response")
                    {
                        HandleResponse(message);
                    }
                }
                catch (JsonException ex)
                {
                    if (isRunning)
                    {
                        OutputReceived?.Invoke(this, $"JSON パースエラー: {ex.Message}");
                    }
                }
                catch (Exception ex)
                {
                    if (isRunning)
                    {
                        OutputReceived?.Invoke(this, $"読み込みエラー: {ex.Message}");
                    }
                }
            }
        }

        // ヘルパーメソッド：BaseStreamから1行読む
        private string ReadLineFromStream(Stream stream)
        {
            var line = new List<byte>();
            int b;

            while ((b = stream.ReadByte()) != -1)
            {
                if (b == '\n')
                {
                    // \r\n の \r を削除
                    if (line.Count > 0 && line[line.Count - 1] == '\r')
                    {
                        line.RemoveAt(line.Count - 1);
                    }
                    return Encoding.ASCII.GetString(line.ToArray());
                }
                line.Add((byte)b);
            }

            return null;
        }

        // イベント処理
        private void HandleEvent(JsonObject evt)
        {
            string eventType = evt["event"]?.ToString();
            var body = evt["body"]?.AsObject();

            OutputReceived?.Invoke(this, $"← イベント: {eventType}");

            switch (eventType)
            {
                case "initialized":
                    Initialized?.Invoke(this, EventArgs.Empty);
                    break;

                case "stopped":
                    var reason = body?["reason"]?.ToString();
                    var threadId = body?["threadId"]?.GetValue<int>() ?? 1;
                    var allThreadsStopped = body?["allThreadsStopped"]?.GetValue<bool>() ?? false;
                    var text = body?["description"]?.ToString();

                    Stopped?.Invoke(this, new StoppedEventArgs
                    {
                        Reason = reason,
                        ThreadId = threadId,
                        AllThreadsStopped = allThreadsStopped,
                        Text = text
                    });
                    break;
                case "breakpoint":
                    // ★ ここを実装
                    var bpReason = body?["reason"]?.ToString(); // "changed", "new", "removed"
                    var breakpoint = body?["breakpoint"]?.AsObject();

                    if (breakpoint != null)
                    {
                        var id = breakpoint["id"]?.GetValue<int>() ?? 0;
                        var verified = breakpoint["verified"]?.GetValue<bool>() ?? false;
                        var message = breakpoint["message"]?.ToString();
                        var line = breakpoint["line"]?.GetValue<int>() ?? 0;
                        var source = breakpoint["source"]?["path"]?.ToString();

                        OutputReceived?.Invoke(this,
                            $"Breakpoint {bpReason}: id={id}, verified={verified}, line={line}");

                        BreakpointChanged?.Invoke(this, new BreakpointChangedEventArgs
                        {
                            Reason = bpReason,
                            Id = id,
                            Verified = verified,
                            Line = line,
                            Source = source,
                            Message = message
                        });
                    }
                    break;

                case "terminated":
                    Terminated?.Invoke(this, EventArgs.Empty);
                    break;

                case "output":
                    var category = body?["category"]?.ToString();
                    var output = body?["output"]?.ToString();
                    OutputReceived?.Invoke(this, $"[{category}] {output}");
                    break;
            }
        }

        // レスポンス処理
        private void HandleResponse(JsonObject response)
        {
            string command = response["command"]?.ToString();
            bool success = response["success"]?.GetValue<bool>() ?? false;
            var body = response["body"]?.AsObject();
            int requestSeq = response["request_seq"]?.GetValue<int>() ?? 0;

            OutputReceived?.Invoke(this, $"← 応答: {command} ({(success ? "成功" : "失敗")})");

            if (!success)
            {
                var message = response["message"]?.ToString();
                OutputReceived?.Invoke(this, $"エラー: {message}");
                return;
            }

            switch (command)
            {
                case "stackTrace":
                    var framesArray = body?["stackFrames"]?.AsArray();
                    if (framesArray != null)
                    {
                        var frames = framesArray.Select(f => new StackFrame
                        {
                            Id = f["id"]?.GetValue<int>() ?? 0,
                            Name = f["name"]?.ToString(),
                            Source = f["source"]?["path"]?.ToString(),
                            Line = f["line"]?.GetValue<int>() ?? 0,
                            Column = f["column"]?.GetValue<int>() ?? 0,
                            requestSeq = requestSeq
                        }).ToArray();

                        StackTraceReceived?.Invoke(this, frames);
                    }
                    break;

                // ★ scopes レスポンス処理を追加
                case "scopes":
                    var scopesArray = body?["scopes"]?.AsArray();
                    if (scopesArray != null)
                    {
                        var scopes = scopesArray.Select(s => new Scope
                        {
                            Name = s["name"]?.ToString(),
                            VariablesReference = s["variablesReference"]?.GetValue<int>() ?? 0,
                            Expensive = s["expensive"]?.GetValue<bool>() ?? false,
                            requestSeq = requestSeq
                        }).ToArray();

                        ScopesReceived?.Invoke(this, scopes);
                    }
                    break;

                case "variables":
                    var variablesArray = body?["variables"]?.AsArray();
                    if (variablesArray != null)
                    {
                        var variables = variablesArray.Select(v => new Variable2
                        {
                            Name = v["name"]?.ToString(),
                            Value = v["value"]?.ToString(),
                            Type = v["type"]?.ToString(),
                            VariablesReference = v["variablesReference"]?.GetValue<int>() ?? 0,
                            requestSeq = requestSeq
                        }).ToArray();

                        VariablesReceived?.Invoke(this, variables);
                    }
                    break;

                case "setBreakpoints":
                    var breakpointsArray = body?["breakpoints"]?.AsArray();
                    if (breakpointsArray != null)
                    {
                        var breakpoints = breakpointsArray.Select(b => new Breakpoint
                        {
                            Id = b["id"]?.GetValue<int>() ?? 0,
                            Verified = b["verified"]?.GetValue<bool>() ?? false,
                            Line = b["line"]?.GetValue<int>() ?? 0,
                            Source = b["source"]?["path"]?.ToString(),
                            Message = b["message"]?.ToString(),
                            requestSeq = requestSeq
                        }).ToArray();

                        BreakpointsSet?.Invoke(this, breakpoints);
                    }
                    break;
                case "readMemory":
                    var address = body?["address"]?.ToString();
                    var data = body?["data"]?.ToString();  // Base64 エンコードされたバイト列

                    if (data != null)
                    {
                        byte[] bytes = Convert.FromBase64String(data);
                        string str = Encoding.UTF8.GetString(bytes);  // \0 含めて全部取れる

                        // イベントで通知
                        MemoryRead?.Invoke(this, new MemoryReadResult
                        {
                            Address = address,
                            Data = bytes,
                            Text = str,
                            requestSeq = requestSeq
                        });
                    }
                    break;
                case "evaluate":
                    var result = body?["result"]?.ToString();
                    var variablesReference = body?["variablesReference"]?.GetValue<int>() ?? 0;

                    EvaluateResult?.Invoke(this, new EvaluateResult
                    {
                        Result = result,
                        VariablesReference = variablesReference,
                        requestSeq = requestSeq
                    });
                    break;
            }
        }

        public void Dispose()
        {
            isRunning = false;

            try
            {
                Disconnect();
                process?.WaitForExit(1000);
                process?.Kill();
            }
            catch { }

            stdin?.Dispose();
            stdout?.Dispose();
            process?.Dispose();
        }
    }
    public class Scope
    {
        public string Name { get; set; }
        public int VariablesReference { get; set; }
        public bool Expensive { get; set; }

        public int requestSeq { get; set; }
    }

    public class EvaluateResult
    {
        public string Result { get; set; }
        public int VariablesReference { get; set; }

        public int requestSeq { get; set; }
    }
    // データクラス
    public class StoppedEventArgs : EventArgs
    {
        public string Reason { get; set; }
        public int ThreadId { get; set; }
        public bool AllThreadsStopped { get; set; }
        public string Text { get; set; }
    }

    public class StackFrame
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string Source { get; set; }
        public int Line { get; set; }
        public int Column { get; set; }

        public int requestSeq { get; set; }
    }

    public class Variable2
    {
        public String Head { get; set; }
        public string Name { get; set; }
        public string Value { get; set; }
        public string Type { get; set; }
        public int VariablesReference { get; set; }

        public int requestSeq { get; set; }
        public List<Variable2> Children { get; set; } = new List<Variable2>();
    }

    public class Breakpoint
    {
        public int Id { get; set; }
        public bool Verified { get; set; }
        public int Line { get; set; }
        public string Message { get; set; }

        public int requestSeq { get; set; }
        public String Source { get; set; }
    }
    public class BreakpointChangedEventArgs : EventArgs
    {
        public string Reason { get; set; }
        public int Id { get; set; }
        public bool Verified { get; set; }
        public int Line { get; set; }
        public string Source { get; set; }
        public string Message { get; set; }
    }
    public class MemoryReadResult
    {
        public string Address { get; set; }
        public byte[] Data { get; set; }
        public string Text { get; set; }
        public int requestSeq { get; set; }
    }
}
