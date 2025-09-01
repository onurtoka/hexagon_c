/**
 * @file ZeroMQDataSender.hpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 */

#pragma once

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types
#include "domain/ports/outgoing/IDataSender.hpp"        // Outbound port interface
#include "domain/model/DelayCalculatedTrackData.hpp"    // Domain data model
#include "common/JsonConfigReader.hpp"                  // Configuration reader
#include <zmq.hpp>                                       // ZeroMQ C++ bindings
#include <string>                                        // String utilities

/**
 * @class ZeroMQDataSender
 * @brief ZeroMQ RADIO adapter for broadcasting DelayCalculatedTrackData via UDP multicast
 * 
 * Implements the RADIO/DISH UDP pattern:
 * - RADIO socket connects to UDP multicast endpoint
 * - Messages are tagged with group identifiers
 * - DISH receivers filter by group using zmq_join()
 */
class ZeroMQDataSender final : public IDataSender {
public:
    // Default constructor - reads configuration from default DelayCalcTrackData.json
    explicit ZeroMQDataSender();

    // JSON configuration constructor - reads from specified config file
    explicit ZeroMQDataSender(const std::string& configFilePath);

    // Legacy constructor - uses direct parameters
    explicit ZeroMQDataSender(const std::string& endpoint,
                             const std::string& group,
                             bool bind);

    // Destructor - RAII cleanup
    ~ZeroMQDataSender() noexcept = default;

    // Disable copy operations
    ZeroMQDataSender(const ZeroMQDataSender& other) = delete;
    ZeroMQDataSender& operator=(const ZeroMQDataSender& other) = delete;

    // Allow move operations
    ZeroMQDataSender(ZeroMQDataSender&& other) noexcept = default;
    ZeroMQDataSender& operator=(ZeroMQDataSender&& other) noexcept = default;

    // Send data via RADIO socket (implements IDataSender)
    void sendData(const DelayCalculatedTrackData& data) override;

private:
    // Load service configuration from JSON file
    ServiceConfig loadServiceConfig(const std::string& configFilePath) const;
    
    zmq::context_t context_;      // ZeroMQ context
    zmq::socket_t socket_;        // RADIO socket for UDP multicast
    const std::string group_;     // Group identifier for DISH filtering
};
