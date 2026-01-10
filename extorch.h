class CustomModuleImpl;


class TensorWrapper {
public:
    torch::Tensor tensor;

    // Use move semantics to preserve computation graph
    TensorWrapper(torch::Tensor&& t) : tensor(std::move(t)) {}

    // Copy constructor that maintains graph connectivity
    TensorWrapper(const torch::Tensor& t) : tensor(t) {}
};
torch::nn::Conv2dImpl* createConv2d(int64_t in_channels, int64_t out_channels, int64_t kernel_size) {
    try {
        auto options = torch::nn::Conv2dOptions(in_channels, out_channels, kernel_size)
            .stride(1)
            .padding(0)
            .bias(true);

        auto conv = new torch::nn::Conv2dImpl(options);
        conv->to(torch::kCUDA);
        return conv;
    }
    catch (...) {
        return nullptr;
    }
}

// Conv2dのプロパティにアクセスする関数
int64_t getInChannels(torch::nn::Conv2dImpl* conv) {
    if (conv) {
        return conv->options.in_channels();
    }
    return 0;
}

int64_t getOutChannels(torch::nn::Conv2dImpl* conv) {
    if (conv) {
        return conv->options.out_channels();
    }
    return 0;
}

int64_t getKernelSize(torch::nn::Conv2dImpl* conv) {
    if (conv) {
        return conv->options.kernel_size()->at(0);
    }
    return 0;
}

// ウェイトデータにアクセスする関数
float* getWeightData(torch::nn::Conv2dImpl* conv) {
    if (conv && conv->weight.defined()) {
        return conv->weight.data_ptr<float>();
    }
    return nullptr;
}

int64_t getWeightSize(torch::nn::Conv2dImpl* conv) {
    if (conv && conv->weight.defined()) {
        return conv->weight.numel();
    }
    return 0;
}


// フォワード関数
void* forwardConv2d(torch::nn::Conv2dImpl* conv, void* input_tensor) {
    if (conv && input_tensor) {
        try {
            TensorWrapper* input_wrapper = static_cast<TensorWrapper*>(input_tensor);
            torch::Tensor output = conv->forward(input_wrapper->tensor);
            // 計算グラフを保持するラッパーを返す
            return new TensorWrapper(output);
        }
        catch (const c10::Error& e) {
            std::cerr << "LibTorch Error: " << e.what() << std::endl;
            return nullptr;
        }
        catch (const std::exception& e) {
            std::cerr << "Standard Exception: " << e.what() << std::endl;
            return nullptr;
        }
        catch (...) {
            std::cerr << "Unknown exception occurred" << std::endl;
            return nullptr;
        }
    }
    return nullptr;
}torch::nn::MaxPool2dImpl* createMaxPool2d(int64_t kernel_size, int64_t stride, int64_t padding) {
    try {
        auto options = torch::nn::MaxPool2dOptions(kernel_size)
            .stride(stride)
            .padding(padding);

        auto pool = new torch::nn::MaxPool2dImpl(options);
        return pool;
    }
    catch (...) {
        return nullptr;
    }
}

int64_t getMaxPoolKernelSize(torch::nn::MaxPool2dImpl* pool) {
    if (pool) {
        return pool->options.kernel_size()->at(0);
    }
    return 0;
}

int64_t getMaxPoolStride(torch::nn::MaxPool2dImpl* pool) {
    if (pool) {
        return pool->options.stride()->at(0);
    }
    return 0;
}

int64_t getMaxPoolPadding(torch::nn::MaxPool2dImpl* pool) {
    if (pool) {
        return pool->options.padding()->at(0);
    }
    return 0;
}

void* forwardMaxPool2d(torch::nn::MaxPool2dImpl* pool, void* input_tensor) {
    if (pool && input_tensor) {
        try {
            torch::Tensor* input = static_cast<torch::Tensor*>(input_tensor);
            torch::Tensor output = pool->forward(*input);
            return new torch::Tensor(output);
        }
        catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

// ReLU関連の実装
torch::nn::ReLUImpl* createReLU(bool inplace) {
    try {
        auto options = torch::nn::ReLUOptions().inplace(inplace);
        auto relu = new torch::nn::ReLUImpl(options);
        relu->to(torch::kCUDA);
        return relu;
    }
    catch (...) {
        return nullptr;
    }
}

bool getReLUInplace(torch::nn::ReLUImpl* relu) {
    if (relu) {
        return relu->options.inplace();
    }
    return false;
}

void* forwardReLU(torch::nn::ReLUImpl* relu, void* input_tensor) {
    if (relu && input_tensor) {
        try {
            TensorWrapper* input_wrapper = static_cast<TensorWrapper*>(input_tensor);
            torch::Tensor output = relu->forward(input_wrapper->tensor);
            // 計算グラフを保持するラッパーを返す
            return new TensorWrapper(output);
        }
        catch (const c10::Error& e) {
            std::cerr << "LibTorch Error: " << e.what() << std::endl;
            return nullptr;
        }
        catch (const std::exception& e) {
            std::cerr << "Standard Exception: " << e.what() << std::endl;
            return nullptr;
        }
        catch (...) {
            std::cerr << "Unknown exception occurred" << std::endl;
            return nullptr;
        }
    }
    return nullptr;
}

torch::nn::FlattenImpl* createFlatten(int64_t start_dim, int64_t end_dim) {
    try {
        auto options = torch::nn::FlattenOptions().start_dim(start_dim).end_dim(end_dim);
        auto flatten = new torch::nn::FlattenImpl(options);
        flatten->to(torch::kCUDA);
        return flatten;
    }
    catch (...) {
        return nullptr;
    }
}

int64_t getFlattenStartDim(torch::nn::FlattenImpl* flatten) {
    if (flatten) {
        return flatten->options.start_dim();
    }
    return 0;
}

int64_t getFlattenEndDim(torch::nn::FlattenImpl* flatten) {
    if (flatten) {
        return flatten->options.end_dim();
    }
    return -1;
}

void* forwardFlatten(torch::nn::FlattenImpl* flatten, void* input_tensor) {
    if (flatten && input_tensor) {
        try {
            torch::Tensor* input = static_cast<torch::Tensor*>(input_tensor);
            torch::Tensor output = flatten->forward(*input);
            return new torch::Tensor(std::move(output));
        }
        catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

// Linear の実装
torch::nn::LinearImpl* createLinear(int64_t in_features, int64_t out_features, bool bias) {
    try {
        auto options = torch::nn::LinearOptions(in_features, out_features).bias(bias);
        auto linear = new torch::nn::LinearImpl(options);
        linear->to(torch::kCUDA);
        return linear;
    }
    catch (...) {
        return nullptr;
    }
}

int64_t getLinearInFeatures(torch::nn::LinearImpl* linear) {
    if (linear) {
        return linear->options.in_features();
    }
    return 0;
}

int64_t getLinearOutFeatures(torch::nn::LinearImpl* linear) {
    if (linear) {
        return linear->options.out_features();
    }
    return 0;
}

bool getLinearBias(torch::nn::LinearImpl* linear) {
    if (linear) {
        return linear->options.bias();
    }
    return false;
}

float* getLinearWeightData(torch::nn::LinearImpl* linear) {
    if (linear && linear->weight.defined()) {
        return linear->weight.data_ptr<float>();
    }
    return nullptr;
}

int64_t getLinearWeightSize(torch::nn::LinearImpl* linear) {
    if (linear && linear->weight.defined()) {
        return linear->weight.numel();
    }
    return 0;
}

float* getLinearBiasData(torch::nn::LinearImpl* linear) {
    if (linear && linear->bias.defined()) {
        return linear->bias.data_ptr<float>();
    }
    return nullptr;
}

int64_t getLinearBiasSize(torch::nn::LinearImpl* linear) {
    if (linear && linear->bias.defined()) {
        return linear->bias.numel();
    }
    return 0;
}

void* forwardLinear(torch::nn::LinearImpl* linear, void* input_tensor) {
    if (linear && input_tensor) {
        try {
            torch::Tensor* input = static_cast<torch::Tensor*>(input_tensor);
            torch::Tensor output = linear->forward(*input);
            return new torch::Tensor(std::move(output));
        }
        catch (const c10::Error& e) {
            std::cerr << "LibTorch Error: " << e.what() << std::endl;
            return nullptr;
        }
        catch (const std::exception& e) {
            std::cerr << "Standard Exception: " << e.what() << std::endl;
            return nullptr;
        }
        catch (...) {
            std::cerr << "Unknown exception occurred" << std::endl;
            return nullptr;
        }
    }
    return nullptr;
}

// Sequential の実装
torch::nn::SequentialImpl* createSequential() {
    try {
        auto seq = new torch::nn::SequentialImpl();
        return seq;
    }
    catch (...) {
        return nullptr;
    }
}

void addModuleToSequential(torch::nn::SequentialImpl* seq, const char* name, void* module) {
    if (seq && module && name) {
        try {
            // module_handle を正しい型にキャスト
            auto* holder = static_cast<torch::nn::AnyModule*>(module);
            // AnyModule のクローン（ムーブ可能なオブジェクト）を得る
            torch::nn::AnyModule cloned = holder->clone();
            // push_back にムーブする
            seq->push_back(std::string(name), std::move(cloned));
        }
        catch (...) {
            // エラー処理
        }
    }
}

size_t getSequentialSize(torch::nn::SequentialImpl* seq) {
    if (seq) {
        return seq->size();
    }
    return 0;
}

void* getSequentialModule(torch::nn::SequentialImpl* seq, size_t index) {
    if (seq && index < seq->size()) {
        return (*seq)[index].get();
    }
    return nullptr;
}

void* forwardSequential(torch::nn::SequentialImpl* seq, void* input_tensor) {
    if (seq && input_tensor) {
        try {
            torch::Tensor* input = static_cast<torch::Tensor*>(input_tensor);
            torch::Tensor output = seq->forward(*input);
            return new torch::Tensor(output);
        }
        catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

void registerModule(torch::nn::Module* parent, const char* name, torch::nn::Module* child) {  
   if (parent && child && name) {  
       try {  
           // 修正: `register_module` の第2引数は `t       orch::nn::Module` の参照ではなく、`std::shared_ptr<torch::nn::Module>` を期待します  
           parent->register_module(name, std::shared_ptr<torch::nn::Module>(child));  
       }  
       catch (...) {  
           // エラー処理  
       }  
   }  
}

using ForwardType = void* (*)(void*, void*);
// カスタムモジュールのフォワード関数のコールバック型
using ForwardCallback = void* (*)(void*, void*);
class CustomModuleImpl : public torch::nn::Module {
public:
    CustomModuleImpl() {
        if (!cuda) {
            torch_cuda_dll = LoadLibraryA("torch_cuda.dll");
            cuda = true;
        }
        // デフォルトコンストラクタ
    }
    // フォワード関数
    torch::Tensor forward(torch::Tensor x) {
        // 入力テンソルをラッパーに入れる
        TensorWrapper* wrapper = new TensorWrapper(x);

        // 処理を実行
        void* result = forwardfunc(obj, wrapper);

        // ラッパーから結果テンソルを取り出す
        TensorWrapper* output_wrapper = static_cast<TensorWrapper*>(result);
        torch::Tensor output = output_wrapper->tensor;

        // メモリ解放
        delete wrapper;
        delete output_wrapper;

        return output;
    }
    ForwardType forwardfunc;
    char* obj;
};

TORCH_MODULE(CustomModule);
CustomModuleImpl* createTorchModule(char* obj, ForwardType forward) {
    try {
        auto module = new CustomModuleImpl();
        module->obj = obj;
        module->forwardfunc = forward;
        return module;
    }
    catch (...) {
        return nullptr;
    }
}

torch::optim::Adam* createAdam(torch::nn::Module** params, int64_t num_params, torch::optim::AdamOptions* options) {
    std::vector<torch::Tensor> parameters;
    for (int64_t i = 0; i < num_params; i++) {
        auto module_params = params[i]->parameters();
        parameters.insert(parameters.end(), module_params.begin(), module_params.end());
    }
    return new torch::optim::Adam(parameters, *options);
}

torch::optim::AdamOptions* createAdamOptions(float learning_rate) {
    return new torch::optim::AdamOptions(learning_rate);
}

void* tensorToCUDA(void* tensor_ptr) {
    TensorWrapper* wrapper = static_cast<TensorWrapper*>(tensor_ptr);
    torch::Tensor cuda_tensor = wrapper->tensor.to(torch::kCUDA);
    return new TensorWrapper(std::move(cuda_tensor));
}

void* tensorRequiresGrad(void* tensor_ptr, bool requires_grad) {
    TensorWrapper* wrapper = static_cast<TensorWrapper*>(tensor_ptr);
    torch::Tensor tensor = wrapper->tensor.requires_grad_(requires_grad);
    return new TensorWrapper(std::move(tensor));
}

void optimizerZeroGrad(torch::optim::Adam* optimizer) {
    optimizer->zero_grad();
}

void tensorBackward(void* tensor_ptr) {
    TensorWrapper* wrapper = static_cast<TensorWrapper*>(tensor_ptr);
    wrapper->tensor.backward();
}

void optimizerStep(torch::optim::Adam* optimizer) {
    optimizer->step();
}

void* crossEntropy(void* output_ptr, void* target_ptr) {
    TensorWrapper* output_wrapper = static_cast<TensorWrapper*>(output_ptr);
    TensorWrapper* target_wrapper = static_cast<TensorWrapper*>(target_ptr);
    torch::Tensor loss = torch::nn::functional::cross_entropy(output_wrapper->tensor, target_wrapper->tensor);
    return new TensorWrapper(std::move(loss));
}

using CreateConv2dType = torch::nn::Conv2dImpl* (*)(int64_t, int64_t, int64_t);
using GetInChannelsType = int64_t(*)(torch::nn::Conv2dImpl* conv);
using GetWeightDataType = float* (*)(torch::nn::Conv2dImpl* conv);
using ForwardConv2dType = void* (*)(torch::nn::Conv2dImpl* conv, void* input_tensor);

using CreateMaxPool2dType = torch::nn::MaxPool2dImpl* (*)(int64_t, int64_t, int64_t);
using GetMaxPoolKernelSizeType = int64_t(*)(torch::nn::MaxPool2dImpl*);
using ForwardMaxPool2dType = void* (*)(torch::nn::MaxPool2dImpl*, void*);

// ReLU用の関数ポインタ型
using CreateReLUType = torch::nn::ReLUImpl* (*)(bool);
using GetReLUInplaceType = bool(*)(torch::nn::ReLUImpl*);
using ForwardReLUType = void* (*)(torch::nn::ReLUImpl*, void*);

using CreateFlattenType = torch::nn::FlattenImpl* (*)(int64_t, int64_t);
using GetFlattenStartDimType = int64_t(*)(torch::nn::FlattenImpl*);
using ForwardFlattenType = void* (*)(torch::nn::FlattenImpl*, void*);

using CreateLinearType = torch::nn::LinearImpl* (*)(int64_t, int64_t, bool);
using GetLinearInFeaturesType = int64_t(*)(torch::nn::LinearImpl*);
using GetLinearBiasType = bool(*)(torch::nn::LinearImpl*);
using GetLinearWeightDataType = float* (*)(torch::nn::LinearImpl*);
using ForwardLinearType = void* (*)(torch::nn::LinearImpl*, void*);

using CreateSequentialType = torch::nn::SequentialImpl* (*)();
using AddModuleToSequentialType = void (*)(torch::nn::SequentialImpl*, const char*, void*);
using GetSequentialSizeType = size_t(*)(torch::nn::SequentialImpl*);
using GetSequentialModuleType = void* (*)(torch::nn::SequentialImpl*, size_t index);
using ForwardSequentialType = void* (*)(torch::nn::SequentialImpl*, void*);

using RegisterModuleType = void (*)(torch::nn::Module*, const char*, torch::nn::Module*);
using CreateTorchModuleType = CustomModuleImpl * (*)(char*, ForwardType);
using SetForwardFunctionType = void (*)(torch::nn::Module*, void* (*)(void*, void*));

using GC_AddClassType = int (*)(ThreadGC*, int, const char*, int, GCCheckFunc, GCFinalizeFunc, bool);
using GC_AddRootType = RootNode * (*)(ThreadGC*);
using GC_ReleaseRootType = void(*)(RootNode*);
using GC_SetRootType = void (*)(RootNode*, char**);
using GC_BackRootType = void (*)(RootNode*, int);
using GC_mallocType = char* (*)(ThreadGC*, int);

using CreateAdamType = torch::optim::Adam* (*)(torch::nn::Module**, int64_t, torch::optim::AdamOptions*);
using CreateAdamOptionsType = torch::optim::AdamOptions* (*)(float);
using OptimizerZeroGradType = void (*)(torch::optim::Adam*);
using OptimizerStepType = void (*)(torch::optim::Adam*);

// テンソル操作関連の関数ポインタ型
using TensorToCUDAType = void* (*)(void*);
using TensorRequiresGradType = void* (*)(void*, bool);
using TensorBackwardType = void (*)(void*);

// 損失計算関連の関数ポインタ型
using CrossEntropyType = void* (*)(void*, void*);

// テンソルアクセス関連（必要に応じて追加）
using TensorUnsqueezeType = void* (*)(void*, int64_t);
using TensorItemIntType = int (*)(void*);
using TensorItemFloatType = float (*)(void*);
using TensorCloneType = void* (*)(void*);

using CopyObjectType = char* (*)(ThreadGC*, char*);
using GetHashValueType = int (*) (String*, int);

using CreateListType = List * (*) (ThreadGC*, int, bool);
using AddListType = void (*)(ThreadGC*, List*, char*);
using GetListType = char** (*)(List*, int);
using GetLastType = char** (*)(List*);
using SetListType = void (*)(List*, int, char*);
using SetLastType = void (*)(List*, char*);

using CreateHashType = Map * (*) (ThreadGC*, bool);
using AddHashType = void (*)(ThreadGC*, Map*, String*, char*);
using GetHashType = char* (*)(Map*, String*);

using CreateStringType = String * (*) (ThreadGC*, char*, int, int);
using AddStringType = String * (*)(ThreadGC*, String*, char*, int, int);
using AddStringType2 = String * (*)(ThreadGC*, String*, String*);
using RemoveStringType = String * (*)(ThreadGC*, String*, int);
using SubStringType = String * (*)(ThreadGC*, String*, int, int);
using NumberStringType = String * (*)(ThreadGC*, int);
using GetCharType = wchar_t (*)(String*, int);
using PrintStringType = void (*)(String*);
using PrintArrayType = void (*)(Map*, int, int);

using DbInitType = SqlPool* (*)(ThreadGC*, String*, int, int, int, String*);
using PushQueueType = void (*)(CoroutineQueue*, char*);
using WaitHandleType = char* (*)(ThreadGC* thgc, char*);

using BeginTransactionType = std::coroutine_handle<Generator::promise_type>(*)(ThreadGC* thgc, SqlPool* sql, CoroutineQueue* queue);
using StringUTF8Type = char* (*)(String* str, int* outlen);
using ExecSqlType = std::coroutine_handle<Generator::promise_type>(*)(Transaction* tx, String* sql, SqlParam params[], int pcount, char* (*func)(ThreadGC*, CoroutineQueue*, std::unique_lock<std::mutex>*, sqlite3_stmt*, char*), char* obj);
using TxFinishType = std::coroutine_handle<Generator::promise_type>(*)(Transaction* tx);
using GCANTType = void(*) (ThreadGC* thgc, CoroutineQueue* queue, std::unique_lock<std::mutex>* lock);
using SqlIntType = int(*) (sqlite3_stmt* st, int n);
using SqlStringType = const unsigned char* (*) (sqlite3_stmt* st, int n);
using CreateDivType = NewElement * (*)(ThreadGC*);
using ElementAddType = void (*)(ThreadGC*, NewElement*, NewElement*);
using CreateLetterType = void (*) (ThreadGC*, NewElement*, String*);
using CreateKaigyouType = void (*) (ThreadGC*, NewElement*);
using CheckTreeElementType = TreeElement * (*)(ThreadGC* thgc, TreeElement* parentt, NewElement* parent, List* deletes, String* id, enum LetterType type, enum OpCode op, NewElement* (*func)(ThreadGC*));
using MakeFrameType = Frame * (*)(RootNode* root, void (*func)(Frame*), char* blockobj);
uint64_t now_us() {
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}
CustomModuleImpl* ReadDll(ThreadGC* thgc) {
    HMODULE h = LoadLibraryA("foo.dll");
    if (!h) {
        std::cerr << "LoadLibrary failed: " << GetLastError() << "\n";
        return NULL;
    }

    auto conv2d = reinterpret_cast<CreateConv2dType*>(GetProcAddress(h, "CreateConv2d"));
    if (!conv2d) {
        std::cerr << "GetProcAddress for 'createconv2d' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    // 3) 自前の関数を代入
    *conv2d = createConv2d;
    auto getinchannels = reinterpret_cast<GetInChannelsType*>(GetProcAddress(h, "GetInChannels"));
    if (!getinchannels) {
        std::cerr << "GetProcAddress for 'cgetinchannels' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    // 3) 自前の関数を代入
    *getinchannels = getInChannels;

    auto getoutchannels = reinterpret_cast<GetInChannelsType*>(GetProcAddress(h, "GetOutChannels"));
    if (!getoutchannels) {
        std::cerr << "GetProcAddress for 'getoutchannels' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    // 3) 自前の関数を代入
    *getinchannels = getOutChannels;
    auto getkernelsize = reinterpret_cast<GetInChannelsType*>(GetProcAddress(h, "GetKernelSize"));
    if (!getkernelsize) {
        std::cerr << "GetProcAddress for 'getkernelsize' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    // 3) 自前の関数を代入
    *getkernelsize = getKernelSize;
    // Conv2dのウェイトデータにアクセスする関数
    auto getweightdata = reinterpret_cast<GetWeightDataType*>(GetProcAddress(h, "GetWeightData"));
    if (!getweightdata) {
        std::cerr << "GetProcAddress for 'getweightdata' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    // 3) 自前の関数を代入
    *getweightdata = getWeightData;
    auto getweightsize = reinterpret_cast<GetInChannelsType*>(GetProcAddress(h, "GetWeightSize"));
    if (!getweightsize) {
        std::cerr << "GetProcAddress for 'getweightdata' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    // 3) 自前の関数を代入
    *getweightsize = getWeightSize;

    // Conv2dをフォワードする関数
    auto forwardconv2d = reinterpret_cast<ForwardConv2dType*>(GetProcAddress(h, "ForwardConv2d"));
    if (!forwardconv2d) {
        std::cerr << "GetProcAddress for 'forwardconv2d' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    // 3) 自前の関数を代入
    *forwardconv2d = forwardConv2d;

    auto createMaxpool = reinterpret_cast<CreateMaxPool2dType*>(GetProcAddress(h, "CreateMaxPool2d"));
    if (!createMaxpool) {
        std::cerr << "GetProcAddress for 'createMaxpool' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createMaxpool = createMaxPool2d;

    auto getMaxpoolKernelSize = reinterpret_cast<GetMaxPoolKernelSizeType*>(GetProcAddress(h, "GetMaxPoolKernelSize"));
    if (!getMaxpoolKernelSize) {
        std::cerr << "GetProcAddress for 'getMaxpoolKernelSize' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getMaxpoolKernelSize = getMaxPoolKernelSize;

    auto getMaxpoolStride = reinterpret_cast<GetMaxPoolKernelSizeType*>(GetProcAddress(h, "GetMaxPoolStride"));
    if (!getMaxpoolStride) {
        std::cerr << "GetProcAddress for 'getMaxpoolStride' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getMaxpoolStride = getMaxPoolStride;

    auto getMaxpoolPadding = reinterpret_cast<GetMaxPoolKernelSizeType*>(GetProcAddress(h, "GetMaxPoolPadding"));
    if (!getMaxpoolPadding) {
        std::cerr << "GetProcAddress for 'getMaxpoolPadding' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getMaxpoolPadding = getMaxPoolPadding;

    auto forwardMaxpool = reinterpret_cast<ForwardMaxPool2dType*>(GetProcAddress(h, "ForwardMaxPool2d"));
    if (!forwardMaxpool) {
        std::cerr << "GetProcAddress for 'forwardMaxpool' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *forwardMaxpool = forwardMaxPool2d;

    // ReLU関連の関数設定
    auto createRelu = reinterpret_cast<CreateReLUType*>(GetProcAddress(h, "CreateReLU"));
    if (!createRelu) {
        std::cerr << "GetProcAddress for 'createRelu' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createRelu = createReLU;

    auto getReLUInplacePtr = reinterpret_cast<GetReLUInplaceType*>(GetProcAddress(h, "GetReLUInplace"));
    if (!getReLUInplacePtr) {
        std::cerr << "GetProcAddress for 'getReLUInplacePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getReLUInplacePtr = getReLUInplace;

    auto forwardReLUPtr = reinterpret_cast<ForwardReLUType*>(GetProcAddress(h, "ForwardReLU"));
    if (!forwardReLUPtr) {
        std::cerr << "GetProcAddress for 'forwardReLUPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *forwardReLUPtr = forwardReLU;

    auto createFlattenPtr = reinterpret_cast<CreateFlattenType*>(GetProcAddress(h, "CreateFlatten"));
    if (!createFlattenPtr) {
        std::cerr << "GetProcAddress for 'createFlattenPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createFlattenPtr = createFlatten;

    auto getFlattenStartDimPtr = reinterpret_cast<GetFlattenStartDimType*>(GetProcAddress(h, "GetFlattenStartDim"));
    if (!getFlattenStartDimPtr) {
        std::cerr << "GetProcAddress for 'getFlattenStartDimPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getFlattenStartDimPtr = getFlattenStartDim;

    auto getFlattenEndDimPtr = reinterpret_cast<GetFlattenStartDimType*>(GetProcAddress(h, "GetFlattenEndDim"));
    if (!getFlattenEndDimPtr) {
        std::cerr << "GetProcAddress for 'getFlattenEndDimPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getFlattenEndDimPtr = getFlattenEndDim;

    auto forwardFlattenPtr = reinterpret_cast<ForwardFlattenType*>(GetProcAddress(h, "ForwardFlatten"));
    if (!getFlattenEndDimPtr) {
        std::cerr << "GetProcAddress for 'forwardFlattenPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *forwardFlattenPtr = forwardFlatten;

    // Linear 関連の関数設定
    auto createLinearPtr = reinterpret_cast<CreateLinearType*>(GetProcAddress(h, "CreateLinear"));
    if (!createLinearPtr) {
        std::cerr << "GetProcAddress for 'createLinearPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createLinearPtr = createLinear;

    auto getLinearInFeaturesPtr = reinterpret_cast<GetLinearInFeaturesType*>(GetProcAddress(h, "GetLinearInFeatures"));
    if (!getLinearInFeaturesPtr) {
        std::cerr << "GetProcAddress for 'getLinearInFeaturesPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLinearInFeaturesPtr = getLinearInFeatures;

    auto getLinearOutFeaturesPtr = reinterpret_cast<GetLinearInFeaturesType*>(GetProcAddress(h, "GetLinearOutFeatures"));
    if (!getLinearOutFeaturesPtr) {
        std::cerr << "GetProcAddress for 'getLinearOutFeaturesPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLinearOutFeaturesPtr = getLinearOutFeatures;

    auto getLinearBiasPtr = reinterpret_cast<GetLinearBiasType*>(GetProcAddress(h, "GetLinearBias"));
    if (!getLinearInFeaturesPtr) {
        std::cerr << "GetProcAddress for 'getLinearBias' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLinearBiasPtr = getLinearBias;

    auto getLinearWeightDataPtr = reinterpret_cast<GetLinearWeightDataType*>(GetProcAddress(h, "GetLinearInFeatures"));
    if (!getLinearInFeaturesPtr) {
        std::cerr << "GetProcAddress for 'getLinearInFeaturesPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLinearWeightDataPtr = getLinearWeightData;

    auto getLinearWeightSizePtr = reinterpret_cast<GetLinearInFeaturesType*>(GetProcAddress(h, "GetLinearInFeatures"));
    if (!getLinearInFeaturesPtr) {
        std::cerr << "GetProcAddress for 'getLinearInFeaturesPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLinearWeightSizePtr = getLinearWeightSize;

    auto getLinearBiasDataPtr = reinterpret_cast<GetLinearWeightDataType*>(GetProcAddress(h, "GetLinearInFeatures"));
    if (!getLinearInFeaturesPtr) {
        std::cerr << "GetProcAddress for 'getLinearInFeaturesPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLinearBiasDataPtr = getLinearBiasData;

    auto getLinearBiasSizePtr = reinterpret_cast<GetLinearInFeaturesType*>(GetProcAddress(h, "GetLinearInFeatures"));
    if (!getLinearInFeaturesPtr) {
        std::cerr << "GetProcAddress for 'getLinearInFeaturesPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLinearBiasSizePtr = getLinearBiasSize;

    auto forwardLinearPtr = reinterpret_cast<ForwardLinearType*>(GetProcAddress(h, "ForwardLinear"));
    if (!forwardLinearPtr) {
        std::cerr << "GetProcAddress for 'forwardLinearPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *forwardLinearPtr = forwardLinear;

    // その他の Linear 関数も同様に設定...

    // Sequential 関連の関数設定
    auto createSequentialPtr = reinterpret_cast<CreateSequentialType*>(GetProcAddress(h, "CreateSequential"));
    if (!createSequentialPtr) {
        std::cerr << "GetProcAddress for 'createSequentialPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createSequentialPtr = createSequential;

    auto addModuleToSequentialPtr = reinterpret_cast<AddModuleToSequentialType*>(GetProcAddress(h, "AddModuleToSequential"));
    if (!addModuleToSequentialPtr) {
        std::cerr << "GetProcAddress for 'addModuleToSequentialPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *addModuleToSequentialPtr = addModuleToSequential;

    auto getSequentialSizePtr = reinterpret_cast<GetSequentialSizeType*>(GetProcAddress(h, "AddModuleToSequential"));
    if (!getSequentialSizePtr) {
        std::cerr << "GetProcAddress for 'getSequentialSizePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getSequentialSizePtr = getSequentialSize;

    auto getSequentialModulePtr = reinterpret_cast<GetSequentialModuleType*>(GetProcAddress(h, "AddModuleToSequential"));
    if (!getSequentialModulePtr) {
        std::cerr << "GetProcAddress for 'getSequetialModulePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getSequentialModulePtr = getSequentialModule;

    auto forwardSequentialPtr = reinterpret_cast<ForwardSequentialType*>(GetProcAddress(h, "AddModuleToSequential"));
    if (!forwardSequentialPtr) {
        std::cerr << "GetProcAddress for 'forwardSequentialPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *forwardSequentialPtr = forwardSequential;

    // その他の Sequential 関数も同様に設定...

    // Module 関連の汎用関数
    auto registerModulePtr = reinterpret_cast<RegisterModuleType*>(GetProcAddress(h, "RegisterModule"));
    if (!registerModulePtr) {
        std::cerr << "GetProcAddress for 'registerModulePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *registerModulePtr = registerModule;

    // カスタムモジュール関連
    auto createTorchModulePtr = reinterpret_cast<CreateTorchModuleType*>(GetProcAddress(h, "CreateTorchModule"));
    if (!createTorchModulePtr) {
        std::cerr << "GetProcAddress for 'createTorchModulePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createTorchModulePtr = createTorchModule;

    /*auto setForwardFunctionPtr = reinterpret_cast<SetForwardFunctionType*>(GetProcAddress(h, "SetForwardFunction"));
    if (!forwardReLUPtr) {
        std::cerr << "GetProcAddress for 'cgetinchannels' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return;
    }
    *setForwardFunctionPtr = setForwardFunction;*/

    auto gcAddClassPtr = reinterpret_cast<GC_AddClassType*>(GetProcAddress(h, "GC_AddClass"));
    if (!gcAddClassPtr) {
        std::cerr << "GetProcAddress for 'GC_AddClass' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *gcAddClassPtr = GC_register_class;

    auto gcAddRootPtr = reinterpret_cast<GC_AddRootType*>(GetProcAddress(h, "GC_AddRoot"));
    if (!gcAddRootPtr) {
        std::cerr << "GetProcAddress for 'GC_AddRoot' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *gcAddRootPtr = GC_add_root_node;

    auto gcReleaseRootPtr = reinterpret_cast<GC_ReleaseRootType*>(GetProcAddress(h, "GC_ReleaseRoot"));
    if (!gcReleaseRootPtr) {
        std::cerr << "GetProcAddress for 'GC_SetRoot' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *gcReleaseRootPtr = GC_release_root_node;

    auto gcSetRootPtr = reinterpret_cast<GC_SetRootType*>(GetProcAddress(h, "GC_SetRoot"));
    if (!gcSetRootPtr) {
        std::cerr << "GetProcAddress for 'GC_SetRoot' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *gcSetRootPtr = GC_add_root;

    auto gcBackRootPtr = reinterpret_cast<GC_BackRootType*>(GetProcAddress(h, "GC_BackRoot"));
    if (!gcBackRootPtr) {
        std::cerr << "GetProcAddress for 'rnptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *gcBackRootPtr = GC_pop_roots;

    auto gcMallocPtr = reinterpret_cast<GC_mallocType*>(GetProcAddress(h, "GC_malloc"));
    if (!gcMallocPtr) {
        std::cerr << "GetProcAddress for 'GC_malloc' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *gcMallocPtr = GC_alloc;

    auto createAdamPtr = reinterpret_cast<CreateAdamType*>(GetProcAddress(h, "CreateAdam"));
    if (!createAdamPtr) {
        std::cerr << "GetProcAddress for 'CreateAdam' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createAdamPtr = createAdam;

    auto createAdamOptionsPtr = reinterpret_cast<CreateAdamOptionsType*>(GetProcAddress(h, "CreateAdamOptions"));
    if (!createAdamOptionsPtr) {
        std::cerr << "GetProcAddress for 'CreateAdamOptions' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createAdamOptionsPtr = createAdamOptions;

    auto optimizerZeroGradPtr = reinterpret_cast<OptimizerZeroGradType*>(GetProcAddress(h, "OptimizerZeroGrad"));
    if (!optimizerZeroGradPtr) {
        std::cerr << "GetProcAddress for 'OptimizerZeroGrad' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *optimizerZeroGradPtr = optimizerZeroGrad;

    auto optimizerStepPtr = reinterpret_cast<OptimizerStepType*>(GetProcAddress(h, "OptimizerStep"));
    if (!optimizerStepPtr) {
        std::cerr << "GetProcAddress for 'OptimizerStep' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *optimizerStepPtr = optimizerStep;

    // テンソル操作関連
    auto tensorToCUDAPtr = reinterpret_cast<TensorToCUDAType*>(GetProcAddress(h, "TensorToCUDA"));
    if (!tensorToCUDAPtr) {
        std::cerr << "GetProcAddress for 'TensorToCUDA' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *tensorToCUDAPtr = tensorToCUDA;

    auto tensorRequiresGradPtr = reinterpret_cast<TensorRequiresGradType*>(GetProcAddress(h, "TensorRequiresGrad"));
    if (!tensorRequiresGradPtr) {
        std::cerr << "GetProcAddress for 'TensorRequiresGrad' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *tensorRequiresGradPtr = tensorRequiresGrad;

    auto tensorBackwardPtr = reinterpret_cast<TensorBackwardType*>(GetProcAddress(h, "TensorBackward"));
    if (!tensorBackwardPtr) {
        std::cerr << "GetProcAddress for 'TensorBackward' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *tensorBackwardPtr = tensorBackward;

    // 損失計算関連
    auto crossEntropyPtr = reinterpret_cast<CrossEntropyType*>(GetProcAddress(h, "CrossEntropy"));
    if (!crossEntropyPtr) {
        std::cerr << "GetProcAddress for 'CrossEntropy' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *crossEntropyPtr = crossEntropy;

    auto copyObjectPtr = reinterpret_cast<CopyObjectType*>(GetProcAddress(h, "CopyObject"));
    if (!copyObjectPtr) {
        std::cerr << "GetProcAddress for 'CopyObject' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *copyObjectPtr = copy_object_young;

    auto cloneObjectPtr = reinterpret_cast<CopyObjectType*>(GetProcAddress(h, "CloneObject"));
    if (!cloneObjectPtr) {
        std::cerr << "GetProcAddress for 'CloneObject' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *cloneObjectPtr = GC_clone;

    auto getHashValuePtr = reinterpret_cast<GetHashValueType*>(GetProcAddress(h, "GetHashValue"));
    if (!getHashValuePtr) {
        std::cerr << "GetProcAddress for 'getHashValuePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getHashValuePtr = get_hashvalue;

    auto thgcp = (ThreadGC**)GetProcAddress(h, "thgcp");
    if (!thgcp) {
        std::cerr << "GetProcAddress for 'thgcp' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *thgcp = thgc;

    auto createListPtr = reinterpret_cast<CreateListType*>(GetProcAddress(h, "CreateList"));
    if (!createListPtr) {
        std::cerr << "GetProcAddress for 'createListPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createListPtr = create_list;

    auto addListPtr = reinterpret_cast<AddListType*>(GetProcAddress(h, "AddList"));
    if (!addListPtr) {
        std::cerr << "GetProcAddress for 'addListPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *addListPtr = add_list;

    auto getListPtr = reinterpret_cast<GetListType*>(GetProcAddress(h, "GetList"));
    if (!getListPtr) {
        std::cerr << "GetProcAddress for 'getListPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getListPtr = get_list;

    auto getLastPtr = reinterpret_cast<GetLastType*>(GetProcAddress(h, "GetLast"));
    if (!getLastPtr) {
        std::cerr << "GetProcAddress for 'getLastPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getLastPtr = get_last;

    auto setListPtr = reinterpret_cast<SetListType*>(GetProcAddress(h, "SetList"));
    if (!setListPtr) {
        std::cerr << "GetProcAddress for 'setListPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *setListPtr = set_list;

    auto setLastPtr = reinterpret_cast<SetLastType*>(GetProcAddress(h, "SetLast"));
    if (!setLastPtr) {
        std::cerr << "GetProcAddress for 'setLastPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *setLastPtr = set_last;

    auto createHashPtr = reinterpret_cast<CreateHashType*>(GetProcAddress(h, "CreateHash"));
    if (!createHashPtr) {
        std::cerr << "GetProcAddress for 'createHashPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createHashPtr = create_mapy;

    auto addHashPtr = reinterpret_cast<AddHashType*>(GetProcAddress(h, "AddHash"));
    if (!addHashPtr) {
        std::cerr << "GetProcAddress for 'addHashPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *addHashPtr = add_mapy;

    auto getHashPtr = reinterpret_cast<GetHashType*>(GetProcAddress(h, "GetHash"));
    if (!getHashPtr) {
        std::cerr << "GetProcAddress for 'getHashPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getHashPtr = get_mapy;

    auto createStringPtr = reinterpret_cast<CreateStringType*>(GetProcAddress(h, "CreateString"));
    if (!createStringPtr) {
        std::cerr << "GetProcAddress for 'createStringPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *createStringPtr = createString;

    auto addStringPtr = reinterpret_cast<AddStringType*>(GetProcAddress(h, "AddString"));
    if (!addStringPtr) {
        std::cerr << "GetProcAddress for 'addStringPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *addStringPtr = StringAdd;

    auto addStringPtr2 = reinterpret_cast<AddStringType2*>(GetProcAddress(h, "AddString2"));
    if (!addStringPtr2) {
        std::cerr << "GetProcAddress for 'addStringPtr2' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *addStringPtr2 = StringAdd2;

    auto removeStringPtr = reinterpret_cast<RemoveStringType*>(GetProcAddress(h, "RemoveString"));
    if (!removeStringPtr) {
        std::cerr << "GetProcAddress for 'removeStringPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *removeStringPtr = StringRemove;

    auto subStringPtr = reinterpret_cast<SubStringType*>(GetProcAddress(h, "SubString"));
    if (!subStringPtr) {
        std::cerr << "GetProcAddress for 'subStringPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *subStringPtr = SubString;

    auto getCharPtr = reinterpret_cast<GetCharType*>(GetProcAddress(h, "GetChar"));
    if (!getCharPtr) {
        std::cerr << "GetProcAddress for 'getCharPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *getCharPtr = GetChar;

    auto numberStringPtr = reinterpret_cast<NumberStringType*>(GetProcAddress(h, "NumberString"));
    if (!numberStringPtr) {
        std::cerr << "GetProcAddress for 'numberStringPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *numberStringPtr = NumberString;

    auto printStringPtr = reinterpret_cast<PrintStringType*>(GetProcAddress(h, "PrintString"));
    if (!printStringPtr) {
        std::cerr << "GetProcAddress for 'printStringPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *printStringPtr = PrintString;

    auto printArrayPtr = reinterpret_cast<PrintArrayType*>(GetProcAddress(h, "PrintArray"));
    if (!printArrayPtr) {
        std::cerr << "GetProcAddress for 'printArrayPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *printArrayPtr = PrintArray;

    auto dbInitPtr = reinterpret_cast<DbInitType*>(GetProcAddress(h, "DbInit"));
    if (!dbInitPtr) {
        std::cerr << "GetProcAddress for 'printArrayPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *dbInitPtr = DbInit;

    auto pushQueuePtr = reinterpret_cast<PushQueueType*>(GetProcAddress(h, "PushQueue"));
    if (!pushQueuePtr) {
        std::cerr << "GetProcAddress for 'pushQueuePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *pushQueuePtr = push_to_queue;

    auto waitHandlePtr = reinterpret_cast<WaitHandleType*>(GetProcAddress(h, "WaitHandle"));
    if (!waitHandlePtr) {
        std::cerr << "GetProcAddress for 'waitHandlePtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *waitHandlePtr = wait_handle;

    auto beginTransactionPtr = reinterpret_cast<BeginTransactionType*>(GetProcAddress(h, "BeginTransaction"));
    if (!beginTransactionPtr) {
        std::cerr << "GetProcAddress for 'beginTransactionPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *beginTransactionPtr = beginTransaction;

    auto execSqlPtr = reinterpret_cast<ExecSqlType*>(GetProcAddress(h, "ExecSql"));
    if (!execSqlPtr) {
        std::cerr << "GetProcAddress for 'execSqlPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *execSqlPtr = execSql;

    auto txCommitPtr = reinterpret_cast<TxFinishType*>(GetProcAddress(h, "TxCommit"));
    if (!txCommitPtr) {
        std::cerr << "GetProcAddress for 'txCommitPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *txCommitPtr = TxCommit;

    auto txRollbackPtr = reinterpret_cast<TxFinishType*>(GetProcAddress(h, "TxRollback"));
    if (!txCommitPtr) {
        std::cerr << "GetProcAddress for 'txRollbackPtr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *txRollbackPtr = TxRollback;

    auto stringUTF8Ptr = reinterpret_cast<StringUTF8Type*>(GetProcAddress(h, "StringUTF8"));
    if (!stringUTF8Ptr) {
        std::cerr << "GetProcAddress for 'stringUTF8' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *stringUTF8Ptr = StringUTF8;

    using CheckType = void (*) (Transaction*);
    auto checkPtr = reinterpret_cast<CheckType*>(GetProcAddress(h, "Check"));
    if (!checkPtr) {
        std::cerr << "GetProcAddress for 'check' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *checkPtr = Check;

    auto sqlIntPtr = reinterpret_cast<SqlIntType*>(GetProcAddress(h, "SqlInt"));
    if (!sqlIntPtr) {
        std::cerr << "GetProcAddress for 'sqlInt' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *sqlIntPtr = sqlite3_column_int;

    auto sqlStringPtr = reinterpret_cast<SqlStringType*>(GetProcAddress(h, "SqlString"));
    if (!sqlStringPtr) {
        std::cerr << "GetProcAddress for 'sqlString' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *sqlStringPtr = sqlite3_column_text;

    auto sqlLengthPtr = reinterpret_cast<SqlIntType*>(GetProcAddress(h, "SqlLength"));
    if (!sqlLengthPtr) {
        std::cerr << "GetProcAddress for 'sqlLength' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *sqlLengthPtr = sqlite3_column_int;

    auto gcANTPtr = reinterpret_cast<GCANTType*>(GetProcAddress(h, "GCANT"));
    if (!gcANTPtr) {
        std::cerr << "GetProcAddress for 'gcANT' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *gcANTPtr = CallGCant;

    auto crsANTPtr = reinterpret_cast<CreateStringType*>(GetProcAddress(h, "CreateStringANT"));
    if (!crsANTPtr) {
        std::cerr << "GetProcAddress for 'crsANT' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *crsANTPtr = createStringant;

    auto cloANTptr = reinterpret_cast<CopyObjectType*>(GetProcAddress(h, "CloneObjectANT"));
    if (!cloANTptr) {
        std::cerr << "GetProcAddress for 'cloANTptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *cloANTptr = GC_clone_unlocked;

    /*auto creDivptr = reinterpret_cast<CreateDivType*>(GetProcAddress(h, "CreateDiv"));
    if (!creDivptr) {
        std::cerr << "GetProcAddress for 'creDivptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *creDivptr = createDiv;

    auto creSheetptr = reinterpret_cast<CreateDivType*>(GetProcAddress(h, "CreateSheet"));
    if (!creSheetptr) {
        std::cerr << "GetProcAddress for 'creSheetptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *creSheetptr = createSheet;

    auto eleAddptr = reinterpret_cast<ElementAddType*>(GetProcAddress(h, "ElementAdd"));
    if (!eleAddptr) {
        std::cerr << "GetProcAddress for 'eleAddptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *eleAddptr = ElementAdd;

    auto creLetptr = reinterpret_cast<CreateLetterType*>(GetProcAddress(h, "CreateLetter"));
    if (!creLetptr) {
        std::cerr << "GetProcAddress for 'creLetptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *creLetptr = createLetter;

    auto creKaiptr = reinterpret_cast<CreateKaigyouType*>(GetProcAddress(h, "CreateKaigyou"));
    if (!creKaiptr) {
        std::cerr << "GetProcAddress for 'creKaiptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *creKaiptr = createKaigyou;

    auto creEndptr = reinterpret_cast<CreateKaigyouType*>(GetProcAddress(h, "CreateEnd"));
    if (!creEndptr) {
        std::cerr << "GetProcAddress for 'creEndptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *creEndptr = createEnd;*/

    auto cheTeptr = reinterpret_cast<CheckTreeElementType*>(GetProcAddress(h, "CheckTreeElement"));
    if (!cheTeptr) {
        std::cerr << "GetProcAddress for 'creTeptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *cheTeptr = checkTreeElement;

    auto mkfptr = reinterpret_cast<MakeFrameType*>(GetProcAddress(h, "MakeFrame"));
    if (!mkfptr) {
        std::cerr << "GetProcAddress for 'mkfptr' failed: " << GetLastError() << "\n";
        FreeLibrary(h);
        return NULL;
    }
    *mkfptr = MakeFrame;



    auto cnl = reinterpret_cast<int*>(GetProcAddress(h, "cnl"));
    int oldcn = thgc->class_count;
    thgc->class_count += *cnl;

    using VoiFun = void (*)(int);
    VoiFun mainF = reinterpret_cast<VoiFun>(GetProcAddress(h, "main"));
    mainF(oldcn);

    using Test2 = int (*)();
    Test2 test2 = reinterpret_cast<Test2>(
        GetProcAddress(h, "test2")
        );
    int nn = test2();
    std::cout << nn << std::endl;
    return NULL;
}
CustomModuleImpl* GoThread(ThreadGC* thgc) {

    GC_register_class(thgc, _String, "Str", sizeof(String), NULL, NULL);
    GC_register_class(thgc, _List, "List", sizeof(List), ListCheck, NULL);
    GC_register_class(thgc, _Element, "Elem", sizeof(NewElement), NULL, NULL);
    GC_register_class(thgc, _FRect, "FRect", sizeof(SDL_FRect), NULL, NULL);
    GC_register_class(thgc, _LetterC, "Letter", sizeof(NewLetter), NULL, NULL);
    GC_register_class(thgc, _LineC, "Line", sizeof(NewLine), NULL, NULL);
    //GC_SetClass(thgc, _VLineC, "VirtualLine", sizeof(VLine), VLineCheck, NULL);
    //GC_SetClass(thgc, _Select, "Select", sizeof(Select), SelectCheck, NULL);
    //GC_SetClass(thgc, _State, "State", sizeof(State), StateCheck, NULL);
    GC_register_class(thgc, _LocalC, "Local", sizeof(NewLocal), NULL, NULL);
    //GC_SetClass(thgc, _LetterPart, "LetterPart", sizeof(LetterPart), LetterPartCheck, NULL);
    GC_register_class(thgc, _KV, "KeyValue", sizeof(KV), KVCheck, NULL);
    GC_register_class(thgc, _MapData, "MapData", sizeof(MapData), MapDataCheck, NULL);
    GC_register_class(thgc, _Map, "Map", sizeof(Map), MapCheck, NULL);
    GC_register_class(thgc, _EndC, "End", sizeof(NewEndElement), NULL, NULL);
    //GC_SetClass(thgc, _CloneElemC, "CloneElem", sizeof(CloneElement), CloneElementCheck, NULL);
    //GC_SetClass(thgc, _HoppyWindow, "HoppyWindow", sizeof(HoppyWindow), HoppyWindowCheck, NULL);
    GC_register_class(thgc, _ColumnMeta, "ColumnMeta", sizeof(ColumnMeta), NULL, NULL);
    GC_register_class(thgc, _Table, "Table", sizeof(Table), NULL, NULL);
    GC_register_class(thgc, _Column, "Column", sizeof(Column), NULL, NULL);
    GC_register_class(thgc, _TreeElement, "TreeElement", sizeof(TreeElement), NULL, NULL);
    //GC_SetClass(thgc, _DivC, "Div", sizeof(Div), NULL, NULL);
    //GC_SetClass(thgc, _SheetC, "Sheet", sizeof(Sheet), NULL, NULL);
    GC_register_class(thgc, _MemTable, "MemTable", sizeof(MemTable), NULL, NULL);
    GC_register_class(thgc, _MemFunc, "MemFunc", sizeof(MemFunc), NULL, NULL);
    GC_register_class(thgc, _FuncType, "FuncType", sizeof(FuncType), NULL, NULL);
    //GC_SetClass(thgc, _MouseEvent, "MouseEvent", sizeof(MouseEvent), NULL, NULL);
    //GC_SetClass(thgc, _KeyEvent, "KeyEvent", sizeof(KeyEvent), NULL, NULL);
    GC_register_class(thgc, _Offscreen, "Offscreen", sizeof(Offscreen), NULL, NULL);
    //HoppyWindow* hw = (HoppyWindow*)GC_malloc(thgc, _HoppyWindow);
    //initHoppyWindow(thgc, hw);
	NewLocal* local = (NewLocal*)GC_alloc(thgc, _LocalC);
    initNewLocal(thgc, local);
    NewLetter* let = (NewLetter*)GC_alloc(thgc, _LetterC);
    initNewLetter(thgc, let, getFont("sans", 16), _Letter);
	let->text = createString(thgc, (char*)"Hello,world!", 13, 1);
    let->color = 0xFFFFFFFF;
    NewElementAddLast(thgc, local, (NewElement*)local, (NewElement*)let);
    thgc->map = create_mapy(magc, false);
    String* str = createString(thgc, (char*)"main", 4, 1);
    TreeElement* te = (TreeElement*)GC_alloc(thgc, _TreeElement);
    te->id = str;
    te->elem = (NewElement*)local;
    te->children = create_list(thgc, sizeof(TreeElement*), true);
    add_mapy(thgc, thgc->map, str, (char*)te);
    /*ReadDll(thgc);*/
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        /*while (thgc->first != NULL) {
            thgc->first->resume_all();
        }
        thgc->queue->resume_all();*/
        thgc->hoppy->buildFrame(now_us());
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(end - start).count();
        if (12.0 - ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(12.0 - ms)));
    }
    // 2) エクスポート名 "add" を関数ポインタとして取得
    /*using BaseFunc = CustomModuleImpl * (*)();
    BaseFunc test = reinterpret_cast<BaseFunc>(
        GetProcAddress(h, "test")
        );
    return test();*/
    return NULL;
}
void runGoThreadAsync(ThreadGC* thgc) {
    std::thread([thgc]() {
        initDone.get_future().wait();
        CustomModuleImpl* result = GoThread(thgc);
        if (result) {
            // 結果を使用
            std::cout << "GoThread completed successfully" << std::endl;
        }
        else {
            std::cerr << "GoThread failed" << std::endl;
        }
        }).detach();
}