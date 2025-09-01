#!/bin/bash

# Domain Logic Test Runner Script
# Compiles and runs all domain logic unit tests

set -e  # Exit on any error

echo "=== B_Hexagon Domain Logic Test Suite ==="
echo "Building and running comprehensive unit tests for domain logic"
echo

# Navigate to build directory
cd "$(dirname "$0")/build"

echo "Building test executables..."
make test_domain_models test_enhanced_calculations test_service_integration -j$(nproc)
echo

echo "=== Running Domain Model Tests ==="
./test_domain_models
echo

echo "=== Running Enhanced Track Calculations Tests ==="
./test_enhanced_calculations
echo

echo "=== Running Calculator Service Integration Tests ==="
./test_service_integration
echo

echo "=== Running Original Tests (for completeness) ==="
echo "Running original calculations tests..."
make test_calculations -j$(nproc)
./test_calculations
echo

echo "Running original service tests..."
make test_service -j$(nproc)
./test_service
echo

echo "=== Test Suite Summary ==="
echo "✅ Domain Model Tests"
echo "✅ Enhanced Track Calculations Tests" 
echo "✅ Calculator Service Integration Tests"
echo "✅ Original Track Calculations Tests"
echo "✅ Original Calculator Service Tests"
echo
echo "🎉 All domain logic tests passed successfully!"
echo "Domain logic is thoroughly tested and verified."
