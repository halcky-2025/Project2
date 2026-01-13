// =============================================================================
// AppleBridge-Swift.h - Swift Bridging Header (Objective-C only)
// =============================================================================
// This header is for Swift bridging. It must NOT contain any C++ code.
// =============================================================================

#pragma once

#import <Foundation/Foundation.h>

#if TARGET_OS_IOS
    #import <UIKit/UIKit.h>
#else
    #import <AppKit/AppKit.h>
#endif

// =============================================================================
// Device Information (Objective-C interface)
// =============================================================================

NS_ASSUME_NONNULL_BEGIN

@interface HopStarDeviceInfo : NSObject
@property (nonatomic, copy) NSString *modelName;
@property (nonatomic, copy) NSString *osVersion;
@property (nonatomic, copy) NSString *osName;
@property (nonatomic, assign) float screenScale;
@property (nonatomic, assign) int screenWidth;
@property (nonatomic, assign) int screenHeight;
@property (nonatomic, assign) BOOL isSimulator;
@end

// =============================================================================
// Feature Availability
// =============================================================================

@interface HopStarFeatures : NSObject
@property (nonatomic, assign) BOOL hasMetal;
@property (nonatomic, assign) BOOL hasCoreML;
@property (nonatomic, assign) BOOL hasARKit;
@property (nonatomic, assign) BOOL hasPhotoKit;
@property (nonatomic, assign) BOOL hasCloudKit;
@end

// =============================================================================
// Apple Bridge (Objective-C interface for Swift)
// =============================================================================

@interface HopStarBridge : NSObject

+ (instancetype)shared;

// Initialization
- (void)initialize;
- (void)shutdown;

// Device info
- (HopStarDeviceInfo *)getDeviceInfo;
- (HopStarFeatures *)checkAvailableFeatures;

// File system
- (NSString *)getDocumentsDirectory;
- (NSString *)getCachesDirectory;
- (NSString *)getTempDirectory;
- (NSString *)getAppSupportDirectory;
- (BOOL)isICloudAvailable;

// Haptics (iOS only)
- (void)triggerHapticLight;
- (void)triggerHapticMedium;
- (void)triggerHapticHeavy;
- (void)triggerHapticSuccess;
- (void)triggerHapticWarning;
- (void)triggerHapticError;

// Audio
- (void)configureAudioSession;

@end

NS_ASSUME_NONNULL_END
