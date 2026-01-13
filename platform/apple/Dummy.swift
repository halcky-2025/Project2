// Dummy Swift file to satisfy Xcode's Swift module generation
// This file can be removed once real Swift code is added

import Foundation

@objc public class HopStarApp: NSObject {
    @objc public static func appVersion() -> String {
        return "1.0.0"
    }

    @objc public static func platformName() -> String {
        #if os(iOS)
        return "iOS"
        #else
        return "macOS"
        #endif
    }
}
