// ugui_ios.h - iOS-specific wrapper for ugui.h
// Skips unused dependencies on iOS

#pragma once

// iOS doesn't need portaudio (using miniaudio/AVFoundation instead)
#define UGUI_SKIP_PORTAUDIO 1

// iOS doesn't need SDL_ttf (using CoreText for fonts)
#define UGUI_SKIP_SDL_TTF 1

// Provide stub portaudio.h for iOS
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR

// Stub portaudio types
typedef void PaStream;
typedef int PaError;
typedef unsigned long PaDeviceIndex;

#endif
#endif

// Now include the real ugui.h
// Note: ugui.h needs to be modified to respect UGUI_SKIP_PORTAUDIO and UGUI_SKIP_SDL_TTF
// For now, we'll include the dependencies directly

#include "shader.h"
#include <fstream>
#include <chrono>
#include <future>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <bgfx/bgfx.h>
#include "thumnailAtlas.h"
#include "imas.h"

// Include the rest of ugui.h content (skipping the includes at the top)
