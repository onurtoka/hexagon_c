/**
 * @file IDataReceiver.hpp
 * @brief Incoming port interface for receiving track data
 */

#pragma once

#include "domain/model/TrackData.hpp"

/**
 * @interface IDataReceiver
 * @brief Inbound port for receiving TrackData from external sources
 * 
 * This interface is implemented by application services and called by
 * incoming adapters (ZeroMQ, file, console, etc.) when new data arrives.
 */
class IDataReceiver {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~IDataReceiver() = default;

    /**
     * @brief Called when new track data is received
     * @param data The received track data to process
     */
    virtual void onDataReceived(const TrackData& data) = 0;
};
