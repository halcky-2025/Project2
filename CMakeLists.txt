# =============================================================================
# HopStar - Cross-Platform CMake Build System
# =============================================================================
# Supports: Windows, Android, macOS, iOS
# =============================================================================

cmake_minimum_required(VERSION 3.21)

# =============================================================================
# Project Configuration
# =============================================================================
project(HopStar
    VERSION 1.0.0
    LANGUAGES C CXX OBJC OBJCXX Swift
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 20)

# =============================================================================
# Platform Detection
# =============================================================================
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_OSX_SYSROOT MATCHES ".*iphoneos.*" OR CMAKE_OSX_SYSROOT MATCHES ".*iphonesimulator.*")
        set(PLATFORM_IOS TRUE)
        set(PLATFORM_NAME "iOS")
    else()
        set(PLATFORM_MACOS TRUE)
        set(PLATFORM_NAME "macOS")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_WINDOWS TRUE)
    set(PLATFORM_NAME "Windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_ANDROID TRUE)
    set(PLATFORM_NAME "Android")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

message(STATUS "Building for: ${PLATFORM_NAME}")

# =============================================================================
# Options
# =============================================================================
option(HOPSTAR_ENABLE_METAL "Enable Metal rendering (Apple only)" ON)
option(HOPSTAR_ENABLE_COREML "Enable Core ML (Apple only)" ON)
option(HOPSTAR_ENABLE_ARKIT "Enable ARKit (iOS only)" ON)
option(HOPSTAR_ENABLE_FFMPEG "Enable FFmpeg for media" ON)
option(HOPSTAR_ENABLE_TORCH "Enable LibTorch (Desktop only)" OFF)

# =============================================================================
# Find Dependencies
# =============================================================================

# SDL3
find_package(SDL3 REQUIRED)

# bgfx (assumes installed via vcpkg or manual)
find_path(BGFX_INCLUDE_DIR bgfx/bgfx.h)
find_library(BGFX_LIBRARY bgfx)
find_library(BX_LIBRARY bx)
find_library(BIMG_LIBRARY bimg)

# SQLite3
find_package(SQLite3 REQUIRED)

# FFmpeg (optional)
if(HOPSTAR_ENABLE_FFMPEG)
    find_path(FFMPEG_INCLUDE_DIR libavcodec/avcodec.h)
    find_library(AVCODEC_LIBRARY avcodec)
    find_library(AVFORMAT_LIBRARY avformat)
    find_library(AVUTIL_LIBRARY avutil)
    find_library(SWSCALE_LIBRARY swscale)
    find_library(SWRESAMPLE_LIBRARY swresample)
endif()

# =============================================================================
# Common Source Files
# =============================================================================
set(COMMON_HEADERS
    gc.h
    arr.h
    arr2.h
    mem.h
    elem.h
    newelem.h
    ugui.h
    audio.h
    db2.h
    shader.h
    shader2.h
    imas.h
    thumnailAtlas.h
    opt.h
    Compile.h
    stb_image.h
    stb_image_write.h
    miniaudio.h
)

# =============================================================================
# macOS Target
# =============================================================================
if(PLATFORM_MACOS)
    message(STATUS "Configuring macOS target")

    set(MACOS_SOURCES
        main4.cpp
        platform/apple/AppleBridge.mm
        platform/apple/SwiftFeatures.swift
    )

    add_executable(HopStar_macOS MACOSX_BUNDLE
        ${MACOS_SOURCES}
        ${COMMON_HEADERS}
    )

    target_include_directories(HopStar_macOS PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/apple
        ${BGFX_INCLUDE_DIR}
        ${FFMPEG_INCLUDE_DIR}
    )

    target_link_libraries(HopStar_macOS PRIVATE
        SDL3::SDL3
        ${BGFX_LIBRARY}
        ${BX_LIBRARY}
        ${BIMG_LIBRARY}
        SQLite::SQLite3
    )

    if(HOPSTAR_ENABLE_FFMPEG)
        target_link_libraries(HopStar_macOS PRIVATE
            ${AVCODEC_LIBRARY}
            ${AVFORMAT_LIBRARY}
            ${AVUTIL_LIBRARY}
            ${SWSCALE_LIBRARY}
            ${SWRESAMPLE_LIBRARY}
        )
        target_compile_definitions(HopStar_macOS PRIVATE HOPSTAR_ENABLE_FFMPEG)
    endif()

    # Apple frameworks
    target_link_libraries(HopStar_macOS PRIVATE
        "-framework Foundation"
        "-framework AppKit"
        "-framework Metal"
        "-framework MetalKit"
        "-framework CoreML"
        "-framework Vision"
        "-framework AVFoundation"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework Photos"
        "-framework UserNotifications"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework CoreServices"
        "-framework Security"
    )

    # macOS deployment target
    set_target_properties(HopStar_macOS PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.hopstar.editor"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/platform/apple/Info-macOS.plist"
        OSX_DEPLOYMENT_TARGET "12.0"
        XCODE_ATTRIBUTE_SWIFT_VERSION "5.0"
        XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES "YES"
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "YES"
    )

    # Swift bridging
    set_property(TARGET HopStar_macOS PROPERTY
        XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/apple/AppleBridge.h"
    )

endif()

# =============================================================================
# iOS Target
# =============================================================================
if(PLATFORM_IOS)
    message(STATUS "Configuring iOS target")

    set(IOS_SOURCES
        main3.cpp
        platform/apple/AppleBridge.mm
        platform/apple/SwiftFeatures.swift
    )

    add_executable(HopStar_iOS MACOSX_BUNDLE
        ${IOS_SOURCES}
        ${COMMON_HEADERS}
    )

    target_include_directories(HopStar_iOS PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/apple
        ${BGFX_INCLUDE_DIR}
        ${FFMPEG_INCLUDE_DIR}
    )

    target_link_libraries(HopStar_iOS PRIVATE
        SDL3::SDL3
        ${BGFX_LIBRARY}
        ${BX_LIBRARY}
        ${BIMG_LIBRARY}
        SQLite::SQLite3
    )

    if(HOPSTAR_ENABLE_FFMPEG)
        target_link_libraries(HopStar_iOS PRIVATE
            ${AVCODEC_LIBRARY}
            ${AVFORMAT_LIBRARY}
            ${AVUTIL_LIBRARY}
            ${SWSCALE_LIBRARY}
            ${SWRESAMPLE_LIBRARY}
        )
        target_compile_definitions(HopStar_iOS PRIVATE HOPSTAR_ENABLE_FFMPEG)
    endif()

    # Apple frameworks (iOS)
    target_link_libraries(HopStar_iOS PRIVATE
        "-framework Foundation"
        "-framework UIKit"
        "-framework Metal"
        "-framework MetalKit"
        "-framework CoreML"
        "-framework Vision"
        "-framework AVFoundation"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework Photos"
        "-framework UserNotifications"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework CoreServices"
        "-framework Security"
    )

    if(HOPSTAR_ENABLE_ARKIT)
        target_link_libraries(HopStar_iOS PRIVATE "-framework ARKit")
        target_compile_definitions(HopStar_iOS PRIVATE HOPSTAR_ENABLE_ARKIT)
    endif()

    # iOS deployment target
    set_target_properties(HopStar_iOS PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.hopstar.editor.ios"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/platform/apple/Info-iOS.plist"
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "15.0"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"  # iPhone and iPad
        XCODE_ATTRIBUTE_SWIFT_VERSION "5.0"
        XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES "YES"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""  # Set your team ID
    )

    # Swift bridging
    set_property(TARGET HopStar_iOS PROPERTY
        XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/apple/AppleBridge.h"
    )

endif()

# =============================================================================
# Windows Target (Reference)
# =============================================================================
if(PLATFORM_WINDOWS)
    message(STATUS "Configuring Windows target")

    set(WINDOWS_SOURCES
        main.cpp
    )

    add_executable(HopStar_Windows WIN32
        ${WINDOWS_SOURCES}
        ${COMMON_HEADERS}
    )

    target_include_directories(HopStar_Windows PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${BGFX_INCLUDE_DIR}
        ${FFMPEG_INCLUDE_DIR}
    )

    target_link_libraries(HopStar_Windows PRIVATE
        SDL3::SDL3
        ${BGFX_LIBRARY}
        ${BX_LIBRARY}
        ${BIMG_LIBRARY}
        SQLite::SQLite3
    )

    if(HOPSTAR_ENABLE_FFMPEG)
        target_link_libraries(HopStar_Windows PRIVATE
            ${AVCODEC_LIBRARY}
            ${AVFORMAT_LIBRARY}
            ${AVUTIL_LIBRARY}
            ${SWSCALE_LIBRARY}
            ${SWRESAMPLE_LIBRARY}
        )
    endif()

    if(HOPSTAR_ENABLE_TORCH)
        find_package(Torch REQUIRED)
        target_link_libraries(HopStar_Windows PRIVATE ${TORCH_LIBRARIES})
        target_compile_definitions(HopStar_Windows PRIVATE HOPSTAR_ENABLE_TORCH)
    endif()

endif()

# =============================================================================
# Android Target (Reference)
# =============================================================================
if(PLATFORM_ANDROID)
    message(STATUS "Configuring Android target")

    set(ANDROID_SOURCES
        main2.cpp
    )

    add_library(HopStar_Android SHARED
        ${ANDROID_SOURCES}
        ${COMMON_HEADERS}
    )

    target_include_directories(HopStar_Android PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${BGFX_INCLUDE_DIR}
    )

    target_link_libraries(HopStar_Android PRIVATE
        SDL3::SDL3
        ${BGFX_LIBRARY}
        ${BX_LIBRARY}
        ${BIMG_LIBRARY}
        SQLite::SQLite3
        android
        log
    )

endif()

# =============================================================================
# Installation
# =============================================================================
if(PLATFORM_MACOS)
    install(TARGETS HopStar_macOS
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
    )
endif()

if(PLATFORM_IOS)
    install(TARGETS HopStar_iOS
        BUNDLE DESTINATION .
    )
endif()

# =============================================================================
# Summary
# =============================================================================
message(STATUS "")
message(STATUS "=== HopStar Build Configuration ===")
message(STATUS "Platform: ${PLATFORM_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "FFmpeg: ${HOPSTAR_ENABLE_FFMPEG}")
if(PLATFORM_MACOS OR PLATFORM_IOS)
    message(STATUS "Metal: ${HOPSTAR_ENABLE_METAL}")
    message(STATUS "Core ML: ${HOPSTAR_ENABLE_COREML}")
endif()
if(PLATFORM_IOS)
    message(STATUS "ARKit: ${HOPSTAR_ENABLE_ARKIT}")
endif()
message(STATUS "===================================")
message(STATUS "")
