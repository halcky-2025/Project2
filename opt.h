AVFormatContext* formatContext = NULL;
const AVInputFormat* input_format;
AVDeviceInfoList* device_list = NULL;
AVCodecParameters* codecParam;
const AVCodec* codec;
AVCodecContext* codecContext;
AVPacket* packet;
bool came = false;
int videoStreamIndex;
AVFrame* frame;
struct Camera2 {
    AVFrame* frameRGB;
    Camera2() {
        if (!came) {
            avdevice_register_all();
            formatContext = NULL;
            input_format = av_find_input_format("dshow");
            device_list = NULL;
            int ret = avdevice_list_input_sources(input_format, "video", NULL, &device_list);
            if (ret < 0) {
                std::cerr << "�f�o�C�X�̎擾�Ɏ��s���܂���" << std::endl;
                return;
            }
            char fullname[128];
            strcpy_s(fullname, sizeof(fullname), "video=");
            strcat_s(fullname, sizeof(fullname), device_list->devices[0]->device_name);
            avformat_open_input(&formatContext, fullname, input_format, NULL);
            if (!formatContext) {
                std::cerr << "�f�o�C�X�̃I�[�v���Ɏ��s���܂���" << std::endl;
                return;
            }
            int videoStreamIndex = -1;
            for (unsigned i = 0; i < formatContext->nb_streams; i++) {
                if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                    videoStreamIndex = i;
                    break;
                }
            }
            if (videoStreamIndex == -1) {
                std::cerr << "�r�f�I�X�g���[����������܂���ł���" << std::endl;
                return;
            }
            codecParam = formatContext->streams[videoStreamIndex]->codecpar;
            codec = avcodec_find_decoder(codecParam->codec_id);
            codecContext = avcodec_alloc_context3(codec);
            avcodec_parameters_to_context(codecContext, codecParam);
            avcodec_open2(codecContext, codec, NULL);
            packet = av_packet_alloc();
            came = true;
            frame = av_frame_alloc();
        }
        frameRGB = av_frame_alloc();
        frameRGB->width = 640;
        frameRGB->height = 480;
        frameRGB->format = AV_PIX_FMT_RGB24;
    }
};
struct NPCNNImpl : torch::nn::Module{

	torch::nn::Sequential net;
	NPCNNImpl() {
        if (!cuda) {
            torch_cuda_dll = LoadLibraryA("torch_cuda.dll");
            cuda = true;
        }
		register_module("net", net);
	}
	torch::Tensor forward(torch::Tensor x) {
		return net->forward(x);
	}
};
TORCH_MODULE(NPCNN);
struct NuroPlan {
    NPCNN model;
    std::unique_ptr<torch::optim::Adam> optimizer;
    std::vector<torch::Tensor> input_tensors;
    std::vector<torch::Tensor> goal_tensors;
    void init() {
        optimizer = std::make_unique<torch::optim::Adam>(model->parameters(), torch::optim::AdamOptions(1e-3));
        std::filesystem::create_directories("inputs/faces");
        std::filesystem::create_directories("inputs/nonfaces");
        for (const auto& entry : std::filesystem::directory_iterator("inputs/faces")) {
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
        for (const auto& entry : std::filesystem::directory_iterator("inputs/nonfaces")) {
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

    }
    void training() {
        model->to(torch::kCUDA);
        model->train();

        torch::Tensor loss;
        for (int epoch = 0; epoch < 1000; epoch++) {
            size_t batch_index = 0;
            for (int i = 0; i < input_tensors.size(); i++) {
                torch::Tensor input = input_tensors[i].unsqueeze(0).to(torch::kCUDA);
                torch::Tensor goal = goal_tensors[i].unsqueeze(0).to(torch::kCUDA);
                int n = goal_tensors[i].item<int>();
                optimizer->zero_grad();
                torch::Tensor output = model->forward(input);
                loss = F::cross_entropy(output, goal);
                loss.backward();
                optimizer->step();
                if (epoch % 100 + 1 == 100) {
                    std::cout << "Raw output: " << output << std::endl;
                    std::cout << "Epoch: " << epoch << ":" << i << ":" << loss.item<float>() << std::endl;
                }
            }
        }
        torch::save(model, "model.pt");
        torch::save(*(this->optimizer), "optimizer.pt");
    }
    int exe(AVFrame* frameRGB) {
        model->to(torch::kCUDA);
        model->eval();

        torch::Tensor input_tensor = torch::from_blob(
            frameRGB->data[0],
            { 1, 480, 640, 3 },  // NHWC
            torch::kUInt8
        ).permute({ 0, 3, 1, 2 })  // �� NCHW
            .to(torch::kFloat).div(255).to(torch::kCUDA);

        // �K�v�ɉ����� resize�i���f���� 64x48 �O��j
        input_tensor = F::interpolate(input_tensor, F::InterpolateFuncOptions().size(std::vector<int64_t>({ 64, 48 })).mode(torch::kBilinear));

        torch::Tensor output = model->forward(input_tensor);  // shape: [1, 2]
        return output.argmax(1).item<int>();
    }
    void input() {}
    torch::Tensor output() {
    }
};