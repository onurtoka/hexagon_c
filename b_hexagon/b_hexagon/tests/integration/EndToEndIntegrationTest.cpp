/**
 * @file EndToEndIntegrationTest.cpp
 * @brief Integration tests for complete data processing pipeline
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "adapters/incoming/ZeroMQDataHandler.hpp"
#include "adapters/outgoing/ZeroMQDataSender.hpp"
#include "domain/logic/CalculatorService.hpp"
#include "domain/model/TrackData.hpp"
#include "domain/model/DelayCalculatedTrackData.hpp"
#include <memory>
#include <chrono>

/**
 * @brief Mock data receiver for integration testing
 */
class IntegrationTestReceiver : public IDataReceiver {
public:
    void onDataReceived(const TrackData& data) override {
        receivedData = data;
        dataReceived = true;
    }

    TrackData receivedData;
    bool dataReceived = false;
};

/**
 * @brief Test fixture for integration tests
 */
class EndToEndIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        receiver = std::make_unique<IntegrationTestReceiver>();
        calculator = std::make_unique<CalculatorService>();
        
        // Setup test data with correct field names
        testData.trackId = 12345;
        testData.xPositionECEF = 100.5;
        testData.yPositionECEF = 200.75;
        testData.zPositionECEF = 300.25;
        testData.firstHopSentTime = 1723623744000000000L;
    }

    std::unique_ptr<IntegrationTestReceiver> receiver;
    std::unique_ptr<CalculatorService> calculator;
    TrackData testData;
};

/**
 * @brief Test complete data processing pipeline
 */
TEST_F(EndToEndIntegrationTest, CompleteDataPipeline_ValidData_ProcessesCorrectly) {
    // Test calculator service processing
    DelayCalculatedTrackData processedData = calculator->calculateDelay(testData);
    
    // Verify processing worked
    EXPECT_EQ(processedData.trackId, testData.trackId);
    EXPECT_DOUBLE_EQ(processedData.xPositionECEF, testData.xPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.yPositionECEF, testData.yPositionECEF);
    EXPECT_DOUBLE_EQ(processedData.zPositionECEF, testData.zPositionECEF);
    EXPECT_EQ(processedData.firstHopSentTime, testData.firstHopSentTime);
    
    // Verify delay calculations were performed
    EXPECT_GT(processedData.firstHopDelayTime, 0);
    EXPECT_GT(processedData.secondHopSentTime, processedData.firstHopSentTime);
}

/**
 * @brief Test adapter construction and destruction
 */
TEST_F(EndToEndIntegrationTest, AdapterLifecycle_CreateAndDestroy_NoExceptions) {
    // Test ZeroMQ handlers can be created and destroyed
    EXPECT_NO_THROW({
        auto handler = std::make_unique<ZeroMQDataHandler>(receiver.get());
        auto sender = std::make_unique<ZeroMQDataSender>();
        
        // Both should be properly constructed
        EXPECT_NE(handler.get(), nullptr);
        EXPECT_NE(sender.get(), nullptr);
    });
}

/**
 * @brief Test calculator service with various input data
 */
TEST_F(EndToEndIntegrationTest, CalculatorService_VariousInputs_ProcessesCorrectly) {
    // Test with zero values
    TrackData zeroData;
    zeroData.trackId = 1;
    zeroData.firstHopSentTime = 0;
    
    EXPECT_NO_THROW({
        DelayCalculatedTrackData result = calculator->calculateDelay(zeroData);
        EXPECT_EQ(result.trackId, 1);
        EXPECT_GE(result.firstHopDelayTime, 0);
    });
    
    // Test with large values
    TrackData largeData;
    largeData.trackId = 999999;
    largeData.xPositionECEF = 1000000.0;
    largeData.yPositionECEF = -1000000.0;
    largeData.zPositionECEF = 500000.0;
    largeData.firstHopSentTime = 9223372036854775000L; // Large timestamp
    
    EXPECT_NO_THROW({
        DelayCalculatedTrackData result = calculator->calculateDelay(largeData);
        EXPECT_EQ(result.trackId, 999999);
        EXPECT_DOUBLE_EQ(result.xPositionECEF, 1000000.0);
    });
}

/**
 * @brief Test timing consistency across components
 */
TEST_F(EndToEndIntegrationTest, TimingConsistency_ProcessingTimes_AreConsistent) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Process data multiple times
    constexpr int iterations = 5;
    std::vector<long> processingTimes;
    
    for (int i = 0; i < iterations; ++i) {
        auto iterStart = std::chrono::high_resolution_clock::now();
        
        DelayCalculatedTrackData result = calculator->calculateDelay(testData);
        
        auto iterEnd = std::chrono::high_resolution_clock::now();
        long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(iterEnd - iterStart).count();
        processingTimes.push_back(duration);
        
        // Verify result is valid
        EXPECT_EQ(result.trackId, testData.trackId);
        EXPECT_GT(result.secondHopSentTime, result.firstHopSentTime);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // Processing should be reasonably fast
    EXPECT_LT(totalTime, 1000000000L); // Less than 1 second for all iterations
    
    // Individual processing times should be reasonable
    for (long time : processingTimes) {
        EXPECT_LT(time, 100000000L); // Less than 100ms per operation
    }
}
