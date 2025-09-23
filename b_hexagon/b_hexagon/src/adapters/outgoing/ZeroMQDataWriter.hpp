/**
 * @file ZeroMQDataWriter.hpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 */

#pragma once

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types
#include "domain/ports/outgoing/IDataWriter.hpp"        // Outbound port interface
#include "domain/model/DelayCalcTrackData.hpp"    // Domain data model
#include <zmq.hpp>                                       // ZeroMQ C++ bindings
#include <string>                                        // String utilities

// Using declarations for convenience
using domain::model::DelayCalcTrackData;

/**
 * @class ZeroMQDataWriter
 * @brief ZeroMQ RADIO adapter for broadcasting DelayCalcTrackData via UDP multicast
 * 
 * Implements the RADIO/DISH UDP pattern:
 * - RADIO socket connects to UDP multicast endpoint
 * - Messages are serialized to binary format for efficiency
 * - Messages are tagged with group identifiers
 * - DISH receivers filter by group using zmq_join()
 * 
 * Uses configuration constants from DelayCalcTrackData domain model.
 */
class ZeroMQDataWriter final : public IDataWriter {
public:
    // Default constructor - uses configuration from DelayCalcTrackData domain model
    explicit ZeroMQDataWriter();

    // Destructor - RAII cleanup
    ~ZeroMQDataWriter() noexcept = default;

    // Disable copy operations
    ZeroMQDataWriter(const ZeroMQDataWriter& other) = delete;
    ZeroMQDataWriter& operator=(const ZeroMQDataWriter& other) = delete;

    // Allow move operations
    ZeroMQDataWriter(ZeroMQDataWriter&& other) noexcept = default;
    ZeroMQDataWriter& operator=(ZeroMQDataWriter&& other) noexcept = default;

    // Send data via RADIO socket (implements IDataWriter)
    void sendData(const DelayCalcTrackData& data) override;

private:
    zmq::context_t context_;      // ZeroMQ context
    zmq::socket_t socket_;        // RADIO socket for UDP multicast
    const std::string group_;     // Group identifier for DISH filtering
};
