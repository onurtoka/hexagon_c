/**
 * @file BinarySerializerTest.cpp
 * @brief Unit tests for binary serialization utilities
 */

#include <gtest/gtest.h>
#include "common/BinarySerializer.hpp"
#include "domain/model/ExtrapTrackData.hpp"
#include "domain/model/DelayCalcTrackData.hpp"

/**
 * @brief Test fixture for BinarySerializer
 */
class BinarySerializerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test ExtrapTrackData
        extrapData.trackId = 12345;
        extrapData.xVelocityECEF = 100.5;
        extrapData.yVelocityECEF = 200.75;
        extrapData.zVelocityECEF = 300.25;
        extrapData.xPositionECEF = 1000.0;
        extrapData.yPositionECEF = 2000.0;
        extrapData.zPositionECEF = 3000.0;
        extrapData.updateTime = 1234567890123456789L;
        extrapData.originalUpdateTime = 1234567890123456780L;
        extrapData.firstHopSentTime = 1234567890123456790L;

        // Create test DelayCalcTrackData
        delayCalcData.trackId = 12345;
        delayCalcData.xVelocityECEF = 100.5;
        delayCalcData.yVelocityECEF = 200.75;
        delayCalcData.zVelocityECEF = 300.25;
        delayCalcData.xPositionECEF = 1000.0;
        delayCalcData.yPositionECEF = 2000.0;
        delayCalcData.zPositionECEF = 3000.0;
        delayCalcData.updateTime = 1234567890123456789L;
        delayCalcData.originalUpdateTime = 1234567890123456780L;
        delayCalcData.firstHopSentTime = 1234567890123456790L;
        delayCalcData.firstHopDelayTime = 10000000L;
        delayCalcData.secondHopSentTime = 1234567890123466790L;
    }

    ExtrapTrackData extrapData;
    DelayCalcTrackData delayCalcData;
};

/**
 * @brief Test ExtrapTrackData serialization and deserialization
 */
TEST_F(BinarySerializerTest, ExtrapTrackData_SerializeDeserialize_RoundTrip) {
    // Serialize
    std::vector<uint8_t> binaryData = BinarySerializer::serialize(extrapData);
    
    // Verify expected size
    EXPECT_EQ(binaryData.size(), BinarySerializer::EXTRAP_TRACK_DATA_SIZE);
    
    // Deserialize
    ExtrapTrackData deserializedData = BinarySerializer::deserializeExtrapTrackData(binaryData);
    
    // Verify all fields match
    EXPECT_EQ(deserializedData.trackId, extrapData.trackId);
    EXPECT_DOUBLE_EQ(deserializedData.xVelocityECEF, extrapData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yVelocityECEF, extrapData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zVelocityECEF, extrapData.zVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.xPositionECEF, extrapData.xPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yPositionECEF, extrapData.yPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zPositionECEF, extrapData.zPositionECEF);
    EXPECT_EQ(deserializedData.updateTime, extrapData.updateTime);
    EXPECT_EQ(deserializedData.originalUpdateTime, extrapData.originalUpdateTime);
    EXPECT_EQ(deserializedData.firstHopSentTime, extrapData.firstHopSentTime);
}

/**
 * @brief Test DelayCalcTrackData serialization and deserialization
 */
TEST_F(BinarySerializerTest, DelayCalcTrackData_SerializeDeserialize_RoundTrip) {
    // Serialize
    std::vector<uint8_t> binaryData = BinarySerializer::serialize(delayCalcData);
    
    // Verify expected size
    EXPECT_EQ(binaryData.size(), BinarySerializer::DELAY_CALC_TRACK_DATA_SIZE);
    
    // Deserialize
    DelayCalcTrackData deserializedData = BinarySerializer::deserializeDelayCalcTrackData(binaryData);
    
    // Verify all fields match
    EXPECT_EQ(deserializedData.trackId, delayCalcData.trackId);
    EXPECT_DOUBLE_EQ(deserializedData.xVelocityECEF, delayCalcData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yVelocityECEF, delayCalcData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zVelocityECEF, delayCalcData.zVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.xPositionECEF, delayCalcData.xPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yPositionECEF, delayCalcData.yPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zPositionECEF, delayCalcData.zPositionECEF);
    EXPECT_EQ(deserializedData.updateTime, delayCalcData.updateTime);
    EXPECT_EQ(deserializedData.originalUpdateTime, delayCalcData.originalUpdateTime);
    EXPECT_EQ(deserializedData.firstHopSentTime, delayCalcData.firstHopSentTime);
    EXPECT_EQ(deserializedData.firstHopDelayTime, delayCalcData.firstHopDelayTime);
    EXPECT_EQ(deserializedData.secondHopSentTime, delayCalcData.secondHopSentTime);
}

/**
 * @brief Test error handling for invalid data size
 */
TEST_F(BinarySerializerTest, InvalidDataSize_ThrowsException) {
    // Test with wrong size data
    std::vector<uint8_t> invalidData(10); // Too small
    
    EXPECT_THROW(BinarySerializer::deserializeExtrapTrackData(invalidData), std::invalid_argument);
    EXPECT_THROW(BinarySerializer::deserializeDelayCalcTrackData(invalidData), std::invalid_argument);
}

/**
 * @brief Test raw buffer deserialization
 */
TEST_F(BinarySerializerTest, RawBuffer_Deserialization_Works) {
    // Serialize ExtrapTrackData
    std::vector<uint8_t> binaryData = BinarySerializer::serialize(extrapData);
    
    // Deserialize using raw buffer method
    ExtrapTrackData deserializedData = BinarySerializer::deserializeExtrapTrackData(
        binaryData.data(), binaryData.size());
    
    // Verify fields match
    EXPECT_EQ(deserializedData.trackId, extrapData.trackId);
    EXPECT_DOUBLE_EQ(deserializedData.xVelocityECEF, extrapData.xVelocityECEF);
}
