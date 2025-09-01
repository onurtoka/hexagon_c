/**
 * @file JsonConfigReaderTest.cpp
 * @brief Unit tests for JsonConfigReader
 */

#include <gtest/gtest.h>
#include "common/JsonConfigReader.hpp"
#include <fstream>
#include <filesystem>

class JsonConfigReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test config file
        testConfigPath_ = "test_config.json";
        createTestConfigFile();
    }
    
    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testConfigPath_)) {
            std::filesystem::remove(testConfigPath_);
        }
    }
    
    void createTestConfigFile() {
        std::ofstream file(testConfigPath_);
        file << R"({
  "title": "TestConfig",
  "x-service-metadata": {
    "protocol": "udp",
    "multicast_address": "239.1.1.5",
    "port": 9596
  },
  "properties": {
    "trackId": {
      "type": "integer"
    }
  }
})";
        file.close();
    }
    
    std::string testConfigPath_;
};

TEST_F(JsonConfigReaderTest, ReadValidConfiguration) {
    ServiceConfig config = JsonConfigReader::readServiceConfig(testConfigPath_);
    
    EXPECT_EQ(config.protocol, "udp");
    EXPECT_EQ(config.multicast_address, "239.1.1.5");
    EXPECT_EQ(config.port, 9596);
    EXPECT_EQ(config.getEndpoint(), "udp://239.1.1.5:9596");
}

TEST_F(JsonConfigReaderTest, FileNotFound) {
    EXPECT_THROW(
        JsonConfigReader::readServiceConfig("nonexistent.json"),
        std::runtime_error
    );
}

TEST_F(JsonConfigReaderTest, EmptyFile) {
    std::string emptyPath = "empty_config.json";
    std::ofstream file(emptyPath);
    file.close();
    
    EXPECT_THROW(
        JsonConfigReader::readServiceConfig(emptyPath),
        std::runtime_error
    );
    
    std::filesystem::remove(emptyPath);
}

TEST_F(JsonConfigReaderTest, MissingServiceMetadata) {
    std::string noMetadataPath = "no_metadata.json";
    std::ofstream file(noMetadataPath);
    file << R"({"title": "NoMetadata"})";
    file.close();
    
    EXPECT_THROW(
        JsonConfigReader::readServiceConfig(noMetadataPath),
        std::runtime_error
    );
    
    std::filesystem::remove(noMetadataPath);
}

TEST_F(JsonConfigReaderTest, ExtrapTrackDataFormat) {
    // Test with actual ExtrapTrackData.json format
    std::string extrapPath = "extrap_test.json";
    std::ofstream file(extrapPath);
    file << R"({
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "ExtrapTrackData",
  "x-service-metadata": {
    "description": "UDP RADIO/DISH yayınının bağlantı bilgileri.",
    "protocol": "udp",
    "multicast_address": "239.1.1.5",
    "port": 9596
  },
  "properties": {
    "trackId": {
      "type": "integer"
    }
  }
})";
    file.close();
    
    ServiceConfig config = JsonConfigReader::readServiceConfig(extrapPath);
    
    EXPECT_EQ(config.protocol, "udp");
    EXPECT_EQ(config.multicast_address, "239.1.1.5");
    EXPECT_EQ(config.port, 9596);
    
    std::filesystem::remove(extrapPath);
}
