/**
 * @file ZeroMQDataHandler.hpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 */

#pragma once

#include "domain/ports/incoming/IDataHandler.hpp"      // Inbound port interface
#include "domain/model/ExtrapTrackData.hpp"                   // Domain data model
#include <zmq.hpp>                                       // ZeroMQ C++ bindings
#include <string>                                        // String utilities
#include <memory>                                        // Smart pointers
#include <stdexcept>                                     // Exception types

// Using declarations for convenience
using domain::model::ExtrapTrackData;

/**
 * @class ZeroMQDataHandler
 * @brief ZeroMQ DISH adapter for receiving ExtrapTrackData via UDP multicast
 * 
 * Implements the RADIO/DISH UDP pattern:
 * - DISH socket binds to UDP multicast endpoint
 * - Joins specific group using zmq_join()
 * - Receives group-filtered messages from RADIO senders
 * - Deserializes binary messages to ExtrapTrackData objects
 * 
 * Uses configuration constants from ExtrapTrackData domain model.
 */
class ZeroMQDataHandler final {
public:
    // Default constructor - uses configuration from ExtrapTrackData domain model
    explicit ZeroMQDataHandler(IDataHandler* dataReceiver = nullptr);

    // Destructor - RAII cleanup
    ~ZeroMQDataHandler() noexcept = default;

    // Disable copy operations
    ZeroMQDataHandler(const ZeroMQDataHandler& other) = delete;
    ZeroMQDataHandler& operator=(const ZeroMQDataHandler& other) = delete;

    // Allow move operations
    ZeroMQDataHandler(ZeroMQDataHandler&& other) noexcept = default;
    ZeroMQDataHandler& operator=(ZeroMQDataHandler&& other) noexcept = default;

    // Start continuous message reception loop
    void startReceiving();

private:
    // Deserialize binary data to ExtrapTrackData object
    static ExtrapTrackData deserializeBinary(const uint8_t* data, std::size_t size);

    zmq::context_t context_;           // ZeroMQ context
    zmq::socket_t socket_;             // DISH socket for UDP multicast
    const std::string group_;          // Group identifier for filtering
    IDataHandler* const dataReceiver_; // Domain notification interface
};
