#!/bin/bash

# Integration Test Script for B_Hexagon Application
echo "=== B_Hexagon Integration Test ==="
echo ""

# Check if executables exist
if [[ ! -f "./build/b_hexagon_app" ]]; then
    echo "❌ Main application not found. Please run: make b_hexagon_app"
    exit 1
fi

if [[ ! -f "./test_sender" ]]; then
    echo "❌ Test sender not found. Please build it first."
    exit 1
fi

echo "✅ Both executables found"
echo ""

echo "🚀 Starting integration test..."
echo ""

# Start the application in background
echo "📡 Starting B_Hexagon application..."
./build/b_hexagon_app &
APP_PID=$!

# Give the app time to start
sleep 2

echo "📤 Sending test data..."
./test_sender

# Wait a bit for processing
sleep 3

echo ""
echo "🛑 Stopping application..."
kill $APP_PID 2>/dev/null

echo "✅ Integration test completed"
echo ""
echo "Expected behavior:"
echo "  - Application should have received 5 messages"
echo "  - Each message should be processed and logged"
echo "  - Calculated delays should be displayed"
