// =============================================================================
// AppleBridge.mm - Objective-C++ Implementation
// =============================================================================
// Implements the C++ interface defined in AppleBridge.h using Apple frameworks.
// =============================================================================

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <CoreML/CoreML.h>
#import <Photos/Photos.h>
#import <AVFoundation/AVFoundation.h>
#import <Vision/Vision.h>
#import <UserNotifications/UserNotifications.h>

#if TARGET_OS_IOS
    #import <UIKit/UIKit.h>
    #import <ARKit/ARKit.h>
#else
    #import <AppKit/AppKit.h>
#endif

#include "AppleBridge.h"
#include <unordered_map>
#include <mutex>
#include <sys/sysctl.h>

// =============================================================================
// Internal State
// =============================================================================
namespace {
    id<MTLDevice> g_metalDevice = nil;
    id<MTLCommandQueue> g_metalCommandQueue = nil;

    std::unordered_map<std::string, MLModel*> g_coreMLModels;
    std::unordered_map<std::string, AVAsset*> g_avAssets;
    std::unordered_map<std::string, AVAssetReader*> g_avReaders;

    std::mutex g_modelMutex;
    std::mutex g_assetMutex;

    // Lifecycle callbacks
    AppleBridge::Lifecycle::LifecycleCallback g_onEnterBackground;
    AppleBridge::Lifecycle::LifecycleCallback g_onEnterForeground;
    AppleBridge::Lifecycle::LifecycleCallback g_onTerminate;
    AppleBridge::Lifecycle::LifecycleCallback g_onMemoryWarning;
}

// =============================================================================
// Initialization / Shutdown
// =============================================================================
namespace AppleBridge {

void initialize() {
    @autoreleasepool {
        // Initialize Metal
        g_metalDevice = MTLCreateSystemDefaultDevice();
        if (g_metalDevice) {
            g_metalCommandQueue = [g_metalDevice newCommandQueue];
            NSLog(@"Metal initialized: %@", g_metalDevice.name);
        } else {
            NSLog(@"Metal not available");
        }

#if TARGET_OS_IOS
        // Set up iOS lifecycle notifications
        [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidEnterBackgroundNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification* note) {
            if (g_onEnterBackground) g_onEnterBackground();
        }];

        [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillEnterForegroundNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification* note) {
            if (g_onEnterForeground) g_onEnterForeground();
        }];

        [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidReceiveMemoryWarningNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification* note) {
            if (g_onMemoryWarning) g_onMemoryWarning();
        }];
#else
        // Set up macOS lifecycle notifications
        [[NSNotificationCenter defaultCenter] addObserverForName:NSApplicationWillTerminateNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification* note) {
            if (g_onTerminate) g_onTerminate();
        }];
#endif
    }
}

void shutdown() {
    @autoreleasepool {
        // Clean up Core ML models
        {
            std::lock_guard<std::mutex> lock(g_modelMutex);
            g_coreMLModels.clear();
        }

        // Clean up AV assets
        {
            std::lock_guard<std::mutex> lock(g_assetMutex);
            g_avAssets.clear();
            g_avReaders.clear();
        }

        g_metalCommandQueue = nil;
        g_metalDevice = nil;

        NSLog(@"AppleBridge shutdown complete");
    }
}

// =============================================================================
// Device Information
// =============================================================================
DeviceInfo getDeviceInfo() {
    DeviceInfo info;

    @autoreleasepool {
#if TARGET_OS_IOS
        UIDevice* device = [UIDevice currentDevice];
        info.modelName = [device.model UTF8String];
        info.osName = "iOS";
        info.osVersion = [device.systemVersion UTF8String];
        info.screenScale = [[UIScreen mainScreen] scale];

        CGRect screenBounds = [[UIScreen mainScreen] bounds];
        info.screenWidth = (int)(screenBounds.size.width * info.screenScale);
        info.screenHeight = (int)(screenBounds.size.height * info.screenScale);

#if TARGET_OS_SIMULATOR
        info.isSimulator = true;
#else
        info.isSimulator = false;
#endif

#else // macOS
        NSProcessInfo* processInfo = [NSProcessInfo processInfo];
        info.osName = "macOS";
        info.osVersion = [processInfo.operatingSystemVersionString UTF8String];

        // Get Mac model
        size_t len = 0;
        sysctlbyname("hw.model", NULL, &len, NULL, 0);
        char* model = (char*)malloc(len);
        sysctlbyname("hw.model", model, &len, NULL, 0);
        info.modelName = model;
        free(model);

        NSScreen* mainScreen = [NSScreen mainScreen];
        info.screenScale = mainScreen.backingScaleFactor;
        NSRect frame = mainScreen.frame;
        info.screenWidth = (int)(frame.size.width * info.screenScale);
        info.screenHeight = (int)(frame.size.height * info.screenScale);
        info.isSimulator = false;
#endif
    }

    return info;
}

// =============================================================================
// Feature Availability
// =============================================================================
AvailableFeatures checkAvailableFeatures() {
    AvailableFeatures features;

    @autoreleasepool {
        // Metal
        features.hasMetal = (g_metalDevice != nil);

        // Core ML
        if (@available(iOS 11.0, macOS 10.13, *)) {
            features.hasCoreML = true;
        } else {
            features.hasCoreML = false;
        }

        // ARKit (iOS only)
#if TARGET_OS_IOS
        if (@available(iOS 11.0, *)) {
            features.hasARKit = [ARConfiguration isSupported];
        } else {
            features.hasARKit = false;
        }
#else
        features.hasARKit = false;
#endif

        // PhotoKit
        features.hasPhotoKit = true;

        // CloudKit
        features.hasCloudKit = true;

        // SiriKit
        features.hasSiriKit = true;

        // Share Extension
        features.hasShareExtension = true;

        // WidgetKit
        if (@available(iOS 14.0, macOS 11.0, *)) {
            features.hasWidgetKit = true;
        } else {
            features.hasWidgetKit = false;
        }

        // Live Activity
#if TARGET_OS_IOS
        if (@available(iOS 16.1, *)) {
            features.hasLiveActivity = true;
        } else {
            features.hasLiveActivity = false;
        }
#else
        features.hasLiveActivity = false;
#endif
    }

    return features;
}

// =============================================================================
// File System
// =============================================================================
namespace FileSystem {

std::string getDocumentsDirectory() {
    @autoreleasepool {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths firstObject];
        return [documentsDirectory UTF8String];
    }
}

std::string getCachesDirectory() {
    @autoreleasepool {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cachesDirectory = [paths firstObject];
        return [cachesDirectory UTF8String];
    }
}

std::string getTempDirectory() {
    @autoreleasepool {
        NSString* tempDir = NSTemporaryDirectory();
        return [tempDir UTF8String];
    }
}

std::string getAppSupportDirectory() {
    @autoreleasepool {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString* appSupportDirectory = [paths firstObject];
        return [appSupportDirectory UTF8String];
    }
}

bool isICloudAvailable() {
    @autoreleasepool {
        return [[NSFileManager defaultManager] ubiquityIdentityToken] != nil;
    }
}

std::string getICloudContainerPath(const std::string& containerID) {
    @autoreleasepool {
        NSString* nsContainerID = [NSString stringWithUTF8String:containerID.c_str()];
        NSURL* containerURL = [[NSFileManager defaultManager] URLForUbiquityContainerIdentifier:nsContainerID];
        if (containerURL) {
            return [containerURL.path UTF8String];
        }
        return "";
    }
}

void showFilePicker(const std::vector<std::string>& allowedTypes, bool allowMultiple, FilePickerCallback callback) {
    @autoreleasepool {
#if TARGET_OS_IOS
        // iOS uses UIDocumentPickerViewController
        dispatch_async(dispatch_get_main_queue(), ^{
            NSMutableArray* utTypes = [NSMutableArray array];
            for (const auto& type : allowedTypes) {
                NSString* utType = [NSString stringWithFormat:@"public.%s", type.c_str()];
                [utTypes addObject:utType];
            }

            // Note: Actual implementation requires UIViewController
            // This is a placeholder - real implementation needs SwiftUI or UIKit integration
            NSLog(@"File picker requested for types: %@", utTypes);

            // Call callback with empty result for now
            std::vector<std::string> paths;
            callback(paths);
        });
#else
        // macOS uses NSOpenPanel
        dispatch_async(dispatch_get_main_queue(), ^{
            NSOpenPanel* panel = [NSOpenPanel openPanel];
            panel.allowsMultipleSelection = allowMultiple;
            panel.canChooseDirectories = NO;
            panel.canChooseFiles = YES;

            NSMutableArray* fileTypes = [NSMutableArray array];
            for (const auto& type : allowedTypes) {
                [fileTypes addObject:[NSString stringWithUTF8String:type.c_str()]];
            }
            panel.allowedContentTypes = @[];  // Set content types based on UTI

            [panel beginWithCompletionHandler:^(NSModalResponse result) {
                std::vector<std::string> paths;
                if (result == NSModalResponseOK) {
                    for (NSURL* url in panel.URLs) {
                        paths.push_back([url.path UTF8String]);
                    }
                }
                callback(paths);
            }];
        });
#endif
    }
}

void showSavePanel(const std::string& defaultName, const std::string& fileType, SaveCallback callback) {
    @autoreleasepool {
#if TARGET_OS_IOS
        // iOS uses share sheet or document export
        dispatch_async(dispatch_get_main_queue(), ^{
            NSLog(@"Save panel requested: %s.%s", defaultName.c_str(), fileType.c_str());
            callback(false, "");
        });
#else
        dispatch_async(dispatch_get_main_queue(), ^{
            NSSavePanel* panel = [NSSavePanel savePanel];
            panel.nameFieldStringValue = [NSString stringWithFormat:@"%s.%s",
                                          defaultName.c_str(), fileType.c_str()];

            [panel beginWithCompletionHandler:^(NSModalResponse result) {
                if (result == NSModalResponseOK && panel.URL) {
                    callback(true, [panel.URL.path UTF8String]);
                } else {
                    callback(false, "");
                }
            }];
        });
#endif
    }
}

} // namespace FileSystem

// =============================================================================
// PhotoKit
// =============================================================================
namespace PhotoKit {

AuthorizationStatus getAuthorizationStatus() {
    @autoreleasepool {
        PHAuthorizationStatus status = [PHPhotoLibrary authorizationStatusForAccessLevel:PHAccessLevelReadWrite];
        switch (status) {
            case PHAuthorizationStatusNotDetermined: return AuthorizationStatus::NotDetermined;
            case PHAuthorizationStatusRestricted: return AuthorizationStatus::Restricted;
            case PHAuthorizationStatusDenied: return AuthorizationStatus::Denied;
            case PHAuthorizationStatusAuthorized: return AuthorizationStatus::Authorized;
            case PHAuthorizationStatusLimited: return AuthorizationStatus::Limited;
            default: return AuthorizationStatus::NotDetermined;
        }
    }
}

void requestAuthorization(std::function<void(AuthorizationStatus)> callback) {
    @autoreleasepool {
        [PHPhotoLibrary requestAuthorizationForAccessLevel:PHAccessLevelReadWrite
                                                   handler:^(PHAuthorizationStatus status) {
            AuthorizationStatus appStatus;
            switch (status) {
                case PHAuthorizationStatusAuthorized: appStatus = AuthorizationStatus::Authorized; break;
                case PHAuthorizationStatusLimited: appStatus = AuthorizationStatus::Limited; break;
                case PHAuthorizationStatusDenied: appStatus = AuthorizationStatus::Denied; break;
                case PHAuthorizationStatusRestricted: appStatus = AuthorizationStatus::Restricted; break;
                default: appStatus = AuthorizationStatus::NotDetermined; break;
            }
            callback(appStatus);
        }];
    }
}

void fetchAssets(int limit, std::function<void(const std::vector<PhotoAsset>&)> callback) {
    @autoreleasepool {
        PHFetchOptions* options = [[PHFetchOptions alloc] init];
        options.sortDescriptors = @[[NSSortDescriptor sortDescriptorWithKey:@"creationDate" ascending:NO]];
        options.fetchLimit = limit;

        PHFetchResult<PHAsset*>* results = [PHAsset fetchAssetsWithOptions:options];

        __block std::vector<PhotoAsset> assets;
        [results enumerateObjectsUsingBlock:^(PHAsset* asset, NSUInteger idx, BOOL* stop) {
            PhotoAsset pa;
            pa.localIdentifier = [asset.localIdentifier UTF8String];

            NSArray* resources = [PHAssetResource assetResourcesForAsset:asset];
            if (resources.count > 0) {
                PHAssetResource* resource = resources[0];
                pa.filename = [resource.originalFilename UTF8String];
            }

            pa.width = (int)asset.pixelWidth;
            pa.height = (int)asset.pixelHeight;
            pa.creationDate = [asset.creationDate timeIntervalSince1970];
            pa.isVideo = (asset.mediaType == PHAssetMediaTypeVideo);
            pa.duration = asset.duration;

            assets.push_back(pa);
        }];

        callback(assets);
    }
}

void loadImageData(const std::string& localIdentifier,
                   int targetWidth,
                   int targetHeight,
                   std::function<void(const uint8_t* data, size_t size, int width, int height)> callback) {
    @autoreleasepool {
        NSString* identifier = [NSString stringWithUTF8String:localIdentifier.c_str()];
        PHFetchResult* results = [PHAsset fetchAssetsWithLocalIdentifiers:@[identifier] options:nil];

        if (results.count == 0) {
            callback(nullptr, 0, 0, 0);
            return;
        }

        PHAsset* asset = results.firstObject;
        PHImageRequestOptions* options = [[PHImageRequestOptions alloc] init];
        options.synchronous = NO;
        options.deliveryMode = PHImageRequestOptionsDeliveryModeHighQualityFormat;

        CGSize targetSize = CGSizeMake(targetWidth, targetHeight);

        [[PHImageManager defaultManager] requestImageForAsset:asset
                                                   targetSize:targetSize
                                                  contentMode:PHImageContentModeAspectFit
                                                      options:options
                                                resultHandler:^(id result, NSDictionary* info) {
#if TARGET_OS_IOS
            UIImage* image = (UIImage*)result;
            if (!image) {
                callback(nullptr, 0, 0, 0);
                return;
            }

            CGImageRef cgImage = image.CGImage;
            size_t width = CGImageGetWidth(cgImage);
            size_t height = CGImageGetHeight(cgImage);
            size_t bytesPerRow = CGImageGetBytesPerRow(cgImage);

            CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
            const uint8_t* bytes = CFDataGetBytePtr(data);
            size_t length = CFDataGetLength(data);

            callback(bytes, length, (int)width, (int)height);

            CFRelease(data);
#else
            NSImage* image = (NSImage*)result;
            if (!image) {
                callback(nullptr, 0, 0, 0);
                return;
            }

            NSBitmapImageRep* rep = [[NSBitmapImageRep alloc] initWithData:[image TIFFRepresentation]];
            callback((const uint8_t*)rep.bitmapData, rep.bytesPerRow * rep.pixelsHigh,
                     (int)rep.pixelsWide, (int)rep.pixelsHigh);
#endif
        }];
    }
}

void loadVideoURL(const std::string& localIdentifier,
                  std::function<void(const std::string& fileURL)> callback) {
    @autoreleasepool {
        NSString* identifier = [NSString stringWithUTF8String:localIdentifier.c_str()];
        PHFetchResult* results = [PHAsset fetchAssetsWithLocalIdentifiers:@[identifier] options:nil];

        if (results.count == 0) {
            callback("");
            return;
        }

        PHAsset* asset = results.firstObject;
        PHVideoRequestOptions* options = [[PHVideoRequestOptions alloc] init];
        options.deliveryMode = PHVideoRequestOptionsDeliveryModeHighQualityFormat;

        [[PHImageManager defaultManager] requestAVAssetForVideo:asset
                                                        options:options
                                                  resultHandler:^(AVAsset* avAsset, AVAudioMix* audioMix, NSDictionary* info) {
            if ([avAsset isKindOfClass:[AVURLAsset class]]) {
                AVURLAsset* urlAsset = (AVURLAsset*)avAsset;
                callback([urlAsset.URL.path UTF8String]);
            } else {
                callback("");
            }
        }];
    }
}

} // namespace PhotoKit

// =============================================================================
// Core ML
// =============================================================================
namespace CoreML {

bool loadModel(const std::string& modelPath, const std::string& identifier) {
    @autoreleasepool {
        std::lock_guard<std::mutex> lock(g_modelMutex);

        NSString* path = [NSString stringWithUTF8String:modelPath.c_str()];
        NSURL* modelURL = [NSURL fileURLWithPath:path];

        NSError* error = nil;
        MLModel* model = [MLModel modelWithContentsOfURL:modelURL error:&error];

        if (error || !model) {
            NSLog(@"Failed to load Core ML model: %@", error);
            return false;
        }

        g_coreMLModels[identifier] = model;
        return true;
    }
}

void unloadModel(const std::string& identifier) {
    std::lock_guard<std::mutex> lock(g_modelMutex);
    g_coreMLModels.erase(identifier);
}

ModelInfo getModelInfo(const std::string& identifier) {
    ModelInfo info;
    info.isLoaded = false;

    std::lock_guard<std::mutex> lock(g_modelMutex);
    auto it = g_coreMLModels.find(identifier);
    if (it != g_coreMLModels.end()) {
        MLModel* model = it->second;
        MLModelDescription* desc = model.modelDescription;

        info.name = identifier;
        info.author = desc.metadata[MLModelAuthorKey] ? [desc.metadata[MLModelAuthorKey] UTF8String] : "";
        info.description = desc.metadata[MLModelDescriptionKey] ? [desc.metadata[MLModelDescriptionKey] UTF8String] : "";
        info.isLoaded = true;
    }

    return info;
}

void detectFaces(const uint8_t* imageData,
                 int width,
                 int height,
                 int bytesPerPixel,
                 std::function<void(const std::vector<FaceDetection>&)> callback) {
    @autoreleasepool {
        // Create CGImage from raw data
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate(
            (void*)imageData,
            width, height,
            8,
            width * bytesPerPixel,
            colorSpace,
            kCGImageAlphaPremultipliedLast
        );

        CGImageRef cgImage = CGBitmapContextCreateImage(context);
        CGContextRelease(context);
        CGColorSpaceRelease(colorSpace);

        if (!cgImage) {
            callback({});
            return;
        }

        // Create Vision request
        VNDetectFaceRectanglesRequest* request = [[VNDetectFaceRectanglesRequest alloc]
            initWithCompletionHandler:^(VNRequest* req, NSError* error) {
                std::vector<FaceDetection> faces;

                if (!error) {
                    for (VNFaceObservation* face in req.results) {
                        FaceDetection fd;
                        fd.x = face.boundingBox.origin.x;
                        fd.y = face.boundingBox.origin.y;
                        fd.width = face.boundingBox.size.width;
                        fd.height = face.boundingBox.size.height;
                        fd.confidence = face.confidence;
                        fd.roll = face.roll ? face.roll.floatValue : 0;
                        fd.yaw = face.yaw ? face.yaw.floatValue : 0;
                        fd.pitch = 0; // Not available in basic detection
                        faces.push_back(fd);
                    }
                }

                callback(faces);
            }];

        // Run request
        VNImageRequestHandler* handler = [[VNImageRequestHandler alloc]
            initWithCGImage:cgImage options:@{}];

        NSError* error = nil;
        [handler performRequests:@[request] error:&error];

        CGImageRelease(cgImage);
    }
}

} // namespace CoreML

// =============================================================================
// AVFoundation
// =============================================================================
namespace AVFoundation {

bool openVideo(const std::string& path, const std::string& identifier) {
    @autoreleasepool {
        std::lock_guard<std::mutex> lock(g_assetMutex);

        NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
        NSURL* url = [NSURL fileURLWithPath:nsPath];
        AVAsset* asset = [AVAsset assetWithURL:url];

        if (!asset) {
            return false;
        }

        g_avAssets[identifier] = asset;
        return true;
    }
}

void closeVideo(const std::string& identifier) {
    std::lock_guard<std::mutex> lock(g_assetMutex);
    g_avAssets.erase(identifier);
    g_avReaders.erase(identifier);
}

VideoInfo getVideoInfo(const std::string& identifier) {
    VideoInfo info = {0, 0, 0, 0, "", false};

    std::lock_guard<std::mutex> lock(g_assetMutex);
    auto it = g_avAssets.find(identifier);
    if (it == g_avAssets.end()) {
        return info;
    }

    @autoreleasepool {
        AVAsset* asset = it->second;

        // Get video track
        NSArray* videoTracks = [asset tracksWithMediaType:AVMediaTypeVideo];
        if (videoTracks.count > 0) {
            AVAssetTrack* videoTrack = videoTracks[0];
            CGSize size = videoTrack.naturalSize;
            info.width = (int)size.width;
            info.height = (int)size.height;
            info.frameRate = videoTrack.nominalFrameRate;

            // Get codec
            NSArray* formatDescriptions = videoTrack.formatDescriptions;
            if (formatDescriptions.count > 0) {
                CMFormatDescriptionRef desc = (__bridge CMFormatDescriptionRef)formatDescriptions[0];
                FourCharCode codec = CMFormatDescriptionGetMediaSubType(desc);
                char codecStr[5] = {
                    (char)((codec >> 24) & 0xFF),
                    (char)((codec >> 16) & 0xFF),
                    (char)((codec >> 8) & 0xFF),
                    (char)(codec & 0xFF),
                    0
                };
                info.codec = codecStr;
            }
        }

        // Check for audio
        NSArray* audioTracks = [asset tracksWithMediaType:AVMediaTypeAudio];
        info.hasAudio = (audioTracks.count > 0);

        // Duration
        info.duration = CMTimeGetSeconds(asset.duration);
    }

    return info;
}

DecodedFrame decodeFrame(const std::string& identifier, double timestamp) {
    DecodedFrame frame = {false, nullptr, 0, 0, 0};

    std::lock_guard<std::mutex> lock(g_assetMutex);
    auto it = g_avAssets.find(identifier);
    if (it == g_avAssets.end()) {
        return frame;
    }

    @autoreleasepool {
        AVAsset* asset = it->second;
        AVAssetImageGenerator* generator = [[AVAssetImageGenerator alloc] initWithAsset:asset];
        generator.appliesPreferredTrackTransform = YES;
        generator.requestedTimeToleranceAfter = kCMTimeZero;
        generator.requestedTimeToleranceBefore = kCMTimeZero;

        CMTime time = CMTimeMakeWithSeconds(timestamp, 600);
        NSError* error = nil;
        CGImageRef cgImage = [generator copyCGImageAtTime:time actualTime:NULL error:&error];

        if (error || !cgImage) {
            return frame;
        }

        size_t width = CGImageGetWidth(cgImage);
        size_t height = CGImageGetHeight(cgImage);
        size_t bytesPerRow = width * 4;
        size_t dataSize = bytesPerRow * height;

        // Allocate RGBA buffer
        uint8_t* data = (uint8_t*)malloc(dataSize);

        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate(
            data, width, height, 8, bytesPerRow, colorSpace,
            kCGImageAlphaPremultipliedLast
        );

        CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);

        CGContextRelease(context);
        CGColorSpaceRelease(colorSpace);
        CGImageRelease(cgImage);

        frame.success = true;
        frame.data = data;
        frame.width = (int)width;
        frame.height = (int)height;
        frame.timestamp = timestamp;
    }

    return frame;
}

void freeFrameData(DecodedFrame& frame) {
    if (frame.data) {
        free(frame.data);
        frame.data = nullptr;
    }
}

} // namespace AVFoundation

// =============================================================================
// Audio
// =============================================================================
namespace Audio {

void configureAudioSession() {
#if TARGET_OS_IOS
    @autoreleasepool {
        AVAudioSession* session = [AVAudioSession sharedInstance];
        NSError* error = nil;

        [session setCategory:AVAudioSessionCategoryPlayAndRecord
                 withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker |
                            AVAudioSessionCategoryOptionAllowBluetooth
                       error:&error];

        if (error) {
            NSLog(@"Audio session configuration error: %@", error);
        }

        [session setActive:YES error:&error];
    }
#endif
}

std::vector<AudioDevice> getAvailableDevices() {
    std::vector<AudioDevice> devices;

#if TARGET_OS_OSX
    @autoreleasepool {
        // macOS audio device enumeration would use Core Audio
        // Simplified placeholder
        AudioDevice defaultDevice;
        defaultDevice.uid = "default";
        defaultDevice.name = "Default Output";
        defaultDevice.isInput = false;
        defaultDevice.isOutput = true;
        devices.push_back(defaultDevice);
    }
#endif

    return devices;
}

bool playSound(const std::string& path) {
    // Simplified - real implementation would use AVAudioPlayer or AVAudioEngine
    return true;
}

void stopAllSounds() {
    // Implementation would stop all active AVAudioPlayers
}

} // namespace Audio

// =============================================================================
// Share
// =============================================================================
namespace Share {

void showShareSheet(const std::vector<ShareItem>& items,
                    std::function<void(bool success, const std::string& activityType)> callback) {
    @autoreleasepool {
        NSMutableArray* activityItems = [NSMutableArray array];

        for (const auto& item : items) {
            switch (item.type) {
                case ItemType::Text:
                    [activityItems addObject:[NSString stringWithUTF8String:item.data.c_str()]];
                    break;
                case ItemType::URL:
                    [activityItems addObject:[NSURL URLWithString:[NSString stringWithUTF8String:item.data.c_str()]]];
                    break;
                case ItemType::File:
                case ItemType::Image:
                case ItemType::Video:
                    [activityItems addObject:[NSURL fileURLWithPath:[NSString stringWithUTF8String:item.data.c_str()]]];
                    break;
            }
        }

        dispatch_async(dispatch_get_main_queue(), ^{
#if TARGET_OS_IOS
            UIActivityViewController* activityVC = [[UIActivityViewController alloc]
                initWithActivityItems:activityItems applicationActivities:nil];

            // Note: Real implementation needs to present from a UIViewController
            callback(true, "");
#else
            NSSharingServicePicker* picker = [[NSSharingServicePicker alloc] initWithItems:activityItems];
            // Note: Real implementation needs to show from a view
            callback(true, "");
#endif
        });
    }
}

} // namespace Share

// =============================================================================
// Haptics (iOS)
// =============================================================================
namespace Haptics {

void trigger(FeedbackStyle style) {
#if TARGET_OS_IOS
    @autoreleasepool {
        switch (style) {
            case FeedbackStyle::Selection: {
                UISelectionFeedbackGenerator* generator = [[UISelectionFeedbackGenerator alloc] init];
                [generator selectionChanged];
                break;
            }
            case FeedbackStyle::Light:
            case FeedbackStyle::Medium:
            case FeedbackStyle::Heavy: {
                UIImpactFeedbackStyle impactStyle;
                switch (style) {
                    case FeedbackStyle::Light: impactStyle = UIImpactFeedbackStyleLight; break;
                    case FeedbackStyle::Heavy: impactStyle = UIImpactFeedbackStyleHeavy; break;
                    default: impactStyle = UIImpactFeedbackStyleMedium; break;
                }
                UIImpactFeedbackGenerator* generator = [[UIImpactFeedbackGenerator alloc]
                    initWithStyle:impactStyle];
                [generator impactOccurred];
                break;
            }
            case FeedbackStyle::Success:
            case FeedbackStyle::Warning:
            case FeedbackStyle::Error: {
                UINotificationFeedbackType notifType;
                switch (style) {
                    case FeedbackStyle::Success: notifType = UINotificationFeedbackTypeSuccess; break;
                    case FeedbackStyle::Warning: notifType = UINotificationFeedbackTypeWarning; break;
                    default: notifType = UINotificationFeedbackTypeError; break;
                }
                UINotificationFeedbackGenerator* generator = [[UINotificationFeedbackGenerator alloc] init];
                [generator notificationOccurred:notifType];
                break;
            }
        }
    }
#endif
}

} // namespace Haptics

// =============================================================================
// Notifications
// =============================================================================
namespace Notifications {

void requestPermission(std::function<void(bool granted)> callback) {
    @autoreleasepool {
        UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
        [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge)
                              completionHandler:^(BOOL granted, NSError* error) {
            callback(granted);
        }];
    }
}

void scheduleLocalNotification(const std::string& title,
                               const std::string& body,
                               double delaySeconds,
                               const std::string& identifier) {
    @autoreleasepool {
        UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
        content.title = [NSString stringWithUTF8String:title.c_str()];
        content.body = [NSString stringWithUTF8String:body.c_str()];
        content.sound = [UNNotificationSound defaultSound];

        UNTimeIntervalNotificationTrigger* trigger =
            [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:delaySeconds repeats:NO];

        NSString* nsIdentifier = [NSString stringWithUTF8String:identifier.c_str()];
        UNNotificationRequest* request =
            [UNNotificationRequest requestWithIdentifier:nsIdentifier content:content trigger:trigger];

        UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
        [center addNotificationRequest:request withCompletionHandler:nil];
    }
}

void cancelNotification(const std::string& identifier) {
    @autoreleasepool {
        NSString* nsIdentifier = [NSString stringWithUTF8String:identifier.c_str()];
        UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
        [center removePendingNotificationRequestsWithIdentifiers:@[nsIdentifier]];
    }
}

void cancelAllNotifications() {
    @autoreleasepool {
        UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
        [center removeAllPendingNotificationRequests];
    }
}

} // namespace Notifications

// =============================================================================
// Lifecycle
// =============================================================================
namespace Lifecycle {

void setOnEnterBackground(LifecycleCallback callback) {
    g_onEnterBackground = callback;
}

void setOnEnterForeground(LifecycleCallback callback) {
    g_onEnterForeground = callback;
}

void setOnTerminate(LifecycleCallback callback) {
    g_onTerminate = callback;
}

void setOnMemoryWarning(LifecycleCallback callback) {
    g_onMemoryWarning = callback;
}

} // namespace Lifecycle

// =============================================================================
// Metal
// =============================================================================
namespace Metal {

void* getDevice() {
    return (__bridge void*)g_metalDevice;
}

void* getCommandQueue() {
    return (__bridge void*)g_metalCommandQueue;
}

void* createTexture(const uint8_t* data, int width, int height, int bytesPerPixel) {
    @autoreleasepool {
        if (!g_metalDevice) return nullptr;

        MTLTextureDescriptor* descriptor = [[MTLTextureDescriptor alloc] init];
        descriptor.pixelFormat = (bytesPerPixel == 4) ? MTLPixelFormatRGBA8Unorm : MTLPixelFormatR8Unorm;
        descriptor.width = width;
        descriptor.height = height;
        descriptor.usage = MTLTextureUsageShaderRead;

        id<MTLTexture> texture = [g_metalDevice newTextureWithDescriptor:descriptor];

        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        [texture replaceRegion:region mipmapLevel:0 withBytes:data bytesPerRow:width * bytesPerPixel];

        return (__bridge_retained void*)texture;
    }
}

void releaseTexture(void* texture) {
    if (texture) {
        id<MTLTexture> mtlTexture = (__bridge_transfer id<MTLTexture>)texture;
        mtlTexture = nil;
    }
}

} // namespace Metal

} // namespace AppleBridge

// =============================================================================
// C Wrapper Functions (for main4.mm compatibility)
// =============================================================================
static std::string g_documentsDirectory;

extern "C" {

void AppleBridge_initialize(void) {
    AppleBridge::initialize();
    g_documentsDirectory = AppleBridge::FileSystem::getDocumentsDirectory();
}

void AppleBridge_shutdown(void) {
    AppleBridge::shutdown();
}

const char* AppleBridge_getDocumentsDirectory(void) {
    if (g_documentsDirectory.empty()) {
        g_documentsDirectory = AppleBridge::FileSystem::getDocumentsDirectory();
    }
    return g_documentsDirectory.c_str();
}

} // extern "C"
