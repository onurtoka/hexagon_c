/**
 * @file JsonConfigReader.cpp
 * @brief JSON configuration reader implementation
 */

#include "common/JsonConfigReader.hpp"
#include "common/Logger.hpp"
#include <fstream>
#include <stdexcept>
#include <sstream>

ServiceConfig JsonConfigReader::readServiceConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    if (jsonContent.empty()) {
        throw std::runtime_error("Configuration file is empty: " + filePath);
    }
    
    return parseServiceConfig(jsonContent);
}

ServiceConfig JsonConfigReader::parseServiceConfig(const std::string& jsonContent) {
    try {
        // Find the x-service-metadata section
        std::string metadataSection = findServiceMetadata(jsonContent);
        
        if (metadataSection.empty()) {
            throw std::runtime_error("x-service-metadata section not found in JSON");
        }
        
        // Extract configuration values
        ServiceConfig config;
        config.protocol = extractStringValue(metadataSection, "protocol");
        config.multicast_address = extractStringValue(metadataSection, "multicast_address");
        config.port = extractIntValue(metadataSection, "port");
        
        // Validate required fields
        if (config.protocol.empty()) {
            throw std::runtime_error("Missing or empty 'protocol' in x-service-metadata");
        }
        if (config.multicast_address.empty()) {
            throw std::runtime_error("Missing or empty 'multicast_address' in x-service-metadata");
        }
        if (config.port <= 0) {
            throw std::runtime_error("Invalid or missing 'port' in x-service-metadata");
        }
        
        Logger::info("Loaded service config: " + config.getEndpoint());
        
        return config;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse service configuration: " + std::string(e.what()));
    }
}

std::string JsonConfigReader::findServiceMetadata(const std::string& jsonContent) {
    // Find the x-service-metadata section
    const std::string metadataKey = "\"x-service-metadata\"";
    size_t keyPos = jsonContent.find(metadataKey);
    
    if (keyPos == std::string::npos) {
        return "";
    }
    
    // Find the opening brace after the key
    size_t colonPos = jsonContent.find(':', keyPos);
    if (colonPos == std::string::npos) {
        return "";
    }
    
    size_t bracePos = jsonContent.find('{', colonPos);
    if (bracePos == std::string::npos) {
        return "";
    }
    
    // Find the matching closing brace
    int braceCount = 1;
    size_t pos = bracePos + 1;
    size_t endPos = bracePos;
    
    while (pos < jsonContent.length() && braceCount > 0) {
        if (jsonContent[pos] == '{') {
            braceCount++;
        } else if (jsonContent[pos] == '}') {
            braceCount--;
            if (braceCount == 0) {
                endPos = pos;
                break;
            }
        }
        pos++;
    }
    
    if (braceCount != 0) {
        return "";
    }
    
    // Extract the metadata section including braces
    return jsonContent.substr(bracePos, endPos - bracePos + 1);
}

std::string JsonConfigReader::extractStringValue(const std::string& json, const std::string& key) {
    // Find the key in quotes
    std::string quotedKey = "\"" + key + "\"";
    size_t keyPos = json.find(quotedKey);
    
    if (keyPos == std::string::npos) {
        return "";
    }
    
    // Find the colon after the key
    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) {
        return "";
    }
    
    // Skip whitespace after colon
    size_t pos = colonPos + 1;
    while (pos < json.length() && std::isspace(json[pos])) {
        pos++;
    }
    
    // Expect opening quote
    if (pos >= json.length() || json[pos] != '"') {
        return "";
    }
    pos++; // Skip opening quote
    
    // Find closing quote
    size_t startPos = pos;
    while (pos < json.length() && json[pos] != '"') {
        if (json[pos] == '\\') {
            pos++; // Skip escaped character
        }
        pos++;
    }
    
    if (pos >= json.length()) {
        return "";
    }
    
    return json.substr(startPos, pos - startPos);
}

int JsonConfigReader::extractIntValue(const std::string& json, const std::string& key) {
    // Find the key in quotes
    std::string quotedKey = "\"" + key + "\"";
    size_t keyPos = json.find(quotedKey);
    
    if (keyPos == std::string::npos) {
        return 0;
    }
    
    // Find the colon after the key
    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) {
        return 0;
    }
    
    // Skip whitespace after colon
    size_t pos = colonPos + 1;
    while (pos < json.length() && std::isspace(json[pos])) {
        pos++;
    }
    
    // Extract number
    size_t startPos = pos;
    while (pos < json.length() && (std::isdigit(json[pos]) || json[pos] == '-')) {
        pos++;
    }
    
    if (pos == startPos) {
        return 0;
    }
    
    try {
        return std::stoi(json.substr(startPos, pos - startPos));
    } catch (const std::exception&) {
        return 0;
    }
}
