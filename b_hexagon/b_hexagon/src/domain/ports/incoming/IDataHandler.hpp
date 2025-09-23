/**
 * @file IDataHandler.hpp
 * @brief Incoming port interface for handling track data
 */

#pragma once

#include "domain/model/ExtrapTrackData.hpp"

// Using declarations for convenience
using domain::model::ExtrapTrackData;

/**
 * @interface IDataHandler
 * @brief Inbound port for handling ExtrapTrackData from external sources
 * 
 * This interface is implemented by application services and called by
 * incoming adapters (ZeroMQ, file, console, etc.) when new data arrives.
 */
class IDataHandler {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~IDataHandler() = default;

    /**
     * @brief Called when new track data is received
     * @param data The received track data to process
     */
    virtual void onDataReceived(const ExtrapTrackData& data) = 0;
};
