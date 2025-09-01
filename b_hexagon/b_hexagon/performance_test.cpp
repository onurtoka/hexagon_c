/**
 * @file performance_test.cpp
 * @brief Performance test for domain calculations
 */

#include "src/domain/logic/CalculatorService.hpp"
#include "src/domain/model/TrackData.hpp"
#include <chrono>
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== B_Hexagon Performance Test ===\n\n";
    
    CalculatorService service;
    const int NUM_ITERATIONS = 100000;
    
    // Create test data
    TrackData testData;
    testData.trackId = 1001;
    testData.xVelocityECEF = 50.0;
    testData.yVelocityECEF = 75.1;
    testData.zVelocityECEF = -10.2;
    testData.xPositionECEF = 887766.1;
    testData.yPositionECEF = 554433.2;
    testData.zPositionECEF = 12345.0;
    testData.updateTime = 1723623745000000000L;
    testData.originalUpdateTime = 1723623744000000000L;
    testData.firstHopSentTime = 1723623744500000000L;
    
    std::vector<DelayCalculatedTrackData> results;
    results.reserve(NUM_ITERATIONS);
    
    std::cout << "Testing " << NUM_ITERATIONS << " calculations...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Vary the input slightly for each iteration
        testData.trackId = 1001 + i;
        testData.updateTime += 1000; // Add 1 microsecond each time
        
        results.push_back(service.calculateDelay(testData));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double average_time_per_calc = static_cast<double>(duration.count()) / NUM_ITERATIONS;
    double calculations_per_second = 1000000.0 / average_time_per_calc;
    
    std::cout << "\n📊 Performance Results:\n";
    std::cout << "  Total time: " << duration.count() << " microseconds\n";
    std::cout << "  Average time per calculation: " << average_time_per_calc << " microseconds\n";
    std::cout << "  Calculations per second: " << static_cast<int>(calculations_per_second) << "\n";
    
    // Verify results are reasonable
    bool all_valid = true;
    for (const auto& result : results) {
        if (!result.isValid()) {
            all_valid = false;
            break;
        }
    }
    
    std::cout << "  All results valid: " << (all_valid ? "✅ Yes" : "❌ No") << "\n";
    std::cout << "  First result delay: " << results[0].firstHopDelayTime << " ns\n";
    std::cout << "  Last result delay: " << results.back().firstHopDelayTime << " ns\n";
    
    return 0;
}
