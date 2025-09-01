#include <gtest/gtest.h>
#include "domain/logic/CalculatorService.hpp"
#include "domain/model/TrackData.hpp"
#include "domain/model/DelayCalculatedTrackData.hpp"

class CalculatorServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test data
        testTrackData.trackId = 456;
        testTrackData.xVelocityECEF = 100.0;
        testTrackData.yVelocityECEF = 200.0;
        testTrackData.zVelocityECEF = 50.0;
        testTrackData.xPositionECEF = 1000.0;
        testTrackData.yPositionECEF = 2000.0;
        testTrackData.zPositionECEF = 500.0;
        testTrackData.updateTime = 1600000000000000000L; // Example timestamp
        testTrackData.originalUpdateTime = 1599999999000000000L; // 1 second earlier
        testTrackData.firstHopSentTime = 1600000000500000000L; // 0.5 seconds after update
    }

    CalculatorService service;
    TrackData testTrackData;
};

TEST_F(CalculatorServiceTest, CalculateDelay_ValidInput_ReturnsCorrectData) {
    // Act
    DelayCalculatedTrackData result = service.calculateDelay(testTrackData);

    // Assert - Check that all original data is preserved
    EXPECT_EQ(testTrackData.trackId, result.trackId);
    EXPECT_DOUBLE_EQ(testTrackData.xVelocityECEF, result.xVelocityECEF);
    EXPECT_DOUBLE_EQ(testTrackData.yVelocityECEF, result.yVelocityECEF);
    EXPECT_DOUBLE_EQ(testTrackData.zVelocityECEF, result.zVelocityECEF);
    EXPECT_DOUBLE_EQ(testTrackData.xPositionECEF, result.xPositionECEF);
    EXPECT_DOUBLE_EQ(testTrackData.yPositionECEF, result.yPositionECEF);
    EXPECT_DOUBLE_EQ(testTrackData.zPositionECEF, result.zPositionECEF);
    EXPECT_EQ(testTrackData.updateTime, result.updateTime);
    EXPECT_EQ(testTrackData.originalUpdateTime, result.originalUpdateTime);
    EXPECT_EQ(testTrackData.firstHopSentTime, result.firstHopSentTime);
}

TEST_F(CalculatorServiceTest, CalculateDelay_ValidInput_CalculatesDelayCorrectly) {
    // Act
    DelayCalculatedTrackData result = service.calculateDelay(testTrackData);

    // Assert - Check calculated delay
    EXPECT_GT(result.firstHopDelayTime, 0L); // Should be positive
    EXPECT_GT(result.secondHopSentTime, testTrackData.firstHopSentTime); // Should be after first hop
}

TEST_F(CalculatorServiceTest, CalculateDelay_ZeroTrackId_PreservesZeroId) {
    // Arrange
    testTrackData.trackId = 0;

    // Act
    DelayCalculatedTrackData result = service.calculateDelay(testTrackData);

    // Assert
    EXPECT_EQ(0, result.trackId);
}

TEST_F(CalculatorServiceTest, CalculateDelay_NegativePositions_PreservesNegativeValues) {
    // Arrange
    testTrackData.xPositionECEF = -1500.0;
    testTrackData.yPositionECEF = -2500.0;
    testTrackData.zPositionECEF = -750.0;

    // Act
    DelayCalculatedTrackData result = service.calculateDelay(testTrackData);

    // Assert
    EXPECT_DOUBLE_EQ(-1500.0, result.xPositionECEF);
    EXPECT_DOUBLE_EQ(-2500.0, result.yPositionECEF);
    EXPECT_DOUBLE_EQ(-750.0, result.zPositionECEF);
}

TEST_F(CalculatorServiceTest, CalculateDelay_NegativeVelocities_PreservesNegativeValues) {
    // Arrange
    testTrackData.xVelocityECEF = -50.0;
    testTrackData.yVelocityECEF = -100.0;
    testTrackData.zVelocityECEF = -25.0;

    // Act
    DelayCalculatedTrackData result = service.calculateDelay(testTrackData);

    // Assert
    EXPECT_DOUBLE_EQ(-50.0, result.xVelocityECEF);
    EXPECT_DOUBLE_EQ(-100.0, result.yVelocityECEF);
    EXPECT_DOUBLE_EQ(-25.0, result.zVelocityECEF);
}

TEST_F(CalculatorServiceTest, CalculateDelay_MultipleCallsWithSameInput_ConsistentResults) {
    // Act
    DelayCalculatedTrackData result1 = service.calculateDelay(testTrackData);
    DelayCalculatedTrackData result2 = service.calculateDelay(testTrackData);

    // Assert - All original data should be identical
    EXPECT_EQ(result1.trackId, result2.trackId);
    EXPECT_DOUBLE_EQ(result1.xVelocityECEF, result2.xVelocityECEF);
    EXPECT_DOUBLE_EQ(result1.yVelocityECEF, result2.yVelocityECEF);
    EXPECT_DOUBLE_EQ(result1.zVelocityECEF, result2.zVelocityECEF);
    EXPECT_DOUBLE_EQ(result1.xPositionECEF, result2.xPositionECEF);
    EXPECT_DOUBLE_EQ(result1.yPositionECEF, result2.yPositionECEF);
    EXPECT_DOUBLE_EQ(result1.zPositionECEF, result2.zPositionECEF);
    EXPECT_EQ(result1.updateTime, result2.updateTime);
    EXPECT_EQ(result1.originalUpdateTime, result2.originalUpdateTime);
    EXPECT_EQ(result1.firstHopSentTime, result2.firstHopSentTime);
    
    // Timing values may differ slightly due to system clock
    EXPECT_LE(result2.secondHopSentTime, result1.secondHopSentTime + 1000000L); // Within 1ms
}

TEST_F(CalculatorServiceTest, CalculateDelay_ZeroTimestamps_HandlesGracefully) {
    // Arrange
    testTrackData.updateTime = 0L;
    testTrackData.originalUpdateTime = 0L;
    testTrackData.firstHopSentTime = 0L;

    // Act
    DelayCalculatedTrackData result = service.calculateDelay(testTrackData);

    // Assert
    EXPECT_EQ(0L, result.updateTime);
    EXPECT_EQ(0L, result.originalUpdateTime);
    EXPECT_EQ(0L, result.firstHopSentTime);
    EXPECT_GT(result.secondHopSentTime, 0L); // Should still be current time
}
