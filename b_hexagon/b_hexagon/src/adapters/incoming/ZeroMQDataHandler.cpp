/**
 * @file ZeroMQDataHandler.cpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 */

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types

#include "adapters/incoming/ZeroMQDataHandler.hpp"  // Own header
#include "messaging/ContractManager.hpp"            // Contract validation
#include "messaging/messaging.hpp"                  // High-level messaging
#include "common/JsonConfigReader.hpp"              // Configuration reader
#include "common/Logger.hpp"                        // Logging
#include <stdexcept>      // Exception types
#include <cctype>         // Character classification for JSON parsing
#include <cstring>        // Memory operations
#include <filesystem>     // File system operations

// Define socket type constants for RADIO/DISH pattern
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif
#ifndef ZMQ_DISH
#define ZMQ_DISH 15
#endif

// Default constructor - reads configuration from default ExtrapTrackData.json
ZeroMQDataHandler::ZeroMQDataHandler(IDataReceiver* dataReceiver)
    : ZeroMQDataHandler("src/zmq_messages/ExtrapTrackData.json", dataReceiver) {
}

// JSON configuration constructor - reads from specified config file
ZeroMQDataHandler::ZeroMQDataHandler(const std::string& configFilePath,
                                     IDataReceiver* dataReceiver) 
    : context_(1),
      socket_(context_, ZMQ_DISH),
      group_("SOURCE_DATA"),  // Group name matching hexagon_c
      dataReceiver_(dataReceiver),
      contractManager_() {
    
    try {
        ServiceConfig config = loadServiceConfig(configFilePath);
        std::string endpoint = config.getEndpoint();
        
        // Bind and join group using C++ API
        socket_.bind(endpoint);
        socket_.join(group_.c_str());
        
        Logger::info("ZeroMQDataHandler configured from " + 
                     configFilePath + " -> " + endpoint);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("ZeroMQDataHandler JSON config error: " + std::string(e.what()));
    }
    
    // Load JSON schema contracts for message validation
    try {
        contractManager_.load_contracts_from_directory("contracts");
    } catch (const std::exception&) {
        // Continue without contracts if loading fails
    }
}

// Main constructor
ZeroMQDataHandler::ZeroMQDataHandler(const std::string& endpoint,
                                     const std::string& group,
                                     bool bind,
                                     IDataReceiver* dataReceiver)
    : context_(1),
      socket_(context_, ZMQ_DISH),
      group_(group),
      dataReceiver_(dataReceiver),
      contractManager_() {
    
    try {
        if (bind) {
            socket_.bind(endpoint);     // DISH binds to UDP multicast (typical)
        } else {
            socket_.connect(endpoint);  // DISH connects (less common)
        }
        
        // Join the specified group for native group filtering using C++ API
        socket_.join(group.c_str());
        
    } catch (const zmq::error_t& e) {
        throw std::runtime_error("ZeroMQDataHandler: Failed to " + 
            std::string(bind ? "bind to " : "connect to ") + 
            endpoint + " - " + e.what());
    }
    
    // Load JSON schema contracts for message validation
    try {
        contractManager_.load_contracts_from_directory("contracts");
    } catch (const std::exception&) {
        // Continue without contracts if loading fails
    }
}

// Start continuous message reception loop
void ZeroMQDataHandler::startReceiving() {
    while (true) {
        
        // Use C++ API for RADIO/DISH group message handling
        zmq::message_t message;
        
        try {
            // Blocking receive operation using C++ API
            auto result = socket_.recv(message, zmq::recv_flags::none);
            if (!result) {
                continue; // No message received
            }
            
            // Get group identifier from received message
            const char* msg_group = message.group();
            
            // Extract message payload
            std::string payload(static_cast<char*>(message.data()), message.size());
            
            // Verify message group matches our subscribed group
            if (msg_group == nullptr || group_ != std::string(msg_group)) {
                continue;  // Skip messages not for our group
            }
            
            // Contract-based message processing
            if (contractManager_.has_contracts()) {
                // Use contract manager to validate message
                if (!contractManager_.validate_message(payload)) {
                    continue;  // Skip invalid messages
                }
                
                // Parse validated JSON to domain object
                TrackData data = parseJson(payload);
                
                // Notify domain layer
                if (dataReceiver_ != nullptr) {
                    dataReceiver_->onDataReceived(data);
                }
            } else {
                // Fallback to legacy parsing if no contracts loaded
                TrackData data = parseJson(payload);
                if (dataReceiver_ != nullptr) {
                    dataReceiver_->onDataReceived(data);
                }
            }
            
        } catch (const zmq::error_t& e) {
            throw std::runtime_error("ZeroMQ receive failed: " + std::string(e.what()));
        } catch (const std::exception& ex) {
            // Log error but continue processing (don't terminate reception loop)
        }
    }
}

// Parse JSON string to TrackData object
TrackData ZeroMQDataHandler::parseJson(const std::string& json) {
    TrackData data;
    
    // Field validation flags
    bool haveTrackId = false, haveXVel = false, haveYVel = false, haveZVel = false;
    bool haveXPos = false, haveYPos = false, haveZPos = false;
    bool haveUpdate = false, haveOriginal = false, haveFirstHop = false;
    
    // Parse all JSON fields
    parseJsonFields(json, data, haveTrackId, haveXVel, haveYVel, haveZVel,
                    haveXPos, haveYPos, haveZPos, haveUpdate, haveOriginal, haveFirstHop);
    
    // Validate all required fields are present
    if (!haveTrackId || !haveXVel || !haveYVel || !haveZVel ||
        !haveXPos || !haveYPos || !haveZPos || !haveUpdate || !haveOriginal || !haveFirstHop) {
        throw std::runtime_error("ZeroMQDataHandler::parseJson: Missing required fields in JSON");
    }
    
    return data;
}

// Skip whitespace characters in JSON
void ZeroMQDataHandler::skipWhitespace(const std::string& json, std::size_t& pos) noexcept {
    while (pos < json.length() && std::isspace(static_cast<unsigned char>(json[pos]))) {
        ++pos;
    }
}

// Extract quoted string value from JSON
std::string ZeroMQDataHandler::extractJsonString(const std::string& json, std::size_t& pos) {
    if (pos >= json.length() || json[pos] != '"') {
        throw std::runtime_error("Expected quoted string in JSON");
    }
    
    ++pos; // Skip opening quote
    std::size_t start = pos;
    
    // Find closing quote
    while (pos < json.length() && json[pos] != '"') {
        if (json[pos] == '\\') {
            ++pos; // Skip escaped character
        }
        ++pos;
    }
    
    if (pos >= json.length()) {
        throw std::runtime_error("Unterminated string in JSON");
    }
    
    std::string result = json.substr(start, pos - start);
    ++pos; // Skip closing quote
    return result;
}

// Parse JSON fields into TrackData object
void ZeroMQDataHandler::parseJsonFields(const std::string& json, TrackData& data,
                                        bool& haveTrackId, bool& haveXVel, bool& haveYVel, bool& haveZVel,
                                        bool& haveXPos, bool& haveYPos, bool& haveZPos,
                                        bool& haveUpdate, bool& haveOriginal, bool& haveFirstHop) noexcept {
    
    std::size_t pos = 0;
    skipWhitespace(json, pos);
    
    if (pos >= json.length() || json[pos] != '{') {
        return; // Invalid JSON object
    }
    
    ++pos; // Skip opening brace
    
    while (pos < json.length()) {
        skipWhitespace(json, pos);
        
        if (pos >= json.length()) break;
        if (json[pos] == '}') break; // End of object
        
        // Extract field name
        if (json[pos] != '"') {
            ++pos;
            continue;
        }
        
        try {
            std::string fieldName = extractJsonString(json, pos);
            
            skipWhitespace(json, pos);
            if (pos >= json.length() || json[pos] != ':') {
                ++pos;
                continue;
            }
            ++pos; // Skip colon
            
            skipWhitespace(json, pos);
            
            // Parse field values based on field name
            if (fieldName == "trackId") {
                data.trackId = std::stoul(json.substr(pos));
                haveTrackId = true;
            } else if (fieldName == "xVelocityECEF") {
                data.xVelocityECEF = std::stod(json.substr(pos));
                haveXVel = true;
            } else if (fieldName == "yVelocityECEF") {
                data.yVelocityECEF = std::stod(json.substr(pos));
                haveYVel = true;
            } else if (fieldName == "zVelocityECEF") {
                data.zVelocityECEF = std::stod(json.substr(pos));
                haveZVel = true;
            } else if (fieldName == "xPositionECEF") {
                data.xPositionECEF = std::stod(json.substr(pos));
                haveXPos = true;
            } else if (fieldName == "yPositionECEF") {
                data.yPositionECEF = std::stod(json.substr(pos));
                haveYPos = true;
            } else if (fieldName == "zPositionECEF") {
                data.zPositionECEF = std::stod(json.substr(pos));
                haveZPos = true;
            } else if (fieldName == "updateTime") {
                data.updateTime = std::stoull(json.substr(pos));
                haveUpdate = true;
            } else if (fieldName == "originalUpdateTime") {
                data.originalUpdateTime = std::stoull(json.substr(pos));
                haveOriginal = true;
            } else if (fieldName == "firstHopSentTime") {
                data.firstHopSentTime = std::stoull(json.substr(pos));
                haveFirstHop = true;
            }
            
            // Skip to next field
            while (pos < json.length() && json[pos] != ',' && json[pos] != '}') {
                ++pos;
            }
            if (pos < json.length() && json[pos] == ',') {
                ++pos;
            }
            
        } catch (const std::exception&) {
            // Skip invalid field
            ++pos;
        }
    }
}

// Load service configuration from JSON file
ServiceConfig ZeroMQDataHandler::loadServiceConfig(const std::string& configFilePath) const {
    // Check if file exists
    if (!std::filesystem::exists(configFilePath)) {
        throw std::runtime_error("Configuration file not found: " + configFilePath);
    }
    
    try {
        ServiceConfig config = JsonConfigReader::readServiceConfig(configFilePath);
        
        // Validate configuration values
        if (config.protocol != "udp") {
            Logger::warn("Non-UDP protocol specified: " + config.protocol + ". ZeroMQ DISH typically uses UDP.");
        }
        
        if (config.port < 1024 || config.port > 65535) {
            throw std::runtime_error("Invalid port number: " + std::to_string(config.port));
        }
        
        return config;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load service configuration from " + 
                                configFilePath + ": " + e.what());
    }
}
