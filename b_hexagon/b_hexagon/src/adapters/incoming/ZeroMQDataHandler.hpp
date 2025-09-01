/**
 * @file ZeroMQDataHandler.hpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 */

#pragma once

#include "domain/ports/incoming/IDataReceiver.hpp"      // Inbound port interface
#include "domain/model/TrackData.hpp"                   // Domain data model
#include "messaging/ContractManager.hpp"                // Contract validation
#include "messaging/messaging.hpp"                      // High-level messaging interface
#include "common/JsonConfigReader.hpp"                  // Configuration reader
#include <zmq.hpp>                                       // ZeroMQ C++ bindings
#include <string>                                        // String utilities
#include <memory>                                        // Smart pointers
#include <stdexcept>                                     // Exception types

/**
 * @class ZeroMQDataHandler
 * @brief ZeroMQ DISH adapter for receiving TrackData via UDP multicast
 * 
 * Implements the RADIO/DISH UDP pattern:
 * - DISH socket binds to UDP multicast endpoint
 * - Joins specific group using zmq_join()
 * - Receives group-filtered messages from RADIO senders
 * - Validates messages against JSON schema contracts
 */
class ZeroMQDataHandler final {
public:
    // Default constructor - reads configuration from default ExtrapTrackData.json
    explicit ZeroMQDataHandler(IDataReceiver* dataReceiver = nullptr);

    // JSON configuration constructor - reads from specified config file
    explicit ZeroMQDataHandler(const std::string& configFilePath,
                               IDataReceiver* dataReceiver);

    // Legacy constructor - uses direct parameters
    explicit ZeroMQDataHandler(const std::string& endpoint,
                               const std::string& group,
                               bool bind,
                               IDataReceiver* dataReceiver = nullptr);

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
    // Load service configuration from JSON file
    ServiceConfig loadServiceConfig(const std::string& configFilePath) const;
    
    // Parse JSON string to TrackData object
    static TrackData parseJson(const std::string& json);
    
    // Helper methods for JSON parsing
    static void skipWhitespace(const std::string& json, std::size_t& pos) noexcept;
    static std::string extractJsonString(const std::string& json, std::size_t& pos);
    static void parseJsonFields(const std::string& json, TrackData& data,
                                bool& haveTrackId, bool& haveXVel, bool& haveYVel, bool& haveZVel,
                                bool& haveXPos, bool& haveYPos, bool& haveZPos,
                                bool& haveUpdate, bool& haveOriginal, bool& haveFirstHop) noexcept;

    zmq::context_t context_;           // ZeroMQ context
    zmq::socket_t socket_;             // DISH socket for UDP multicast
    const std::string group_;          // Group identifier for filtering
    IDataReceiver* const dataReceiver_; // Domain notification interface
    ContractManager contractManager_;   // Message validation
};
