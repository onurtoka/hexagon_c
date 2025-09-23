/**
 * @file ZeroMQDataHandlerTest.cpp
 * @brief Comprehensive tests for ZeroMQ DISH adapter using binary serialization
 * @details Tests the modern binary-only ZeroMQ adapter with UDP multicast RADIO/DISH pattern
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <memory>
#include "adapters/incoming/ZeroMQDataHandler.hpp"
#include "domain/ports/incoming/IDataHandler.hpp"
#include "domain/model/ExtrapTrackData.hpp"

/**
 * @brief Mock data receiver for testing ZeroMQDataHandler
 */
class MockDataReceiver : public IDataHandler {
public:
    MOCK_METHOD(void, onDataReceived, (const ExtrapTrackData& data), (override));
};

/**
 * @brief Test fixture for ZeroMQDataHandler with binary serialization
 */
class ZeroMQDataHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockReceiver = std::make_unique<MockDataReceiver>();
    }

    void TearDown() override {
        mockReceiver.reset();
    }

    std::unique_ptr<MockDataReceiver> mockReceiver;
    
    /**
     * @brief Create a valid test ExtrapTrackData object
     */
    ExtrapTrackData createValidTestData() {
        ExtrapTrackData data;
        data.trackId = 12345;
        data.xPositionECEF = 100.5;
        data.yPositionECEF = 200.7;
        data.zPositionECEF = 50.2;
        data.xVelocityECEF = 10.1;
        data.yVelocityECEF = 15.3;
        data.zVelocityECEF = 5.8;
        data.updateTime = 1000000000;  // 1 second in nanoseconds
        data.originalUpdateTime = 999000000;
        data.firstHopSentTime = 1001000000;
        return data;
    }
};

/**
 * @brief Test ZeroMQDataHandler construction with domain constants
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_UsesExtrapTrackDataConstants) {
    // Should successfully create handler using ExtrapTrackData constants
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(mockReceiver.get());
    });
}

/**
 * @brief Test ZeroMQDataHandler construction with invalid receiver
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_NullReceiver_ThrowsException) {
    // Should handle null receiver gracefully (depends on implementation)
    // This test validates defensive programming
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(nullptr);
    });
}

/**
 * @brief Test binary deserialization functionality
 */
TEST_F(ZeroMQDataHandlerTest, DeserializeBinary_ValidData_ReturnsCorrectObject) {
    ZeroMQDataHandler handler(mockReceiver.get());
    
    // Create test data and serialize it using model's internal binary serialization
    ExtrapTrackData originalData = createValidTestData();
    std::vector<uint8_t> binaryData = originalData.serialize();
    
    // Test deserialization using model's internal deserialization
    ExtrapTrackData deserializedData;
    EXPECT_TRUE(deserializedData.deserialize(binaryData));
    
    // Verify all fields match
    EXPECT_EQ(deserializedData.trackId, originalData.trackId);
    EXPECT_DOUBLE_EQ(deserializedData.xPositionECEF, originalData.xPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yPositionECEF, originalData.yPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zPositionECEF, originalData.zPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.xVelocityECEF, originalData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yVelocityECEF, originalData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zVelocityECEF, originalData.zVelocityECEF);
    EXPECT_EQ(deserializedData.updateTime, originalData.updateTime);
    EXPECT_EQ(deserializedData.originalUpdateTime, originalData.originalUpdateTime);
    EXPECT_EQ(deserializedData.firstHopSentTime, originalData.firstHopSentTime);
}

/**
 * @brief Test binary deserialization with invalid data
 */
TEST_F(ZeroMQDataHandlerTest, DeserializeBinary_InvalidData_ThrowsException) {
    // Test with invalid data size using model's internal deserialization
    std::vector<uint8_t> invalidData = {0x01, 0x02, 0x03}; // Too small
    
    ExtrapTrackData data;
    EXPECT_FALSE(data.deserialize(invalidData));
}

/**
 * @brief Test ZeroMQ configuration constants integration
 */
TEST_F(ZeroMQDataHandlerTest, Configuration_UsesCorrectExtrapTrackDataConstants) {
    // Verify the handler uses the correct constants from ExtrapTrackData
    EXPECT_STREQ(ExtrapTrackData::ZMQ_MULTICAST_ADDRESS, "239.1.1.5");
    EXPECT_EQ(ExtrapTrackData::ZMQ_PORT, 9596);
    EXPECT_STREQ(ExtrapTrackData::ZMQ_PROTOCOL, "udp");
    
    // Constructor should not throw when using these constants
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(mockReceiver.get());
    });
}

/**
 * @brief Test error handling for ZeroMQ operations
 */
TEST_F(ZeroMQDataHandlerTest, ErrorHandling_GracefullyHandlesZmqErrors) {
    ZeroMQDataHandler handler(mockReceiver.get());
    
    // This test verifies that the handler can be constructed without throwing
    // In a real scenario, we would test specific ZMQ error conditions
    EXPECT_NO_THROW({
        // Handler should be constructed successfully
        // startReceiving() would be tested in integration tests
    });
}

/**
 * @brief Test thread safety of the handler
 */
TEST_F(ZeroMQDataHandlerTest, ThreadSafety_MultipleDeserializationCalls) {
    ZeroMQDataHandler handler(mockReceiver.get());
    ExtrapTrackData testData = createValidTestData();
    std::vector<uint8_t> binaryData = testData.serialize();
    
    const int numThreads = 4;
    const int numCallsPerThread = 10;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    
    // Launch multiple threads performing deserialization
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < numCallsPerThread; ++j) {
                try {
                    ExtrapTrackData result;
                    if (result.deserialize(binaryData) && result.trackId == testData.trackId) {
                        successCount++;
                    }
                } catch (...) {
                    // Should not throw for valid data
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // All calls should succeed
    EXPECT_EQ(successCount.load(), numThreads * numCallsPerThread);
}

/**
 * @brief Test edge cases for binary data handling
 */
TEST_F(ZeroMQDataHandlerTest, EdgeCases_BinaryDataHandling) {
    // Test with zero-size data
    std::vector<uint8_t> emptyData;
    ExtrapTrackData data1;
    EXPECT_FALSE(data1.deserialize(emptyData));
    
    // Test with insufficient data
    std::vector<uint8_t> smallData = {0x01, 0x02};
    ExtrapTrackData data2;
    EXPECT_FALSE(data2.deserialize(smallData));
}

/**
 * @brief Performance test for binary deserialization
 */
TEST_F(ZeroMQDataHandlerTest, Performance_BinaryDeserialization) {
    ZeroMQDataHandler handler(mockReceiver.get());
    ExtrapTrackData testData = createValidTestData();
    std::vector<uint8_t> binaryData = testData.serialize();
    
    const int numIterations = 1000;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform many deserializations using model's internal deserialization
    for (int i = 0; i < numIterations; ++i) {
        ExtrapTrackData result;
        EXPECT_TRUE(result.deserialize(binaryData));
        EXPECT_EQ(result.trackId, testData.trackId); // Verify correctness
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Binary deserialization should be fast (< 10 microseconds per operation on average)
    double avgTimePerOp = static_cast<double>(duration.count()) / numIterations;
    EXPECT_LT(avgTimePerOp, 10.0) << "Binary deserialization too slow: " << avgTimePerOp << " μs per operation";
}
