/**
 * @file IDataWriter.hpp
 * @brief Outgoing port interface for writing processed track data
 */

#pragma once

#include "domain/model/DelayCalcTrackData.hpp"

// Using declarations for convenience
using domain::model::DelayCalcTrackData;

/**
 * @interface IDataWriter
 * @brief Outbound port for writing processed DelayCalcTrackData
 * 
 * This interface is called by application services and implemented by
 * outgoing adapters (ZeroMQ, file, console, etc.) to send processed data.
 */
class IDataWriter {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~IDataWriter() = default;

    /**
     * @brief Send processed track data to external destination
     * @param data The processed track data with delay calculations
     */
    virtual void sendData(const DelayCalcTrackData& data) = 0;
};
