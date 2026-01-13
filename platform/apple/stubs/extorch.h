// extorch.h - iOS stub header
// PyTorch is not available on iOS, so we provide stub declarations

#pragma once

// Stub class for TensorWrapper
class TensorWrapper {
public:
    TensorWrapper() {}
};

// Stub functions - return nullptr/0 on iOS
inline void* createConv2d(int64_t, int64_t, int64_t) { return nullptr; }
inline int64_t getInChannels(void*) { return 0; }
inline int64_t getOutChannels(void*) { return 0; }
inline int64_t getKernelSize(void*) { return 0; }
inline float* getWeightData(void*) { return nullptr; }
inline int64_t getWeightSize(void*) { return 0; }
inline void* forwardConv2d(void*, void*) { return nullptr; }
inline void* createMaxPool2d(int64_t, int64_t, int64_t) { return nullptr; }
inline void* forwardMaxPool2d(void*, void*) { return nullptr; }
inline void* createLinear(int64_t, int64_t) { return nullptr; }
inline void* forwardLinear(void*, void*) { return nullptr; }
inline void* createReLU() { return nullptr; }
inline void* forwardReLU(void*, void*) { return nullptr; }
inline void* createFlatten() { return nullptr; }
inline void* forwardFlatten(void*, void*) { return nullptr; }
inline void* createTensor(float*, int64_t*, int) { return nullptr; }
inline void deleteTensor(void*) {}
inline void deleteModule(void*) {}
