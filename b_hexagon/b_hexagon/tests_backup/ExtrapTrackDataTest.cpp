#include <gtest/gtest.h>
#include "domain/model/ExtrapTrackData.hpp"
#include <limits>
#include <cmath>

class ExtrapTrackDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup valid test data
        validTrackData.trackId = 123;
        validTrackData.xVelocityECEF = 100.0;
        validTrackData.yVelocityECEF = 200.0;
        validTrackData.zVelocityECEF = 50.0;
        validTrackData.xPositionECEF = 1000.0;
        validTrackData.yPositionECEF = 2000.0;
        validTrackData.zPositionECEF = 500.0;
        validTrackData.updateTime = 1600000000000000000L;
        validTrackData.originalUpdateTime = 1599999999000000000L;
        validTrackData.firstHopSentTime = 1600000000500000000L;
    }

    ExtrapTrackData validTrackData;
};

TEST_F(ExtrapTrackDataTest, DefaultConstructor_InitializesAllFieldsToZero) {
    // Act
    ExtrapTrackData defaultData;

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
}

TEST_F(ExtrapTrackDataTest, IsValid_ValidData_ReturnsTrue) {
    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_ZeroTrackId_ReturnsFalse) {
    // Arrange
    validTrackData.trackId = 0;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NegativeTrackId_ReturnsFalse) {
    // Arrange
    validTrackData.trackId = -1;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_InfiniteVelocityX_ReturnsFalse) {
    // Arrange
    validTrackData.xVelocityECEF = std::numeric_limits<double>::infinity();

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_InfiniteVelocityY_ReturnsFalse) {
    // Arrange
    validTrackData.yVelocityECEF = std::numeric_limits<double>::infinity();

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_InfiniteVelocityZ_ReturnsFalse) {
    // Arrange
    validTrackData.zVelocityECEF = std::numeric_limits<double>::infinity();

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NaNVelocityX_ReturnsFalse) {
    // Arrange
    validTrackData.xVelocityECEF = std::numeric_limits<double>::quiet_NaN();

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_InfinitePositionX_ReturnsFalse) {
    // Arrange
    validTrackData.xPositionECEF = std::numeric_limits<double>::infinity();

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NaNPositionY_ReturnsFalse) {
    // Arrange
    validTrackData.yPositionECEF = std::numeric_limits<double>::quiet_NaN();

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NegativeUpdateTime_ReturnsFalse) {
    // Arrange
    validTrackData.updateTime = -1L;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NegativeOriginalUpdateTime_ReturnsFalse) {
    // Arrange
    validTrackData.originalUpdateTime = -1L;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NegativeFirstHopSentTime_ReturnsFalse) {
    // Arrange
    validTrackData.firstHopSentTime = -1L;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_ExcessiveVelocityMagnitude_ReturnsFalse) {
    // Arrange - Set velocity magnitude greater than speed of light * 1000
    validTrackData.xVelocityECEF = 200000000000.0; // Extremely high velocity
    validTrackData.yVelocityECEF = 200000000000.0;
    validTrackData.zVelocityECEF = 200000000000.0;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_ZeroVelocities_ReturnsTrue) {
    // Arrange
    validTrackData.xVelocityECEF = 0.0;
    validTrackData.yVelocityECEF = 0.0;
    validTrackData.zVelocityECEF = 0.0;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_ZeroPositions_ReturnsTrue) {
    // Arrange
    validTrackData.xPositionECEF = 0.0;
    validTrackData.yPositionECEF = 0.0;
    validTrackData.zPositionECEF = 0.0;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_ZeroTimestamps_ReturnsTrue) {
    // Arrange
    validTrackData.updateTime = 0L;
    validTrackData.originalUpdateTime = 0L;
    validTrackData.firstHopSentTime = 0L;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NegativePositions_ReturnsTrue) {
    // Arrange - Negative positions are valid (can be on opposite side of Earth)
    validTrackData.xPositionECEF = -1000.0;
    validTrackData.yPositionECEF = -2000.0;
    validTrackData.zPositionECEF = -500.0;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ExtrapTrackDataTest, IsValid_NegativeVelocities_ReturnsTrue) {
    // Arrange - Negative velocities are valid (direction)
    validTrackData.xVelocityECEF = -100.0;
    validTrackData.yVelocityECEF = -200.0;
    validTrackData.zVelocityECEF = -50.0;

    // Act
    bool result = validTrackData.isValid();

    // Assert
    EXPECT_TRUE(result);
}
