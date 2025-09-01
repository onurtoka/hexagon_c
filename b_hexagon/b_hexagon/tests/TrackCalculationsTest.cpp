#include <gtest/gtest.h>
#include "domain/logic/TrackCalculations.hpp"
#include "domain/model/TrackData.hpp"
#include <chrono>
#include <thread>

class TrackCalculationsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test data
        testTrackData.trackId = 123;
        testTrackData.firstHopSentTime = 1000000000L; // 1 second in nanoseconds
    }

    TrackData testTrackData;
};

TEST_F(TrackCalculationsTest, CalculateFirstHopDelayTime_ValidInput_ReturnsCorrectDelay) {
    // Arrange
    long receiveTime = 1500000000L; // 1.5 seconds in nanoseconds
    long expectedDelay = 500000000L; // 0.5 seconds in nanoseconds

    // Act
    long actualDelay = TrackCalculations::calculateFirstHopDelayTime(testTrackData, receiveTime);

    // Assert
    EXPECT_EQ(expectedDelay, actualDelay);
}

TEST_F(TrackCalculationsTest, CalculateFirstHopDelayTime_ZeroDelay_ReturnsZero) {
    // Arrange
    long receiveTime = testTrackData.firstHopSentTime; // Same time = zero delay

    // Act
    long actualDelay = TrackCalculations::calculateFirstHopDelayTime(testTrackData, receiveTime);

    // Assert
    EXPECT_EQ(0L, actualDelay);
}

TEST_F(TrackCalculationsTest, CalculateFirstHopDelayTime_NegativeDelay_ReturnsNegativeValue) {
    // Arrange - receive time before sent time (edge case)
    long receiveTime = 500000000L; // 0.5 seconds in nanoseconds
    long expectedDelay = -500000000L; // -0.5 seconds in nanoseconds

    // Act
    long actualDelay = TrackCalculations::calculateFirstHopDelayTime(testTrackData, receiveTime);

    // Assert
    EXPECT_EQ(expectedDelay, actualDelay);
}

TEST_F(TrackCalculationsTest, CalculateSecondHopSentTime_ReturnsReasonableTime) {
    // Arrange
    auto beforeCall = std::chrono::system_clock::now();
    
    // Act
    long secondHopTime = TrackCalculations::calculateSecondHopSentTime();
    
    // Arrange (continued)
    auto afterCall = std::chrono::system_clock::now();
    long beforeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(beforeCall.time_since_epoch()).count();
    long afterNs = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCall.time_since_epoch()).count();

    // Assert
    EXPECT_GE(secondHopTime, beforeNs);
    EXPECT_LE(secondHopTime, afterNs);
    EXPECT_GT(secondHopTime, 0L); // Should be positive (after epoch)
}

TEST_F(TrackCalculationsTest, CalculateSecondHopSentTime_ConsecutiveCalls_IncreasingValues) {
    // Act
    long firstCall = TrackCalculations::calculateSecondHopSentTime();
    
    // Small delay to ensure time difference
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    
    long secondCall = TrackCalculations::calculateSecondHopSentTime();

    // Assert
    EXPECT_LT(firstCall, secondCall);
}

TEST_F(TrackCalculationsTest, CalculateFirstHopDelayTime_LargeValues_HandlesCorrectly) {
    // Arrange - Test with large timestamp values (year 2024+)
    testTrackData.firstHopSentTime = 1700000000000000000L; // Large timestamp
    long receiveTime = 1700000001000000000L; // 1 second later
    long expectedDelay = 1000000000L; // 1 second in nanoseconds

    // Act
    long actualDelay = TrackCalculations::calculateFirstHopDelayTime(testTrackData, receiveTime);

    // Assert
    EXPECT_EQ(expectedDelay, actualDelay);
}
