/**
 * @file ZeroMQDataHandler.cpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 */

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types

#include "adapters/incoming/ZeroMQDataHandler.hpp"  // Own header
#include "common/Logger.hpp"                        // Logging
#include <stdexcept>      // Exception types
#include <cstring>        // Memory operations
#include <sstream>        // String stream for endpoint formatting

// Using declarations for convenience
using domain::model::ExtrapTrackData;

// Define socket type constants for RADIO/DISH pattern
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif
#ifndef ZMQ_DISH
#define ZMQ_DISH 15
#endif

// Default constructor - uses configuration from ExtrapTrackData domain model
ZeroMQDataHandler::ZeroMQDataHandler(IDataHandler* dataReceiver)
    : context_(1),
      socket_(context_, ZMQ_DISH),
      group_("ExtrapTrackData"),  // Group name matches message type
      dataReceiver_(dataReceiver) {
    
    try {
        // Build endpoint from ExtrapTrackData configuration constants
        std::ostringstream oss;
        oss << ExtrapTrackData::ZMQ_PROTOCOL << "://"
            << ExtrapTrackData::ZMQ_MULTICAST_ADDRESS << ":"
            << ExtrapTrackData::ZMQ_PORT;
        std::string endpoint = oss.str();
        
        // Bind and join group using C++ API
        socket_.bind(endpoint);
        socket_.join(group_.c_str());
        
        Logger::info("ZeroMQDataHandler configured from ExtrapTrackData constants -> " + endpoint);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("ZeroMQDataHandler config error: " + std::string(e.what()));
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
            
            Logger::debug("Received ZMQ message, size: ", message.size(), " bytes");
            
            // Get group identifier from received message
            const char* msg_group = message.group();
            
            // Verify message group matches our subscribed group
            if (msg_group == nullptr || group_ != std::string(msg_group)) {
                Logger::warn("Message received for wrong group: ", (msg_group ? msg_group : "null"));
                continue;  // Skip messages not for our group
            }
            
            Logger::info("Processing message from group: ", msg_group);
            
            // Extract binary payload
            const uint8_t* binaryData = static_cast<const uint8_t*>(message.data());
            std::size_t dataSize = message.size();
            
            // Deserialize binary data to domain object
            ExtrapTrackData data = deserializeBinary(binaryData, dataSize);
            
            // Notify domain layer
            if (dataReceiver_ != nullptr) {
                dataReceiver_->onDataReceived(data);
            }
            
        } catch (const zmq::error_t& e) {
            throw std::runtime_error("ZeroMQ receive failed: " + std::string(e.what()));
        } catch (const std::exception& ex) {
            // Log error but continue processing (don't terminate reception loop)
            Logger::error("Message processing error: ", ex.what());
        }
    }
}

// Deserialize binary data to ExtrapTrackData object
ExtrapTrackData ZeroMQDataHandler::deserializeBinary(const uint8_t* data, std::size_t size) {
    // Güncellenmiş modelin binary deserialization özelliğini kullan
    ExtrapTrackData extrapData; // default constructed
    std::vector<uint8_t> binary_data(data, data + size);
    
    if (extrapData.deserialize(binary_data)) {
        return extrapData;
    } else {
        throw std::runtime_error("ZeroMQDataHandler: Binary deserialization failed");
    }
}
