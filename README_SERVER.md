# HopStar Linux Server (main7.cpp)

main7.cppは、フル機能のWebRTCメディアサーバーです。ディスプレイの有無を自動検出し、GUIモードまたはヘッドレスモードで起動します。

## 主な機能

### コア機能
- **自動ディスプレイ検出** - 起動時にX11/Waylandをチェックし、自動でモード選択
- **GUIサポート（オプション）** - SDL3/bgfxによるGUI（ディスプレイがある場合）
- **コマンドキューシステム** - HTTPリクエストをスレッドセーフに処理
- **RESTful HTTP API** - サーバー機能を外部から制御
- **FileEngine** - クロスプラットフォームファイル管理
- **SQLite** - データベースサポート

### WebRTC機能（オプション）
- **WebSocketシグナリング** - SDP/ICE交換サーバー
- **WebRTC** - ビデオ/オーディオ/データチャネル
- **メディア処理パイプライン** - FFmpeg + PyTorchによるリアルタイム処理
- **録画機能** - ストリームの録画/保存
- **マルチピア対応** - 複数クライアントの同時接続

### AI/ML機能（オプション）
- **PyTorch/CUDA** - GPU加速推論
- **ONNX Runtime** - 最適化されたモデル実行
- **リアルタイム解析** - ビデオストリームのリアルタイム処理

### HTTP/HTTPS機能
- **HTTP Client** - 送信用HTTPクライアント（getaddrinfo使用、IPv4/IPv6対応）
- **HTTPS Client** - libcurl使用の完全HTTPS対応（オプション）
- **非同期処理** - スレッドプールで並列実行

## 必須依存関係

1. **CMake** (3.21以上)
2. **GCC/G++** (C++20対応)
3. **SQLite3**
4. **libcoro** (コルーチンライブラリ)
5. **nlohmann/json**

## オプション依存関係

### GUI機能
- **SDL3** - ウィンドウ管理
- **bgfx** - グラフィックスレンダリング
- **X11** または **Wayland** - ディスプレイサーバー

### AI/ML機能
- **PyTorch** (LibTorch) - ディープラーニング推論
- **ONNX Runtime** - 最適化されたモデル実行

### メディア処理
- **FFmpeg** - エンコード/デコード

### WebRTC機能
- **libwebsockets** - WebSocketシグナリングサーバー
- **libdatachannel** - WebRTC実装（ビデオ/オーディオ/データチャネル）

### HTTP/HTTPS機能
- **libcurl** - HTTPS対応HTTPクライアント（推奨）
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libcurl4-openssl-dev

  # Fedora/RHEL
  sudo dnf install libcurl-devel
  ```

## ビルド方法

### ビルドスクリプトを使用（推奨）

```bash
# 基本ビルド（HTTP API + HTTPS対応）
./build_server.sh

# HTTPS無効（HTTP のみ）
./build_server.sh --no-curl

# GUI無効（ヘッドレス専用）
./build_server.sh --no-gui

# WebRTC対応サーバー（HTTPS付き）
./build_server.sh --websocket --webrtc

# AI機能付きWebRTCサーバー
./build_server.sh --websocket --webrtc --torch

# フル機能（全オプション有効）
./build_server.sh --torch --onnx --websocket --webrtc
```

### CMakeで直接ビルド

#### 1. 基本ビルド（最小構成）

```bash
cd linux
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make HopStar_Server
```

#### 2. WebRTC対応サーバー

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DHOPSTAR_ENABLE_WEBSOCKET=ON \
    -DHOPSTAR_ENABLE_WEBRTC=ON \
    -DHOPSTAR_ENABLE_FFMPEG=ON
make HopStar_Server
```

#### 3. フル機能サーバー

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DHOPSTAR_ENABLE_TORCH=ON \
    -DHOPSTAR_ENABLE_ONNX=ON \
    -DHOPSTAR_ENABLE_FFMPEG=ON \
    -DHOPSTAR_ENABLE_WEBSOCKET=ON \
    -DHOPSTAR_ENABLE_WEBRTC=ON \
    -DHOPSTAR_ENABLE_GUI=ON
make HopStar_Server
```

## 実行方法

### デフォルトポートで起動（root権限必要）

```bash
cd linux/build/bin
sudo ./HopStar_Server
```

デフォルトポート（標準）：
- HTTP: 80
- WebSocket (WSS): 443

**注意:** ポート80と443は特権ポート（1-1023）のため、root権限が必要です。

### 非rootユーザーで起動（開発環境）

非特権ポート（1024以上）を使用してroot権限なしで起動：

```bash
# 開発用ポートで起動
./HopStar_Server 8080 8443
```

または、Linuxのcapabilitiesを使用：

```bash
# バイナリに権限を付与（一度だけ実行）
sudo setcap CAP_NET_BIND_SERVICE=+eip ./HopStar_Server

# 以降はrootなしで起動可能
./HopStar_Server
```

### カスタムポートで起動

```bash
# HTTP_port WebSocket_port
./HopStar_Server 3000 3443
```

## HTTP API エンドポイント

サーバーは以下のRESTful APIエンドポイントを提供します：

### 1. ステータス確認
```bash
curl http://localhost:8080/status
```
レスポンス例：
```json
{"status":"ok","mode":"GUI","gui_active":true}
```

### 2. Ping (接続テスト)
```bash
curl http://localhost:8080/ping
```
レスポンス例：
```json
{"status":"ok","message":"pong"}
```

### 3. GUIメッセージ送信（GUIモードのみ）
```bash
curl "http://localhost:8080/gui/message?message=Hello"
```
レスポンス例：
```json
{"status":"ok","message":"GUI message sent"}
```

### 4. PyTorchテスト（PyTorch有効時のみ）
```bash
curl http://localhost:8080/torch/test
```
レスポンス例：
```json
{"status":"ok","tensor":"..."}
```

### 5. シャットダウン
```bash
curl http://localhost:8080/shutdown
```
レスポンス例：
```json
{"status":"ok","message":"Shutting down"}
```

## サーバーテスト

基本的な接続テスト：

```bash
curl http://localhost:8080/ping
```

## 停止方法

サーバーを停止するには、`Ctrl+C` を押してください。

## ディレクトリ構成

- データベース: `~/.local/share/hopstar/server.db`
- 設定ファイル等: `~/.local/share/hopstar/`

## トラブルシューティング

### エラー: "Bind failed on port 80" または "Permission denied"

**原因:** ポート80/443は特権ポートのため、root権限が必要です。

**解決方法1:** rootで実行

```bash
sudo ./HopStar_Server
```

**解決方法2:** CAP_NET_BIND_SERVICEを付与（推奨）

```bash
sudo setcap CAP_NET_BIND_SERVICE=+eip ./HopStar_Server
./HopStar_Server  # rootなしで実行可能
```

**解決方法3:** 非特権ポートを使用

```bash
./HopStar_Server 8080 8443
```

### ポートが使用中

他のサービスがポートを使用しています。使用中のポートを確認：

```bash
sudo lsof -i :80
sudo lsof -i :443
```

別のポートを指定：

```bash
./HopStar_Server 3000 3443
```

### ビルドエラー: coro/coro.hpp が見つからない

libcoroがインストールされていません。linux/deps/libcoroディレクトリを確認してください。

### ビルドエラー: PyTorch/ONNX Runtime が見つからない

オプション機能なので、無効にしてビルドできます：

```bash
cmake .. -DHOPSTAR_ENABLE_TORCH=OFF -DHOPSTAR_ENABLE_ONNX=OFF
```

### WebSocket/WebRTCが動作しない

該当機能を有効にしてビルドしてください：

```bash
./build_server.sh --websocket --webrtc
```

## アーキテクチャ

### システム概要

```
┌───────────────────────────────────────────────────────────────────┐
│                      HopStar WebRTC Server                        │
│                    Hybrid Thread Pool Architecture                │
├───────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐            │
│  │ HTTP Server  │  │ WebSocket    │  │ HTTP Client  │            │
│  │ (REST API)   │  │ (Signaling)  │  │ (Outgoing)   │            │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘            │
│         │                  │                  │                     │
│         └──────────────────┴──────────────────┘                    │
│                            ↓                                        │
│                  ┌──────────────────────┐                          │
│                  │  Fast Task Pool      │                          │
│                  │  (8 threads)         │                          │
│                  │  HTTP, WebSocket     │                          │
│                  └──────────────────────┘                          │
│                                                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐            │
│  │ File I/O     │  │ Database     │  │ Heavy Ops    │            │
│  │ (Async)      │  │ Operations   │  │              │            │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘            │
│         │                  │                  │                     │
│         └──────────────────┴──────────────────┘                    │
│                            ↓                                        │
│                  ┌──────────────────────┐                          │
│                  │  Blocking I/O Pool   │                          │
│                  │  (4 threads)         │                          │
│                  │  File I/O, DB ops    │                          │
│                  └──────────────────────┘                          │
│                                                                     │
│                  ┌──────────────────────┐                          │
│                  │  Command Queue       │                          │
│                  │  (Thread-safe)       │                          │
│                  └──────────┬───────────┘                          │
│                             ↓                                       │
│                  ┌──────────────────────┐                          │
│                  │  Command Processor   │                          │
│                  │  (Single Thread)     │                          │
│                  └──────────┬───────────┘                          │
│                             │                                       │
│     ┌───────────────────────┼───────────────────────┐             │
│     ↓                       ↓                       ↓               │
│  ┌──────┐          ┌────────────┐          ┌─────────┐           │
│  │ GUI  │          │  WebRTC    │          │ Media   │            │
│  │      │          │  Manager   │          │ Proc.   │            │
│  └──────┘          └──────┬─────┘          └────┬────┘           │
│                            │                     │                  │
│                            └──────────┬──────────┘                 │
│                                       ↓                             │
│                            ┌──────────────────────┐                │
│                            │ Media Processing     │                │
│                            │ (Single Thread)      │                │
│                            │ FFmpeg + PyTorch     │                │
│                            │ Encode/Decode        │                │
│                            │ Inference            │                │
│                            └──────────────────────┘                │
└───────────────────────────────────────────────────────────────────┘
```

### スレッドプールアーキテクチャ

HopStar サーバーは、ハイブリッドスレッドプール構成を採用しています。

#### Fast Task Pool（8スレッド）
**用途:** 軽量で高速な操作
- HTTP リクエスト処理
- WebSocket メッセージ処理
- 送信用 HTTP クライアントリクエスト
- 非ブロッキング操作全般

**特徴:**
- 多くのスレッドで並列処理
- 低レイテンシー
- CPU バウンドなタスク向け

#### Blocking I/O Pool（4スレッド）
**用途:** ブロッキング I/O 操作
- ファイル読み書き（AsyncFileIO 経由）
- データベース操作
- 長時間実行される重い処理

**特徴:**
- 中程度のスレッド数
- ブロッキング操作を分離
- Fast Task Pool の性能を保護

#### Media Processing（単一スレッド）
**用途:** メディア処理パイプライン
- FFmpeg エンコード/デコード
- PyTorch 推論
- ビデオ/オーディオ処理
- 録画機能

**特徴:**
- 専用スレッド
- メディアフレームの順序保証
- GPU アクセスの競合回避

#### なぜハイブリッド構成？

1. **Fast Task Pool**: リクエストごとにスレッドを作成せず、プールで再利用
   - スレッド作成/破棄のオーバーヘッド削減
   - 同時接続数が多くてもスレッド数は一定

2. **Blocking I/O Pool**: ブロッキング操作を分離
   - ファイル I/O でメインプールがブロックされない
   - DB 操作が HTTP レスポンスを遅延させない

3. **Media Processing**: 専用スレッドで独立
   - GPU コンテキストの管理が容易
   - フレーム処理の順序保証
   - 他の操作に影響を与えない

### モジュール構成

#### server_common.h
- 共通定義（PeerInfo, MediaFrame, ServerConfig）
- ロギング機能
- コールバック型定義

#### thread_pool.h
- 汎用スレッドプール実装
- タスクキュー管理
- 非同期タスク実行
- Fast Task Pool と Blocking I/O Pool で使用

#### file_io_async.h
- 非同期ファイル I/O 操作
- Blocking I/O Pool を使用
- ファイル読み書き、削除、情報取得
- ディレクトリ操作

#### http_client.h
- HTTP クライアント（送信用）
- GET, POST, PUT, DELETE サポート
- Fast Task Pool で実行
- 基本的な HTTP/1.1 実装
- getaddrinfo() 使用（IPv4/IPv6対応、スレッドセーフ）
- HTTPS非対応（HTTP のみ）

#### http_client_curl.h
- HTTP/HTTPS クライアント（libcurl版）
- 完全なHTTPS対応（SSL/TLS）
- HTTP/2 対応
- SSL証明書検証
- リダイレクト自動追従
- libcurl が利用可能な場合に自動選択

#### websocket_client.h
- WebSocket クライアント（送信用）
- libwebsockets 使用
- 接続、切断、メッセージ送受信
- コールバックベース

#### command_queue.h
- コマンドキュー（スレッドセーフ）
- コマンドプロセッサ（ワーカースレッド）
- コマンド型定義

#### http_server.h
- HTTP RESTful API サーバー
- リクエストパーサー
- レスポンスビルダー
- Fast Task Pool で並列処理

#### websocket_server.h
- WebSocketシグナリングサーバー
- SDP/ICE交換処理
- ルーム管理

#### webrtc_manager.h
- WebRTC PeerConnection管理
- ビデオ/オーディオ/データチャネル
- ピア接続/切断処理

#### media_processor.h
- FFmpeg エンコーダー/デコーダー
- PyTorch 推論エンジン
- メディア処理パイプライン
- 録画機能

### 起動フロー

1. **ディスプレイ検出** - X11/Waylandサーバーへの接続テスト
2. **プラットフォーム初期化** - FileEngine, GC, SQLite
3. **ハイブリッドスレッドプール初期化**
   - Fast Task Pool (8スレッド) - HTTP/WebSocket処理用
   - Blocking I/O Pool (4スレッド) - ファイルI/O/DB操作用
   - Async File I/O Manager
   - HTTP Client (送信用)
4. **サーバーコンポーネント初期化**
   - CommandQueue & Processor
   - HTTP Server (Fast Task Pool使用)
   - WebSocket Server (オプション)
   - WebRTC Manager (オプション)
   - Media Processor (オプション、専用スレッド)
5. **コールバック接続** - 各コンポーネント間の通信設定
6. **GUI初期化**（ディスプレイがある場合のみ）
7. **メインイベントループ**
   - GUIモード: SDL イベント処理
   - ヘッドレスモード: スリープループ

## 開発メモ

### main.cppとの違い

- Windows専用コード（windows.h、HMODULE等）を削除
- ディスプレイ自動検出機能を追加
- CUDA DLLのロードを`dlopen()`に変更
- HTTPサーバー + コマンドキューシステムを追加
- ハイブリッドスレッドプールアーキテクチャを実装
- 非同期ファイル I/O システムを追加
- HTTP/WebSocket クライアント機能を追加
- すべての機能をコンパイル時オプションに対応
- GUI機能をオプション化（`HOPSTAR_ENABLE_GUI`）

### 新機能の使用方法

#### AsyncFileIO の使用例
```cpp
// main7.cpp で初期化済み
AsyncFileIO asyncFileIO(blockingIOPool);

// 非同期ファイル読み込み
auto future = asyncFileIO.readFile("/path/to/file.txt");
auto result = future.get();  // ブロッキング待機
if (result.success) {
    // result.data を使用
}

// 非同期ファイル書き込み
std::vector<uint8_t> data = {...};
auto writeFuture = asyncFileIO.writeFile("/path/to/output.dat", data);
auto writeResult = writeFuture.get();
```

#### HttpClient の使用例
```cpp
// main7.cpp で初期化済み
// libcurl有効: HttpClientCurl (HTTPS対応)
// libcurl無効: HttpClient (HTTP のみ)

// 非同期 GET リクエスト（HTTP）
auto future = httpClient.get("http://example.com/api/data");
auto response = future.get();
if (response.success && response.status_code == 200) {
    // response.body を使用
}

// HTTPS GET リクエスト（libcurl有効時のみ）
auto httpsFuture = httpClient.get("https://api.github.com/users/octocat");
auto httpsResponse = httpsFuture.get();
if (httpsResponse.success) {
    // HTTPS レスポンス処理
}

// POST リクエスト
std::map<std::string, std::string> headers = {
    {"Content-Type", "application/json"}
};
auto postFuture = httpClient.post("https://example.com/api/submit",
                                   "{\"key\":\"value\"}", headers);
```

#### WebSocketClient の使用例
```cpp
WebSocketClient wsClient("wss://example.com/socket");

wsClient.setOnConnected([]() {
    LOG_INFO("WebSocket connected!");
});

wsClient.setOnMessage([](const std::string& msg) {
    LOG_INFO("Received: " + msg);
});

wsClient.connect();
wsClient.send("{\"type\":\"hello\"}");
```

### 新しいコマンドの追加方法

1. **CommandType enumに追加**
```cpp
enum class CommandType {
    // ...
    YOUR_COMMAND,
};
```

2. **CommandProcessor::executeCommand()に処理を追加**
```cpp
case CommandType::YOUR_COMMAND: {
    // 処理を実装
    return "{\"status\":\"ok\"}";
}
```

3. **HTTPハンドラーにエンドポイントを追加**
```cpp
else if (req.path == "/your/endpoint") {
    auto cmd = std::make_unique<Command>(CommandType::YOUR_COMMAND, req.params);
    auto future = cmd->result.get_future();
    cmd_queue.enqueue(std::move(cmd));
    json_response = future.get();
}
```

## ライセンス

プロジェクトのライセンスに従います。
