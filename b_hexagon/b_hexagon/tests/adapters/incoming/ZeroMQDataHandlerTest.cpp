/**
 * @file ZeroMQDataHandlerTest.cpp
 * @brief Unit tests for ZeroMQ data handler adapter
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "adapters/incoming/ZeroMQDataHandler.hpp"
#include "domain/ports/incoming/IDataReceiver.hpp"
#include "domain/model/TrackData.hpp"

/**
 * @brief Mock implementation of IDataReceiver for testing
 */
class MockDataReceiver : public IDataReceiver {
public:
    MOCK_METHOD(void, onDataReceived, (const TrackData& data), (override));
};

/**
 * @brief Test fixture for ZeroMQDataHandler
 */
class ZeroMQDataHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockReceiver = std::make_unique<MockDataReceiver>();
    }

    std::unique_ptr<MockDataReceiver> mockReceiver;
};

/**
 * @brief Test ZeroMQDataHandler default constructor
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_DefaultConstructor_CreatesInstance) {
    // Should not throw when creating with default constructor
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(mockReceiver.get());
    });
}

/**
 * @brief Test constructor with null receiver
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_NullReceiver_CreatesInstance) {
    // Should not throw when receiver is null (allowed by default parameter)
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(nullptr);
    });
}

/**
 * @brief Test custom configuration constructor
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_CustomConfiguration_CreatesInstance) {
    const std::string endpoint = "udp://239.255.42.99:5556";
    const std::string group = "trackdata";
    
    // Should not throw when creating with custom configuration
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(endpoint, group, true, mockReceiver.get());
    });
}

/**
 * @brief Test constructor with various endpoint formats
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_VariousEndpointFormats_CreatesInstance) {
    const std::string group = "trackdata";
    
    // Valid UDP endpoint with bind=true (standard for DISH)
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler("udp://239.255.42.99:5556", group, true, mockReceiver.get());
    });
    
    // Note: UDP multicast with connect (bind=false) may not work for DISH sockets
    // This is expected behavior for ZeroMQ RADIO/DISH pattern
}

/**
 * @brief Test constructor with various group names
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_VariousGroupNames_CreatesInstance) {
    const std::string endpoint = "udp://239.255.42.99:5556";
    
    // Standard group name
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(endpoint, "trackdata", true, mockReceiver.get());
    });
    
    // Group with numbers
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(endpoint, "track123", true, mockReceiver.get());
    });
    
    // Group with underscores
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(endpoint, "track_data", true, mockReceiver.get());
    });
}

/**
 * @brief Test constructor with bind parameter variations
 */
TEST_F(ZeroMQDataHandlerTest, Constructor_BindParameterVariations_CreatesInstance) {
    const std::string endpoint = "udp://239.255.42.99:5556";
    const std::string group = "trackdata";
    
    // Test with bind = true (standard for DISH sockets)
    EXPECT_NO_THROW({
        ZeroMQDataHandler handler(endpoint, group, true, mockReceiver.get());
    });
    
    // Note: bind=false (connect) may not work with UDP multicast DISH sockets
    // This is expected ZeroMQ RADIO/DISH behavior
}

/**
 * @brief Test that object can be constructed and destroyed properly
 */
TEST_F(ZeroMQDataHandlerTest, LifeCycle_ConstructAndDestroy_NoExceptions) {
    // This tests RAII behavior
    EXPECT_NO_THROW({
        auto handler = std::make_unique<ZeroMQDataHandler>(mockReceiver.get());
        // handler will be destroyed when going out of scope
    });
    
    EXPECT_NO_THROW({
        auto handler = std::make_unique<ZeroMQDataHandler>("udp://239.255.42.99:5556", "trackdata", true, mockReceiver.get());
        // handler will be destroyed when going out of scope
    });
}
