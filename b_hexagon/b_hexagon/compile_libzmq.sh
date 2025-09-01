#!/bin/bash

# Script to compile and install libzmq with draft API support
# Usage: ./compile_libzmq.sh [install_prefix]

set -e  # Exit on any error

# Default installation prefix
INSTALL_PREFIX="${1:-/opt/zmq}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(nproc)}"

echo "=== libzmq Compilation Script ==="
echo "Install prefix: $INSTALL_PREFIX"
echo "Build type: $BUILD_TYPE"
echo "Parallel jobs: $JOBS"
echo

# Check if we're in the b_hexagon project directory
if [ ! -d "third_party/libzmq" ]; then
    echo "Error: This script should be run from the b_hexagon project root directory"
    echo "Expected to find: third_party/libzmq"
    exit 1
fi

# Navigate to vendored libzmq
cd third_party/libzmq

# Clean any previous build
if [ -d "build" ]; then
    echo "Cleaning previous build..."
    rm -rf build
fi

# Create build directory
mkdir build
cd build

echo "Configuring libzmq with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DENABLE_DRAFTS=ON \
    -DBUILD_TESTS=OFF \
    -DWITH_PERF_TOOL=OFF \
    -DBUILD_STATIC=OFF \
    -DBUILD_SHARED=ON \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON

echo "Compiling libzmq..."
make -j"$JOBS"

echo "Installing libzmq to $INSTALL_PREFIX..."
if [ "$EUID" -eq 0 ]; then
    # Running as root
    make install
else
    # Not root, try sudo
    sudo make install
fi

# Update library cache
echo "Updating library cache..."
if [ -f "/etc/ld.so.conf.d/zmq.conf" ]; then
    sudo sh -c "echo '$INSTALL_PREFIX/lib' > /etc/ld.so.conf.d/zmq.conf"
fi
sudo ldconfig

# Create pkg-config file if it doesn't exist
PKGCONFIG_DIR="$INSTALL_PREFIX/lib/pkgconfig"
if [ ! -f "$PKGCONFIG_DIR/libzmq.pc" ] && [ -f "../src/libzmq.pc.in" ]; then
    echo "Creating pkg-config file..."
    sudo mkdir -p "$PKGCONFIG_DIR"
    
    # Generate pkg-config file
    ZMQ_VERSION=$(grep "^set(ZMQ_VERSION_MAJOR" ../CMakeLists.txt | sed 's/[^0-9]*//g')
    if [ -z "$ZMQ_VERSION" ]; then
        ZMQ_VERSION="4.3.4"  # fallback version
    fi
    
    sudo tee "$PKGCONFIG_DIR/libzmq.pc" > /dev/null <<EOF
prefix=$INSTALL_PREFIX
exec_prefix=\${prefix}
libdir=\${prefix}/lib
includedir=\${prefix}/include

Name: libzmq
Description: 0MQ messaging library
Version: $ZMQ_VERSION
Libs: -L\${libdir} -lzmq
Libs.private: -lstdc++ -lpthread
Cflags: -I\${includedir} -DZMQ_BUILD_DRAFT_API
EOF
fi

echo
echo "=== Installation Complete ==="
echo "libzmq installed to: $INSTALL_PREFIX"
echo "Include directory: $INSTALL_PREFIX/include"
echo "Library directory: $INSTALL_PREFIX/lib"
echo
echo "To use external libzmq in your build:"
echo "  export PKG_CONFIG_PATH=\"$INSTALL_PREFIX/lib/pkgconfig:\$PKG_CONFIG_PATH\""
echo "  cmake .. # (USE_VENDORED_ZMQ=OFF is now default)"
echo
echo "Or set paths manually:"
echo "  cmake .. -DCMAKE_PREFIX_PATH=\"$INSTALL_PREFIX\""
echo

# Verify installation
echo "Verifying installation..."
if [ -f "$INSTALL_PREFIX/lib/libzmq.so" ] && [ -f "$INSTALL_PREFIX/include/zmq.h" ]; then
    echo "✓ libzmq files found"
    
    # Check for draft API symbols
    if nm -D "$INSTALL_PREFIX/lib/libzmq.so" | grep -q "zmq_radio\|zmq_dish"; then
        echo "✓ Draft API symbols found (RADIO/DISH support available)"
    else
        echo "⚠ Draft API symbols not found - RADIO/DISH may not be available"
    fi
    
    # Test pkg-config
    if pkg-config --exists libzmq; then
        echo "✓ pkg-config working: $(pkg-config --modversion libzmq)"
    else
        echo "⚠ pkg-config not working - you may need to set PKG_CONFIG_PATH"
    fi
else
    echo "✗ Installation verification failed"
    exit 1
fi

echo
echo "libzmq compilation and installation successful!"
