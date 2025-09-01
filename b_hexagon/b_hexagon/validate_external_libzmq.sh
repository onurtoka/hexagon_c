#!/bin/bash

# Script to validate external libzmq installation and b_hexagon build configuration
# Usage: ./validate_external_libzmq.sh [libzmq_prefix]

LIBZMQ_PREFIX="${1:-/opt/zmq}"

echo "=== External libzmq Validation ==="
echo "Checking libzmq installation at: $LIBZMQ_PREFIX"
echo

# Check if libzmq files exist
echo "1. Checking libzmq files..."
if [ -f "$LIBZMQ_PREFIX/lib/libzmq.so" ]; then
    echo "✓ libzmq.so found at $LIBZMQ_PREFIX/lib/libzmq.so"
else
    echo "✗ libzmq.so not found at $LIBZMQ_PREFIX/lib/"
    echo "   Try running: ./compile_libzmq.sh $LIBZMQ_PREFIX"
    exit 1
fi

if [ -f "$LIBZMQ_PREFIX/include/zmq.h" ]; then
    echo "✓ zmq.h found at $LIBZMQ_PREFIX/include/zmq.h"
else
    echo "✗ zmq.h not found at $LIBZMQ_PREFIX/include/"
    exit 1
fi

# Check for C++ header
if [ -f "$LIBZMQ_PREFIX/include/zmq.hpp" ]; then
    echo "✓ zmq.hpp found at $LIBZMQ_PREFIX/include/zmq.hpp"
else
    echo "⚠ zmq.hpp not found - using project vendored copy"
fi

# Check for draft API symbols
echo
echo "2. Checking draft API support..."
if nm -D "$LIBZMQ_PREFIX/lib/libzmq.so" 2>/dev/null | grep -q "zmq_radio\|zmq_dish"; then
    echo "✓ Draft API symbols found (RADIO/DISH support available)"
else
    echo "✗ Draft API symbols not found"
    echo "   libzmq may not be compiled with ENABLE_DRAFTS=ON"
    echo "   Try recompiling: ./compile_libzmq.sh $LIBZMQ_PREFIX"
    exit 1
fi

# Check pkg-config
echo
echo "3. Checking pkg-config..."
if command -v pkg-config >/dev/null 2>&1; then
    if pkg-config --exists libzmq; then
        VERSION=$(pkg-config --modversion libzmq)
        echo "✓ pkg-config working: libzmq version $VERSION"
    else
        echo "⚠ pkg-config doesn't find libzmq"
        echo "   Try: export PKG_CONFIG_PATH=\"$LIBZMQ_PREFIX/lib/pkgconfig:\$PKG_CONFIG_PATH\""
    fi
else
    echo "⚠ pkg-config not found"
    echo "   Install with: sudo apt-get install pkg-config"
fi

# Test compilation with external libzmq
echo
echo "4. Testing b_hexagon compilation..."
if [ ! -f "CMakeLists.txt" ]; then
    echo "✗ Not in b_hexagon project directory"
    echo "   Run this script from the project root directory"
    exit 1
fi

# Create temporary build directory
BUILD_DIR="build_validation_$$"
mkdir -p "$BUILD_DIR"

# Set environment variables for CMake
export PKG_CONFIG_PATH="$LIBZMQ_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"
export CMAKE_PREFIX_PATH="$LIBZMQ_PREFIX:$CMAKE_PREFIX_PATH"

echo "  Configuring with external libzmq..."
if cmake -S . -B "$BUILD_DIR" -DUSE_VENDORED_ZMQ=OFF >/dev/null 2>&1; then
    echo "✓ CMake configuration successful"
    
    echo "  Building b_hexagon_app..."
    if cmake --build "$BUILD_DIR" --target b_hexagon_app >/dev/null 2>&1; then
        echo "✓ Build successful"
        
        # Check if the binary links correctly
        if ldd "$BUILD_DIR/b_hexagon_app" | grep -q libzmq; then
            echo "✓ Application correctly linked to libzmq"
        else
            echo "⚠ libzmq not found in application dependencies"
        fi
    else
        echo "✗ Build failed"
        echo "   Check build output with: cmake --build $BUILD_DIR --target b_hexagon_app"
    fi
else
    echo "✗ CMake configuration failed"
    echo "   Check configuration with: cmake -S . -B $BUILD_DIR -DUSE_VENDORED_ZMQ=OFF"
fi

# Cleanup
rm -rf "$BUILD_DIR"

echo
echo "5. Summary..."
echo "libzmq installation: $LIBZMQ_PREFIX"
echo "Include path: $LIBZMQ_PREFIX/include"
echo "Library path: $LIBZMQ_PREFIX/lib"
echo
echo "To use external libzmq in your builds:"
echo "  export PKG_CONFIG_PATH=\"$LIBZMQ_PREFIX/lib/pkgconfig:\$PKG_CONFIG_PATH\""
echo "  export CMAKE_PREFIX_PATH=\"$LIBZMQ_PREFIX:\$CMAKE_PREFIX_PATH\""
echo "  cmake -S . -B build -DUSE_VENDORED_ZMQ=OFF"
echo
echo "Validation complete!"
