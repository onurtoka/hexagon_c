/**
 * @file ZeroMQDataWriterTest.cpp
 * @brief Comprehensive tests for ZeroMQ RADIO adapter using binary serialization
 * @details Tests the modern binary-only ZeroMQ adapter with UDP multicast RADIO/DISH pattern
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <memory>
#include "adapters/outgoing/ZeroMQDataWriter.hpp"
#include "domain/model/DelayCalcTrackData.hpp"

/**
 * @brief Test fixture for ZeroMQDataWriter with binary serialization
 */
class ZeroMQDataWriterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
    }

    void TearDown() override {
        // Cleanup
    }
    
    /**
     * @brief Create a valid test DelayCalcTrackData object
     */
    DelayCalcTrackData createValidTestData() {
        DelayCalcTrackData data;
        data.trackId = 54321;
        data.xPositionECEF = 150.8;
        data.yPositionECEF = 250.4;
        data.zPositionECEF = 75.6;
        data.xVelocityECEF = 12.5;
        data.yVelocityECEF = 18.2;
        data.zVelocityECEF = 7.1;
        data.updateTime = 2000000000;  // 2 seconds in nanoseconds
        data.firstHopDelayTime = 500000000;  // 0.5 seconds
        data.secondHopSentTime = 2001000000;
        return data;
    }
};

/**
 * @brief Test ZeroMQDataWriter construction with domain constants
 */
TEST_F(ZeroMQDataWriterTest, Constructor_UsesDelayCalcTrackDataConstants) {
    // Should successfully create writer using DelayCalcTrackData constants
    EXPECT_NO_THROW({
        ZeroMQDataWriter writer;
    });
}

/**
 * @brief Test ZeroMQ configuration constants integration
 */
TEST_F(ZeroMQDataWriterTest, Configuration_UsesCorrectDelayCalcTrackDataConstants) {
    // Verify the writer uses the correct constants from DelayCalcTrackData
    EXPECT_STREQ(DelayCalcTrackData::ZMQ_MULTICAST_ADDRESS, "239.1.1.5");
    EXPECT_EQ(DelayCalcTrackData::ZMQ_PORT, 9595);
    EXPECT_STREQ(DelayCalcTrackData::ZMQ_PROTOCOL, "udp");
    
    // Constructor should not throw when using these constants
    EXPECT_NO_THROW({
        ZeroMQDataWriter writer;
    });
}

/**
 * @brief Test data sending functionality
 */
TEST_F(ZeroMQDataWriterTest, SendData_ValidData_DoesNotThrow) {
    ZeroMQDataWriter writer;
    DelayCalcTrackData testData = createValidTestData();
    
    // Sending valid data should not throw
    EXPECT_NO_THROW({
        writer.sendData(testData);
    });
}

/**
 * @brief Test data sending with invalid data
 */
TEST_F(ZeroMQDataWriterTest, SendData_InvalidData_HandlesGracefully) {
    ZeroMQDataWriter writer;
    DelayCalcTrackData invalidData; // Default constructed (invalid)
    
    // Should handle invalid data gracefully (may log warning but not throw)
    EXPECT_NO_THROW({
        writer.sendData(invalidData);
    });
}

/**
 * @brief Test binary serialization integration
 */
TEST_F(ZeroMQDataWriterTest, BinarySerialization_ValidData_ProducesCorrectBinary) {
    DelayCalcTrackData testData = createValidTestData();
    
    // Serialize the data using model's internal binary serialization
    std::vector<uint8_t> binaryData = testData.serialize();
    
    // Verify serialization produces reasonable binary data
    EXPECT_GT(binaryData.size(), 0);
    // Binary serialization is more compact than struct due to padding
    EXPECT_GE(binaryData.size(), 88); // Minimum expected size
    EXPECT_LE(binaryData.size(), sizeof(DelayCalcTrackData)); // Max struct size
    
    // Verify round-trip serialization
    DelayCalcTrackData deserializedData;
    EXPECT_TRUE(deserializedData.deserialize(binaryData));
    
    EXPECT_EQ(deserializedData.trackId, testData.trackId);
    EXPECT_DOUBLE_EQ(deserializedData.xPositionECEF, testData.xPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yPositionECEF, testData.yPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zPositionECEF, testData.zPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.xVelocityECEF, testData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yVelocityECEF, testData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zVelocityECEF, testData.zVelocityECEF);
    EXPECT_EQ(deserializedData.updateTime, testData.updateTime);
    EXPECT_EQ(deserializedData.firstHopDelayTime, testData.firstHopDelayTime);
    EXPECT_EQ(deserializedData.secondHopSentTime, testData.secondHopSentTime);
}

/**
 * @brief Test multiple data sends
 */
TEST_F(ZeroMQDataWriterTest, SendData_MultipleMessages_AllSucceed) {
    ZeroMQDataWriter writer;
    
    const int numMessages = 10;
    for (int i = 0; i < numMessages; ++i) {
        DelayCalcTrackData testData = createValidTestData();
        testData.trackId = 1000 + i; // Unique track ID for each message
        
        EXPECT_NO_THROW({
            writer.sendData(testData);
        }) << "Failed to send message " << i;
    }
}

/**
 * @brief Test thread safety of the writer
 */
TEST_F(ZeroMQDataWriterTest, ThreadSafety_ConcurrentSends) {
    ZeroMQDataWriter writer;
    
    const int numThreads = 4;
    const int numMessagesPerThread = 25;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    
    // Launch multiple threads sending data
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < numMessagesPerThread; ++j) {
                try {
                    DelayCalcTrackData testData = createValidTestData();
                    testData.trackId = (i * 1000) + j; // Unique track ID
                    writer.sendData(testData);
                    successCount++;
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
    
    // All sends should succeed
    EXPECT_EQ(successCount.load(), numThreads * numMessagesPerThread);
}

/**
 * @brief Performance test for binary serialization and sending
 */
TEST_F(ZeroMQDataWriterTest, Performance_BinarySerialization) {
    DelayCalcTrackData testData = createValidTestData();
    
    const int numIterations = 1000;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform many serializations using model's internal binary serialization
    for (int i = 0; i < numIterations; ++i) {
        std::vector<uint8_t> binaryData = testData.serialize();
        EXPECT_GT(binaryData.size(), 0); // Verify serialization worked
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Binary serialization should be fast (< 5 microseconds per operation on average)
    double avgTimePerOp = static_cast<double>(duration.count()) / numIterations;
    EXPECT_LT(avgTimePerOp, 5.0) << "Binary serialization too slow: " << avgTimePerOp << " μs per operation";
}

/**
 * @brief Performance test for complete send operation
 */
TEST_F(ZeroMQDataWriterTest, Performance_CompleteSendOperation) {
    ZeroMQDataWriter writer;
    DelayCalcTrackData testData = createValidTestData();
    
    const int numIterations = 100; // Fewer iterations for network operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform many send operations
    for (int i = 0; i < numIterations; ++i) {
        testData.trackId = 10000 + i; // Unique track ID
        writer.sendData(testData);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Complete send operation should be reasonably fast (< 100 microseconds per operation on average)
    double avgTimePerOp = static_cast<double>(duration.count()) / numIterations;
    EXPECT_LT(avgTimePerOp, 100.0) << "Complete send operation too slow: " << avgTimePerOp << " μs per operation";
}

/**
 * @brief Test error handling for invalid ZeroMQ operations
 */
TEST_F(ZeroMQDataWriterTest, ErrorHandling_GracefullyHandlesZmqErrors) {
    ZeroMQDataWriter writer;
    
    // This test verifies that the writer can be constructed and used without throwing
    // In a real scenario, we would test specific ZMQ error conditions
    DelayCalcTrackData testData = createValidTestData();
    
    EXPECT_NO_THROW({
        writer.sendData(testData);
    });
}

/**
 * @brief Test writer configuration and endpoint construction
 */
TEST_F(ZeroMQDataWriterTest, Configuration_EndpointConstruction) {
    // Verify that the endpoint is constructed correctly from domain constants
    std::string expectedEndpoint = std::string(DelayCalcTrackData::ZMQ_PROTOCOL) + "://" +
                                  DelayCalcTrackData::ZMQ_MULTICAST_ADDRESS + ":" +
                                  std::to_string(DelayCalcTrackData::ZMQ_PORT);
    
    EXPECT_EQ(expectedEndpoint, "udp://239.1.1.5:9595");
    
    // Writer should successfully use this endpoint
    EXPECT_NO_THROW({
        ZeroMQDataWriter writer;
    });
}

/**
 * @brief Test data validation integration
 */
TEST_F(ZeroMQDataWriterTest, DataValidation_ValidatesDataBeforeSending) {
    ZeroMQDataWriter writer;
    
    // Test with valid data
    DelayCalcTrackData validData = createValidTestData();
    EXPECT_TRUE(validData.isValid());
    EXPECT_NO_THROW({
        writer.sendData(validData);
    });
    
    // Test with invalid data (may still send but should handle gracefully)
    DelayCalcTrackData invalidData;
    invalidData.trackId = -1; // Invalid track ID
    EXPECT_FALSE(invalidData.isValid());
    EXPECT_NO_THROW({
        writer.sendData(invalidData);
    });
}
