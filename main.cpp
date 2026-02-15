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
#include "file_engine.h"
namespace F = torch::nn::functional;
#include "gc.h"
#include "arr.h"
#include "db2.h"
#include "audio.h"
#include "elem.h"
bool cuda = false;
HMODULE torch_cuda_dll;

// ============================================================================
// Global FileEngine instance
// ============================================================================
static std::unique_ptr<HopStarIO::FileEngine> g_fileEngine;

HopStarIO::FileEngine* getFileEngine() {
    if (!g_fileEngine) {
        g_fileEngine = HopStarIO::FileEngineFactory::createDefault();
    }
    return g_fileEngine.get();
}

// ============================================================================
// FileEngine-based file helpers
// ============================================================================

// Read file into memory using FileEngine
std::vector<uint8_t> readFileToMemory(const std::string& path) {
    auto* engine = getFileEngine();
    auto desc = engine->fromExternalPath(path, HopStarIO::Access::Read);
    auto result = engine->read(desc);
    if (result.success) {
        return std::move(result.data);
    }
    return {};
}

// Read file from URL or local path
std::vector<uint8_t> readFileFromAddress(const std::string& address) {
    auto* engine = getFileEngine();

    // Check if it's a URL or local path
    std::string scheme = HopStarIO::parseAddressScheme(address);
    if (scheme.empty()) {
        // Local file
        return readFileToMemory(address);
    }

    // Remote file
    auto desc = engine->createDescriptor(address, HopStarIO::Location::Server,
                                         HopStarIO::Access::Read, address);
    auto result = engine->read(desc);
    if (result.success) {
        return std::move(result.data);
    }
    return {};
}

// Write file using FileEngine
bool writeFileFromMemory(const std::string& path, const uint8_t* data, size_t length) {
    auto* engine = getFileEngine();
    auto desc = engine->fromExternalPath(path, HopStarIO::Access::Write);
    auto result = engine->write(desc, data, length);
    return result.success;
}

// Load image using FileEngine + stbi
unsigned char* loadImageViaFileEngine(const std::string& path, int* width, int* height, int* channels, int desired_channels) {
    auto data = readFileFromAddress(path);
    if (data.empty()) {
        return nullptr;
    }
    return stbi_load_from_memory(data.data(), static_cast<int>(data.size()),
                                  width, height, channels, desired_channels);
}

// Check if file exists - delegates to PlatformIO
bool fileExists(const std::string& path, HopStarIO::Location location) {
    return PlatformIO::fileExists(path, location);
}

// List files in directory - delegates to PlatformIO
std::vector<std::string> listDirectory(const std::string& dirPath, HopStarIO::Location location, const std::string& extension = "") {
    return PlatformIO::listDirectory(dirPath, location, extension);
}

// Create directory - delegates to PlatformIO
bool createDirectory(const std::string& path, HopStarIO::Location location) {
    return PlatformIO::createDirectory(path, location);
}
#include "opt.h"
#include "ugui.h"
#include "extorch.h"
//#include "db2.h"
SDL_HitTestResult
hit_test_cb(SDL_Window* win, const SDL_Point* pt, void* data)
{
    int w, h;
    const int border = 8;  // ���T�C�Y�\�G���A�̌���(px)
    SDL_GetWindowSize(win, &w, &h);

    // �l��
    if (pt->x < border && pt->y < border)               return SDL_HITTEST_RESIZE_TOPLEFT;
    if (pt->x > w - border && pt->y < border)           return SDL_HITTEST_RESIZE_TOPRIGHT;
    if (pt->x < border && pt->y > h - border)           return SDL_HITTEST_RESIZE_BOTTOMLEFT;
    if (pt->x > w - border && pt->y > h - border)       return SDL_HITTEST_RESIZE_BOTTOMRIGHT;

    // ��
    if (pt->y < border)                                 return SDL_HITTEST_RESIZE_TOP;
    if (pt->y > h - border)                             return SDL_HITTEST_RESIZE_BOTTOM;
    if (pt->x < border)                                 return SDL_HITTEST_RESIZE_LEFT;
    if (pt->x > w - border)                             return SDL_HITTEST_RESIZE_RIGHT;

    // ���̑��̈�͒ʏ�̃N���C�A���g�̈�Ƃ��Ĉ���
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
    // Initialize FileEngine (lazy init on first use)
    auto* fileEngine = getFileEngine();
    std::cout << "FileEngine initialized" << std::endl;

    magc = GC_init(1000 * 1000 * 1000);

    // Use FileEngine's internal storage path for database
    std::string dbPath = fileEngine->getInternalPath("test.db");
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }
    else {
        std::cout << "Opened database successfully at: " << dbPath << std::endl;
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
            createDirectory("inputs/faces", HopStarIO::Location::External);
            createDirectory("inputs/nonfaces", HopStarIO::Location::External);
            std::vector<torch::Tensor> input_tensors;
            std::vector<torch::Tensor> goal_tensors;

            // Load face images via FileEngine
            for (const auto& filename : listDirectory("inputs/faces", HopStarIO::Location::External, "png")) {
                int width, height, channels;
                unsigned char* data = loadImageViaFileEngine(filename, &width, &height, &channels, 3);
                if (data) {
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

            // Load non-face images via FileEngine
            for (const auto& filename : listDirectory("inputs/nonfaces", HopStarIO::Location::External, "png")) {
                int width, height, channels;
                unsigned char* data = loadImageViaFileEngine(filename, &width, &height, &channels, 3);
                if (data) {
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
            //torch::load(model, "model.pt");
            model->to(torch::kCUDA);
            model->eval();
        }*/

        // SDL3 �̏�����
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
            return 1;
        }
        if (!TTF_Init()) {
            fprintf(stderr, "ttferror");
        }
        // �E�B���h�E�̍쐬
        SDL_Window* window = SDL_CreateWindow("SDL3 Example",
            800, 600,
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
        if (!window) {
            fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }

        


        // �����_�����O�p�� SDL �O���t�B�b�N�R���e�L�X�g���쐬
        /*SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
        if (!renderer) {
            fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        TTF_Font* font = TTF_OpenFont("Gen.ttf", 16);  // 48px �̃T�C�Y
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

        // ���f�������[�h�i�R�}���h���C�������܂��̓f�t�H���g�j
        const char* modelPath = "CesiumMan.glb";
        float deltaTime = 1.0f / 100.0f;
        if (gltfRenderer.loadModel(modelPath, HopStarIO::Location::Resource)) {
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
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    if (event.key.key == SDLK_KP_ENTER || event.key.key == SDLK_RETURN || event.key.key == SDLK_DELETE || event.key.key == SDLK_BACKSPACE ||
                        event.key.key == SDLK_LEFT || event.key.key == SDLK_RIGHT || event.key.key == SDLK_UP || event.key.key == SDLK_DOWN) {
                        bool select = false;
                        KeyEvent e = KeyEvent();
						e.key = event.key.key;
                        e.text = (String*)GC_alloc(hoppy->target, _String);
                        e.text->size = 0;
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
                    String* str = (String*)GC_alloc(hoppy->target, _String);
                    utf8proc_int32_t utf32Buf[256];
					utf8proc_ssize_t utf32len = utf8proc_decompose((const utf8proc_uint8_t*)text, -1, utf32Buf, 256, UTF8PROC_NULLTERM);
                    if (utf32len < 0) {
                        fprintf(stderr, "utf8proc_decompose_utf8 failed: %zd\n", utf32len);
                        continue;
                    }

                    // UTF-32 �� UTF-16
                    uint16_t utf16Buf[512];
                    size_t utf16len = utf32_to_utf16(utf32Buf, (size_t)utf32len, utf16Buf);
                    str = createString(hoppy->target, (char*)utf16Buf, utf16len, 2);
                    e.text = str;
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
					auto me = MouseButton(hoppy, &e);
					hoppy->target->queue->push(me.coro);
                    me.coro = {};
                }
                else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                    MouseEvent e = MouseEvent();
                    e.x = event.motion.x;
                    e.y = event.motion.y;
                    SDL_MouseButtonFlags s = event.motion.state;

                    // ���{�^����������Ă��邩
                    if (s & SDL_BUTTON_LMASK) {
                        e.click = true;
                    }
                    e.action = event.type;
                    auto me = MouseButton(hoppy, &e);
                    hoppy->target->queue->push(me.coro);
                    me.coro = {};
                }
                else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                    
                }
                else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                }
            }
        }

        // ���݂̃f�o�C�X���擾����ʂ̕��@
        c10::DeviceIndex device_idx = 0; // �f�t�H���g��0�Ԗڂ�GPU���g�p
        torch::Device device(torch::kCUDA, device_idx);
        std::cout << "Using CUDA device: " << device_idx << std::endl;

        // CPU�e���\�����쐬���Ă���CUDA�ɓ]���i�i�K�I�Ɋm�F�j
        torch::Tensor cpu_tensor = torch::rand({ 3, 4 });
        std::cout << "CPU Tensor created successfully" << std::endl;

        // �����I�ȃf�o�C�X�w���GPU�ɓ]��
        torch::Tensor cuda_tensor = cpu_tensor.to(device);
        std::cout << "Tensor transferred to CUDA successfully" << std::endl;

        std::cout << cuda_tensor << std::endl;

        const OrtApi* ort = OrtGetApiBase()->GetApi(16);
        OrtEnv* env;
        ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "scrfd", &env);

        OrtSessionOptions* session_options;
        ort->CreateSessionOptions(&session_options);

        OrtSession* session;

        // Load ONNX model via FileEngine (supports local and remote)
        std::string onnxModelPath = "det_10g.onnx";
        std::string resolvedPath = PlatformIO::resolvePath(onnxModelPath, HopStarIO::Location::Resource);

        // Check if model exists in resources, otherwise use current directory
        if (!fileExists(resolvedPath, HopStarIO::Location::External)) {
            resolvedPath = onnxModelPath;
        }

        // Convert to wide string for ONNX Runtime
        std::wstring wModelPath(resolvedPath.begin(), resolvedPath.end());

        OrtStatus* status = ort->CreateSession(env, wModelPath.c_str(), session_options, &session);
        if (status != NULL) {
            const char* msg = ort->GetErrorMessage(status);
            fprintf(stderr, "ONNX session load error: %s (path: %s)\n", msg, resolvedPath.c_str());
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

    // Cleanup FileEngine
    g_fileEngine.reset();
    std::cout << "FileEngine cleaned up" << std::endl;

    return 0;
}