/**
 * @file ZeroMQDataWriterTest.cpp
 * @brief Unit tests for ZeroMQ data writer adapter
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "adapters/outgoing/ZeroMQDataWriter.hpp"
#include "domain/ports/outgoing/IDataWriter.hpp"
#include "domain/model/DelayCalcTrackData.hpp"

/**
 * @brief Test fixture for ZeroMQDataWriter
 */
class ZeroMQDataWriterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test data with correct field names
        testData.trackId = 12345;
        testData.xPositionECEF = 100.5;
        testData.yPositionECEF = 200.75;
        testData.zPositionECEF = 300.25;
        testData.firstHopSentTime = 1723623744000000000L;
        testData.firstHopDelayTime = 1000000L; // 1ms delay
        testData.secondHopSentTime = 1723623744001000000L;
    }

    DelayCalcTrackData testData;
};

/**
 * @brief Test ZeroMQDataWriter default constructor
 */
TEST_F(ZeroMQDataWriterTest, Constructor_DefaultConstructor_CreatesInstance) {
    // Should not throw when creating with default constructor
    EXPECT_NO_THROW({
        ZeroMQDataWriter sender;
    });
}

/**
 * @brief Test custom configuration constructor
 */
TEST_F(ZeroMQDataWriterTest, Constructor_CustomConfiguration_CreatesInstance) {
    const std::string endpoint = "udp://239.255.42.99:5556";
    const std::string group = "trackdata";
    
    // RADIO sockets typically connect (bind=false) rather than bind
    EXPECT_NO_THROW({
        ZeroMQDataWriter sender(endpoint, group, false);
    });
}

/**
 * @brief Test constructor with various endpoint formats
 */
TEST_F(ZeroMQDataSenderTest, Constructor_VariousEndpointFormats_CreatesInstance) {
    const std::string group = "trackdata";
    
    // RADIO sockets work with connect (bind=false)
    EXPECT_NO_THROW({
        ZeroMQDataSender sender("udp://239.255.42.99:5556", group, false);
    });
}

/**
 * @brief Test constructor with various group names
 */
TEST_F(ZeroMQDataSenderTest, Constructor_VariousGroupNames_CreatesInstance) {
    const std::string endpoint = "udp://239.255.42.99:5556";
    
    // Test different group names with connect mode
    EXPECT_NO_THROW({
        ZeroMQDataSender sender(endpoint, "trackdata", false);
    });
    
    EXPECT_NO_THROW({
        ZeroMQDataSender sender(endpoint, "track123", false);
    });
    
    EXPECT_NO_THROW({
        ZeroMQDataSender sender(endpoint, "track_data", false);
    });
}

/**
 * @brief Test interface compliance
 */
TEST_F(ZeroMQDataWriterTest, InterfaceCompliance_ImplementsIDataWriter_CorrectlyTyped) {
    // Test that ZeroMQDataWriter can be used as IDataWriter
    EXPECT_NO_THROW({
        std::unique_ptr<IDataWriter> sender = std::make_unique<ZeroMQDataWriter>();
        // Verify it's the correct type
        EXPECT_NE(sender.get(), nullptr);
    });
}

/**
 * @brief Test that object can be constructed and destroyed properly
 */
TEST_F(ZeroMQDataWriterTest, LifeCycle_ConstructAndDestroy_NoExceptions) {
    // This tests RAII behavior
    EXPECT_NO_THROW({
        auto sender = std::make_unique<ZeroMQDataSender>();
        // sender will be destroyed when going out of scope
    });
    
    EXPECT_NO_THROW({
        auto sender = std::make_unique<ZeroMQDataSender>("udp://239.255.42.99:5556", "trackdata", false);
        // sender will be destroyed when going out of scope
    });
}
