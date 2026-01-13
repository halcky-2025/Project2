// portaudio.h - iOS implementation using AudioUnit
// Provides PortAudio-compatible API using iOS AudioToolbox

#pragma once

#include <TargetConditionals.h>

#ifdef __cplusplus
extern "C" {
#endif

// Basic types
typedef int PaError;
typedef int PaDeviceIndex;
typedef int PaHostApiIndex;
typedef double PaTime;
typedef unsigned long PaSampleFormat;
typedef unsigned long PaStreamFlags;
typedef void PaStream;

// Error codes
#define paNoError 0
#define paNotInitialized -10000
#define paInvalidDevice -9996

// Callback return values
#define paContinue 0
#define paComplete 1
#define paAbort 2

// Stream callback types
typedef unsigned long PaStreamCallbackFlags;
typedef struct PaStreamCallbackTimeInfo {
    PaTime inputBufferAdcTime;
    PaTime currentTime;
    PaTime outputBufferDacTime;
} PaStreamCallbackTimeInfo;

typedef int (*PaStreamCallback)(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData);

// Sample formats
#define paFloat32 0x00000001
#define paInt32   0x00000002
#define paInt24   0x00000004
#define paInt16   0x00000008
#define paInt8    0x00000010
#define paUInt8   0x00000020

// Structures
typedef struct PaStreamParameters {
    PaDeviceIndex device;
    int channelCount;
    PaSampleFormat sampleFormat;
    PaTime suggestedLatency;
    void *hostApiSpecificStreamInfo;
} PaStreamParameters;

typedef struct PaDeviceInfo {
    int structVersion;
    const char *name;
    PaHostApiIndex hostApi;
    int maxInputChannels;
    int maxOutputChannels;
    PaTime defaultLowInputLatency;
    PaTime defaultLowOutputLatency;
    PaTime defaultHighInputLatency;
    PaTime defaultHighOutputLatency;
    double defaultSampleRate;
} PaDeviceInfo;

typedef struct PaHostApiInfo {
    int structVersion;
    int type;
    const char *name;
    int deviceCount;
    PaDeviceIndex defaultInputDevice;
    PaDeviceIndex defaultOutputDevice;
} PaHostApiInfo;

typedef struct PaStreamInfo {
    int structVersion;
    PaTime inputLatency;
    PaTime outputLatency;
    double sampleRate;
} PaStreamInfo;

#if TARGET_OS_IOS || TARGET_OS_SIMULATOR

// iOS: Real implementation declarations (implemented in portaudio_ios.mm)
PaError Pa_Initialize(void);
PaError Pa_Terminate(void);
const char* Pa_GetErrorText(PaError errorCode);
PaDeviceIndex Pa_GetDeviceCount(void);
PaDeviceIndex Pa_GetDefaultInputDevice(void);
PaDeviceIndex Pa_GetDefaultOutputDevice(void);
const PaDeviceInfo* Pa_GetDeviceInfo(PaDeviceIndex device);
const PaHostApiInfo* Pa_GetHostApiInfo(PaHostApiIndex hostApi);
const PaStreamInfo* Pa_GetStreamInfo(PaStream *stream);

PaError Pa_OpenStream(
    PaStream** stream,
    const PaStreamParameters *inputParameters,
    const PaStreamParameters *outputParameters,
    double sampleRate,
    unsigned long framesPerBuffer,
    PaStreamFlags streamFlags,
    PaStreamCallback streamCallback,
    void *userData);

PaError Pa_OpenDefaultStream(
    PaStream** stream,
    int numInputChannels,
    int numOutputChannels,
    PaSampleFormat sampleFormat,
    double sampleRate,
    unsigned long framesPerBuffer,
    PaStreamCallback streamCallback,
    void *userData);

PaError Pa_StartStream(PaStream *stream);
PaError Pa_StopStream(PaStream *stream);
PaError Pa_AbortStream(PaStream *stream);
PaError Pa_CloseStream(PaStream *stream);
PaError Pa_IsStreamStopped(PaStream *stream);
PaError Pa_IsStreamActive(PaStream *stream);

#else

// Non-iOS: Stub implementations
static inline PaError Pa_Initialize(void) { return paNotInitialized; }
static inline PaError Pa_Terminate(void) { return paNoError; }
static inline const char* Pa_GetErrorText(PaError errorCode) { return "PortAudio not available"; }
static inline PaDeviceIndex Pa_GetDeviceCount(void) { return 0; }
static inline PaDeviceIndex Pa_GetDefaultInputDevice(void) { return -1; }
static inline PaDeviceIndex Pa_GetDefaultOutputDevice(void) { return -1; }
static inline const PaDeviceInfo* Pa_GetDeviceInfo(PaDeviceIndex device) { return NULL; }
static inline const PaHostApiInfo* Pa_GetHostApiInfo(PaHostApiIndex hostApi) { return NULL; }
static inline const PaStreamInfo* Pa_GetStreamInfo(PaStream *stream) { return NULL; }

static inline PaError Pa_OpenStream(
    PaStream** stream,
    const PaStreamParameters *inputParameters,
    const PaStreamParameters *outputParameters,
    double sampleRate,
    unsigned long framesPerBuffer,
    PaStreamFlags streamFlags,
    PaStreamCallback streamCallback,
    void *userData) { return paNotInitialized; }

static inline PaError Pa_OpenDefaultStream(
    PaStream** stream,
    int numInputChannels,
    int numOutputChannels,
    PaSampleFormat sampleFormat,
    double sampleRate,
    unsigned long framesPerBuffer,
    PaStreamCallback streamCallback,
    void *userData) { return paNotInitialized; }

static inline PaError Pa_StartStream(PaStream *stream) { return paNotInitialized; }
static inline PaError Pa_StopStream(PaStream *stream) { return paNotInitialized; }
static inline PaError Pa_AbortStream(PaStream *stream) { return paNotInitialized; }
static inline PaError Pa_CloseStream(PaStream *stream) { return paNoError; }
static inline PaError Pa_IsStreamStopped(PaStream *stream) { return 1; }
static inline PaError Pa_IsStreamActive(PaStream *stream) { return 0; }

#endif

#ifdef __cplusplus
}
#endif
