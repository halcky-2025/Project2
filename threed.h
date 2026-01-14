#include <vector>
#include <string>
#include <map>
#include "platform_io.h"

// glTF 2.0 �f�[�^�\��
struct Node {
    std::string name;
    int parent = -1;
    float translation[3] = { 0, 0, 0 };
    float rotation[4] = { 0, 0, 0, 1 };  // �N�H�[�^�j�I�� [x, y, z, w]
    float scale[3] = { 1, 1, 1 };
    std::vector<int> children;
    int skinIndex = -1;
    int meshIndex = -1;
};

struct Skin {
    std::string name;
    std::vector<int> joints;  // �W���C���g�̃m�[�h�C���f�b�N�X
    std::vector<float> inverseBindMatrices;  // 16�v�f * joints��
    int skeletonRoot = -1;
};

struct AnimationSampler {
    enum Interpolation { LINEAR, STEP, CUBICSPLINE };
    std::vector<float> inputTimes;
    std::vector<float> outputValues;
    Interpolation interpolation = LINEAR;
};

struct AnimationChannel {
    enum PathType { TRANSLATION, ROTATION, SCALE };
    int samplerIndex;
    int targetNode;
    PathType path;
};

struct Animation {
    std::string name;
    std::vector<AnimationSampler> samplers;
    std::vector<AnimationChannel> channels;
    float duration = 0.0f;
};

// �A�j���[�V�����V�X�e��
class GltfAnimator {
private:
    std::vector<Node> nodes;
    std::vector<Skin> skins;
    std::vector<Animation> animations;
    std::vector<float> nodeMatrices;  // �e�m�[�h�̃��[�J�������[���h�ϊ�
    std::vector<float> skinMatrices;   // �X�L�j���O�p�̍ŏI�}�g���b�N�X

    float currentTime = 0.0f;
    int currentAnimation = 0;

public:
    void init(const std::vector<Node>& n, const std::vector<Skin>& s,
        const std::vector<Animation>& a) {
        nodes = n;
        skins = s;
        animations = a;
        nodeMatrices.resize(nodes.size() * 16);

        // ���m�[�h�}�g���b�N�X��P�ʍs��ŏ�������
        for (size_t i = 0; i < nodes.size(); i++) {
            float* mtx = &nodeMatrices[i * 16];
            bx::mtxIdentity(mtx);
        }

        if (!skins.empty()) {
            skinMatrices.resize(skins[0].joints.size() * 16);

            // ���X�L���}�g���b�N�X���P�ʍs��ŏ�������
            for (size_t i = 0; i < skins[0].joints.size(); i++) {
                float* mtx = &skinMatrices[i * 16];
                bx::mtxIdentity(mtx);
            }
        }

        // ���d�v�F�����|�[�Y���v�Z��
        updateNodeHierarchy();
        if (!skins.empty()) {
            updateSkinMatrices();
        }

        printf("Initialized animator with %zu nodes, %zu joints\n",
            nodes.size(), skins.empty() ? 0 : skins[0].joints.size());
    }

    // �A�j���[�V�����X�V
    void update(float deltaTime) {
        if (animations.empty()) return;

        Animation& anim = animations[currentAnimation];
        currentTime += deltaTime;

        // ���[�v����
        if (currentTime > anim.duration) {
            currentTime = fmod(currentTime, anim.duration);
        }

        // �A�j���[�V�����`�����l����K�p
        for (const auto& channel : anim.channels) {
            applyAnimation(channel, anim.samplers[channel.samplerIndex]);
        }

        // �m�[�h�K�w���X�V
        updateNodeHierarchy();

        // �X�L���}�g���b�N�X���v�Z
        if (!skins.empty()) {
            updateSkinMatrices();
        }
    }

    void setAnimation(int animIndex) {
        if (animIndex >= 0 && animIndex < animations.size()) {
            currentAnimation = animIndex;
            currentTime = 0.0f;
        }
    }

    const float* getSkinMatrices() const {
        return skinMatrices.data();
    }

    int getSkinMatrixCount() const {
        return skinMatrices.size() / 16;
    }
    int getAnimationCount() const {
        return animations.size();
    }

private:
    void applyAnimation(const AnimationChannel& channel,
        const AnimationSampler& sampler) {
        if (channel.targetNode < 0 || channel.targetNode >= nodes.size()) return;
        if (sampler.inputTimes.empty()) return;

        Node& node = nodes[channel.targetNode];

        int idx0 = 0, idx1 = 0;
        float t = 0.0f;

        // �ŏ��̃L�[�t���[�����O�̏ꍇ
        if (currentTime <= sampler.inputTimes[0]) {
            idx0 = 0;
            idx1 = 0;
            t = 0.0f;
        }
        // �Ō�̃L�[�t���[������̏ꍇ
        else if (currentTime >= sampler.inputTimes.back()) {
            idx0 = sampler.inputTimes.size() - 1;
            idx1 = idx0;
            t = 0.0f;
        }
        // ���Ԃ̏ꍇ
        else {
            for (size_t i = 0; i < sampler.inputTimes.size() - 1; i++) {
                if (currentTime >= sampler.inputTimes[i] &&
                    currentTime < sampler.inputTimes[i + 1]) {
                    idx0 = i;
                    idx1 = i + 1;
                    float dt = sampler.inputTimes[idx1] - sampler.inputTimes[idx0];
                    if (dt > 0) {
                        t = (currentTime - sampler.inputTimes[idx0]) / dt;
                    }
                    break;
                }
            }
        }

        // �����C���f�b�N�X�̏ꍇ�͕�Ԃ��Ȃ�
        if (idx0 == idx1) {
            t = 0.0f;
        }

        // ��Ԃ��Ēl��K�p
        switch (channel.path) {
        case AnimationChannel::TRANSLATION: {
            float v0[3] = {
                sampler.outputValues[idx0 * 3 + 0],
                sampler.outputValues[idx0 * 3 + 1],
                sampler.outputValues[idx0 * 3 + 2]
            };
            float v1[3] = {
                sampler.outputValues[idx1 * 3 + 0],
                sampler.outputValues[idx1 * 3 + 1],
                sampler.outputValues[idx1 * 3 + 2]
            };
            node.translation[0] = v0[0] + (v1[0] - v0[0]) * t;
            node.translation[1] = v0[1] + (v1[1] - v0[1]) * t;
            node.translation[2] = v0[2] + (v1[2] - v0[2]) * t;
            break;
        }
        case AnimationChannel::ROTATION: {
            float q0[4] = {
                sampler.outputValues[idx0 * 4 + 0],
                sampler.outputValues[idx0 * 4 + 1],
                sampler.outputValues[idx0 * 4 + 2],
                sampler.outputValues[idx0 * 4 + 3]
            };
            float q1[4] = {
                sampler.outputValues[idx1 * 4 + 0],
                sampler.outputValues[idx1 * 4 + 1],
                sampler.outputValues[idx1 * 4 + 2],
                sampler.outputValues[idx1 * 4 + 3]
            };
            quatSlerp(node.rotation, q0, q1, t);
            break;
        }
        case AnimationChannel::SCALE: {
            float s0[3] = {
                sampler.outputValues[idx0 * 3 + 0],
                sampler.outputValues[idx0 * 3 + 1],
                sampler.outputValues[idx0 * 3 + 2]
            };
            float s1[3] = {
                sampler.outputValues[idx1 * 3 + 0],
                sampler.outputValues[idx1 * 3 + 1],
                sampler.outputValues[idx1 * 3 + 2]
            };
            node.scale[0] = s0[0] + (s1[0] - s0[0]) * t;
            node.scale[1] = s0[1] + (s1[1] - s0[1]) * t;
            node.scale[2] = s0[2] + (s1[2] - s0[2]) * t;
            break;
        }
        }
    }

    void updateNodeHierarchy() {
        // ���[�g�m�[�h����K�w�I�ɍX�V
        for (size_t i = 0; i < nodes.size(); i++) {
            if (nodes[i].parent == -1) {
                updateNodeRecursive(i);
            }
        }
    }

    void updateNodeRecursive(int nodeIndex) {

        Node& node = nodes[nodeIndex];
        float* nodeMtx = &nodeMatrices[nodeIndex * 16];

        // TRS�s����\�z
        float mtxT[16], mtxR[16], mtxS[16];
        bx::mtxTranslate(mtxT, node.translation[0], node.translation[1], node.translation[2]);
        quatToMatrix(mtxR, node.rotation);
        bx::mtxScale(mtxS, node.scale[0], node.scale[1], node.scale[2]);

        float temp[16], localMtx[16];
        bx::mtxMul(temp, mtxR, mtxS);
        bx::mtxMul(localMtx, mtxT, temp);

        // �e�̕ϊ���K�p
        if (node.parent >= 0) {
            float* parentMtx = &nodeMatrices[node.parent * 16];
            float worldMtx[16];
            bx::mtxMul(worldMtx, parentMtx, localMtx);

            // bgfx�͗�D��Ȃ̂œ]�u
            bx::mtxTranspose(nodeMtx, worldMtx);
        }
        else {
            // bgfx�͗�D��Ȃ̂œ]�u
            bx::mtxTranspose(nodeMtx, localMtx);
        }

        // �q�m�[�h���ċA�I�ɏ���
        for (int childIdx : node.children) {
            updateNodeRecursive(childIdx);
        }
    }

    // �N�H�[�^�j�I������4x4��]�s��ւ̕ϊ�
    void quatToMatrix(float* mtx, const float* q) {
        float x = q[0], y = q[1], z = q[2], w = q[3];
        float x2 = x * x, y2 = y * y, z2 = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = w * x, wy = w * y, wz = w * z;

        mtx[0] = 1.0f - 2.0f * (y2 + z2);
        mtx[1] = 2.0f * (xy + wz);
        mtx[2] = 2.0f * (xz - wy);
        mtx[3] = 0.0f;

        mtx[4] = 2.0f * (xy - wz);
        mtx[5] = 1.0f - 2.0f * (x2 + z2);
        mtx[6] = 2.0f * (yz + wx);
        mtx[7] = 0.0f;

        mtx[8] = 2.0f * (xz + wy);
        mtx[9] = 2.0f * (yz - wx);
        mtx[10] = 1.0f - 2.0f * (x2 + y2);
        mtx[11] = 0.0f;

        mtx[12] = 0.0f;
        mtx[13] = 0.0f;
        mtx[14] = 0.0f;
        mtx[15] = 1.0f;
    }

    void updateSkinMatrices() {
        if (skins.empty()) return;

        const Skin& skin = skins[0];

        for (size_t i = 0; i < skin.joints.size(); i++) {
            int jointNode = skin.joints[i];
            float* jointMtxTransposed = &nodeMatrices[jointNode * 16];
            const float* invBindMtxTransposed = &skin.inverseBindMatrices[i * 16];

            // �����Ƃ����ɓ]�u�ς݂Ȃ̂ŁA���̂܂܏�Z
            float* skinMtx = &skinMatrices[i * 16];
            bx::mtxMul(skinMtx, jointMtxTransposed, invBindMtxTransposed);
        }
    }

    // ���ʐ��`���
    void quatSlerp(float* result, const float* q0, const float* q1, float t) {
        float dot = q0[0] * q1[0] + q0[1] * q1[1] + q0[2] * q1[2] + q0[3] * q1[3];

        // �ŒZ�o�H��I��
        float q1_adj[4];
        if (dot < 0.0f) {
            q1_adj[0] = -q1[0];
            q1_adj[1] = -q1[1];
            q1_adj[2] = -q1[2];
            q1_adj[3] = -q1[3];
            dot = -dot;
        }
        else {
            q1_adj[0] = q1[0];
            q1_adj[1] = q1[1];
            q1_adj[2] = q1[2];
            q1_adj[3] = q1[3];
        }

        // ���`��Ԃŋߎ�
        if (dot > 0.9995f) {
            result[0] = q0[0] + t * (q1_adj[0] - q0[0]);
            result[1] = q0[1] + t * (q1_adj[1] - q0[1]);
            result[2] = q0[2] + t * (q1_adj[2] - q0[2]);
            result[3] = q0[3] + t * (q1_adj[3] - q0[3]);
        }
        else {
            float theta = acosf(dot);
            float sinTheta = sinf(theta);
            float w0 = sinf((1.0f - t) * theta) / sinTheta;
            float w1 = sinf(t * theta) / sinTheta;

            result[0] = w0 * q0[0] + w1 * q1_adj[0];
            result[1] = w0 * q0[1] + w1 * q1_adj[1];
            result[2] = w0 * q0[2] + w1 * q1_adj[2];
            result[3] = w0 * q0[3] + w1 * q1_adj[3];
        }

        // ���K��
        float len = sqrtf(result[0] * result[0] + result[1] * result[1] +
            result[2] * result[2] + result[3] * result[3]);
        if (len > 0) {
            result[0] /= len;
            result[1] /= len;
            result[2] /= len;
            result[3] /= len;
        }
    }
};
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// JSON���C�u�����̑I���i�ǂ��炩1�j
#define TINYGLTF_NO_EXTERNAL_IMAGE
// tinygltf ���g�p�������[�_�[
#include "tiny_gltf.h"

class GltfLoader {
public:
    tinygltf::Model* loadFromFile(const std::string& filename, GltfAnimator& animator) {
        tinygltf::Model* model = new tinygltf::Model();
        tinygltf::TinyGLTF loader;
        std::string err, warn;

        // �t�@�C���g���q���擾�i�啶������������ʂ��Ȃ��j
        std::string ext = getFileExtension(filename);

        bool ret = false;
        if (ext == "glb") {
            // �o�C�i���`��(.glb)
            printf("Loading binary glTF file: %s\n", filename.c_str());
            ret = loader.LoadBinaryFromFile(model, &err, &warn, filename);
        }
        else if (ext == "gltf") {
            // ASCII/JSON�`��(.gltf)
            printf("Loading ASCII glTF file: %s\n", filename.c_str());
            ret = loader.LoadASCIIFromFile(model, &err, &warn, filename);
        }
        else {
            printf("Unknown file extension: %s (expected .gltf or .glb)\n", ext.c_str());
            return NULL;
        }

        if (!ret) {
            printf("Failed to load glTF: %s\n", err.c_str());
            return NULL;
        }

        if (!warn.empty()) {
            printf("glTF warning: %s\n", warn.c_str());
        }

        printf("Successfully loaded glTF file:\n");
        printf("  Nodes: %zu\n", model->nodes.size());
        printf("  Meshes: %zu\n", model->meshes.size());
        printf("  Animations: %zu\n", model->animations.size());
        printf("  Skins: %zu\n", model->skins.size());

        // �m�[�h�����[�h
        std::vector<Node> nodes = loadNodes(*model);

        // �X�L�������[�h
        std::vector<Skin> skins = loadSkins(*model);

        // �A�j���[�V���������[�h
        std::vector<Animation> animations = loadAnimations(*model);

        animator.init(nodes, skins, animations);
        return model;
    }

private:
    // �t�@�C���g���q���擾�i�������ɕϊ��j
    std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        if (pos == std::string::npos) {
            return "";
        }

        std::string ext = filename.substr(pos + 1);
        // �������ɕϊ�
        std::transform(ext.begin(), ext.end(), ext.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return ext;
    }

    std::vector<Node> loadNodes(const tinygltf::Model& model) {
        std::vector<Node> nodes;

        for (size_t i = 0; i < model.nodes.size(); i++) {
            const tinygltf::Node& gltfNode = model.nodes[i];
            Node node;

            node.name = gltfNode.name;

            // Translation
            if (gltfNode.translation.size() == 3) {
                node.translation[0] = (float)gltfNode.translation[0];
                node.translation[1] = (float)gltfNode.translation[1];
                node.translation[2] = (float)gltfNode.translation[2];
            }

            // Rotation (�N�H�[�^�j�I��)
            if (gltfNode.rotation.size() == 4) {
                node.rotation[0] = (float)gltfNode.rotation[0];
                node.rotation[1] = (float)gltfNode.rotation[1];
                node.rotation[2] = (float)gltfNode.rotation[2];
                node.rotation[3] = (float)gltfNode.rotation[3];
            }

            // Scale
            if (gltfNode.scale.size() == 3) {
                node.scale[0] = (float)gltfNode.scale[0];
                node.scale[1] = (float)gltfNode.scale[1];
                node.scale[2] = (float)gltfNode.scale[2];
            }

            // Matrix (���ڍs�񂪎w�肳��Ă���ꍇ)
            if (gltfNode.matrix.size() == 16) {
                // �s�񂩂�TRS�𒊏o
                extractTRS(gltfNode.matrix, node);
            }

            node.children = gltfNode.children;
            node.skinIndex = gltfNode.skin;
            node.meshIndex = gltfNode.mesh;

            nodes.push_back(node);
        }

        // �e�q�֌W��ݒ�
        for (size_t i = 0; i < nodes.size(); i++) {
            for (int childIdx : nodes[i].children) {
                if (childIdx >= 0 && childIdx < (int)nodes.size()) {
                    nodes[childIdx].parent = i;
                }
            }
        }

        return nodes;
    }

    // �s�񂩂�TRS�𒊏o�i�ȈՔŁj
    void extractTRS(const std::vector<double>& matrix, Node& node) {
        // ���s�ړ�
        node.translation[0] = (float)matrix[12];
        node.translation[1] = (float)matrix[13];
        node.translation[2] = (float)matrix[14];

        // �X�P�[���𒊏o
        float sx = sqrtf(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
        float sy = sqrtf(matrix[4] * matrix[4] + matrix[5] * matrix[5] + matrix[6] * matrix[6]);
        float sz = sqrtf(matrix[8] * matrix[8] + matrix[9] * matrix[9] + matrix[10] * matrix[10]);

        node.scale[0] = sx;
        node.scale[1] = sy;
        node.scale[2] = sz;

        // ��]�s��i�X�P�[���������j
        if (sx > 0 && sy > 0 && sz > 0) {
            float m[9] = {
                (float)(matrix[0] / sx), (float)(matrix[1] / sx), (float)(matrix[2] / sx),
                (float)(matrix[4] / sy), (float)(matrix[5] / sy), (float)(matrix[6] / sy),
                (float)(matrix[8] / sz), (float)(matrix[9] / sz), (float)(matrix[10] / sz)
            };
            matrixToQuat(m, node.rotation);
        }
    }

    // 3x3��]�s�񂩂�N�H�[�^�j�I���ւ̕ϊ�
    void matrixToQuat(const float* m, float* q) {
        float trace = m[0] + m[4] + m[8];

        if (trace > 0) {
            float s = sqrtf(trace + 1.0f) * 2.0f;
            q[3] = 0.25f * s;
            q[0] = (m[7] - m[5]) / s;
            q[1] = (m[2] - m[6]) / s;
            q[2] = (m[3] - m[1]) / s;
        }
        else if (m[0] > m[4] && m[0] > m[8]) {
            float s = sqrtf(1.0f + m[0] - m[4] - m[8]) * 2.0f;
            q[3] = (m[7] - m[5]) / s;
            q[0] = 0.25f * s;
            q[1] = (m[1] + m[3]) / s;
            q[2] = (m[2] + m[6]) / s;
        }
        else if (m[4] > m[8]) {
            float s = sqrtf(1.0f + m[4] - m[0] - m[8]) * 2.0f;
            q[3] = (m[2] - m[6]) / s;
            q[0] = (m[1] + m[3]) / s;
            q[1] = 0.25f * s;
            q[2] = (m[5] + m[7]) / s;
        }
        else {
            float s = sqrtf(1.0f + m[8] - m[0] - m[4]) * 2.0f;
            q[3] = (m[3] - m[1]) / s;
            q[0] = (m[2] + m[6]) / s;
            q[1] = (m[5] + m[7]) / s;
            q[2] = 0.25f * s;
        }
    }

    std::vector<Skin> loadSkins(const tinygltf::Model& model) {

        std::vector<Skin> skins;

        for (const auto& gltfSkin : model.skins) {
            Skin skin;
            skin.name = gltfSkin.name;
            skin.joints = gltfSkin.joints;
            skin.skeletonRoot = gltfSkin.skeleton;

            // �t�o�C���h�s����擾
            if (gltfSkin.inverseBindMatrices >= 0) {
                const tinygltf::Accessor& accessor =
                    model.accessors[gltfSkin.inverseBindMatrices];
                const tinygltf::BufferView& bufferView =
                    model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer =
                    model.buffers[bufferView.buffer];

                const float* matrices = reinterpret_cast<const float*>(
                    &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                int matrixCount = accessor.count;
                skin.inverseBindMatrices.resize(matrixCount * 16);

                // glTF�̗�D��s���]�u
                for (int i = 0; i < matrixCount; i++) {
                    const float* src = &matrices[i * 16];
                    float* dst = &skin.inverseBindMatrices[i * 16];

                    // �]�u�R�s�[
                    for (int row = 0; row < 4; row++) {
                        for (int col = 0; col < 4; col++) {
                            dst[row * 4 + col] = src[col * 4 + row];
                        }
                    }
                }
            }

            skins.push_back(skin);
        }

        return skins;
    }

    std::vector<Animation> loadAnimations(const tinygltf::Model& model) {
        std::vector<Animation> animations;

        for (const auto& gltfAnim : model.animations) {
            Animation anim;
            anim.name = gltfAnim.name.empty() ? "Animation" : gltfAnim.name;

            // �T���v���[�����[�h
            for (const auto& gltfSampler : gltfAnim.samplers) {
                AnimationSampler sampler;

                // ���͎���
                const tinygltf::Accessor& inputAccessor =
                    model.accessors[gltfSampler.input];
                sampler.inputTimes = getAccessorData<float>(model, inputAccessor);

                // �o�͒l
                const tinygltf::Accessor& outputAccessor =
                    model.accessors[gltfSampler.output];
                sampler.outputValues = getAccessorData<float>(model, outputAccessor);

                // ��ԕ��@
                if (gltfSampler.interpolation == "LINEAR") {
                    sampler.interpolation = AnimationSampler::LINEAR;
                }
                else if (gltfSampler.interpolation == "STEP") {
                    sampler.interpolation = AnimationSampler::STEP;
                }
                else if (gltfSampler.interpolation == "CUBICSPLINE") {
                    sampler.interpolation = AnimationSampler::CUBICSPLINE;
                }

                anim.samplers.push_back(sampler);
            }

            // �`�����l�������[�h
            for (const auto& gltfChannel : gltfAnim.channels) {
                AnimationChannel channel;
                channel.samplerIndex = gltfChannel.sampler;
                channel.targetNode = gltfChannel.target_node;

                if (gltfChannel.target_path == "translation") {
                    channel.path = AnimationChannel::TRANSLATION;
                }
                else if (gltfChannel.target_path == "rotation") {
                    channel.path = AnimationChannel::ROTATION;
                }
                else if (gltfChannel.target_path == "scale") {
                    channel.path = AnimationChannel::SCALE;
                }

                anim.channels.push_back(channel);
            }

            // �A�j���[�V���������v�Z
            for (const auto& sampler : anim.samplers) {
                if (!sampler.inputTimes.empty()) {
                    anim.duration = std::max(anim.duration,
                        sampler.inputTimes.back());
                }
            }

            printf("  Animation '%s': duration=%.2fs, channels=%zu\n",
                anim.name.c_str(), anim.duration, anim.channels.size());

            animations.push_back(anim);
        }

        return animations;
    }

    template<typename T>
    std::vector<T> getAccessorData(const tinygltf::Model& model,
        const tinygltf::Accessor& accessor) {
        const tinygltf::BufferView& bufferView =
            model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        const T* data = reinterpret_cast<const T*>(
            &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

        int componentCount = 1;
        switch (accessor.type) {
        case TINYGLTF_TYPE_SCALAR: componentCount = 1; break;
        case TINYGLTF_TYPE_VEC2: componentCount = 2; break;
        case TINYGLTF_TYPE_VEC3: componentCount = 3; break;
        case TINYGLTF_TYPE_VEC4: componentCount = 4; break;
        case TINYGLTF_TYPE_MAT4: componentCount = 16; break;
        }

        std::vector<T> result(accessor.count * componentCount);
        memcpy(result.data(), data,
            accessor.count * componentCount * sizeof(T));

        return result;
    }
};

// �g�p��
struct GltfMesh {
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
    uint32_t indexCount = 0;


    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;  // UV���W�ǉ�
        int16_t joints[4];
        float weights[4];
    };
};
class GltfMeshLoader {
public:
    // tinygltf::Model���烁�b�V����ǂݍ���
    std::vector<GltfMesh> loadMeshes(const tinygltf::Model& model) {
        std::vector<GltfMesh> meshes;

        printf("Loading %zu meshes from glTF\n", model.meshes.size());

        for (size_t i = 0; i < model.meshes.size(); i++) {
            const tinygltf::Mesh& mesh = model.meshes[i];
            printf("  Mesh %zu: '%s' with %zu primitives\n",
                i, mesh.name.c_str(), mesh.primitives.size());

            for (size_t j = 0; j < mesh.primitives.size(); j++) {
                const tinygltf::Primitive& primitive = mesh.primitives[j];

                GltfMesh gltfMesh = loadPrimitive(model, primitive);
                if (bgfx::isValid(gltfMesh.vbh)) {
                    meshes.push_back(gltfMesh);
                    printf("    Primitive %zu loaded successfully\n", j);
                }
                else {
                    printf("    WARNING: Primitive %zu failed to load\n", j);
                }
            }
        }

        printf("Total meshes loaded: %zu\n", meshes.size());
        return meshes;
    }

private:
    GltfMesh loadPrimitive(const tinygltf::Model& model,
        const tinygltf::Primitive& primitive) {
        GltfMesh mesh;

        // �ʒu�f�[�^���擾
        auto posIt = primitive.attributes.find("POSITION");
        if (posIt == primitive.attributes.end()) {
            printf("      ERROR: No POSITION attribute\n");
            return mesh;
        }

        const tinygltf::Accessor& posAccessor = model.accessors[posIt->second];
        std::vector<float> positions = getAccessorData<float>(model, posAccessor);
        int vertexCount = posAccessor.count;

        printf("      Vertices: %d\n", vertexCount);

        // �@���f�[�^�i�I�v�V�����j
        std::vector<float> normals;
        auto normIt = primitive.attributes.find("NORMAL");
        if (normIt != primitive.attributes.end()) {
            const tinygltf::Accessor& normAccessor = model.accessors[normIt->second];
            normals = getAccessorData<float>(model, normAccessor);
        }
        else {
            // �@�����Ȃ��ꍇ�͓K���Ȓl�Ŗ��߂�
            normals.resize(vertexCount * 3);
            for (int i = 0; i < vertexCount; i++) {
                normals[i * 3 + 0] = 0.0f;
                normals[i * 3 + 1] = 1.0f;
                normals[i * 3 + 2] = 0.0f;
            }
        }

        // ������ ������UV���W�̓ǂݍ��݂�ǉ� ������
        std::vector<float> texcoords;
        auto texIt = primitive.attributes.find("TEXCOORD_0");
        if (texIt != primitive.attributes.end()) {
            const tinygltf::Accessor& texAccessor = model.accessors[texIt->second];
            texcoords = getAccessorData<float>(model, texAccessor);
            printf("      Has texture coordinates\n");
        }
        else {
            // UV���Ȃ��ꍇ�̓_�~�[�f�[�^
            texcoords.resize(vertexCount * 2, 0.0f);
            printf("      No texture coordinates, using default\n");
        }
        printf("      Has skinning data\n");
        // �W���C���g�E�E�F�C�g�i�X�L�j���O�p�A�I�v�V�����j
        std::vector<uint16_t> joints;
        std::vector<float> weights;

        auto jointsIt = primitive.attributes.find("JOINTS_0");
        auto weightsIt = primitive.attributes.find("WEIGHTS_0");

        bool hasSkinning = (jointsIt != primitive.attributes.end() &&
            weightsIt != primitive.attributes.end());
        if (hasSkinning) {

            const tinygltf::Accessor& jointsAccessor = model.accessors[jointsIt->second];
            const tinygltf::Accessor& weightsAccessor = model.accessors[weightsIt->second];

            // �W���C���g�f�[�^�̓ǂݍ���
            if (jointsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                joints = getAccessorData<uint16_t>(model, jointsAccessor);
            }
            else {
                // UNSIGNED_BYTE�̏ꍇ
                std::vector<uint8_t> joints8 = getAccessorData<uint8_t>(model, jointsAccessor);
                joints.resize(joints8.size());
                for (size_t i = 0; i < joints8.size(); i++) {
                    joints[i] = joints8[i];
                }
            }

            weights = getAccessorData<float>(model, weightsAccessor);
        }
        else {
            // �X�L�j���O�Ȃ��̏ꍇ�̓_�~�[�f�[�^
            joints.resize(vertexCount * 4, 0);
            weights.resize(vertexCount * 4);
            for (int i = 0; i < vertexCount; i++) {
                weights[i * 4 + 0] = 1.0f;
                weights[i * 4 + 1] = 0.0f;
                weights[i * 4 + 2] = 0.0f;
                weights[i * 4 + 3] = 0.0f;
            }
        }

        std::vector<GltfMesh::Vertex> vertices(vertexCount);
        for (int i = 0; i < vertexCount; i++) {
            vertices[i].x = positions[i * 3 + 0];
            vertices[i].y = positions[i * 3 + 1];
            vertices[i].z = positions[i * 3 + 2];

            vertices[i].nx = normals[i * 3 + 0];
            vertices[i].ny = normals[i * 3 + 1];
            vertices[i].nz = normals[i * 3 + 2];

            // ������ UV���W��ݒ� ������
            vertices[i].u = texcoords[i * 2 + 0];
            vertices[i].v = texcoords[i * 2 + 1];

            vertices[i].joints[0] = joints[i * 4 + 0];
            vertices[i].joints[1] = joints[i * 4 + 1];
            vertices[i].joints[2] = joints[i * 4 + 2];
            vertices[i].joints[3] = joints[i * 4 + 3];

            vertices[i].weights[0] = weights[i * 4 + 0];
            vertices[i].weights[1] = weights[i * 4 + 1];
            vertices[i].weights[2] = weights[i * 4 + 2];
            vertices[i].weights[3] = weights[i * 4 + 3];
        }

        // ���_���C�A�E�g
        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)  // UV�ǉ�
            .add(bgfx::Attrib::Indices, 4, bgfx::AttribType::Int16)
            .add(bgfx::Attrib::Weight, 4, bgfx::AttribType::Float)
            .end();

        // ���_�o�b�t�@���쐬
        const bgfx::Memory* vbMem = bgfx::copy(vertices.data(),
            vertices.size() * sizeof(GltfMesh::Vertex));
        mesh.vbh = bgfx::createVertexBuffer(vbMem, layout);

        // �C���f�b�N�X�o�b�t�@
        if (primitive.indices >= 0) {
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            mesh.indexCount = indexAccessor.count;

            printf("      Indices: %d\n", mesh.indexCount);

            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                std::vector<uint16_t> indices = getAccessorData<uint16_t>(model, indexAccessor);
                const bgfx::Memory* ibMem = bgfx::copy(indices.data(),
                    indices.size() * sizeof(uint16_t));
                mesh.ibh = bgfx::createIndexBuffer(ibMem);
            }
            else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                std::vector<uint32_t> indices = getAccessorData<uint32_t>(model, indexAccessor);
                const bgfx::Memory* ibMem = bgfx::copy(indices.data(),
                    indices.size() * sizeof(uint32_t));
                mesh.ibh = bgfx::createIndexBuffer(ibMem, BGFX_BUFFER_INDEX32);
            }
        }
        else {
            printf("      WARNING: No indices (using vertex array)\n");
        }

        return mesh;
    }

    template<typename T>
    std::vector<T> getAccessorData(const tinygltf::Model& model,
        const tinygltf::Accessor& accessor) {
        const tinygltf::BufferView& bufferView =
            model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        const T* data = reinterpret_cast<const T*>(
            &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

        int componentCount = 1;
        switch (accessor.type) {
        case TINYGLTF_TYPE_SCALAR: componentCount = 1; break;
        case TINYGLTF_TYPE_VEC2: componentCount = 2; break;
        case TINYGLTF_TYPE_VEC3: componentCount = 3; break;
        case TINYGLTF_TYPE_VEC4: componentCount = 4; break;
        case TINYGLTF_TYPE_MAT4: componentCount = 16; break;
        }

        size_t count = accessor.count * componentCount;
        std::vector<T> result(count);

        // �X�g���C�h���l��
        if (bufferView.byteStride > 0 && bufferView.byteStride != sizeof(T) * componentCount) {
            // �C���^�[���[�u���ꂽ�f�[�^
            const uint8_t* src = reinterpret_cast<const uint8_t*>(data);
            for (size_t i = 0; i < accessor.count; i++) {
                const T* elem = reinterpret_cast<const T*>(src + i * bufferView.byteStride);
                for (int j = 0; j < componentCount; j++) {
                    result[i * componentCount + j] = elem[j];
                }
            }
        }
        else {
            // �A���f�[�^
            memcpy(result.data(), data, count * sizeof(T));
        }

        return result;
    }
};
// glTF�����_���[�N���X
class GltfRenderer {
private:
    GltfAnimator animator;
    std::vector<GltfMesh> meshes;
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_skinMatrices = BGFX_INVALID_HANDLE;

    float cameraDistance = 10.0f;
    float cameraAngleX = 0.0f;
    float cameraAngleY = 0.3f;
    std::vector<bgfx::TextureHandle> textures;
    bgfx::UniformHandle u_texColor = BGFX_INVALID_HANDLE;
    void loadTextures(const tinygltf::Model& model) {
        printf("Loading %zu textures\n", model.textures.size());

        for (const auto& gltfTexture : model.textures) {
            if (gltfTexture.source < 0) continue;

            const tinygltf::Image& image = model.images[gltfTexture.source];

            // �摜�f�[�^����bgfx�e�N�X�`�����쐬
            const bgfx::Memory* mem = bgfx::copy(
                image.image.data(),
                image.image.size()
            );

            bgfx::TextureHandle texHandle = bgfx::createTexture2D(
                image.width,
                image.height,
                false,
                1,
                bgfx::TextureFormat::RGBA8,
                BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
                mem
            );

            textures.push_back(texHandle);
            printf("  Texture %zu: %dx%d\n", textures.size() - 1,
                image.width, image.height);
        }
    }
public:
    bool loadModel(const char* filepath, HopStarIO::Location shaderLocation) {
        // glTF file load
        GltfLoader loader;
		tinygltf::Model* model = loader.loadFromFile(filepath, animator);
        if (model == NULL) {
            printf("Failed to load glTF: %s\n", filepath);
            return false;
        }

        // Start animation
        if (animator.getAnimationCount() > 0) {
            animator.setAnimation(0);
        }
        u_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

        // Load mesh data
        loadMeshData(model);
        // Compile shaders
        createShaders(shaderLocation);

        // ���j�t�H�[�����쐬
        u_skinMatrices = bgfx::createUniform("u_skinMatrices",
            bgfx::UniformType::Mat4, 64);

        return true;
    }

    void update(float deltaTime) {
        animator.update(deltaTime);
    }

    void render(int width, int height) {
        static int frameCount = 0;
        if (frameCount++ % 60 == 0) {
            printf("Rendering frame %d, meshes: %zu\n", frameCount, meshes.size());
        }
        // �J�����ʒu�v�Z
        bx::Vec3 eye = bx::Vec3(
            cameraDistance * sinf(cameraAngleX) * cosf(cameraAngleY),
            cameraDistance * sinf(cameraAngleY),
            cameraDistance * cosf(cameraAngleX) * cosf(cameraAngleY)
        );
        bx::Vec3 at = bx::Vec3(0.0f, 0.5f, 0.0f);
        bx::Vec3 up = bx::Vec3(0.0f, 1.0f, 0.0f);

        float view[16];
        bx::mtxLookAt(view, eye, at, up);

        // �v���W�F�N�V�����s��
        float proj[16];
        bx::mtxProj(proj, 45.0f, float(width) / float(height),
            0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

        // �r���[�ݒ�
        bgfx::setViewTransform(0, view, proj);
        bgfx::setViewRect(0, 0, 0, width, height);

        // ���f���s��
        float model[16];
        bx::mtxIdentity(model);

        // MVP�s��
        float mvp[16];
        float tmp[16];
        bx::mtxMul(tmp, model, view);
        bx::mtxMul(mvp, tmp, proj);

        // �X�L���}�g���b�N�X�����j�t�H�[���ɐݒ�
        if (animator.getSkinMatrixCount() > 0) {
            bgfx::setUniform(u_skinMatrices, animator.getSkinMatrices(),
                bx::min(64, animator.getSkinMatrixCount()));
        }

        // ���b�V����`��
        for (const auto& mesh : meshes) {
            if (bgfx::isValid(mesh.vbh) && bgfx::isValid(mesh.ibh)) {
                if (frameCount % 60 == 0) {
                    printf("  Drawing mesh: vbh=%d, ibh=%d, indices=%d\n",
                        mesh.vbh.idx, mesh.ibh.idx, mesh.indexCount);
                    if (animator.getSkinMatrixCount() > 0) {
                        const float* matrices = animator.getSkinMatrices();
                        printf("First skin matrix:\n");
                        printf("  [%.2f %.2f %.2f %.2f]\n", matrices[0], matrices[1], matrices[2], matrices[3]);
                        printf("  [%.2f %.2f %.2f %.2f]\n", matrices[4], matrices[5], matrices[6], matrices[7]);
                        printf("  [%.2f %.2f %.2f %.2f]\n", matrices[8], matrices[9], matrices[10], matrices[11]);
                        printf("  [%.2f %.2f %.2f %.2f]\n", matrices[12], matrices[13], matrices[14], matrices[15]);
                    }
                }
                bgfx::setVertexBuffer(0, mesh.vbh);
                bgfx::setIndexBuffer(mesh.ibh);
                bgfx::setTransform(model);
                if (!textures.empty() && bgfx::isValid(textures[0])) {
                    bgfx::setTexture(0, u_texColor, textures[0]);
                }

                // ��Ԑݒ�
                uint64_t state = BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    | BGFX_STATE_WRITE_Z
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_CULL_CW
                    | BGFX_STATE_MSAA;

                bgfx::setState(state);
                bgfx::submit(0, program);
            }
        }
    }

    void rotateCamera(float dx, float dy) {
        cameraAngleX += dx;
        cameraAngleY = bx::clamp(cameraAngleY + dy, -1.5f, 1.5f);
    }

    void zoomCamera(float delta) {
        cameraDistance = bx::clamp(cameraDistance + delta, 1.0f, 20.0f);
    }

    void cleanup() {
        for (auto& mesh : meshes) {
            if (bgfx::isValid(mesh.vbh)) bgfx::destroy(mesh.vbh);
            if (bgfx::isValid(mesh.ibh)) bgfx::destroy(mesh.ibh);
        }
        if (bgfx::isValid(program)) bgfx::destroy(program);
        if (bgfx::isValid(u_skinMatrices)) bgfx::destroy(u_skinMatrices);
    }

private:
    // Get platform suffix for shader files
    // Windows: no suffix (DirectX), macOS/iOS: _mtl (Metal), Android/Linux: _spv (Vulkan/SPIRV)
    static const char* getShaderPlatformSuffix() {
#if defined(_WIN32)
        return "";  // Windows DirectX shaders have no suffix
#elif defined(__ANDROID__)
        return "_spv";  // Android uses Vulkan/SPIRV
#elif TARGET_OS_IOS || TARGET_OS_SIMULATOR
        return "_mtl";  // iOS uses Metal
#elif TARGET_OS_MAC
        return "_mtl";  // macOS uses Metal
#elif defined(__linux__)
        return "_spv";  // Linux uses Vulkan/SPIRV
#else
        return "";
#endif
    }

    // Load shader with automatic platform suffix
    // e.g., loadShader("shaders/vs_mesh", location) loads "shaders/vs_mesh_w.bin" on Windows
    bgfx::ShaderHandle loadShader(const char* baseName, HopStarIO::Location location) {
        // Build platform-specific filename: baseName + platform_suffix + .bin
        std::string filename = std::string(baseName) + getShaderPlatformSuffix() + ".bin";

        // Use PlatformIO/FileEngine for unified cross-platform file access
        auto data = PlatformIO::readFile(filename, location);
        if (data.empty()) {
            printf("ERROR: Cannot open shader file: %s\n", filename.c_str());
            return BGFX_INVALID_HANDLE;
        }

        printf("Loading shader: %s (%zu bytes)\n", filename.c_str(), data.size());

        const bgfx::Memory* mem = bgfx::copy(data.data(), static_cast<uint32_t>(data.size()));
        bgfx::ShaderHandle handle = bgfx::createShader(mem);
        printf("Shader handle: %d, valid: %s\n",
            handle.idx, bgfx::isValid(handle) ? "YES" : "NO");
        return handle;
    }

    void createShaders(HopStarIO::Location location) {
        printf("=== Loading pre-compiled shaders ===\n");

        // Use base names - platform suffix and .bin extension are added automatically
        bgfx::ShaderHandle vsh = loadShader("shaders/vs_mesh", location);
        bgfx::ShaderHandle fsh = loadShader("shaders/fs_mesh", location);

        if (bgfx::isValid(vsh) && bgfx::isValid(fsh)) {
            program = bgfx::createProgram(vsh, fsh, true);
            printf("Program created: %d, valid: %s\n",
                program.idx, bgfx::isValid(program) ? "YES" : "NO");
        }
        else {
            printf("ERROR: Shader loading failed\n");
        }
    }

    void loadMeshData(tinygltf::Model* model) {

        // �O�ɒ񋟂���GltfMeshLoader���g�p
        GltfMeshLoader meshLoader;
        meshes = meshLoader.loadMeshes(*model);

        printf("Loaded %zu mesh primitives\n", meshes.size());
        loadTextures(*model);
    }
};