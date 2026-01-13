# =============================================================================
# HopStar - Linux CMake Build System
# =============================================================================

cmake_minimum_required(VERSION 3.21)

project(HopStar
    VERSION 1.0.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source directory is parent
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps)

message(STATUS "Building for: Linux")
message(STATUS "Source directory: ${SRC_DIR}")
message(STATUS "Dependencies directory: ${DEPS_DIR}")

# =============================================================================
# Options
# =============================================================================
option(HOPSTAR_ENABLE_FFMPEG "Enable FFmpeg for media" ON)
option(HOPSTAR_ENABLE_TORCH "Enable LibTorch" OFF)

# =============================================================================
# Local Dependencies (built from source)
# =============================================================================

# SDL3 (local build)
set(SDL3_INCLUDE_DIR ${DEPS_DIR}/SDL3/include)
set(SDL3_LIBRARY ${DEPS_DIR}/SDL3/build/libSDL3.so)

# SDL3_ttf (local build)
set(SDL3_TTF_INCLUDE_DIR ${DEPS_DIR}/SDL3_ttf/include)
set(SDL3_TTF_LIBRARY ${DEPS_DIR}/SDL3_ttf/build/libSDL3_ttf.so)

# bgfx (local build)
set(BGFX_INCLUDE_DIR ${DEPS_DIR}/bgfx/include)
set(BX_INCLUDE_DIR ${DEPS_DIR}/bx/include)
set(BIMG_INCLUDE_DIR ${DEPS_DIR}/bimg/include)

# libcoro
set(LIBCORO_INCLUDE_DIR ${DEPS_DIR}/libcoro/include)
set(LIBCORO_GENERATED_DIR ${DEPS_DIR}/libcoro/build/include)
set(LIBCORO_LIBRARY ${DEPS_DIR}/libcoro/build/libcoro.a)

# nlohmann json
set(NLOHMANN_JSON_INCLUDE_DIR ${DEPS_DIR}/nlohmann_json/include)
set(BGFX_LIBRARY ${DEPS_DIR}/bgfx/.build/linux64_gcc/bin/libbgfxRelease.a)
set(BX_LIBRARY ${DEPS_DIR}/bgfx/.build/linux64_gcc/bin/libbxRelease.a)
set(BIMG_LIBRARY ${DEPS_DIR}/bgfx/.build/linux64_gcc/bin/libbimgRelease.a)
set(BIMG_DECODE_LIBRARY ${DEPS_DIR}/bgfx/.build/linux64_gcc/bin/libbimg_decodeRelease.a)

# =============================================================================
# System Dependencies
# =============================================================================

# SQLite3
find_package(SQLite3 REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# X11
find_package(X11 REQUIRED)

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
# Common Headers
# =============================================================================
set(COMMON_HEADERS
    ${SRC_DIR}/gc.h
    ${SRC_DIR}/arr.h
    ${SRC_DIR}/arr2.h
    ${SRC_DIR}/mem.h
    ${SRC_DIR}/elem.h
    ${SRC_DIR}/newelem.h
    ${SRC_DIR}/ugui.h
    ${SRC_DIR}/audio.h
    ${SRC_DIR}/db2.h
    ${SRC_DIR}/shader.h
    ${SRC_DIR}/shader2.h
    ${SRC_DIR}/imas.h
    ${SRC_DIR}/thumnailAtlas.h
    ${SRC_DIR}/opt.h
    ${SRC_DIR}/Compile.h
    ${SRC_DIR}/stb_image.h
    ${SRC_DIR}/stb_image_write.h
    ${SRC_DIR}/miniaudio.h
)

# =============================================================================
# Linux Target
# =============================================================================
set(LINUX_SOURCES
    ${SRC_DIR}/main5.cpp
)

add_executable(HopStar_Linux
    ${LINUX_SOURCES}
    ${COMMON_HEADERS}
)

target_include_directories(HopStar_Linux PRIVATE
    ${SRC_DIR}
    ${SDL3_INCLUDE_DIR}
    ${SDL3_TTF_INCLUDE_DIR}
    ${BGFX_INCLUDE_DIR}
    ${BX_INCLUDE_DIR}
    ${BIMG_INCLUDE_DIR}
    ${LIBCORO_INCLUDE_DIR}
    ${LIBCORO_GENERATED_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
)

# Find PortAudio
find_library(PORTAUDIO_LIBRARY portaudio)

# Link order matters for static libraries
target_link_libraries(HopStar_Linux PRIVATE
    ${SDL3_LIBRARY}
    ${SDL3_TTF_LIBRARY}
    ${BGFX_LIBRARY}
    ${BIMG_LIBRARY}
    ${BIMG_DECODE_LIBRARY}
    ${BX_LIBRARY}
    ${LIBCORO_LIBRARY}
    SQLite::SQLite3
    OpenGL::GL
    ${X11_LIBRARIES}
    ${PORTAUDIO_LIBRARY}
    pthread
    dl
    rt
)

# FFmpeg
if(HOPSTAR_ENABLE_FFMPEG AND AVCODEC_LIBRARY)
    target_link_libraries(HopStar_Linux PRIVATE
        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWSCALE_LIBRARY}
        ${SWRESAMPLE_LIBRARY}
    )
    target_compile_definitions(HopStar_Linux PRIVATE HOPSTAR_ENABLE_FFMPEG)
endif()

# Torch
if(HOPSTAR_ENABLE_TORCH)
    find_package(Torch REQUIRED)
    target_link_libraries(HopStar_Linux PRIVATE ${TORCH_LIBRARIES})
    target_compile_definitions(HopStar_Linux PRIVATE HOPSTAR_ENABLE_TORCH)
endif()

# Debug/Release flags
target_compile_options(HopStar_Linux PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O2>
)

# bx requires BX_CONFIG_DEBUG to be defined
target_compile_definitions(HopStar_Linux PRIVATE
    $<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>
    $<$<CONFIG:Release>:BX_CONFIG_DEBUG=0>
)

set_target_properties(HopStar_Linux PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Set RPATH to find shared libraries
set_target_properties(HopStar_Linux PROPERTIES
    BUILD_RPATH "${DEPS_DIR}/SDL3/build"
    INSTALL_RPATH "$ORIGIN/../lib"
)

# =============================================================================
# Installation
# =============================================================================
install(TARGETS HopStar_Linux
    RUNTIME DESTINATION bin
)

# =============================================================================
# CPack Configuration for .deb package
# =============================================================================
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "hopstar")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HopStar Application")
set(CPACK_PACKAGE_MAINTAINER "HopStar Team")
set(CPACK_PACKAGE_CONTACT "hopstar@example.com")

# Debian package dependencies - these will be auto-installed
# Includes both X11 and Wayland dependencies for maximum compatibility
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libfreetype6, libharfbuzz0b, libavcodec60|libavcodec59|libavcodec58, libavformat60|libavformat59|libavformat58, libavutil58|libavutil57|libavutil56, libswscale7|libswscale6|libswscale5, libswresample4|libswresample3, libsqlite3-0, libgl1, libx11-6, libxext6, libxrandr2, libxcursor1, libxi6, libxss1, libxtst6, libasound2, libwayland-client0, libwayland-cursor0, libwayland-egl1, libxkbcommon0, libdecor-0-0|libdecor-0-plugin-1-gtk"
)

set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# Include bundled shared libraries
install(FILES
    ${SDL3_LIBRARY}
    ${SDL3_TTF_LIBRARY}
    DESTINATION lib
)

# Create symlinks for shared libraries
install(CODE "
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink libSDL3.so.0.5.0 libSDL3.so.0 WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}/lib)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink libSDL3.so.0 libSDL3.so WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}/lib)
")

include(CPack)

# =============================================================================
# Summary
# =============================================================================
message(STATUS "")
message(STATUS "=== HopStar Linux Build Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "SDL3: ${SDL3_LIBRARY}")
message(STATUS "bgfx: ${BGFX_LIBRARY}")
message(STATUS "FFmpeg: ${HOPSTAR_ENABLE_FFMPEG}")
message(STATUS "")
message(STATUS "To create .deb package: cmake --build build --target package")
message(STATUS "=========================================")
message(STATUS "")
