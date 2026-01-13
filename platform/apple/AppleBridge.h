// =============================================================================
// AppleBridge.h - C++ Interface to Apple Platform Features
// =============================================================================
// This header provides a C++ interface to access macOS/iOS specific features
// implemented in Objective-C++ and Swift.
// =============================================================================

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace AppleBridge {

// =============================================================================
// Device Information
// =============================================================================
struct DeviceInfo {
    std::string modelName;      // e.g., "MacBook Pro", "iPhone 15"
    std::string osVersion;      // e.g., "14.0"
    std::string osName;         // e.g., "macOS", "iOS"
    float screenScale;          // Retina scale factor
    int screenWidth;
    int screenHeight;
    bool isSimulator;
};

DeviceInfo getDeviceInfo();

// =============================================================================
// Feature Availability
// =============================================================================
struct AvailableFeatures {
    bool hasMetal;
    bool hasCoreML;
    bool hasARKit;          // iOS only
    bool hasPhotoKit;
    bool hasCloudKit;
    bool hasSiriKit;
    bool hasShareExtension;
    bool hasWidgetKit;      // iOS 14+ / macOS 11+
    bool hasLiveActivity;   // iOS 16.1+
};

AvailableFeatures checkAvailableFeatures();

// =============================================================================
// Initialization / Shutdown
// =============================================================================
void initialize();
void shutdown();

// =============================================================================
// File System Access
// =============================================================================
namespace FileSystem {
    // Get standard directories
    std::string getDocumentsDirectory();
    std::string getCachesDirectory();
    std::string getTempDirectory();
    std::string getAppSupportDirectory();

    // iCloud
    bool isICloudAvailable();
    std::string getICloudContainerPath(const std::string& containerID);

    // File picker (async, callback-based)
    using FilePickerCallback = std::function<void(const std::vector<std::string>& paths)>;
    void showFilePicker(const std::vector<std::string>& allowedTypes, bool allowMultiple, FilePickerCallback callback);

    // Save panel (macOS) / Share sheet (iOS)
    using SaveCallback = std::function<void(bool success, const std::string& path)>;
    void showSavePanel(const std::string& defaultName, const std::string& fileType, SaveCallback callback);
}

// =============================================================================
// Photo Library Access (PhotoKit)
// =============================================================================
namespace PhotoKit {
    enum class AuthorizationStatus {
        NotDetermined,
        Restricted,
        Denied,
        Authorized,
        Limited     // iOS 14+
    };

    AuthorizationStatus getAuthorizationStatus();
    void requestAuthorization(std::function<void(AuthorizationStatus)> callback);

    struct PhotoAsset {
        std::string localIdentifier;
        std::string filename;
        int width;
        int height;
        double creationDate;    // Unix timestamp
        bool isVideo;
        double duration;        // For video, in seconds
    };

    void fetchAssets(int limit, std::function<void(const std::vector<PhotoAsset>&)> callback);

    // Load image data
    void loadImageData(const std::string& localIdentifier,
                       int targetWidth,
                       int targetHeight,
                       std::function<void(const uint8_t* data, size_t size, int width, int height)> callback);

    // Load video URL
    void loadVideoURL(const std::string& localIdentifier,
                      std::function<void(const std::string& fileURL)> callback);
}

// =============================================================================
// Core ML Integration
// =============================================================================
namespace CoreML {
    struct ModelInfo {
        std::string name;
        std::string author;
        std::string description;
        bool isLoaded;
    };

    // Load a Core ML model from bundle or path
    bool loadModel(const std::string& modelPath, const std::string& identifier);
    void unloadModel(const std::string& identifier);
    ModelInfo getModelInfo(const std::string& identifier);

    // Run inference (returns raw output data)
    struct InferenceResult {
        bool success;
        std::vector<float> outputs;
        std::string errorMessage;
    };

    InferenceResult runInference(const std::string& identifier,
                                 const float* inputData,
                                 size_t inputSize,
                                 const std::vector<int>& inputShape);

    // Face detection using Vision framework
    struct FaceDetection {
        float x, y, width, height;  // Normalized coordinates [0-1]
        float confidence;
        float roll, yaw, pitch;     // Face angles
    };

    void detectFaces(const uint8_t* imageData,
                     int width,
                     int height,
                     int bytesPerPixel,
                     std::function<void(const std::vector<FaceDetection>&)> callback);
}

// =============================================================================
// AVFoundation Integration
// =============================================================================
namespace AVFoundation {
    // Hardware-accelerated video decoding
    struct VideoInfo {
        int width;
        int height;
        double duration;
        double frameRate;
        std::string codec;
        bool hasAudio;
    };

    bool openVideo(const std::string& path, const std::string& identifier);
    void closeVideo(const std::string& identifier);
    VideoInfo getVideoInfo(const std::string& identifier);

    // Decode frame at timestamp
    struct DecodedFrame {
        bool success;
        uint8_t* data;      // RGBA data, caller must free
        int width;
        int height;
        double timestamp;
    };

    DecodedFrame decodeFrame(const std::string& identifier, double timestamp);
    void freeFrameData(DecodedFrame& frame);

    // Hardware encoder
    bool startEncoding(const std::string& outputPath,
                       int width, int height,
                       double frameRate,
                       int bitrate);
    bool encodeFrame(const uint8_t* rgbaData, int width, int height, double timestamp);
    bool finishEncoding();
}

// =============================================================================
// Audio (Core Audio / AVAudioEngine)
// =============================================================================
namespace Audio {
    // Audio session (iOS)
    void configureAudioSession();

    // Get available devices
    struct AudioDevice {
        std::string uid;
        std::string name;
        bool isInput;
        bool isOutput;
    };

    std::vector<AudioDevice> getAvailableDevices();

    // Simple playback
    bool playSound(const std::string& path);
    void stopAllSounds();
}

// =============================================================================
// Share / Activity
// =============================================================================
namespace Share {
    enum class ItemType {
        Text,
        URL,
        Image,
        Video,
        File
    };

    struct ShareItem {
        ItemType type;
        std::string data;       // Text/URL string or file path
    };

    void showShareSheet(const std::vector<ShareItem>& items,
                        std::function<void(bool success, const std::string& activityType)> callback);
}

// =============================================================================
// Haptic Feedback (iOS)
// =============================================================================
namespace Haptics {
    enum class FeedbackStyle {
        Light,
        Medium,
        Heavy,
        Selection,
        Success,
        Warning,
        Error
    };

    void trigger(FeedbackStyle style);
}

// =============================================================================
// Notifications
// =============================================================================
namespace Notifications {
    void requestPermission(std::function<void(bool granted)> callback);

    void scheduleLocalNotification(const std::string& title,
                                   const std::string& body,
                                   double delaySeconds,
                                   const std::string& identifier);

    void cancelNotification(const std::string& identifier);
    void cancelAllNotifications();
}

// =============================================================================
// App Lifecycle
// =============================================================================
namespace Lifecycle {
    using LifecycleCallback = std::function<void()>;

    void setOnEnterBackground(LifecycleCallback callback);
    void setOnEnterForeground(LifecycleCallback callback);
    void setOnTerminate(LifecycleCallback callback);
    void setOnMemoryWarning(LifecycleCallback callback);
}

// =============================================================================
// Metal Integration (for custom rendering)
// =============================================================================
namespace Metal {
    // Get Metal device pointer (id<MTLDevice>)
    void* getDevice();

    // Get Metal command queue (id<MTLCommandQueue>)
    void* getCommandQueue();

    // Create texture from image data
    void* createTexture(const uint8_t* data, int width, int height, int bytesPerPixel);
    void releaseTexture(void* texture);
}

// =============================================================================
// ARKit (iOS only)
// =============================================================================
#if TARGET_OS_IOS
namespace ARKit {
    bool isSupported();

    enum class TrackingState {
        NotAvailable,
        Limited,
        Normal
    };

    struct ARFrame {
        void* capturedImage;    // CVPixelBufferRef
        float viewMatrix[16];
        float projectionMatrix[16];
        TrackingState trackingState;
    };

    bool startSession();
    void stopSession();
    ARFrame getCurrentFrame();
}
#endif

} // namespace AppleBridge
