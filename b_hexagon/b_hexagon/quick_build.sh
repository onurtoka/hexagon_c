#!/bin/bash

# Quick Build Script for B_Hexagon (Debug Mode)
# Fast compilation without optimizations for development

PROJECT_DIR="/workspaces/hexagon_c/b_hexagon/b_hexagon"
LIBZMQ_INCLUDE="/workspaces/hexagon_c/hexagon_c/libzmq/include"
LIBZMQ_LIB="/workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a"

cd "$PROJECT_DIR"

echo "🔨 Quick building b_hexagon (debug mode)..."

g++ -std=c++17 \
    -Isrc \
    -I"$LIBZMQ_INCLUDE" \
    -DZMQ_BUILD_DRAFT_API \
    -pthread \
    -g \
    -O0 \
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
    -o b_hexagon_app_debug

if [ $? -eq 0 ]; then
    echo "✅ Build successful! Running..."
    ./b_hexagon_app_debug
else
    echo "❌ Build failed!"
fi
