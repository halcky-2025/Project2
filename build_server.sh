#!/bin/bash
# =============================================================================
# HopStar Server Build Script
# =============================================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}==================================${NC}"
echo -e "${GREEN}HopStar Server Build Script${NC}"
echo -e "${GREEN}==================================${NC}"
echo ""

# Check if we're in the project root
if [ ! -f "main7.cpp" ]; then
    echo -e "${RED}Error: main7.cpp not found. Please run this script from the project root.${NC}"
    exit 1
fi

# Parse command line arguments
ENABLE_TORCH=OFF
ENABLE_ONNX=OFF
ENABLE_FFMPEG=ON
ENABLE_GUI=ON
ENABLE_WEBSOCKET=OFF
ENABLE_WEBRTC=OFF
ENABLE_CURL=ON
BUILD_TYPE=Release

while [[ $# -gt 0 ]]; do
    case $1 in
        --torch)
            ENABLE_TORCH=ON
            shift
            ;;
        --onnx)
            ENABLE_ONNX=ON
            shift
            ;;
        --websocket)
            ENABLE_WEBSOCKET=ON
            shift
            ;;
        --webrtc)
            ENABLE_WEBRTC=ON
            shift
            ;;
        --no-ffmpeg)
            ENABLE_FFMPEG=OFF
            shift
            ;;
        --no-gui)
            ENABLE_GUI=OFF
            shift
            ;;
        --no-curl)
            ENABLE_CURL=OFF
            shift
            ;;
        --debug)
            BUILD_TYPE=Debug
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --torch       Enable PyTorch support"
            echo "  --onnx        Enable ONNX Runtime support"
            echo "  --websocket   Enable WebSocket signaling server"
            echo "  --webrtc      Enable WebRTC support (libdatachannel)"
            echo "  --no-ffmpeg   Disable FFmpeg support"
            echo "  --no-gui      Disable GUI support (headless only)"
            echo "  --no-curl     Disable libcurl (HTTPS support)"
            echo "  --debug       Build in debug mode"
            echo "  --help        Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo "Build configuration:"
echo "  Build type: $BUILD_TYPE"
echo "  PyTorch:    $ENABLE_TORCH"
echo "  ONNX:       $ENABLE_ONNX"
echo "  FFmpeg:     $ENABLE_FFMPEG"
echo "  GUI:        $ENABLE_GUI"
echo "  WebSocket:  $ENABLE_WEBSOCKET"
echo "  WebRTC:     $ENABLE_WEBRTC"
echo "  libcurl:    $ENABLE_CURL"
echo ""

# Create build directory
cd linux
mkdir -p build
cd build

echo -e "${YELLOW}Running CMake...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DHOPSTAR_ENABLE_TORCH=$ENABLE_TORCH \
    -DHOPSTAR_ENABLE_ONNX=$ENABLE_ONNX \
    -DHOPSTAR_ENABLE_FFMPEG=$ENABLE_FFMPEG \
    -DHOPSTAR_ENABLE_GUI=$ENABLE_GUI \
    -DHOPSTAR_ENABLE_WEBSOCKET=$ENABLE_WEBSOCKET \
    -DHOPSTAR_ENABLE_WEBRTC=$ENABLE_WEBRTC \
    -DHOPSTAR_ENABLE_CURL=$ENABLE_CURL

echo ""
echo -e "${YELLOW}Building HopStar_Server...${NC}"
make HopStar_Server -j$(nproc)

echo ""
echo -e "${GREEN}==================================${NC}"
echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}==================================${NC}"
echo ""
echo "Server binary location: linux/build/bin/HopStar_Server"
echo ""
echo "To run the server:"
echo "  cd linux/build/bin"
echo "  ./HopStar_Server [port]"
echo ""
echo "Example:"
echo "  ./HopStar_Server 8080"
echo ""
