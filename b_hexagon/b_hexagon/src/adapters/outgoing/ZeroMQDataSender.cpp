/**
 * @file ZeroMQDataSender.cpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 */

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types

#include "adapters/outgoing/ZeroMQDataSender.hpp"  // Own header
#include "common/Logger.hpp"                       // Logging utility
#include "common/JsonConfigReader.hpp"             // Configuration reader
#include <sstream>        // JSON string building
#include <cstring>        // Memory operations
#include <stdexcept>      // Exception types
#include <iostream>       // Error output
#include <filesystem>     // File system operations

// Default constructor - reads configuration from default DelayCalcTrackData.json
ZeroMQDataSender::ZeroMQDataSender()
    : ZeroMQDataSender("src/zmq_messages/DelayCalcTrackData.json") {
}

// JSON configuration constructor - reads from specified config file
ZeroMQDataSender::ZeroMQDataSender(const std::string& configFilePath)
    : context_(1),
      socket_(context_, zmq::socket_type::radio),
      group_("SOURCE_DATA") {  // Group name matching hexagon_c
    
    try {
        ServiceConfig config = loadServiceConfig(configFilePath);
        std::string endpoint = config.getEndpoint();
        
        Logger::info("Initializing ZeroMQDataSender from config: ", configFilePath);
        Logger::info("Endpoint: ", endpoint, ", Group: ", group_);
        
        // Connect to the endpoint (RADIO typically connects)
        Logger::debug("Connecting RADIO socket to endpoint: ", endpoint);
        socket_.connect(endpoint);
        
        Logger::info("ZeroMQDataSender successfully configured from ", configFilePath, " -> ", endpoint);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("ZeroMQDataSender JSON config error: " + std::string(e.what()));
    }
}

// Legacy constructor - uses direct parameters
ZeroMQDataSender::ZeroMQDataSender(const std::string& endpoint,
                                   const std::string& group,
                                   bool bind)
    : context_(1),
      socket_(context_, zmq::socket_type::radio),
      group_(group) {
    
    Logger::info("Initializing ZeroMQDataSender - Endpoint: ", endpoint, ", Group: ", group, ", Bind: ", (bind ? "true" : "false"));
    
    if (endpoint.empty()) {
        Logger::error("ZeroMQDataSender endpoint cannot be empty");
        throw std::runtime_error("ZeroMQDataSender: endpoint cannot be empty");
    }
    
    try {
        if (bind) {
            Logger::debug("Binding RADIO socket to endpoint: ", endpoint);
            socket_.bind(endpoint);     // RADIO binds (less common)
        } else {
            Logger::debug("Connecting RADIO socket to endpoint: ", endpoint);
            socket_.connect(endpoint);  // RADIO connects to multicast (typical)
        }
        Logger::info("ZeroMQDataSender successfully initialized");
    } catch (const zmq::error_t& e) {
        Logger::error("ZMQ error during sender setup: ", e.what());
        throw std::runtime_error("ZeroMQDataSender: Failed to " + 
            std::string(bind ? "bind to " : "connect to ") + 
            endpoint + " - " + e.what());
    }
}

// Send DelayCalculatedTrackData via RADIO socket
void ZeroMQDataSender::sendData(const DelayCalculatedTrackData& data) {
    
    Logger::debug("Sending DelayCalculatedTrackData for track ID: ", data.trackId);
    
    // Validate input data
    if (!data.isValid()) {
        Logger::error("Attempted to send invalid DelayCalculatedTrackData for track ID: ", data.trackId);
        throw std::invalid_argument("ZeroMQDataSender::send: Invalid DelayCalculatedTrackData");
    }
    
    try {
        // Build JSON payload
        std::ostringstream jsonStream;
        jsonStream << '{'
            << "\"trackId\":"            << data.trackId            << ','
            << "\"xVelocityECEF\":"      << data.xVelocityECEF      << ','
            << "\"yVelocityECEF\":"      << data.yVelocityECEF      << ','
            << "\"zVelocityECEF\":"      << data.zVelocityECEF      << ','
            << "\"xPositionECEF\":"      << data.xPositionECEF      << ','
            << "\"yPositionECEF\":"      << data.yPositionECEF      << ','
            << "\"zPositionECEF\":"      << data.zPositionECEF      << ','
            << "\"updateTime\":"         << data.updateTime         << ','
            << "\"originalUpdateTime\":" << data.originalUpdateTime << ','
            << "\"firstHopSentTime\":"   << data.firstHopSentTime   << ','
            << "\"firstHopDelayTime\":" << data.firstHopDelayTime
            << '}';
        
        std::string jsonPayload = jsonStream.str();
        
        Logger::debug("Generated JSON payload for track ", data.trackId, ": ", jsonPayload.substr(0, 100), "...");
        
        if (jsonPayload.empty()) {
            Logger::error("Empty JSON payload generated for track ID: ", data.trackId);
            throw std::runtime_error("ZeroMQDataSender::send: Empty JSON payload generated");
        }
        
        // Add timestamp for latency measurement (like app2_processor example)
        auto send_time = std::chrono::high_resolution_clock::now();
        auto send_timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
            send_time.time_since_epoch()).count();
        
        std::ostringstream payloadWithTimestamp;
        payloadWithTimestamp << jsonPayload << "|" << send_timestamp_us;
        std::string messageWithTimestamp = payloadWithTimestamp.str();
        
        // Create C++ API message with timestamped payload
        zmq::message_t processed_msg(messageWithTimestamp.begin(), messageWithTimestamp.end());
        
        // Set group identifier for DISH filtering
        Logger::debug("Setting message group to: ", group_);
        processed_msg.set_group(group_.c_str());
        
        // Send via RADIO socket
        Logger::debug("Transmitting message via RADIO socket...");
        auto send_result = socket_.send(processed_msg, zmq::send_flags::none);
        
        if (!send_result || *send_result != messageWithTimestamp.size()) {
            Logger::error("ZeroMQ RADIO transmission failed or partial send - expected: ", messageWithTimestamp.size(), 
                         ", sent: ", (send_result ? *send_result : 0));
            throw std::runtime_error("ZeroMQ RADIO transmission failed or partial send");
        }
        
        // Logger::info("Successfully transmitted track ", data.trackId, " (", *send_result, " bytes) to group: ", group_);
        
    } catch (const zmq::error_t& e) {
        Logger::error("ZeroMQ error during transmission for track ", data.trackId, ": ", e.what());
        throw std::runtime_error("ZeroMQDataSender::send: ZeroMQ RADIO transmission error - " + 
            std::string(e.what()));
    } catch (const std::exception& e) {
        Logger::error("Critical sendData failure for track ", data.trackId, ": ", e.what());
        throw std::runtime_error("ZeroMQDataSender::send: DelayCalculatedTrackData transmission failed - " + 
            std::string(e.what()));
    }
    
    Logger::debug("sendData completed successfully for track: ", data.trackId);
}

// Load service configuration from JSON file
ServiceConfig ZeroMQDataSender::loadServiceConfig(const std::string& configFilePath) const {
    // Check if file exists
    if (!std::filesystem::exists(configFilePath)) {
        throw std::runtime_error("Configuration file not found: " + configFilePath);
    }
    
    try {
        ServiceConfig config = JsonConfigReader::readServiceConfig(configFilePath);
        
        // Validate configuration values
        if (config.protocol != "udp") {
            Logger::warn("Non-UDP protocol specified: ", config.protocol, ". ZeroMQ RADIO typically uses UDP.");
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
