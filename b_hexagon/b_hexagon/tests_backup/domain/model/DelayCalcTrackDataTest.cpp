#include <gtest/gtest.h>
#include "domain/model/DelayCalcTrackData.hpp"
#include <limits>
#include <cmath>

class DelayCalculatedTrackDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup valid test data
        validData.trackId = 456;
        validData.xVelocityECEF = 150.0;
        validData.yVelocityECEF = 250.0;
        validData.zVelocityECEF = 75.0;
        validData.xPositionECEF = 1500.0;
        validData.yPositionECEF = 2500.0;
        validData.zPositionECEF = 750.0;
        validData.updateTime = 1600000000000000000L;
        validData.originalUpdateTime = 1599999999000000000L;
        validData.firstHopSentTime = 1600000000500000000L;
        validData.firstHopDelayTime = 500000000L; // 0.5 seconds
        validData.secondHopSentTime = 1600000001000000000L; // 1 second after first hop
    }

    DelayCalculatedTrackData validData;
};

TEST_F(DelayCalculatedTrackDataTest, DefaultConstructor_InitializesAllFieldsToZero) {
    // Act
    DelayCalculatedTrackData defaultData;

    // Assert
    EXPECT_EQ(0, defaultData.trackId);
    EXPECT_DOUBLE_EQ(0.0, defaultData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(0.0, defaultData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(0.0, defaultData.zVelocityECEF);
    EXPECT_DOUBLE_EQ(0.0, defaultData.xPositionECEF);
    EXPECT_DOUBLE_EQ(0.0, defaultData.yPositionECEF);
    EXPECT_DOUBLE_EQ(0.0, defaultData.zPositionECEF);
    EXPECT_EQ(0L, defaultData.updateTime);
    EXPECT_EQ(0L, defaultData.originalUpdateTime);
    EXPECT_EQ(0L, defaultData.firstHopSentTime);
    EXPECT_EQ(0L, defaultData.firstHopDelayTime);
    EXPECT_EQ(0L, defaultData.secondHopSentTime);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_ValidData_ReturnsTrue) {
    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_ZeroTrackId_ReturnsFalse) {
    // Arrange
    validData.trackId = 0;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NegativeTrackId_ReturnsFalse) {
    // Arrange
    validData.trackId = -1;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_InfiniteVelocityX_ReturnsFalse) {
    // Arrange
    validData.xVelocityECEF = std::numeric_limits<double>::infinity();

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NaNVelocityY_ReturnsFalse) {
    // Arrange
    validData.yVelocityECEF = std::numeric_limits<double>::quiet_NaN();

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_InfinitePositionZ_ReturnsFalse) {
    // Arrange
    validData.zPositionECEF = std::numeric_limits<double>::infinity();

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NaNPositionX_ReturnsFalse) {
    // Arrange
    validData.xPositionECEF = std::numeric_limits<double>::quiet_NaN();

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NegativeUpdateTime_ReturnsFalse) {
    // Arrange
    validData.updateTime = -1L;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NegativeFirstHopDelayTime_ReturnsFalse) {
    // Arrange
    validData.firstHopDelayTime = -1L;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NegativeSecondHopSentTime_ReturnsFalse) {
    // Arrange
    validData.secondHopSentTime = -1L;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_ExcessiveVelocityMagnitude_ReturnsFalse) {
    // Arrange - Set velocity magnitude greater than speed of light * 1000
    validData.xVelocityECEF = 250000000000.0; // Extremely high velocity
    validData.yVelocityECEF = 250000000000.0;
    validData.zVelocityECEF = 250000000000.0;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_SecondHopBeforeFirstHop_ReturnsFalse) {
    // Arrange - Second hop sent before first hop (invalid timing)
    validData.firstHopSentTime = 1600000001000000000L;
    validData.secondHopSentTime = 1600000000500000000L; // Earlier time
    validData.firstHopDelayTime = 100000000L; // Positive delay

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_ZeroDelayTimesButValidOtherTimes_ReturnsTrue) {
    // Arrange - Zero delay times are acceptable when other timing is valid
    validData.firstHopDelayTime = 0L;
    validData.secondHopSentTime = 0L;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_ValidTimingSequence_ReturnsTrue) {
    // Arrange - Ensure proper timing sequence
    validData.firstHopSentTime = 1000000000L;
    validData.firstHopDelayTime = 500000000L;
    validData.secondHopSentTime = 1600000000L; // After first hop

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_ZeroVelocities_ReturnsTrue) {
    // Arrange
    validData.xVelocityECEF = 0.0;
    validData.yVelocityECEF = 0.0;
    validData.zVelocityECEF = 0.0;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NegativePositions_ReturnsTrue) {
    // Arrange - Negative positions are valid
    validData.xPositionECEF = -1500.0;
    validData.yPositionECEF = -2500.0;
    validData.zPositionECEF = -750.0;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_NegativeVelocities_ReturnsTrue) {
    // Arrange - Negative velocities are valid (direction)
    validData.xVelocityECEF = -150.0;
    validData.yVelocityECEF = -250.0;
    validData.zVelocityECEF = -75.0;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_ZeroTimestamps_ReturnsTrue) {
    // Arrange
    validData.updateTime = 0L;
    validData.originalUpdateTime = 0L;
    validData.firstHopSentTime = 0L;
    validData.firstHopDelayTime = 0L;
    validData.secondHopSentTime = 0L;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(DelayCalculatedTrackDataTest, IsValid_LargeValidVelocities_ReturnsTrue) {
    // Arrange - Large but reasonable velocities (below speed of light limit)
    validData.xVelocityECEF = 10000000.0; // 10 million m/s (well below light speed)
    validData.yVelocityECEF = 15000000.0;
    validData.zVelocityECEF = 5000000.0;

    // Act
    bool result = validData.isValid();

    // Assert
    EXPECT_TRUE(result);
}
