// =============================================================================
// AppleBridge-ObjC.m - Objective-C Implementation for Swift Bridge
// =============================================================================

#import "AppleBridge-Swift.h"
#import <Metal/Metal.h>
#import <AVFoundation/AVFoundation.h>

#if TARGET_OS_IOS
#import <UIKit/UIKit.h>
#endif

// =============================================================================
// C Interface for C++ (extern "C" functions)
// =============================================================================

static NSString *g_documentsDirectory = nil;

#if TARGET_OS_IOS
static UIWindow *g_window = nil;
#endif

void AppleBridge_initialize(void) {
    [[HopStarBridge shared] initialize];
    g_documentsDirectory = [[HopStarBridge shared] getDocumentsDirectory];
}

void AppleBridge_shutdown(void) {
    [[HopStarBridge shared] shutdown];
    g_documentsDirectory = nil;
}

const char* AppleBridge_getDocumentsDirectory(void) {
    if (!g_documentsDirectory) {
        g_documentsDirectory = [[HopStarBridge shared] getDocumentsDirectory];
    }
    return [g_documentsDirectory UTF8String];
}

#if TARGET_OS_IOS
void AppleBridge_showMainUI(void) {
    dispatch_async(dispatch_get_main_queue(), ^{
        // Get the first connected scene
        UIWindowScene *windowScene = nil;
        for (UIScene *scene in [UIApplication sharedApplication].connectedScenes) {
            if ([scene isKindOfClass:[UIWindowScene class]]) {
                windowScene = (UIWindowScene *)scene;
                break;
            }
        }

        if (!windowScene) {
            NSLog(@"No window scene found");
            return;
        }

        // Create window
        g_window = [[UIWindow alloc] initWithWindowScene:windowScene];
        g_window.frame = windowScene.coordinateSpace.bounds;

        // Create view controller
        UIViewController *vc = [[UIViewController alloc] init];
        vc.view.backgroundColor = [UIColor colorWithRed:0.1 green:0.1 blue:0.15 alpha:1.0];

        // Create title label
        UILabel *titleLabel = [[UILabel alloc] init];
        titleLabel.text = @"HopStar";
        titleLabel.font = [UIFont boldSystemFontOfSize:32];
        titleLabel.textColor = [UIColor whiteColor];
        titleLabel.textAlignment = NSTextAlignmentCenter;
        titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
        [vc.view addSubview:titleLabel];

        // Create status label
        UILabel *statusLabel = [[UILabel alloc] init];
        statusLabel.text = @"iOS App Ready\nAdd SDL3/bgfx for full functionality";
        statusLabel.font = [UIFont systemFontOfSize:16];
        statusLabel.textColor = [UIColor lightGrayColor];
        statusLabel.textAlignment = NSTextAlignmentCenter;
        statusLabel.numberOfLines = 0;
        statusLabel.translatesAutoresizingMaskIntoConstraints = NO;
        [vc.view addSubview:statusLabel];

        // Create device info label
        HopStarDeviceInfo *info = [[HopStarBridge shared] getDeviceInfo];
        UILabel *infoLabel = [[UILabel alloc] init];
        infoLabel.text = [NSString stringWithFormat:@"Device: %@\niOS %@\nScreen: %dx%d @%.0fx",
                          info.modelName, info.osVersion,
                          info.screenWidth, info.screenHeight, info.screenScale];
        infoLabel.font = [UIFont monospacedSystemFontOfSize:12 weight:UIFontWeightRegular];
        infoLabel.textColor = [UIColor colorWithWhite:0.5 alpha:1.0];
        infoLabel.textAlignment = NSTextAlignmentCenter;
        infoLabel.numberOfLines = 0;
        infoLabel.translatesAutoresizingMaskIntoConstraints = NO;
        [vc.view addSubview:infoLabel];

        // Layout constraints
        [NSLayoutConstraint activateConstraints:@[
            [titleLabel.centerXAnchor constraintEqualToAnchor:vc.view.centerXAnchor],
            [titleLabel.centerYAnchor constraintEqualToAnchor:vc.view.centerYAnchor constant:-60],

            [statusLabel.centerXAnchor constraintEqualToAnchor:vc.view.centerXAnchor],
            [statusLabel.topAnchor constraintEqualToAnchor:titleLabel.bottomAnchor constant:20],
            [statusLabel.leadingAnchor constraintEqualToAnchor:vc.view.leadingAnchor constant:20],
            [statusLabel.trailingAnchor constraintEqualToAnchor:vc.view.trailingAnchor constant:-20],

            [infoLabel.centerXAnchor constraintEqualToAnchor:vc.view.centerXAnchor],
            [infoLabel.bottomAnchor constraintEqualToAnchor:vc.view.safeAreaLayoutGuide.bottomAnchor constant:-40],
            [infoLabel.leadingAnchor constraintEqualToAnchor:vc.view.leadingAnchor constant:20],
            [infoLabel.trailingAnchor constraintEqualToAnchor:vc.view.trailingAnchor constant:-20],
        ]];

        g_window.rootViewController = vc;
        [g_window makeKeyAndVisible];

        NSLog(@"Main UI displayed");
    });
}
#endif

// =============================================================================

#if TARGET_OS_IOS
    #import <UIKit/UIKit.h>
    #import <ARKit/ARKit.h>
#else
    #import <AppKit/AppKit.h>
    #include <sys/sysctl.h>
#endif

// =============================================================================
// HopStarDeviceInfo Implementation
// =============================================================================

@implementation HopStarDeviceInfo
@end

// =============================================================================
// HopStarFeatures Implementation
// =============================================================================

@implementation HopStarFeatures
@end

// =============================================================================
// HopStarBridge Implementation
// =============================================================================

@implementation HopStarBridge {
    id<MTLDevice> _metalDevice;
    id<MTLCommandQueue> _metalCommandQueue;
}

+ (instancetype)shared {
    static HopStarBridge *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[HopStarBridge alloc] init];
    });
    return instance;
}

- (void)initialize {
    _metalDevice = MTLCreateSystemDefaultDevice();
    if (_metalDevice) {
        _metalCommandQueue = [_metalDevice newCommandQueue];
        NSLog(@"Metal initialized: %@", _metalDevice.name);
    } else {
        NSLog(@"Metal not available");
    }
}

- (void)shutdown {
    _metalCommandQueue = nil;
    _metalDevice = nil;
    NSLog(@"HopStarBridge shutdown");
}

- (HopStarDeviceInfo *)getDeviceInfo {
    HopStarDeviceInfo *info = [[HopStarDeviceInfo alloc] init];

#if TARGET_OS_IOS
    UIDevice *device = [UIDevice currentDevice];
    info.modelName = device.model;
    info.osName = @"iOS";
    info.osVersion = device.systemVersion;
    info.screenScale = [[UIScreen mainScreen] scale];

    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    info.screenWidth = (int)(screenBounds.size.width * info.screenScale);
    info.screenHeight = (int)(screenBounds.size.height * info.screenScale);

#if TARGET_OS_SIMULATOR
    info.isSimulator = YES;
#else
    info.isSimulator = NO;
#endif

#else // macOS
    NSProcessInfo *processInfo = [NSProcessInfo processInfo];
    info.osName = @"macOS";
    info.osVersion = processInfo.operatingSystemVersionString;

    // Get Mac model
    size_t len = 0;
    sysctlbyname("hw.model", NULL, &len, NULL, 0);
    char *model = (char *)malloc(len);
    sysctlbyname("hw.model", model, &len, NULL, 0);
    info.modelName = [NSString stringWithUTF8String:model];
    free(model);

    NSScreen *mainScreen = [NSScreen mainScreen];
    info.screenScale = mainScreen.backingScaleFactor;
    NSRect frame = mainScreen.frame;
    info.screenWidth = (int)(frame.size.width * info.screenScale);
    info.screenHeight = (int)(frame.size.height * info.screenScale);
    info.isSimulator = NO;
#endif

    return info;
}

- (HopStarFeatures *)checkAvailableFeatures {
    HopStarFeatures *features = [[HopStarFeatures alloc] init];

    features.hasMetal = (_metalDevice != nil);
    features.hasCoreML = YES;  // Available on iOS 11+ / macOS 10.13+

#if TARGET_OS_IOS
    if (@available(iOS 11.0, *)) {
        features.hasARKit = [ARConfiguration isSupported];
    } else {
        features.hasARKit = NO;
    }
#else
    features.hasARKit = NO;
#endif

    features.hasPhotoKit = YES;
    features.hasCloudKit = YES;

    return features;
}

// File System

- (NSString *)getDocumentsDirectory {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    return [paths firstObject];
}

- (NSString *)getCachesDirectory {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    return [paths firstObject];
}

- (NSString *)getTempDirectory {
    return NSTemporaryDirectory();
}

- (NSString *)getAppSupportDirectory {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    return [paths firstObject];
}

- (BOOL)isICloudAvailable {
    return [[NSFileManager defaultManager] ubiquityIdentityToken] != nil;
}

// Haptics

- (void)triggerHapticLight {
#if TARGET_OS_IOS
    UIImpactFeedbackGenerator *generator = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleLight];
    [generator impactOccurred];
#endif
}

- (void)triggerHapticMedium {
#if TARGET_OS_IOS
    UIImpactFeedbackGenerator *generator = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleMedium];
    [generator impactOccurred];
#endif
}

- (void)triggerHapticHeavy {
#if TARGET_OS_IOS
    UIImpactFeedbackGenerator *generator = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleHeavy];
    [generator impactOccurred];
#endif
}

- (void)triggerHapticSuccess {
#if TARGET_OS_IOS
    UINotificationFeedbackGenerator *generator = [[UINotificationFeedbackGenerator alloc] init];
    [generator notificationOccurred:UINotificationFeedbackTypeSuccess];
#endif
}

- (void)triggerHapticWarning {
#if TARGET_OS_IOS
    UINotificationFeedbackGenerator *generator = [[UINotificationFeedbackGenerator alloc] init];
    [generator notificationOccurred:UINotificationFeedbackTypeWarning];
#endif
}

- (void)triggerHapticError {
#if TARGET_OS_IOS
    UINotificationFeedbackGenerator *generator = [[UINotificationFeedbackGenerator alloc] init];
    [generator notificationOccurred:UINotificationFeedbackTypeError];
#endif
}

// Audio

- (void)configureAudioSession {
#if TARGET_OS_IOS
    @try {
        AVAudioSession *session = [AVAudioSession sharedInstance];
        NSError *error = nil;

        [session setCategory:AVAudioSessionCategoryPlayAndRecord
                 withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker |
                            AVAudioSessionCategoryOptionAllowBluetooth
                       error:&error];

        if (error) {
            NSLog(@"Audio session configuration error: %@", error);
        }

        [session setActive:YES error:&error];
    } @catch (NSException *exception) {
        NSLog(@"Audio session exception: %@", exception);
    }
#endif
}

@end
