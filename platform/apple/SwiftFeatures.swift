// =============================================================================
// SwiftFeatures.swift - Swift Implementation of Apple-specific Features
// =============================================================================
// This file contains features that are easier to implement in Swift.
// These are exposed to Objective-C++ via @objc annotations.
// =============================================================================

import Foundation
import CoreML
import Vision
import AVFoundation

#if canImport(UIKit)
import UIKit
#endif

#if canImport(AppKit)
import AppKit
#endif

#if canImport(SwiftUI)
import SwiftUI
#endif

// =============================================================================
// MARK: - Swift Bridge Class (Exposed to Objective-C++)
// =============================================================================

@objc public class SwiftBridge: NSObject {

    // MARK: - Singleton
    @objc public static let shared = SwiftBridge()

    private override init() {
        super.init()
    }

    // MARK: - Core ML Model Loading

    private var loadedModels: [String: MLModel] = [:]

    @objc public func loadCoreMLModel(at path: String, identifier: String) -> Bool {
        guard let modelURL = URL(string: path) ?? URL(fileURLWithPath: path) as URL? else {
            return false
        }

        do {
            let config = MLModelConfiguration()
            config.computeUnits = .all  // Use Neural Engine when available

            let model = try MLModel(contentsOf: modelURL, configuration: config)
            loadedModels[identifier] = model
            return true
        } catch {
            print("Failed to load Core ML model: \(error)")
            return false
        }
    }

    @objc public func unloadCoreMLModel(identifier: String) {
        loadedModels.removeValue(forKey: identifier)
    }

    // MARK: - Vision Face Detection

    @objc public func detectFaces(
        in imageData: Data,
        width: Int,
        height: Int,
        completion: @escaping ([[String: Any]]) -> Void
    ) {
        guard let cgImage = createCGImage(from: imageData, width: width, height: height) else {
            completion([])
            return
        }

        let request = VNDetectFaceRectanglesRequest { request, error in
            guard error == nil,
                  let observations = request.results as? [VNFaceObservation] else {
                completion([])
                return
            }

            let faces: [[String: Any]] = observations.map { face in
                return [
                    "x": face.boundingBox.origin.x,
                    "y": face.boundingBox.origin.y,
                    "width": face.boundingBox.width,
                    "height": face.boundingBox.height,
                    "confidence": face.confidence,
                    "roll": face.roll?.floatValue ?? 0,
                    "yaw": face.yaw?.floatValue ?? 0
                ]
            }

            completion(faces)
        }

        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        try? handler.perform([request])
    }

    // MARK: - Vision Face Landmarks

    @objc public func detectFaceLandmarks(
        in imageData: Data,
        width: Int,
        height: Int,
        completion: @escaping ([[String: Any]]) -> Void
    ) {
        guard let cgImage = createCGImage(from: imageData, width: width, height: height) else {
            completion([])
            return
        }

        let request = VNDetectFaceLandmarksRequest { request, error in
            guard error == nil,
                  let observations = request.results as? [VNFaceObservation] else {
                completion([])
                return
            }

            let faces: [[String: Any]] = observations.compactMap { face in
                guard let landmarks = face.landmarks else { return nil }

                var result: [String: Any] = [
                    "boundingBox": [
                        "x": face.boundingBox.origin.x,
                        "y": face.boundingBox.origin.y,
                        "width": face.boundingBox.width,
                        "height": face.boundingBox.height
                    ]
                ]

                // Extract key landmarks
                if let leftEye = landmarks.leftEye {
                    result["leftEye"] = landmarkPoints(leftEye)
                }
                if let rightEye = landmarks.rightEye {
                    result["rightEye"] = landmarkPoints(rightEye)
                }
                if let nose = landmarks.nose {
                    result["nose"] = landmarkPoints(nose)
                }
                if let outerLips = landmarks.outerLips {
                    result["outerLips"] = landmarkPoints(outerLips)
                }
                if let faceContour = landmarks.faceContour {
                    result["faceContour"] = landmarkPoints(faceContour)
                }

                return result
            }

            completion(faces)
        }

        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        try? handler.perform([request])
    }

    private func landmarkPoints(_ region: VNFaceLandmarkRegion2D) -> [[String: CGFloat]] {
        return region.normalizedPoints.map { point in
            return ["x": point.x, "y": point.y]
        }
    }

    // MARK: - Text Recognition (OCR)

    @objc public func recognizeText(
        in imageData: Data,
        width: Int,
        height: Int,
        completion: @escaping ([String]) -> Void
    ) {
        guard let cgImage = createCGImage(from: imageData, width: width, height: height) else {
            completion([])
            return
        }

        let request = VNRecognizeTextRequest { request, error in
            guard error == nil,
                  let observations = request.results as? [VNRecognizedTextObservation] else {
                completion([])
                return
            }

            let texts = observations.compactMap { observation -> String? in
                return observation.topCandidates(1).first?.string
            }

            completion(texts)
        }

        request.recognitionLevel = .accurate
        request.recognitionLanguages = ["ja-JP", "en-US"]  // Japanese and English

        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        try? handler.perform([request])
    }

    // MARK: - Object Detection

    @objc public func detectObjects(
        in imageData: Data,
        width: Int,
        height: Int,
        completion: @escaping ([[String: Any]]) -> Void
    ) {
        guard let cgImage = createCGImage(from: imageData, width: width, height: height) else {
            completion([])
            return
        }

        // Use rectangle detection as a basic object detection
        let request = VNDetectRectanglesRequest { request, error in
            guard error == nil,
                  let observations = request.results as? [VNRectangleObservation] else {
                completion([])
                return
            }

            let objects: [[String: Any]] = observations.map { rect in
                return [
                    "topLeft": ["x": rect.topLeft.x, "y": rect.topLeft.y],
                    "topRight": ["x": rect.topRight.x, "y": rect.topRight.y],
                    "bottomLeft": ["x": rect.bottomLeft.x, "y": rect.bottomLeft.y],
                    "bottomRight": ["x": rect.bottomRight.x, "y": rect.bottomRight.y],
                    "confidence": rect.confidence
                ]
            }

            completion(objects)
        }

        request.minimumConfidence = 0.5
        request.maximumObservations = 10

        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        try? handler.perform([request])
    }

    // MARK: - Helper: Create CGImage from raw data

    private func createCGImage(from data: Data, width: Int, height: Int) -> CGImage? {
        let bytesPerPixel = 4
        let bytesPerRow = width * bytesPerPixel

        guard let provider = CGDataProvider(data: data as CFData) else {
            return nil
        }

        return CGImage(
            width: width,
            height: height,
            bitsPerComponent: 8,
            bitsPerPixel: bytesPerPixel * 8,
            bytesPerRow: bytesPerRow,
            space: CGColorSpaceCreateDeviceRGB(),
            bitmapInfo: CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue),
            provider: provider,
            decode: nil,
            shouldInterpolate: true,
            intent: .defaultIntent
        )
    }
}

// =============================================================================
// MARK: - Video Processing Extensions
// =============================================================================

@objc public class SwiftVideoProcessor: NSObject {

    private var assetWriter: AVAssetWriter?
    private var videoInput: AVAssetWriterInput?
    private var pixelBufferAdaptor: AVAssetWriterInputPixelBufferAdaptor?

    @objc public func startExport(
        outputPath: String,
        width: Int,
        height: Int,
        frameRate: Double,
        bitrate: Int
    ) -> Bool {
        let outputURL = URL(fileURLWithPath: outputPath)

        // Remove existing file
        try? FileManager.default.removeItem(at: outputURL)

        do {
            assetWriter = try AVAssetWriter(outputURL: outputURL, fileType: .mp4)
        } catch {
            print("Failed to create asset writer: \(error)")
            return false
        }

        // Video settings
        let videoSettings: [String: Any] = [
            AVVideoCodecKey: AVVideoCodecType.h264,
            AVVideoWidthKey: width,
            AVVideoHeightKey: height,
            AVVideoCompressionPropertiesKey: [
                AVVideoAverageBitRateKey: bitrate,
                AVVideoExpectedSourceFrameRateKey: frameRate,
                AVVideoProfileLevelKey: AVVideoProfileLevelH264HighAutoLevel
            ]
        ]

        videoInput = AVAssetWriterInput(mediaType: .video, outputSettings: videoSettings)
        videoInput?.expectsMediaDataInRealTime = false

        // Pixel buffer attributes
        let pixelBufferAttributes: [String: Any] = [
            kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_32BGRA,
            kCVPixelBufferWidthKey as String: width,
            kCVPixelBufferHeightKey as String: height
        ]

        pixelBufferAdaptor = AVAssetWriterInputPixelBufferAdaptor(
            assetWriterInput: videoInput!,
            sourcePixelBufferAttributes: pixelBufferAttributes
        )

        guard let writer = assetWriter, let input = videoInput else {
            return false
        }

        if writer.canAdd(input) {
            writer.add(input)
        }

        return writer.startWriting()
    }

    @objc public func appendFrame(
        rgbaData: Data,
        width: Int,
        height: Int,
        frameTime: CMTime
    ) -> Bool {
        guard let adaptor = pixelBufferAdaptor,
              let input = videoInput,
              input.isReadyForMoreMediaData else {
            return false
        }

        var pixelBuffer: CVPixelBuffer?
        let status = CVPixelBufferCreate(
            kCFAllocatorDefault,
            width,
            height,
            kCVPixelFormatType_32BGRA,
            nil,
            &pixelBuffer
        )

        guard status == kCVReturnSuccess, let buffer = pixelBuffer else {
            return false
        }

        CVPixelBufferLockBaseAddress(buffer, [])
        let pixelData = CVPixelBufferGetBaseAddress(buffer)

        // Copy RGBA data (convert to BGRA)
        rgbaData.withUnsafeBytes { ptr in
            let src = ptr.bindMemory(to: UInt8.self)
            let dst = pixelData!.assumingMemoryBound(to: UInt8.self)

            for i in stride(from: 0, to: width * height * 4, by: 4) {
                dst[i + 0] = src[i + 2]  // B <- R
                dst[i + 1] = src[i + 1]  // G <- G
                dst[i + 2] = src[i + 0]  // R <- B
                dst[i + 3] = src[i + 3]  // A <- A
            }
        }

        CVPixelBufferUnlockBaseAddress(buffer, [])

        return adaptor.append(buffer, withPresentationTime: frameTime)
    }

    @objc public func finishExport(completion: @escaping (Bool) -> Void) {
        guard let writer = assetWriter, let input = videoInput else {
            completion(false)
            return
        }

        input.markAsFinished()
        writer.finishWriting {
            completion(writer.status == .completed)
        }
    }
}

// =============================================================================
// MARK: - App Intents (Siri Shortcuts) - iOS 16+ / macOS 13+
// =============================================================================

#if swift(>=5.7)
import AppIntents

@available(iOS 16.0, macOS 13.0, *)
struct OpenProjectIntent: AppIntent {
    static var title: LocalizedStringResource = "Open HopStar Project"
    static var description = IntentDescription("Opens a HopStar project file")

    @Parameter(title: "Project Name")
    var projectName: String

    func perform() async throws -> some IntentResult {
        // TODO: Implement project opening logic
        return .result()
    }
}

@available(iOS 16.0, macOS 13.0, *)
struct ExportVideoIntent: AppIntent {
    static var title: LocalizedStringResource = "Export Video"
    static var description = IntentDescription("Exports the current HopStar project as video")

    @Parameter(title: "Quality")
    var quality: String

    func perform() async throws -> some IntentResult {
        // TODO: Implement export logic
        return .result()
    }
}
#endif

// =============================================================================
// MARK: - CloudKit Sync Manager
// =============================================================================

@objc public class CloudKitManager: NSObject {

    @objc public static let shared = CloudKitManager()

    private override init() {
        super.init()
    }

    @objc public func checkAccountStatus(completion: @escaping (Bool, String) -> Void) {
        // CKContainer.default() requires CloudKit entitlement
        // This is a placeholder that would need CloudKit setup
        completion(false, "CloudKit not configured")
    }

    @objc public func syncProject(projectData: Data, identifier: String, completion: @escaping (Bool, String?) -> Void) {
        // Placeholder for CloudKit sync
        completion(false, "CloudKit sync not implemented")
    }
}

// =============================================================================
// MARK: - Accessibility Support
// =============================================================================

@objc public class AccessibilityManager: NSObject {

    @objc public static let shared = AccessibilityManager()

    private override init() {
        super.init()
    }

    @objc public var isVoiceOverRunning: Bool {
        #if canImport(UIKit)
        return UIAccessibility.isVoiceOverRunning
        #else
        return NSWorkspace.shared.isVoiceOverEnabled
        #endif
    }

    @objc public var isReduceMotionEnabled: Bool {
        #if canImport(UIKit)
        return UIAccessibility.isReduceMotionEnabled
        #else
        return NSWorkspace.shared.accessibilityDisplayShouldReduceMotion
        #endif
    }

    @objc public func announce(_ message: String) {
        #if canImport(UIKit)
        UIAccessibility.post(notification: .announcement, argument: message)
        #else
        NSAccessibility.post(element: NSApp as Any, notification: .announcementRequested, userInfo: [
            .announcement: message,
            .priority: NSAccessibilityPriorityLevel.high
        ])
        #endif
    }
}

// =============================================================================
// MARK: - Handoff Support
// =============================================================================

@objc public class HandoffManager: NSObject {

    @objc public static let shared = HandoffManager()

    private var currentActivity: NSUserActivity?

    private override init() {
        super.init()
    }

    @objc public func startActivity(type: String, title: String, userInfo: [String: Any]) {
        currentActivity?.invalidate()

        currentActivity = NSUserActivity(activityType: type)
        currentActivity?.title = title
        currentActivity?.userInfo = userInfo
        currentActivity?.isEligibleForHandoff = true
        currentActivity?.becomeCurrent()
    }

    @objc public func updateActivity(userInfo: [String: Any]) {
        currentActivity?.addUserInfoEntries(from: userInfo)
        currentActivity?.needsSave = true
    }

    @objc public func stopActivity() {
        currentActivity?.invalidate()
        currentActivity = nil
    }
}
