// portaudio_ios.mm - iOS implementation of PortAudio API using AudioUnit
// Provides PortAudio-compatible callback-based audio output

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#include "stubs/portaudio.h"
#include <cstring>

#if TARGET_OS_IOS || TARGET_OS_SIMULATOR

// Internal stream structure
typedef struct PaStreamInternal {
    AudioComponentInstance audioUnit;
    PaStreamCallback callback;
    void* userData;
    double sampleRate;
    unsigned long framesPerBuffer;
    int numOutputChannels;
    bool isRunning;
    bool isInitialized;
    PaStreamInfo streamInfo;
} PaStreamInternal;

// Global state
static bool g_paInitialized = false;
static PaDeviceInfo g_defaultDeviceInfo;
static PaHostApiInfo g_hostApiInfo;

// Audio render callback
static OSStatus audioRenderCallback(
    void* inRefCon,
    AudioUnitRenderActionFlags* ioActionFlags,
    const AudioTimeStamp* inTimeStamp,
    UInt32 inBusNumber,
    UInt32 inNumberFrames,
    AudioBufferList* ioData)
{
    // Safety checks
    if (!ioData || ioData->mNumberBuffers == 0) {
        return noErr;
    }

    PaStreamInternal* stream = (PaStreamInternal*)inRefCon;

    // Fill with silence if stream is not ready
    if (!stream || !stream->callback || !stream->isRunning || !stream->isInitialized || !stream->userData) {
        for (UInt32 i = 0; i < ioData->mNumberBuffers; i++) {
            if (ioData->mBuffers[i].mData && ioData->mBuffers[i].mDataByteSize > 0) {
                memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize);
            }
        }
        return noErr;
    }

    // Check output buffer
    if (!ioData->mBuffers[0].mData || ioData->mBuffers[0].mDataByteSize == 0) {
        return noErr;
    }

    // Prepare callback time info
    PaStreamCallbackTimeInfo timeInfo;
    memset(&timeInfo, 0, sizeof(timeInfo));
    timeInfo.currentTime = 0;  // Simplified - avoid CACurrentMediaTime in callback
    timeInfo.outputBufferDacTime = 0;

    // Call user callback with exception protection
    float* outputBuffer = (float*)ioData->mBuffers[0].mData;
    int result = paContinue;

    @try {
        result = stream->callback(
            NULL,  // No input
            outputBuffer,
            inNumberFrames,
            &timeInfo,
            0,  // No status flags
            stream->userData
        );
    } @catch (NSException *exception) {
        NSLog(@"PortAudio iOS: Exception in audio callback: %@", exception);
        // Fill with silence on error
        memset(outputBuffer, 0, inNumberFrames * stream->numOutputChannels * sizeof(float));
        result = paContinue;
    }

    // Handle callback return value
    if (result == paComplete || result == paAbort) {
        stream->isRunning = false;
    }

    return noErr;
}

// Public API implementation
PaError Pa_Initialize(void) {
    if (g_paInitialized) {
        return paNoError;
    }

    NSLog(@"PortAudio iOS: Initializing...");

    // Configure audio session
    NSError* error = nil;
    AVAudioSession* session = [AVAudioSession sharedInstance];

    [session setCategory:AVAudioSessionCategoryPlayback error:&error];
    if (error) {
        NSLog(@"PortAudio iOS: Failed to set audio session category: %@", error);
        return paNotInitialized;
    }

    [session setActive:YES error:&error];
    if (error) {
        NSLog(@"PortAudio iOS: Failed to activate audio session: %@", error);
        return paNotInitialized;
    }

    // Setup default device info
    memset(&g_defaultDeviceInfo, 0, sizeof(g_defaultDeviceInfo));
    g_defaultDeviceInfo.structVersion = 2;
    g_defaultDeviceInfo.name = "iOS Audio Output";
    g_defaultDeviceInfo.hostApi = 0;
    g_defaultDeviceInfo.maxInputChannels = 0;
    g_defaultDeviceInfo.maxOutputChannels = 2;
    g_defaultDeviceInfo.defaultLowInputLatency = 0.0;
    g_defaultDeviceInfo.defaultLowOutputLatency = 0.005;
    g_defaultDeviceInfo.defaultHighInputLatency = 0.0;
    g_defaultDeviceInfo.defaultHighOutputLatency = 0.010;
    g_defaultDeviceInfo.defaultSampleRate = [session sampleRate];

    // Setup host API info
    memset(&g_hostApiInfo, 0, sizeof(g_hostApiInfo));
    g_hostApiInfo.structVersion = 1;
    g_hostApiInfo.type = 0;  // Custom type
    g_hostApiInfo.name = "iOS AudioUnit";
    g_hostApiInfo.deviceCount = 1;
    g_hostApiInfo.defaultInputDevice = -1;
    g_hostApiInfo.defaultOutputDevice = 0;

    g_paInitialized = true;
    NSLog(@"PortAudio iOS: Initialized successfully, sample rate: %.0f", g_defaultDeviceInfo.defaultSampleRate);

    return paNoError;
}

PaError Pa_Terminate(void) {
    if (!g_paInitialized) {
        return paNoError;
    }

    NSLog(@"PortAudio iOS: Terminating...");
    g_paInitialized = false;

    return paNoError;
}

const char* Pa_GetErrorText(PaError errorCode) {
    switch (errorCode) {
        case paNoError: return "Success";
        case paNotInitialized: return "PortAudio not initialized";
        case paInvalidDevice: return "Invalid device";
        default: return "Unknown error";
    }
}

PaDeviceIndex Pa_GetDeviceCount(void) {
    return g_paInitialized ? 1 : 0;
}

PaDeviceIndex Pa_GetDefaultInputDevice(void) {
    return -1;  // No input device
}

PaDeviceIndex Pa_GetDefaultOutputDevice(void) {
    return g_paInitialized ? 0 : -1;
}

const PaDeviceInfo* Pa_GetDeviceInfo(PaDeviceIndex device) {
    if (!g_paInitialized || device != 0) {
        return NULL;
    }
    return &g_defaultDeviceInfo;
}

const PaHostApiInfo* Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
    if (!g_paInitialized || hostApi != 0) {
        return NULL;
    }
    return &g_hostApiInfo;
}

const PaStreamInfo* Pa_GetStreamInfo(PaStream* stream) {
    if (!stream) {
        return NULL;
    }
    PaStreamInternal* internal = (PaStreamInternal*)stream;
    return &internal->streamInfo;
}

PaError Pa_OpenStream(
    PaStream** stream,
    const PaStreamParameters* inputParameters,
    const PaStreamParameters* outputParameters,
    double sampleRate,
    unsigned long framesPerBuffer,
    PaStreamFlags streamFlags,
    PaStreamCallback streamCallback,
    void* userData)
{
    if (!g_paInitialized) {
        return paNotInitialized;
    }

    if (!stream || !outputParameters || !streamCallback) {
        return paInvalidDevice;
    }

    NSLog(@"PortAudio iOS: Opening stream, sample rate: %.0f, frames: %lu, channels: %d",
          sampleRate, framesPerBuffer, outputParameters->channelCount);

    // Allocate stream structure
    PaStreamInternal* internal = (PaStreamInternal*)calloc(1, sizeof(PaStreamInternal));
    if (!internal) {
        return paNotInitialized;
    }

    internal->callback = streamCallback;
    internal->userData = userData;
    internal->sampleRate = sampleRate;
    internal->framesPerBuffer = framesPerBuffer;
    internal->numOutputChannels = outputParameters->channelCount;
    internal->isRunning = false;
    internal->isInitialized = false;

    // Setup stream info
    internal->streamInfo.structVersion = 1;
    internal->streamInfo.inputLatency = 0.0;
    internal->streamInfo.outputLatency = framesPerBuffer / sampleRate;
    internal->streamInfo.sampleRate = sampleRate;

    // Create Audio Unit
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    AudioComponent component = AudioComponentFindNext(NULL, &desc);
    if (!component) {
        NSLog(@"PortAudio iOS: Failed to find audio component");
        free(internal);
        return paNotInitialized;
    }

    OSStatus status = AudioComponentInstanceNew(component, &internal->audioUnit);
    if (status != noErr) {
        NSLog(@"PortAudio iOS: Failed to create audio unit instance: %d", (int)status);
        free(internal);
        return paNotInitialized;
    }

    // Set audio format
    AudioStreamBasicDescription format;
    memset(&format, 0, sizeof(format));
    format.mSampleRate = sampleRate;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    format.mBytesPerPacket = outputParameters->channelCount * sizeof(float);
    format.mFramesPerPacket = 1;
    format.mBytesPerFrame = outputParameters->channelCount * sizeof(float);
    format.mChannelsPerFrame = outputParameters->channelCount;
    format.mBitsPerChannel = 32;

    status = AudioUnitSetProperty(
        internal->audioUnit,
        kAudioUnitProperty_StreamFormat,
        kAudioUnitScope_Input,
        0,  // Output bus
        &format,
        sizeof(format)
    );

    if (status != noErr) {
        NSLog(@"PortAudio iOS: Failed to set stream format: %d", (int)status);
        AudioComponentInstanceDispose(internal->audioUnit);
        free(internal);
        return paNotInitialized;
    }

    // Set render callback
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = audioRenderCallback;
    callbackStruct.inputProcRefCon = internal;

    status = AudioUnitSetProperty(
        internal->audioUnit,
        kAudioUnitProperty_SetRenderCallback,
        kAudioUnitScope_Input,
        0,  // Output bus
        &callbackStruct,
        sizeof(callbackStruct)
    );

    if (status != noErr) {
        NSLog(@"PortAudio iOS: Failed to set render callback: %d", (int)status);
        AudioComponentInstanceDispose(internal->audioUnit);
        free(internal);
        return paNotInitialized;
    }

    // Initialize audio unit
    status = AudioUnitInitialize(internal->audioUnit);
    if (status != noErr) {
        NSLog(@"PortAudio iOS: Failed to initialize audio unit: %d", (int)status);
        AudioComponentInstanceDispose(internal->audioUnit);
        free(internal);
        return paNotInitialized;
    }

    internal->isInitialized = true;
    *stream = (PaStream*)internal;

    NSLog(@"PortAudio iOS: Stream opened successfully");

    return paNoError;
}

PaError Pa_OpenDefaultStream(
    PaStream** stream,
    int numInputChannels,
    int numOutputChannels,
    PaSampleFormat sampleFormat,
    double sampleRate,
    unsigned long framesPerBuffer,
    PaStreamCallback streamCallback,
    void* userData)
{
    if (numInputChannels != 0) {
        return paInvalidDevice;  // Input not supported
    }

    if (sampleFormat != paFloat32) {
        return paInvalidDevice;  // Only float32 supported
    }

    PaStreamParameters outputParams;
    outputParams.device = 0;
    outputParams.channelCount = numOutputChannels;
    outputParams.sampleFormat = sampleFormat;
    outputParams.suggestedLatency = 0.01;
    outputParams.hostApiSpecificStreamInfo = NULL;

    return Pa_OpenStream(
        stream,
        NULL,  // No input
        &outputParams,
        sampleRate,
        framesPerBuffer,
        0,  // No flags
        streamCallback,
        userData
    );
}

PaError Pa_StartStream(PaStream* stream) {
    if (!stream) {
        return paInvalidDevice;
    }

    PaStreamInternal* internal = (PaStreamInternal*)stream;

    if (!internal->isInitialized) {
        return paNotInitialized;
    }

    NSLog(@"PortAudio iOS: Starting stream...");

    OSStatus status = AudioOutputUnitStart(internal->audioUnit);
    if (status != noErr) {
        NSLog(@"PortAudio iOS: Failed to start audio unit: %d", (int)status);
        return paNotInitialized;
    }

    internal->isRunning = true;
    NSLog(@"PortAudio iOS: Stream started");

    return paNoError;
}

PaError Pa_StopStream(PaStream* stream) {
    if (!stream) {
        return paInvalidDevice;
    }

    PaStreamInternal* internal = (PaStreamInternal*)stream;

    NSLog(@"PortAudio iOS: Stopping stream...");

    internal->isRunning = false;

    if (internal->isInitialized) {
        AudioOutputUnitStop(internal->audioUnit);
    }

    NSLog(@"PortAudio iOS: Stream stopped");

    return paNoError;
}

PaError Pa_AbortStream(PaStream* stream) {
    return Pa_StopStream(stream);
}

PaError Pa_CloseStream(PaStream* stream) {
    if (!stream) {
        return paNoError;
    }

    PaStreamInternal* internal = (PaStreamInternal*)stream;

    NSLog(@"PortAudio iOS: Closing stream...");

    if (internal->isRunning) {
        Pa_StopStream(stream);
    }

    if (internal->isInitialized) {
        AudioUnitUninitialize(internal->audioUnit);
        AudioComponentInstanceDispose(internal->audioUnit);
    }

    free(internal);

    NSLog(@"PortAudio iOS: Stream closed");

    return paNoError;
}

PaError Pa_IsStreamStopped(PaStream* stream) {
    if (!stream) {
        return 1;
    }

    PaStreamInternal* internal = (PaStreamInternal*)stream;
    return internal->isRunning ? 0 : 1;
}

PaError Pa_IsStreamActive(PaStream* stream) {
    if (!stream) {
        return 0;
    }

    PaStreamInternal* internal = (PaStreamInternal*)stream;
    return internal->isRunning ? 1 : 0;
}

#endif // TARGET_OS_IOS || TARGET_OS_SIMULATOR
