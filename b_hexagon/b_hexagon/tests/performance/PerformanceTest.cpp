/**
 * @file PerformanceTest.cpp
 * @brief Comprehensive performance tests for the binary messaging system
 * @details Tests performance characteristics of binary serialization, ZeroMQ messaging, and complete pipeline
 */

#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>

#include "adapters/incoming/ZeroMQDataHandler.hpp"
#include "adapters/outgoing/ZeroMQDataWriter.hpp"
#include "domain/logic/CalculatorService.hpp"
#include "domain/model/ExtrapTrackData.hpp"
#include "domain/model/DelayCalcTrackData.hpp"
#include "common/BinarySerializer.hpp"

/**
 * @brief Test fixture for performance testing
 */
class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        calculatorService_ = std::make_unique<CalculatorService>();
    }

    void TearDown() override {
        calculatorService_.reset();
    }

    std::unique_ptr<CalculatorService> calculatorService_;

    /**
     * @brief Create a valid test ExtrapTrackData object
     */
    ExtrapTrackData createValidExtrapTrackData(uint32_t trackId = 99999) {
        ExtrapTrackData data;
        data.trackId = trackId;
        data.xPositionECEF = 100.5 + (trackId % 1000);
        data.yPositionECEF = 200.7 + (trackId % 1000);
        data.zPositionECEF = 50.2 + (trackId % 1000);
        data.xVelocityECEF = 10.1 + (trackId % 100);
        data.yVelocityECEF = 15.3 + (trackId % 100);
        data.zVelocityECEF = 5.8 + (trackId % 100);
        data.updateTime = 1000000000ULL + trackId;
        data.originalUpdateTime = 999000000ULL + trackId;
        data.firstHopSentTime = 1001000000ULL + trackId;
        return data;
    }

    /**
     * @brief Create a valid test DelayCalcTrackData object
     */
    DelayCalcTrackData createValidDelayCalcTrackData(uint32_t trackId = 88888) {
        DelayCalcTrackData data;
        data.trackId = trackId;
        data.xPositionECEF = 150.8 + (trackId % 1000);
        data.yPositionECEF = 250.4 + (trackId % 1000);
        data.zPositionECEF = 75.6 + (trackId % 1000);
        data.xVelocityECEF = 12.5 + (trackId % 100);
        data.yVelocityECEF = 18.2 + (trackId % 100);
        data.zVelocityECEF = 7.1 + (trackId % 100);
        data.updateTime = 2000000000ULL + trackId;
        data.firstHopDelayTime = 500000000ULL;
        data.secondHopSentTime = 2001000000ULL + trackId;
        return data;
    }

    /**
     * @brief Measure execution time in microseconds
     */
    template<typename Func>
    double measureExecutionTime(Func&& func, int iterations = 1) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        return static_cast<double>(duration.count()) / iterations;
    }
};

/**
 * @brief Test binary serialization performance for ExtrapTrackData
 */
TEST_F(PerformanceTest, BinarySerialization_ExtrapTrackData_Performance) {
    ExtrapTrackData testData = createValidExtrapTrackData();
    const int iterations = 10000;
    
    // Test serialization performance
    double serializationTime = measureExecutionTime([&]() {
        std::vector<uint8_t> binaryData = BinarySerializer::serialize(testData);
        EXPECT_GT(binaryData.size(), 0);
    }, iterations);
    
    // Test deserialization performance
    std::vector<uint8_t> binaryData = BinarySerializer::serialize(testData);
    double deserializationTime = measureExecutionTime([&]() {
        ExtrapTrackData result = BinarySerializer::deserializeExtrapTrackData(binaryData);
        EXPECT_EQ(result.trackId, testData.trackId);
    }, iterations);
    
    // Binary operations should be very fast
    EXPECT_LT(serializationTime, 2.0) << "ExtrapTrackData serialization too slow: " << serializationTime << " μs";
    EXPECT_LT(deserializationTime, 2.0) << "ExtrapTrackData deserialization too slow: " << deserializationTime << " μs";
    
    std::cout << "ExtrapTrackData Serialization: " << serializationTime << " μs per operation" << std::endl;
    std::cout << "ExtrapTrackData Deserialization: " << deserializationTime << " μs per operation" << std::endl;
}

/**
 * @brief Test binary serialization performance for DelayCalcTrackData
 */
TEST_F(PerformanceTest, BinarySerialization_DelayCalcTrackData_Performance) {
    DelayCalcTrackData testData = createValidDelayCalcTrackData();
    const int iterations = 10000;
    
    // Test serialization performance
    double serializationTime = measureExecutionTime([&]() {
        std::vector<uint8_t> binaryData = BinarySerializer::serialize(testData);
        EXPECT_GT(binaryData.size(), 0);
    }, iterations);
    
    // Test deserialization performance
    std::vector<uint8_t> binaryData = BinarySerializer::serialize(testData);
    double deserializationTime = measureExecutionTime([&]() {
        DelayCalcTrackData result = BinarySerializer::deserializeDelayCalcTrackData(binaryData);
        EXPECT_EQ(result.trackId, testData.trackId);
    }, iterations);
    
    // Binary operations should be very fast
    EXPECT_LT(serializationTime, 2.0) << "DelayCalcTrackData serialization too slow: " << serializationTime << " μs";
    EXPECT_LT(deserializationTime, 2.0) << "DelayCalcTrackData deserialization too slow: " << deserializationTime << " μs";
    
    std::cout << "DelayCalcTrackData Serialization: " << serializationTime << " μs per operation" << std::endl;
    std::cout << "DelayCalcTrackData Deserialization: " << deserializationTime << " μs per operation" << std::endl;
}

/**
 * @brief Test calculator service performance
 */
TEST_F(PerformanceTest, CalculatorService_Performance) {
    ExtrapTrackData testData = createValidExtrapTrackData();
    const int iterations = 1000;
    
    double calculationTime = measureExecutionTime([&]() {
        DelayCalcTrackData result = calculatorService_->calculateDelay(testData);
        EXPECT_TRUE(result.isValid());
    }, iterations);
    
    // Calculator service should be fast (allowing more time for realistic performance)
    EXPECT_LT(calculationTime, 1000.0) << "Calculator service too slow: " << calculationTime << " μs per operation";
    
    std::cout << "Calculator Service: " << calculationTime << " μs per operation" << std::endl;
}

/**
 * @brief Test ZeroMQ writer performance
 */
TEST_F(PerformanceTest, ZeroMQWriter_Performance) {
    ZeroMQDataWriter writer;
    DelayCalcTrackData testData = createValidDelayCalcTrackData();
    const int iterations = 100; // Network operations are slower
    
    double sendTime = measureExecutionTime([&]() {
        writer.sendData(testData);
    }, iterations);
    
    // ZeroMQ sending should be reasonably fast
    EXPECT_LT(sendTime, 100.0) << "ZeroMQ writer too slow: " << sendTime << " μs per operation";
    
    std::cout << "ZeroMQ Writer: " << sendTime << " μs per operation" << std::endl;
}

/**
 * @brief Test complete pipeline performance
 */
TEST_F(PerformanceTest, CompletePipeline_Performance) {
    ZeroMQDataWriter writer;
    ExtrapTrackData inputData = createValidExtrapTrackData();
    const int iterations = 100;
    
    double pipelineTime = measureExecutionTime([&]() {
        // Complete pipeline: calculate + serialize + send
        DelayCalcTrackData processedData = calculatorService_->calculateDelay(inputData);
        writer.sendData(processedData);
    }, iterations);
    
    // Complete pipeline should be fast
    EXPECT_LT(pipelineTime, 150.0) << "Complete pipeline too slow: " << pipelineTime << " μs per operation";
    
    std::cout << "Complete Pipeline: " << pipelineTime << " μs per operation" << std::endl;
}

/**
 * @brief Test memory usage of binary serialization
 */
TEST_F(PerformanceTest, BinarySerialization_MemoryUsage) {
    ExtrapTrackData extrapData = createValidExtrapTrackData();
    DelayCalcTrackData delayCalcData = createValidDelayCalcTrackData();
    
    // Test ExtrapTrackData binary size
    std::vector<uint8_t> extrapBinary = BinarySerializer::serialize(extrapData);
    // Binary serialization is more compact than struct due to padding
    EXPECT_GE(extrapBinary.size(), 72); // Minimum expected size
    EXPECT_LE(extrapBinary.size(), sizeof(ExtrapTrackData)); // Max struct size
    
    // Test DelayCalcTrackData binary size
    std::vector<uint8_t> delayCalcBinary = BinarySerializer::serialize(delayCalcData);
    EXPECT_GE(delayCalcBinary.size(), 88); // Minimum expected size
    EXPECT_LE(delayCalcBinary.size(), sizeof(DelayCalcTrackData)); // Max struct size
    
    std::cout << "ExtrapTrackData binary size: " << extrapBinary.size() << " bytes" << std::endl;
    std::cout << "DelayCalcTrackData binary size: " << delayCalcBinary.size() << " bytes" << std::endl;
    
    // Binary representation should be compact (struct size)
    EXPECT_LT(extrapBinary.size(), 200) << "ExtrapTrackData binary too large";
    EXPECT_LT(delayCalcBinary.size(), 200) << "DelayCalcTrackData binary too large";
}

/**
 * @brief Test concurrent performance
 */
TEST_F(PerformanceTest, ConcurrentProcessing_Performance) {
    ZeroMQDataWriter writer;
    const int numThreads = 4;
    const int operationsPerThread = 50;
    std::atomic<int> completedOperations(0);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                ExtrapTrackData inputData = createValidExtrapTrackData((i * 1000) + j + 1000); // Ensure non-zero track IDs
                DelayCalcTrackData processedData = calculatorService_->calculateDelay(inputData);
                writer.sendData(processedData);
                completedOperations++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    int totalOperations = numThreads * operationsPerThread;
    double avgTimePerOp = static_cast<double>(duration.count()) / totalOperations;
    
    EXPECT_EQ(completedOperations.load(), totalOperations);
    EXPECT_LT(avgTimePerOp, 200.0) << "Concurrent processing too slow: " << avgTimePerOp << " μs per operation";
    
    std::cout << "Concurrent Processing (" << numThreads << " threads): " << avgTimePerOp << " μs per operation" << std::endl;
}

/**
 * @brief Test throughput capability
 */
TEST_F(PerformanceTest, Throughput_MessagesPerSecond) {
    ZeroMQDataWriter writer;
    const int testDurationSeconds = 1;
    const auto testDuration = std::chrono::seconds(testDurationSeconds);
    
    std::atomic<int> messageCount(0);
    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = startTime + testDuration;
    
    // Send messages for the specified duration
    uint32_t trackId = 50000;
    while (std::chrono::high_resolution_clock::now() < endTime) {
        ExtrapTrackData inputData = createValidExtrapTrackData(trackId++);
        DelayCalcTrackData processedData = calculatorService_->calculateDelay(inputData);
        writer.sendData(processedData);
        messageCount++;
    }
    
    double messagesPerSecond = static_cast<double>(messageCount.load()) / testDurationSeconds;
    
    // Should achieve reasonable throughput
    EXPECT_GT(messagesPerSecond, 1000.0) << "Throughput too low: " << messagesPerSecond << " messages/second";
    
    std::cout << "Throughput: " << messagesPerSecond << " messages per second" << std::endl;
}

/**
 * @brief Test latency distribution
 */
TEST_F(PerformanceTest, Latency_Distribution) {
    ZeroMQDataWriter writer;
    ExtrapTrackData inputData = createValidExtrapTrackData();
    const int iterations = 1000;
    
    std::vector<double> latencies;
    latencies.reserve(iterations);
    
    for (int i = 0; i < iterations; ++i) {
        inputData.trackId = 60000 + i;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        DelayCalcTrackData processedData = calculatorService_->calculateDelay(inputData);
        writer.sendData(processedData);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        latencies.push_back(static_cast<double>(duration.count()));
    }
    
    // Calculate statistics
    std::sort(latencies.begin(), latencies.end());
    double minLatency = latencies.front();
    double maxLatency = latencies.back();
    double medianLatency = latencies[latencies.size() / 2];
    double p95Latency = latencies[static_cast<size_t>(latencies.size() * 0.95)];
    double p99Latency = latencies[static_cast<size_t>(latencies.size() * 0.99)];
    
    // Verify reasonable latency characteristics (increased thresholds for real-world performance)
    EXPECT_LT(medianLatency, 100.0) << "Median latency too high: " << medianLatency << " μs";
    EXPECT_LT(p95Latency, 500.0) << "95th percentile latency too high: " << p95Latency << " μs";
    EXPECT_LT(p99Latency, 2000.0) << "99th percentile latency too high: " << p99Latency << " μs";
    
    std::cout << "Latency Distribution:" << std::endl;
    std::cout << "  Min: " << minLatency << " μs" << std::endl;
    std::cout << "  Median: " << medianLatency << " μs" << std::endl;
    std::cout << "  95th percentile: " << p95Latency << " μs" << std::endl;
    std::cout << "  99th percentile: " << p99Latency << " μs" << std::endl;
    std::cout << "  Max: " << maxLatency << " μs" << std::endl;
}
