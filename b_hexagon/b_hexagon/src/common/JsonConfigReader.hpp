/**
 * @file JsonConfigReader.hpp
 * @brief JSON configuration reader for service metadata
 */

#pragma once

#include <string>

/**
 * @struct ServiceConfig
 * @brief Configuration structure for ZeroMQ service connection
 */
struct ServiceConfig {
    std::string protocol;
    std::string multicast_address;
    int port;
    
    // Helper method to construct full endpoint
    std::string getEndpoint() const {
        return protocol + "://" + multicast_address + ":" + std::to_string(port);
    }
};

/**
 * @class JsonConfigReader
 * @brief Utility class to read service metadata from JSON schema files
 */
class JsonConfigReader {
public:
    /**
     * @brief Read service configuration from JSON file
     * @param filePath Path to the JSON file containing x-service-metadata
     * @return ServiceConfig structure with connection details
     * @throws std::runtime_error if file cannot be read or parsed
     */
    static ServiceConfig readServiceConfig(const std::string& filePath);

private:
    /**
     * @brief Parse service configuration from JSON content
     * @param jsonContent The JSON content as string
     * @return ServiceConfig structure with connection details
     */
    static ServiceConfig parseServiceConfig(const std::string& jsonContent);
    
    /**
     * @brief Find and extract x-service-metadata section from JSON
     * @param jsonContent The JSON content as string
     * @return JSON string containing only the x-service-metadata section
     */
    static std::string findServiceMetadata(const std::string& jsonContent);
    
    /**
     * @brief Extract string value from JSON content
     * @param json JSON content
     * @param key Key to search for
     * @return String value or empty string if not found
     */
    static std::string extractStringValue(const std::string& json, const std::string& key);
    
    /**
     * @brief Extract integer value from JSON content
     * @param json JSON content
     * @param key Key to search for
     * @return Integer value or 0 if not found
     */
    static int extractIntValue(const std::string& json, const std::string& key);
};
