/**
 * @file TrackCalculationsAdvancedTest.cpp
 * @brief Advanced unit tests for track calculation utilities
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "domain/logic/TrackCalculations.hpp"
#include "domain/model/ExtrapTrackData.hpp"
#include <limits>
#include <cmath>
#include <thread>
#include <vector>
#include <chrono>

class TrackCalculationsAdvancedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup common test values
        baseTime = 1723623744000000000L; // Base timestamp
        futureTime = 1723623745000000000L; // 1 second later
    }

    int64_t baseTime;
    int64_t futureTime;
};

/**
 * @brief Test delay calculation with various time differences using proper ExtrapTrackData
 */
TEST_F(TrackCalculationsAdvancedTest, CalculateFirstHopDelayTime_VariousTimeDifferences_CalculatesCorrectly) {
    // Test microsecond precision
    ExtrapTrackData trackData1;
    trackData1.firstHopSentTime = baseTime;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData1, baseTime + 1), 1);
    
    // Test millisecond precision
    ExtrapTrackData trackData2;
    trackData2.firstHopSentTime = baseTime;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData2, baseTime + 1000000), 1000000);
    
    // Test second precision
    ExtrapTrackData trackData3;
    trackData3.firstHopSentTime = baseTime;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData3, baseTime + 1000000000), 1000000000);
}

/**
 * @brief Test edge case timestamps
 */
TEST_F(TrackCalculationsAdvancedTest, CalculateFirstHopDelayTime_EdgeCaseTimestamps_HandlesCorrectly) {
    int64_t maxTime = std::numeric_limits<int64_t>::max();
    int64_t nearMaxTime = maxTime - 1000000000L; // 1 second before max
    
    ExtrapTrackData trackData;
    trackData.firstHopSentTime = nearMaxTime;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData, maxTime), 1000000000L);
    
    // Test near minimum values
    int64_t minTime = 1000000000L; // Reasonable minimum time
    trackData.firstHopSentTime = minTime;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData, minTime + 500000000L), 500000000L);
}

/**
 * @brief Test same timestamps return zero delay
 */
TEST_F(TrackCalculationsAdvancedTest, CalculateFirstHopDelayTime_SameTimestamps_ReturnsZero) {
    ExtrapTrackData trackData;
    trackData.firstHopSentTime = baseTime;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData, baseTime), 0);
    
    trackData.firstHopSentTime = 0;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData, 0), 0);
    
    trackData.firstHopSentTime = futureTime;
    EXPECT_EQ(TrackCalculations::calculateFirstHopDelayTime(trackData, futureTime), 0);
}

/**
 * @brief Test second hop sent time consistency
 */
TEST_F(TrackCalculationsAdvancedTest, CalculateSecondHopSentTime_ConsistentGeneration_ReturnsReasonableTimes) {
    long time1 = TrackCalculations::calculateSecondHopSentTime();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    long time2 = TrackCalculations::calculateSecondHopSentTime();
    
    // Time should be increasing
    EXPECT_GT(time2, time1);
    
    // Times should be reasonable (after year 2020)
    int64_t year2020Ns = 1577836800000000000L; // Jan 1, 2020 00:00:00 UTC in nanoseconds
    EXPECT_GT(time1, year2020Ns);
    EXPECT_GT(time2, year2020Ns);
    
    // Difference should be reasonable (between 1ms and 10ms for this test)
    int64_t diff = time2 - time1;
    EXPECT_GT(diff, 1000000L);  // > 1ms
    EXPECT_LT(diff, 10000000L); // < 10ms
}

/**
 * @brief Test monotonicity of second hop sent time
 */
TEST_F(TrackCalculationsAdvancedTest, CalculateSecondHopSentTime_Monotonicity_AlwaysIncreases) {
    constexpr int iterations = 10;
    std::vector<long> times(static_cast<size_t>(iterations));
    
    for (int i = 0; i < iterations; ++i) {
        times[static_cast<size_t>(i)] = TrackCalculations::calculateSecondHopSentTime();
        if (i > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
    
    // Verify monotonic increase
    for (int i = 1; i < iterations; ++i) {
        EXPECT_GT(times[static_cast<size_t>(i)], times[static_cast<size_t>(i-1)]) 
            << "Time should be monotonically increasing at iteration " << i;
    }
}

/**
 * @brief Test nanosecond precision is maintained
 */
TEST_F(TrackCalculationsAdvancedTest, CalculateFirstHopDelayTime_Precision_MaintainsNanosecondAccuracy) {
    constexpr int64_t precisionTests[] = {1, 10, 100, 999, 1000, 9999, 10000, 99999, 100000};
    
    for (int64_t precision : precisionTests) {
        ExtrapTrackData trackData;
        int64_t start = baseTime;
        int64_t end = start + precision;
        
        trackData.firstHopSentTime = start;
        int64_t delay = TrackCalculations::calculateFirstHopDelayTime(trackData, end);
        
        EXPECT_EQ(delay, precision) << "Precision test failed for " << precision << " nanoseconds";
    }
}

/**
 * @brief Test overflow scenarios
 */
TEST_F(TrackCalculationsAdvancedTest, CalculateFirstHopDelayTime_OverflowScenarios_HandlesCorrectly) {
    // Test near maximum values without overflow
    int64_t max = std::numeric_limits<int64_t>::max();
    int64_t nearMax = max - 1000000000L;
    
    ExtrapTrackData trackData;
    trackData.firstHopSentTime = nearMax;
    
    // This should work without overflow
    int64_t delay = TrackCalculations::calculateFirstHopDelayTime(trackData, max);
    EXPECT_EQ(delay, 1000000000L);
    
    // Verify result is reasonable
    EXPECT_GT(delay, 0);
    EXPECT_LT(delay, 2000000000L); // Less than 2 seconds
}

/**
 * @brief Test with realistic timestamp ranges
 */
TEST_F(TrackCalculationsAdvancedTest, TimestampValidation_ReasonableRanges_ValidatesCorrectly) {
    // Test with year 2020-2030 range
    int64_t year2020 = 1577836800000000000L; // Jan 1, 2020 00:00:00 UTC
    int64_t year2030 = 1893456000000000000L; // Jan 1, 2030 00:00:00 UTC
    
    ExtrapTrackData trackData;
    trackData.firstHopSentTime = year2020;
    int64_t delay = TrackCalculations::calculateFirstHopDelayTime(trackData, year2020 + 1000000000L);
    EXPECT_EQ(delay, 1000000000L); // 1 second delay
    
    // Test larger time span
    trackData.firstHopSentTime = year2020;
    int64_t largeDelay = TrackCalculations::calculateFirstHopDelayTime(trackData, year2030);
    EXPECT_GT(largeDelay, 0);
    EXPECT_EQ(largeDelay, year2030 - year2020); // Should equal the time difference
}

/**
 * @brief Test thread safety of calculations
 */
TEST_F(TrackCalculationsAdvancedTest, ThreadSafety_ConcurrentCalculations_ProducesConsistentResults) {
    constexpr int numThreads = 4;
    constexpr int calculationsPerThread = 100;
    
    std::vector<std::thread> threads;
    std::vector<std::vector<long>> results(static_cast<size_t>(numThreads));
    
    // Launch threads to perform calculations concurrently
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([t, calculationsPerThread, &results, this]() {
            for (int i = 0; i < calculationsPerThread; ++i) {
                ExtrapTrackData trackData;
                trackData.firstHopSentTime = baseTime + i * 1000; // Vary the sent time
                long delay = TrackCalculations::calculateFirstHopDelayTime(trackData, baseTime + i * 1000 + 500000);
                results[static_cast<size_t>(t)].push_back(delay);
                
                long sentTime = TrackCalculations::calculateSecondHopSentTime();
                results[static_cast<size_t>(t)].push_back(sentTime);
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify results
    for (int t = 0; t < numThreads; ++t) {
        EXPECT_EQ(results[static_cast<size_t>(t)].size(), static_cast<size_t>(calculationsPerThread * 2));
        
        for (size_t i = 0; i < results[static_cast<size_t>(t)].size(); i += 2) {
            EXPECT_GE(results[static_cast<size_t>(t)][i], 0); // Delay should be non-negative
            EXPECT_GT(results[static_cast<size_t>(t)][i + 1], baseTime); // Sent time should be reasonable
        }
    }
}
