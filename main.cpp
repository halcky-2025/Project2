#define _CRT_SECURE_NO_WARNINGS
#include <torch/torch.h>
#include <iostream>
#include <windows.h>
#undef max
#undef min
#include <onnxruntime_c_api.h>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libswresample/swresample.h>
}
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include "threed.h"
#include <cstdint>
#include "miniaudio.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define UTF8PROC_STATIC
#include <utf8proc.h>
#pragma comment(lib, "utf8proc_static.lib")
#include <filesystem>
namespace fs = std::filesystem;
namespace F = torch::nn::functional;
#include "gc.h"
#include "arr.h"
#include "sqlite3.h"
#include "db2.h"
#include "audio.h"
#include "elem.h"
bool cuda = false;
HMODULE torch_cuda_dll;
#include "opt.h"
#include "ugui.h"
#include "extorch.h"
//#include "db2.h"
SDL_HitTestResult
hit_test_cb(SDL_Window* win, const SDL_Point* pt, void* data)
{
    int w, h;
    const int border = 8;  // リサイズ可能エリアの厚さ(px)
    SDL_GetWindowSize(win, &w, &h);

    // 四隅
    if (pt->x < border && pt->y < border)               return SDL_HITTEST_RESIZE_TOPLEFT;
    if (pt->x > w - border && pt->y < border)           return SDL_HITTEST_RESIZE_TOPRIGHT;
    if (pt->x < border && pt->y > h - border)           return SDL_HITTEST_RESIZE_BOTTOMLEFT;
    if (pt->x > w - border && pt->y > h - border)       return SDL_HITTEST_RESIZE_BOTTOMRIGHT;

    // 辺
    if (pt->y < border)                                 return SDL_HITTEST_RESIZE_TOP;
    if (pt->y > h - border)                             return SDL_HITTEST_RESIZE_BOTTOM;
    if (pt->x < border)                                 return SDL_HITTEST_RESIZE_LEFT;
    if (pt->x > w - border)                             return SDL_HITTEST_RESIZE_RIGHT;

    // その他領域は通常のクライアント領域として扱う
    return SDL_HITTEST_NORMAL;
}
struct SimpleCNNImpl : torch::nn::Module {
public:
    torch::nn::Sequential net;
    SimpleCNNImpl() {
        if (!cuda) {
            torch_cuda_dll = LoadLibraryA("torch_cuda.dll");
            cuda = true;
        }
        net = torch::nn::Sequential(
            torch::nn::Conv2d(3, 16, 3),
            torch::nn::ReLU(),
            torch::nn::MaxPool2d(2),
            torch::nn::Conv2d(16, 32, 3),
            torch::nn::ReLU(),
            torch::nn::MaxPool2d(2),
            torch::nn::Flatten(),
            torch::nn::Linear(32 * 14 * 10, 64),
            torch::nn::ReLU(),
            torch::nn::Linear(64, 2)
        );
		register_module("net", net);
    }
	torch::Tensor forward(torch::Tensor x) {
		return net->forward(x);
	}
};
TORCH_MODULE(SimpleCNN);
size_t utf32_to_utf16(const utf8proc_int32_t* utf32, size_t len, uint16_t* utf16_out) {
    size_t j = 0;
    for (size_t i = 0; i < len; ++i) {
        uint32_t cp = utf32[i];
        if (cp <= 0xFFFF) {
            utf16_out[j++] = (uint16_t)cp;
        }
        else {
            cp -= 0x10000;
            utf16_out[j++] = (uint16_t)(0xD800 + (cp >> 10));
            utf16_out[j++] = (uint16_t)(0xDC00 + (cp & 0x3FF));
        }
    }
    return j;
}
int main() {
    magc = GC_init(1000 * 1000 * 1000);
    sqlite3* db;
    int rc = sqlite3_open("test.db", &db); if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }
    else {
        std::cout << "Opened database successfully" << std::endl;
    }
    const char* sql = "create table if not exists users(id integer primary key, name text);";
    char* err = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err << std::endl;
        sqlite3_free(err);
    }
    else {
        std::cout << "Table created successfully" << std::endl;
    }
    sqlite3_close(db);
    try {
        /*
        torch::manual_seed(1);
        int ne = 0;
        if (ne == 0) {
            model->to(torch::kCUDA);
            model->train();
            torch::optim::Adam optimizer(model->parameters(), torch::optim::AdamOptions(1e-3));
            fs::create_directories("inputs/faces");
            fs::create_directories("inputs/nonfaces");
            std::vector<torch::Tensor> input_tensors;
            std::vector<torch::Tensor> goal_tensors;
            for (const auto& entry : fs::directory_iterator("inputs/faces")) {
                std::string filename = entry.path().string();
                if (filename.substr(filename.find_last_of(".") + 1) == "png") {
                    int width, height, channels;
                    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
                    torch::Tensor img_tensor = torch::from_blob(data, { height, width, 3 }, torch::kUInt8)
                        .permute({ 2, 0, 1 }).to(torch::kFloat32).div(255.0).unsqueeze(0);
                    img_tensor = F::interpolate(img_tensor, F::InterpolateFuncOptions().size(std::vector<int64_t>({ 64, 48 })).mode(torch::kBilinear).align_corners(false));
                    img_tensor = img_tensor.squeeze(0).clone();
                    input_tensors.push_back(img_tensor);
                    torch::Tensor label = torch::tensor(1, torch::kLong);
                    goal_tensors.push_back(label);
                    stbi_image_free(data);
                }
            }
            for (const auto& entry : fs::directory_iterator("inputs/nonfaces")) {
                std::string filename = entry.path().string();
                if (filename.substr(filename.find_last_of(".") + 1) == "png") {
                    int width, height, channels;
                    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
                    torch::Tensor img_tensor = torch::from_blob(data, { height, width, 3 }, torch::kUInt8)
                        .permute({ 2, 0, 1 }).to(torch::kFloat32).div(255.0).unsqueeze(0);
                    img_tensor = F::interpolate(img_tensor, F::InterpolateFuncOptions().size(std::vector<int64_t>({ 64, 48 })).mode(torch::kBilinear).align_corners(false));
                    img_tensor = img_tensor.squeeze(0).clone();
                    input_tensors.push_back(img_tensor);
                    torch::Tensor label = torch::tensor(0, torch::kLong);
                    goal_tensors.push_back(label);
                    stbi_image_free(data);
                }
            }
            torch::Tensor loss;
            for (int epoch = 0; epoch < 100; epoch++) {
                size_t batch_index = 0;
                for (int i = 0; i < input_tensors.size(); i++) {
                }
            }
            //torch::save(model, "model.pt");
            model->eval();
        }
        else {
            //torch::load(model, "model.pt");  // ファイルから読み込む
            model->to(torch::kCUDA);  // 必要に応じてGPUへ
            model->eval();
        }*/

        // SDL3 の初期化
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
            return 1;
        }
        if (!TTF_Init()) {
            fprintf(stderr, "ttferror");
        }
        // ウィンドウの作成
        SDL_Window* window = SDL_CreateWindow("SDL3 Example",
            800, 600,
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
        if (!window) {
            fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }

        


        // レンダリング用の SDL グラフィックコンテキストを作成
        /*SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
        if (!renderer) {
            fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        TTF_Font* font = TTF_OpenFont("Gen.ttf", 16);  // 48px のサイズ
        if (!font) {
            printf("Failed to load font: \n");
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return 1;
        }*/
        SDL_Event event;
        SDL_FRect rect;
        SDL_FRect rect2;
        float mean[3] = { 123.675f, 116.28f, 103.53f };
        float std[3] = { 58.395f, 57.12f, 57.375f };

        int running = 1;
        int record = 0;
        int count = 0;
        /*Camera camera = Camera();
        AVFrame* frameRGB = av_frame_alloc();
        frameRGB->width = 640;
        frameRGB->height = 480;
        frameRGB->format = AV_PIX_FMT_RGB24;
        av_frame_get_buffer(frameRGB, 0);*/
        SDL_StartTextInput(window);
        SDL_SetWindowHitTest(window, hit_test_cb, NULL);
		/*Db* db = (Db*)GC_malloc(sizeof(Db));

        InitializeCriticalSection(&db->callback_mutex);
        for (int i = 0; i < WORKER_COUNT; i++) {
            char filename[32];
            sprintf(filename, "test.db", i);
            sqlite3_open(filename, &dbs[i]);
            sqlite3_exec(dbs[i], "CREATE TABLE IF NOT EXISTS logs(msg TEXT);", NULL, NULL, NULL);
            InitializeCriticalSection(&db->request_mutex[i]);
            db->request_event[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        }


        ThreadParam param[WORKER_COUNT];
        int ids[WORKER_COUNT];
        for (int i = 0; i < WORKER_COUNT; i++) {
			param[i].db = db;
            param[i].worker_id = i;
            ids[i] = i;
            db->threads[i] = CreateThread(NULL, 0, sqlite_thread, &param[i], 0, NULL);
        }*/
        /*GltfRenderer gltfRenderer;
        bool hasModel = false;

        // モデルをロード（コマンドライン引数またはデフォルト）
        const char* modelPath = "CesiumMan.glb";
        float deltaTime = 1.0f / 100.0f;
        if (gltfRenderer.loadModel(modelPath)) {
            hasModel = true;
            printf("Loaded model: %s\n", modelPath);
        }
        else {
            printf("Warning: Could not load model, using fallback rendering\n");
        }*/
		int width, height;
        SDL_GetWindowSize(window, &width, &height);
        bool isDragging = false;
        int lastMouseX = 0, lastMouseY = 0;
        std::chrono::high_resolution_clock::time_point lastTime_ = std::chrono::high_resolution_clock::now();
        float time = 0;
		HopStar* hoppy = new HopStar();
        hoppy->push_tab(magc);
		magc->hoppy = hoppy;
        RenderThread* render = new RenderThread(hoppy, window);
        render->start();
		runGoThreadAsync(magc);
        while (running) {

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            /*for (int i = 0; i < WORKER_COUNT; i++) {
                SqlRequest* req = (SqlRequest*)malloc(sizeof(SqlRequest));
                snprintf(req->sql, sizeof(req->sql), "%s", "SELECT * FROM logs;");
                req->callback = print_result;
                req->user_data = NULL;
                enqueue_sql(db, req, i);
            }
            for (int i = 0; i < 20; i++) {
                poll_callbacks(db);
                Sleep(100);
            }*/
            while (SDL_PollEvent(&event)) {
                /*if (event.type == SDL_EVENT_KEY_DOWN) {
                    if (event.key.key == SDLK_KP_ENTER || event.key.key == SDLK_RETURN || event.key.key == SDLK_DELETE || event.key.key == SDLK_BACKSPACE ||
                        event.key.key == SDLK_LEFT || event.key.key == SDLK_RIGHT || event.key.key == SDLK_UP || event.key.key == SDLK_DOWN) {
                        bool select = false;
                        KeyEvent e = KeyEvent();
						e.key = event.key.key;
                        e.text = (String*)GC_malloc(hoppy->target, _String);
                        e.text->size = 0;
						State* state = (State*)GC_malloc(hoppy->target, _State);
                        initState(hoppy->target, state);
                        e.state = state;
                        auto ke = KeyButton(hoppy, &e);
                        hoppy->target->queue->push(ke.coro);
                        ke.coro = {};
                    }
                }
                else if (event.type == SDL_EVENT_TEXT_INPUT) {
                    const char* text = event.text.text;
                    bool select = false;
                    KeyEvent e = KeyEvent();
                    e.key = -1;
                    String* str = (String*)GC_malloc(hoppy->target, _String);
                    utf8proc_int32_t utf32Buf[256];
					utf8proc_ssize_t utf32len = utf8proc_decompose((const utf8proc_uint8_t*)text, -1, utf32Buf, 256, UTF8PROC_NULLTERM);
                    if (utf32len < 0) {
                        fprintf(stderr, "utf8proc_decompose_utf8 failed: %zd\n", utf32len);
                        continue;
                    }

                    // UTF-32 → UTF-16
                    uint16_t utf16Buf[512];
                    size_t utf16len = utf32_to_utf16(utf32Buf, (size_t)utf32len, utf16Buf);
                    str = createString(hoppy->target, (char*)utf16Buf, utf16len * 2, 2);
                    e.text = str;
                    State* state = (State*)GC_malloc(hoppy->target, _State);
                    initState(hoppy->target, state);
                    e.state = state;
					auto ke = KeyButton(hoppy, &e);
					hoppy->target->queue->push(ke.coro);
					ke.coro = {};
				}
				else if (event.type == SDL_EVENT_QUIT) {
					running = 0;
                }
                else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                    MouseEvent e = MouseEvent();
					e.x = event.button.x;
                    e.y = event.button.y;
                    e.action = event.type;
                    State* state = (State*)GC_malloc(hoppy->target, _State);
                    initState(hoppy->target, state);
                    e.state = state;
					auto me = MouseButton(hoppy, &e);
					hoppy->target->queue->push(me.coro);
                    me.coro = {};
                }
                else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                    MouseEvent e = MouseEvent();
                    e.x = event.motion.x;
                    e.y = event.motion.y;
                    SDL_MouseButtonFlags s = event.motion.state;

                    // 左ボタンが押されているか
                    if (s & SDL_BUTTON_LMASK) {
                        e.click = true;
                    }
                    e.action = event.type;
                    State* state = (State*)GC_malloc(hoppy->target, _State);
                    initState(hoppy->target, state);
                    e.state = state;
                    auto me = MouseButton(hoppy, &e);
                    hoppy->target->queue->push(me.coro);
                    me.coro = {};
                }
                else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                    
                }
                else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                }*/
            }
        }

        // 現在のデバイスを取得する別の方法
        c10::DeviceIndex device_idx = 0; // デフォルトで0番目のGPUを使用
        torch::Device device(torch::kCUDA, device_idx);
        std::cout << "Using CUDA device: " << device_idx << std::endl;

        // CPUテンソルを作成してからCUDAに転送（段階的に確認）
        torch::Tensor cpu_tensor = torch::rand({ 3, 4 });
        std::cout << "CPU Tensor created successfully" << std::endl;

        // 明示的なデバイス指定でGPUに転送
        torch::Tensor cuda_tensor = cpu_tensor.to(device);
        std::cout << "Tensor transferred to CUDA successfully" << std::endl;

        std::cout << cuda_tensor << std::endl;

        const OrtApi* ort = OrtGetApiBase()->GetApi(16);
        OrtEnv* env;
        ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "scrfd", &env);

        OrtSessionOptions* session_options;
        ort->CreateSessionOptions(&session_options);

        OrtSession* session;
        const wchar_t* model_path = L"det_10g.onnx";
        OrtStatus* status = ort->CreateSession(env, model_path, session_options, &session);
        if (status != NULL) {
            const char* msg = ort->GetErrorMessage(status);
            fprintf(stderr, "ONNX session load error: %s\n", msg);
            ort->ReleaseStatus(status);
            exit(1);
        }
        OrtAllocator* allocator;
        ort->GetAllocatorWithDefaultOptions(&allocator);



        /*for (int i = 0; i < WORKER_COUNT; i++) {
            sqlite3_close(dbs[i]);
        }*/

    }
    catch (const c10::Error& e) {
        std::cerr << "LibTorch Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Standard Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
    }

    return 0;
}