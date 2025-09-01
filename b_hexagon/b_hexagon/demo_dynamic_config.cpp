/**
 * @file demo_dynamic_config.cpp
 * @brief Demonstration of dynamic configuration reading
 */

#include "common/JsonConfigReader.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

void createTestConfig(const std::string& protocol, const std::string& address, int port) {
    std::ofstream file("demo_config.json");
    file << R"({
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "DemoConfig",
  "x-service-metadata": {
    "description": "Demo configuration for dynamic testing",
    "protocol": ")" << protocol << R"(",
    "multicast_address": ")" << address << R"(",
    "port": )" << port << R"(
  },
  "properties": {
    "trackId": {
      "type": "integer"
    }
  }
})";
    file.close();
}

int main() {
    try {
        std::cout << "=== Dynamic Configuration Demo ===" << std::endl;
        
        // Test 1: Read original ExtrapTrackData.json
        std::cout << "\n1. Reading original ExtrapTrackData.json:" << std::endl;
        ServiceConfig config1 = JsonConfigReader::readServiceConfig("src/zmq_messages/ExtrapTrackData.json");
        std::cout << "   Endpoint: " << config1.getEndpoint() << std::endl;
        
        // Test 2: Create and read a different configuration
        std::cout << "\n2. Creating and reading modified configuration:" << std::endl;
        createTestConfig("udp", "239.2.2.10", 8080);
        ServiceConfig config2 = JsonConfigReader::readServiceConfig("demo_config.json");
        std::cout << "   Endpoint: " << config2.getEndpoint() << std::endl;
        
        // Test 3: Create another different configuration
        std::cout << "\n3. Creating and reading another configuration:" << std::endl;
        createTestConfig("udp", "224.0.0.100", 5555);
        ServiceConfig config3 = JsonConfigReader::readServiceConfig("demo_config.json");
        std::cout << "   Endpoint: " << config3.getEndpoint() << std::endl;
        
        std::cout << "\n✓ Demonstration complete!" << std::endl;
        std::cout << "The ZeroMQDataHandler will use whatever configuration is in the JSON file." << std::endl;
        std::cout << "No code changes needed when configuration changes!" << std::endl;
        
        // Cleanup
        if (std::filesystem::exists("demo_config.json")) {
            std::filesystem::remove("demo_config.json");
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
