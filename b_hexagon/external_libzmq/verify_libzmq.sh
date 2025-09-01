#!/bin/bash

# External libzmq Verification Script
# Verifies that the external libzmq build is working correctly

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LIBZMQ_INSTALL_DIR="$SCRIPT_DIR/install"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}=== External libzmq Verification Script ===${NC}"

# Check if libzmq is built
if [ ! -d "$LIBZMQ_INSTALL_DIR" ]; then
    echo -e "${RED}Error: libzmq not built yet. Run ./build_libzmq.sh first${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Installation directory exists${NC}"

# Check library files
if [ -f "$LIBZMQ_INSTALL_DIR/lib/libzmq.so" ]; then
    echo -e "${GREEN}✓ Shared library found${NC}"
    ls -la "$LIBZMQ_INSTALL_DIR"/lib/libzmq.so*
else
    echo -e "${RED}✗ Shared library not found${NC}"
fi

if [ -f "$LIBZMQ_INSTALL_DIR/lib/libzmq.a" ]; then
    echo -e "${GREEN}✓ Static library found${NC}"
    ls -la "$LIBZMQ_INSTALL_DIR/lib/libzmq.a"
else
    echo -e "${YELLOW}⚠ Static library not found (optional)${NC}"
fi

# Check headers
if [ -f "$LIBZMQ_INSTALL_DIR/include/zmq.h" ]; then
    echo -e "${GREEN}✓ Header files found${NC}"
    echo "  Headers: $(ls "$LIBZMQ_INSTALL_DIR"/include/zmq*.h | wc -l) files"
else
    echo -e "${RED}✗ Header files not found${NC}"
fi

# Check pkg-config
PKG_CONFIG_FILE="$LIBZMQ_INSTALL_DIR/lib/pkgconfig/libzmq-external.pc"
if [ -f "$PKG_CONFIG_FILE" ]; then
    echo -e "${GREEN}✓ pkg-config file found${NC}"
else
    echo -e "${RED}✗ pkg-config file not found${NC}"
fi

# Test compilation
echo -e "${BLUE}Testing compilation with external libzmq...${NC}"

# Create a simple test program
TEST_DIR="$SCRIPT_DIR/test"
mkdir -p "$TEST_DIR"

cat > "$TEST_DIR/test_external_zmq.cpp" << 'EOF'
#define ZMQ_BUILD_DRAFT_API 1
#include <zmq.h>
#include <iostream>
#include <string>

int main() {
    // Test basic ZMQ functionality
    void *context = zmq_ctx_new();
    if (!context) {
        std::cerr << "Failed to create ZMQ context" << std::endl;
        return 1;
    }
    
    void *socket = zmq_socket(context, ZMQ_REQ);
    if (!socket) {
        std::cerr << "Failed to create ZMQ socket" << std::endl;
        zmq_ctx_destroy(context);
        return 1;
    }
    
    // Test DRAFT API (RADIO/DISH pattern)
    void *radio = zmq_socket(context, ZMQ_RADIO);
    void *dish = zmq_socket(context, ZMQ_DISH);
    
    if (radio && dish) {
        std::cout << "✓ DRAFT APIs (RADIO/DISH) are available" << std::endl;
        zmq_close(radio);
        zmq_close(dish);
    } else {
        std::cout << "⚠ DRAFT APIs not available (this is expected in some builds)" << std::endl;
    }
    
    // Get version
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    std::cout << "✓ ZeroMQ version: " << major << "." << minor << "." << patch << std::endl;
    
    // Cleanup
    zmq_close(socket);
    zmq_ctx_destroy(context);
    
    std::cout << "✓ External libzmq test completed successfully!" << std::endl;
    return 0;
}
EOF

# Compile test
echo -e "${YELLOW}Compiling test program...${NC}"
export PKG_CONFIG_PATH="$LIBZMQ_INSTALL_DIR/lib/pkgconfig:$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="$LIBZMQ_INSTALL_DIR/lib:$LD_LIBRARY_PATH"

if g++ -std=c++17 \
    $(pkg-config --cflags libzmq-external) \
    "$TEST_DIR/test_external_zmq.cpp" \
    $(pkg-config --libs libzmq-external) \
    -o "$TEST_DIR/test_external_zmq"; then
    echo -e "${GREEN}✓ Compilation successful${NC}"
else
    echo -e "${RED}✗ Compilation failed${NC}"
    exit 1
fi

# Run test
echo -e "${YELLOW}Running test program...${NC}"
if "$TEST_DIR/test_external_zmq"; then
    echo -e "${GREEN}✓ Test execution successful${NC}"
else
    echo -e "${RED}✗ Test execution failed${NC}"
    exit 1
fi

# Clean up test
rm -rf "$TEST_DIR"

echo -e "${GREEN}=== External libzmq Verification Complete! ===${NC}"
echo -e "${BLUE}All tests passed. External libzmq is ready for use.${NC}"
