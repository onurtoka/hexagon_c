/**
 * @file ZeroMQDataWriter.cpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 */

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types

#include "adapters/outgoing/ZeroMQDataWriter.hpp"  // Own header
#include "common/Logger.hpp"                       // Logging utility
#include <sstream>        // String stream for endpoint formatting
#include <cstring>        // Memory operations
#include <stdexcept>      // Exception types
#include <iostream>       // Error output

// Using declarations for convenience
using domain::model::DelayCalcTrackData;

// Default constructor - uses configuration from DelayCalcTrackData domain model
ZeroMQDataWriter::ZeroMQDataWriter()
    : context_(1),
      socket_(context_, ZMQ_RADIO),
      group_("DelayCalcTrackData") {  // Group name matches message type
    
    try {
        // Build endpoint from DelayCalcTrackData configuration constants
        std::ostringstream oss;
        oss << DelayCalcTrackData::ZMQ_PROTOCOL << "://"
            << DelayCalcTrackData::ZMQ_MULTICAST_ADDRESS << ":"
            << DelayCalcTrackData::ZMQ_PORT;
        std::string endpoint = oss.str();
        
        Logger::info("Initializing ZeroMQDataWriter from DelayCalcTrackData constants");
        Logger::info("Endpoint: ", endpoint, ", Group: ", group_);
        
        // Connect to the endpoint (RADIO typically connects)
        Logger::debug("Connecting RADIO socket to endpoint: ", endpoint);
        socket_.connect(endpoint);
        
        Logger::info("ZeroMQDataWriter successfully configured from DelayCalcTrackData constants -> ", endpoint);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("ZeroMQDataWriter config error: " + std::string(e.what()));
    }
}

// Send DelayCalcTrackData via RADIO socket
void ZeroMQDataWriter::sendData(const DelayCalcTrackData& data) {
    
    Logger::debug("Sending DelayCalcTrackData for track ID: ", data.getTrackId());
    
    // Validate input data
    if (!data.isValid()) {
        Logger::error("Attempted to send invalid DelayCalcTrackData for track ID: ", data.getTrackId());
        throw std::invalid_argument("ZeroMQDataWriter::send: Invalid DelayCalcTrackData");
    }
    
    try {
        // Güncellenmiş modelin binary serialization özelliğini kullan
        std::vector<uint8_t> binaryData = data.serialize();
        
        Logger::debug("Generated binary payload for track ", data.getTrackId(), " - Size: ", binaryData.size(), " bytes");
        
        if (binaryData.empty()) {
            Logger::error("Empty binary payload generated for track ID: ", data.getTrackId());
            throw std::runtime_error("ZeroMQDataWriter::sendData: Empty binary payload generated");
        }
        
        // Create C++ API message with binary payload
        zmq::message_t processed_msg(binaryData.begin(), binaryData.end());
        
        // Set group identifier for DISH filtering
        Logger::debug("Setting message group to: ", group_);
        processed_msg.set_group(group_.c_str());
        
        // Send via RADIO socket
        Logger::debug("Transmitting message via RADIO socket...");
        auto send_result = socket_.send(processed_msg, zmq::send_flags::none);
        
        if (!send_result || *send_result != binaryData.size()) {
            Logger::error("ZeroMQ RADIO transmission failed or partial send - expected: ", binaryData.size(), 
                         ", sent: ", (send_result ? *send_result : 0));
            throw std::runtime_error("ZeroMQ RADIO transmission failed or partial send");
        }
        
        Logger::info("Successfully transmitted track ", data.getTrackId(), " (", *send_result, " bytes) to group: ", group_);
        
    } catch (const zmq::error_t& e) {
        Logger::error("ZeroMQ error during transmission for track ", data.getTrackId(), ": ", e.what());
        throw std::runtime_error("ZeroMQDataWriter::send: ZeroMQ RADIO transmission error - " + 
            std::string(e.what()));
    } catch (const std::exception& e) {
        Logger::error("Critical sendData failure for track ", data.getTrackId(), ": ", e.what());
        throw std::runtime_error("ZeroMQDataWriter::send: DelayCalcTrackData transmission failed - " + 
            std::string(e.what()));
    }
    
    Logger::debug("sendData completed successfully for track: ", data.getTrackId());
}
