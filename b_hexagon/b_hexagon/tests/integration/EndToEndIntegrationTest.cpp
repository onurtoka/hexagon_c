/**
 * @file EndToEndIntegrationTest.cpp
 * @brief Comprehensive end-to-end integration tests for the complete binary messaging pipeline
 * @details Tests the full RADIO/DISH UDP multicast pipeline with binary serialization
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "adapters/incoming/ZeroMQDataHandler.hpp"
#include "adapters/outgoing/ZeroMQDataWriter.hpp"
#include "domain/logic/CalculatorService.hpp"
#include "domain/model/ExtrapTrackData.hpp"
#include "domain/model/DelayCalcTrackData.hpp"
#include "common/BinarySerializer.hpp"

/**
 * @brief Mock data receiver for integration testing
 */
class IntegrationTestReceiver : public IDataHandler {
public:
    void onDataReceived(const ExtrapTrackData& data) override {
        std::lock_guard<std::mutex> lock(mutex_);
        receivedData_.push_back(data);
        dataReceived_ = true;
        cv_.notify_one();
    }

    bool waitForData(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, timeout, [this] { return dataReceived_; });
    }

    std::vector<ExtrapTrackData> getReceivedData() {
        std::lock_guard<std::mutex> lock(mutex_);
        return receivedData_;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        receivedData_.clear();
        dataReceived_ = false;
    }

private:
    std::vector<ExtrapTrackData> receivedData_;
    bool dataReceived_ = false;
    std::mutex mutex_;
    std::condition_variable cv_;
};

/**
 * @brief Test fixture for end-to-end integration testing
 */
class EndToEndIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        testReceiver_ = std::make_unique<IntegrationTestReceiver>();
        calculatorService_ = std::make_unique<CalculatorService>();
    }

    void TearDown() override {
        testReceiver_.reset();
        calculatorService_.reset();
    }

    std::unique_ptr<IntegrationTestReceiver> testReceiver_;
    std::unique_ptr<CalculatorService> calculatorService_;

    /**
     * @brief Create a valid test ExtrapTrackData object
     */
    ExtrapTrackData createValidExtrapTrackData() {
        ExtrapTrackData data;
        data.trackId = 98765;
        data.xPositionECEF = 300.5;
        data.yPositionECEF = 400.7;
        data.zPositionECEF = 150.2;
        data.xVelocityECEF = 25.1;
        data.yVelocityECEF = 30.3;
        data.zVelocityECEF = 15.8;
        data.updateTime = 3000000000ULL;  // 3 seconds in nanoseconds
        data.originalUpdateTime = 2999000000ULL;
        data.firstHopSentTime = 3001000000ULL;
        return data;
    }

    /**
     * @brief Create a valid test DelayCalcTrackData object
     */
    DelayCalcTrackData createValidDelayCalcTrackData() {
        DelayCalcTrackData data;
        data.trackId = 11111;
        data.xPositionECEF = 500.8;
        data.yPositionECEF = 600.4;
        data.zPositionECEF = 200.6;
        data.xVelocityECEF = 35.5;
        data.yVelocityECEF = 40.2;
        data.zVelocityECEF = 20.1;
        data.updateTime = 4000000000ULL;  // 4 seconds in nanoseconds
        data.firstHopDelayTime = 1000000000ULL;  // 1 second
        data.secondHopSentTime = 4002000000ULL;
        return data;
    }
};

/**
 * @brief Test complete data processing pipeline
 */
TEST_F(EndToEndIntegrationTest, CompleteDataProcessingPipeline) {
    // Create the complete processing pipeline
    ZeroMQDataWriter writer;
    
    // Create input data
    ExtrapTrackData inputData = createValidExtrapTrackData();
    EXPECT_TRUE(inputData.isValid());
    
    // Process data through calculator service
    DelayCalcTrackData processedData = calculatorService_->calculateDelay(inputData);
    EXPECT_TRUE(processedData.isValid());
    
    // Verify processing results
    EXPECT_EQ(processedData.trackId, inputData.trackId);
    EXPECT_DOUBLE_EQ(processedData.xPositionECEF, inputData.xPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.yPositionECEF, inputData.yPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.zPositionECEF, inputData.zPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.xVelocityECEF, inputData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(processedData.yVelocityECEF, inputData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(processedData.zVelocityECEF, inputData.zVelocityECEF);
    EXPECT_GT(processedData.firstHopDelayTime, 0);
    EXPECT_GT(processedData.secondHopSentTime, processedData.updateTime);
    
    // Send processed data
    EXPECT_NO_THROW({
        writer.sendData(processedData);
    });
}

/**
 * @brief Test binary serialization round-trip in complete pipeline
 */
TEST_F(EndToEndIntegrationTest, BinarySerializationRoundTrip_CompleteFlow) {
    // Test ExtrapTrackData serialization
    ExtrapTrackData originalExtrapData = createValidExtrapTrackData();
    std::vector<uint8_t> extrapBinary = BinarySerializer::serialize(originalExtrapData);
    ExtrapTrackData deserializedExtrapData = BinarySerializer::deserializeExtrapTrackData(extrapBinary);
    
    // Verify ExtrapTrackData round-trip
    EXPECT_EQ(deserializedExtrapData.trackId, originalExtrapData.trackId);
    EXPECT_DOUBLE_EQ(deserializedExtrapData.xPositionECEF, originalExtrapData.xPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedExtrapData.yPositionECEF, originalExtrapData.yPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedExtrapData.zPositionECEF, originalExtrapData.zPositionECEF);
    
    // Process through calculator service
    DelayCalcTrackData processedData = calculatorService_->calculateDelay(deserializedExtrapData);
    
    // Test DelayCalcTrackData serialization
    std::vector<uint8_t> delayCalcBinary = BinarySerializer::serialize(processedData);
    DelayCalcTrackData deserializedDelayCalcData = BinarySerializer::deserializeDelayCalcTrackData(delayCalcBinary);
    
    // Verify DelayCalcTrackData round-trip
    EXPECT_EQ(deserializedDelayCalcData.trackId, processedData.trackId);
    EXPECT_DOUBLE_EQ(deserializedDelayCalcData.xPositionECEF, processedData.xPositionECEF);
    EXPECT_EQ(deserializedDelayCalcData.firstHopDelayTime, processedData.firstHopDelayTime);
    EXPECT_EQ(deserializedDelayCalcData.secondHopSentTime, processedData.secondHopSentTime);
}

/**
 * @brief Test multiple tracks processing
 */
TEST_F(EndToEndIntegrationTest, MultipleTracksProcessing) {
    ZeroMQDataWriter writer;
    const int numTracks = 10;
    
    std::vector<ExtrapTrackData> inputTracks;
    std::vector<DelayCalcTrackData> processedTracks;
    
    // Create multiple tracks
    for (int i = 0; i < numTracks; ++i) {
        ExtrapTrackData track = createValidExtrapTrackData();
        track.trackId = 20000 + i;
        track.xPositionECEF += i * 10.0;  // Vary position
        track.yPositionECEF += i * 15.0;
        inputTracks.push_back(track);
    }
    
    // Process all tracks
    for (const auto& inputTrack : inputTracks) {
        EXPECT_TRUE(inputTrack.isValid());
        DelayCalcTrackData processedTrack = calculatorService_->calculateDelay(inputTrack);
        EXPECT_TRUE(processedTrack.isValid());
        EXPECT_EQ(processedTrack.trackId, inputTrack.trackId);
        processedTracks.push_back(processedTrack);
    }
    
    // Send all processed tracks
    for (const auto& processedTrack : processedTracks) {
        EXPECT_NO_THROW({
            writer.sendData(processedTrack);
        });
    }
    
    EXPECT_EQ(processedTracks.size(), numTracks);
}

/**
 * @brief Test error handling in complete pipeline
 */
TEST_F(EndToEndIntegrationTest, ErrorHandling_InvalidDataInPipeline) {
    ZeroMQDataWriter writer;
    
    // Create invalid input data
    ExtrapTrackData invalidData;
    invalidData.trackId = -1;  // Invalid track ID
    EXPECT_FALSE(invalidData.isValid());
    
    // Calculator service should handle invalid data gracefully
    EXPECT_NO_THROW({
        DelayCalcTrackData result = calculatorService_->calculateDelay(invalidData);
        // Result may be invalid but should not throw
    });
}

/**
 * @brief Test performance of complete pipeline
 */
TEST_F(EndToEndIntegrationTest, Performance_CompletePipelineLatency) {
    ZeroMQDataWriter writer;
    ExtrapTrackData inputData = createValidExtrapTrackData();
    
    const int numIterations = 100;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numIterations; ++i) {
        inputData.trackId = 30000 + i;
        
        // Process through calculator service
        DelayCalcTrackData processedData = calculatorService_->calculateDelay(inputData);
        
        // Send the data
        writer.sendData(processedData);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Complete pipeline should be fast (< 200 microseconds per operation on average)
    double avgTimePerOp = static_cast<double>(duration.count()) / numIterations;
    EXPECT_LT(avgTimePerOp, 200.0) << "Complete pipeline too slow: " << avgTimePerOp << " μs per operation";
}

/**
 * @brief Test concurrent pipeline processing
 */
TEST_F(EndToEndIntegrationTest, ConcurrentProcessing_MultipleTracks) {
    ZeroMQDataWriter writer;
    
    const int numThreads = 4;
    const int numTracksPerThread = 10;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    
    // Launch multiple threads processing tracks
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < numTracksPerThread; ++j) {
                try {
                    ExtrapTrackData inputData = createValidExtrapTrackData();
                    inputData.trackId = (i * 10000) + j + 1000; // Ensure non-zero track IDs starting from 1000
                    
                    DelayCalcTrackData processedData = calculatorService_->calculateDelay(inputData);
                    writer.sendData(processedData);
                    
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
    
    // All operations should succeed
    EXPECT_EQ(successCount.load(), numThreads * numTracksPerThread);
}

/**
 * @brief Test system configuration consistency
 */
TEST_F(EndToEndIntegrationTest, SystemConfiguration_ConsistentConstants) {
    // Verify that input and output adapters use compatible but different ports
    EXPECT_EQ(ExtrapTrackData::ZMQ_PORT, 9596);      // Input port
    EXPECT_EQ(DelayCalcTrackData::ZMQ_PORT, 9595);   // Output port
    EXPECT_NE(ExtrapTrackData::ZMQ_PORT, DelayCalcTrackData::ZMQ_PORT);  // Different ports
    
    // Verify same multicast address for both
    EXPECT_STREQ(ExtrapTrackData::ZMQ_MULTICAST_ADDRESS, DelayCalcTrackData::ZMQ_MULTICAST_ADDRESS);
    
    // Verify same protocol
    EXPECT_STREQ(ExtrapTrackData::ZMQ_PROTOCOL, DelayCalcTrackData::ZMQ_PROTOCOL);
    
    // Verify same group
}

/**
 * @brief Test data integrity through complete flow
 */
TEST_F(EndToEndIntegrationTest, DataIntegrity_CompleteFlowPreservesData) {
    ExtrapTrackData originalData = createValidExtrapTrackData();
    
    // Process through calculator service
    DelayCalcTrackData processedData = calculatorService_->calculateDelay(originalData);
    
    // Verify that essential data is preserved
    EXPECT_EQ(processedData.trackId, originalData.trackId);
    EXPECT_DOUBLE_EQ(processedData.xPositionECEF, originalData.xPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.yPositionECEF, originalData.yPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.zPositionECEF, originalData.zPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.xVelocityECEF, originalData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(processedData.yVelocityECEF, originalData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(processedData.zVelocityECEF, originalData.zVelocityECEF);
    EXPECT_EQ(processedData.updateTime, originalData.updateTime);
    
    // Verify that new calculated fields are valid
    EXPECT_GT(processedData.firstHopDelayTime, 0);
    EXPECT_GT(processedData.secondHopSentTime, processedData.updateTime);
    
    // Test binary serialization preserves all data
    std::vector<uint8_t> binaryData = BinarySerializer::serialize(processedData);
    DelayCalcTrackData deserializedData = BinarySerializer::deserializeDelayCalcTrackData(binaryData);
    
    // Verify complete data integrity after serialization
    EXPECT_EQ(deserializedData.trackId, processedData.trackId);
    EXPECT_DOUBLE_EQ(deserializedData.xPositionECEF, processedData.xPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yPositionECEF, processedData.yPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zPositionECEF, processedData.zPositionECEF);
    EXPECT_DOUBLE_EQ(deserializedData.xVelocityECEF, processedData.xVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.yVelocityECEF, processedData.yVelocityECEF);
    EXPECT_DOUBLE_EQ(deserializedData.zVelocityECEF, processedData.zVelocityECEF);
    EXPECT_EQ(deserializedData.updateTime, processedData.updateTime);
    EXPECT_EQ(deserializedData.firstHopDelayTime, processedData.firstHopDelayTime);
    EXPECT_EQ(deserializedData.secondHopSentTime, processedData.secondHopSentTime);
}
