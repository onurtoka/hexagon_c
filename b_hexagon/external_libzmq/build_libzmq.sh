#!/bin/bash

# External libzmq Compilation Script
# This script builds libzmq independently from the main project
# Run this before building the main b_hexagon project

set -e  # Exit on any error

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
LIBZMQ_SOURCE_DIR="$PROJECT_ROOT/b_hexagon/third_party/libzmq"
LIBZMQ_BUILD_DIR="$SCRIPT_DIR/build"
LIBZMQ_INSTALL_DIR="$SCRIPT_DIR/install"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== External libzmq Compilation Script ===${NC}"
echo -e "${YELLOW}Source Directory: $LIBZMQ_SOURCE_DIR${NC}"
echo -e "${YELLOW}Build Directory: $LIBZMQ_BUILD_DIR${NC}"
echo -e "${YELLOW}Install Directory: $LIBZMQ_INSTALL_DIR${NC}"

# Check if libzmq source exists
if [ ! -d "$LIBZMQ_SOURCE_DIR" ]; then
    echo -e "${RED}Error: libzmq source directory not found at $LIBZMQ_SOURCE_DIR${NC}"
    echo -e "${YELLOW}Please run: git submodule update --init --recursive${NC}"
    exit 1
fi

# Clean previous builds if requested
if [ "$1" = "clean" ]; then
    echo -e "${YELLOW}Cleaning previous builds...${NC}"
    rm -rf "$LIBZMQ_BUILD_DIR" "$LIBZMQ_INSTALL_DIR"
fi

# Create build directories
mkdir -p "$LIBZMQ_BUILD_DIR"
mkdir -p "$LIBZMQ_INSTALL_DIR"

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo -e "${RED}cmake is required but not installed.${NC}"; exit 1; }
command -v make >/dev/null 2>&1 || { echo -e "${RED}make is required but not installed.${NC}"; exit 1; }

echo -e "${GREEN}Step 1: Configuring libzmq with CMake...${NC}"
cd "$LIBZMQ_BUILD_DIR"

# Configure libzmq with CMake
# Enable DRAFT APIs for RADIO/DISH pattern used in your project
cmake "$LIBZMQ_SOURCE_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$LIBZMQ_INSTALL_DIR" \
    -DBUILD_SHARED_LIBS=ON \
    -DBUILD_STATIC_LIBS=ON \
    -DBUILD_TESTS=OFF \
    -DENABLE_DRAFTS=ON \
    -DENABLE_CURVE=OFF \
    -DWITH_PERF_TOOL=OFF \
    -DWITH_DOCS=OFF

echo -e "${GREEN}Step 2: Building libzmq...${NC}"
make -j$(nproc)

echo -e "${GREEN}Step 3: Installing libzmq...${NC}"
make install

# Create pkg-config file for easy integration
echo -e "${GREEN}Step 4: Creating pkg-config file...${NC}"
PKG_CONFIG_DIR="$LIBZMQ_INSTALL_DIR/lib/pkgconfig"
mkdir -p "$PKG_CONFIG_DIR"

cat > "$PKG_CONFIG_DIR/libzmq-external.pc" << EOF
prefix=$LIBZMQ_INSTALL_DIR
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: libzmq-external
Description: The ZeroMQ messaging library (external build)
Version: $(cat "$LIBZMQ_SOURCE_DIR/VERSION" 2>/dev/null || echo "4.3.5")
Libs: -L\${libdir} -lzmq
Libs.private: -lpthread -lrt -ldl
Cflags: -I\${includedir}
EOF

# Create environment setup script
echo -e "${GREEN}Step 5: Creating environment setup script...${NC}"
cat > "$SCRIPT_DIR/setup_env.sh" << 'EOF'
#!/bin/bash
# Source this script to set up environment for external libzmq

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LIBZMQ_INSTALL_DIR="$SCRIPT_DIR/install"

export PKG_CONFIG_PATH="$LIBZMQ_INSTALL_DIR/lib/pkgconfig:$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="$LIBZMQ_INSTALL_DIR/lib:$LD_LIBRARY_PATH"
export CMAKE_PREFIX_PATH="$LIBZMQ_INSTALL_DIR:$CMAKE_PREFIX_PATH"

echo "Environment set up for external libzmq:"
echo "  Install directory: $LIBZMQ_INSTALL_DIR"
echo "  To use in your project:"
echo "    export PKG_CONFIG_PATH=$LIBZMQ_INSTALL_DIR/lib/pkgconfig:\$PKG_CONFIG_PATH"
echo "    pkg-config --cflags --libs libzmq-external"
EOF

chmod +x "$SCRIPT_DIR/setup_env.sh"

echo -e "${GREEN}=== libzmq External Build Complete! ===${NC}"
echo -e "${YELLOW}Installation location: $LIBZMQ_INSTALL_DIR${NC}"
echo -e "${YELLOW}Library: $LIBZMQ_INSTALL_DIR/lib/libzmq.so${NC}"
echo -e "${YELLOW}Headers: $LIBZMQ_INSTALL_DIR/include/zmq.h${NC}"
echo ""
echo -e "${BLUE}To use this external libzmq build:${NC}"
echo -e "  1. Source the environment: source $SCRIPT_DIR/setup_env.sh"
echo -e "  2. Use pkg-config: pkg-config --cflags --libs libzmq-external"
echo -e "  3. Or set CMAKE_PREFIX_PATH to: $LIBZMQ_INSTALL_DIR"
echo ""
echo -e "${GREEN}Build Summary:${NC}"
echo -e "  ✓ Shared library: $(ls "$LIBZMQ_INSTALL_DIR"/lib/libzmq.so* 2>/dev/null | head -1 || echo 'Not found')"
echo -e "  ✓ Static library: $(ls "$LIBZMQ_INSTALL_DIR"/lib/libzmq.a 2>/dev/null || echo 'Not found')"
echo -e "  ✓ Headers: $(ls "$LIBZMQ_INSTALL_DIR"/include/zmq*.h 2>/dev/null | wc -l) files"
echo -e "  ✓ DRAFT APIs: Enabled (includes RADIO/DISH pattern)"
