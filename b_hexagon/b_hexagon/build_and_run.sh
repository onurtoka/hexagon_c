#!/bin/bash

# B_Hexagon Build and Run Script
# This script compiles and runs the b_hexagon application without rebuilding libzmq

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== B_Hexagon Build and Run Script ===${NC}"

# Project paths
PROJECT_DIR="/workspaces/hexagon_c/b_hexagon/b_hexagon"
LIBZMQ_INCLUDE="/workspaces/hexagon_c/hexagon_c/libzmq/include"
LIBZMQ_LIB="/workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a"
APP_NAME="b_hexagon_app"

# Check if libzmq.a exists
if [ ! -f "$LIBZMQ_LIB" ]; then
    echo -e "${RED}Error: libzmq.a not found at $LIBZMQ_LIB${NC}"
    echo -e "${YELLOW}Please run the following commands first:${NC}"
    echo "cd /workspaces/hexagon_c/hexagon_c/libzmq"
    echo "cmake -B build -DENABLE_DRAFTS=ON -DBUILD_TESTS=OFF -DZMQ_BUILD_TESTS=OFF"
    echo "make -C build -j\$(nproc)"
    exit 1
fi

echo -e "${GREEN}✓ libzmq.a found${NC}"

# Change to project directory
cd "$PROJECT_DIR"

# Clean previous build
if [ -f "$APP_NAME" ]; then
    echo -e "${YELLOW}Removing previous build...${NC}"
    rm -f "$APP_NAME"
fi

echo -e "${BLUE}Compiling b_hexagon application...${NC}"

# Compile with optimizations
g++ -std=c++17 \
    -Isrc \
    -I"$LIBZMQ_INCLUDE" \
    -DZMQ_BUILD_DRAFT_API \
    -pthread \
    -O3 \
    -march=native \
    -mtune=native \
    -flto \
    -ffast-math \
    -DNDEBUG \
    src/application/main.cpp \
    src/domain/logic/CalculatorService.cpp \
    src/domain/model/DelayCalculatedTrackData.cpp \
    src/domain/model/TrackData.cpp \
    src/adapters/outgoing/ZeroMQDataSender.cpp \
    src/adapters/incoming/ZeroMQDataHandler.cpp \
    src/messaging/ContractManager.cpp \
    src/messaging/messaging.cpp \
    src/common/JsonConfigReader.cpp \
    "$LIBZMQ_LIB" \
    -lsodium \
    -lrt \
    -lgnutls \
    -o "$APP_NAME"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Compilation successful!${NC}"
    echo -e "${BLUE}Running b_hexagon application...${NC}"
    echo -e "${YELLOW}Press Ctrl+C to stop${NC}"
    echo ""
    
    # Run the application
    ./"$APP_NAME"
else
    echo -e "${RED}✗ Compilation failed!${NC}"
    exit 1
fi
