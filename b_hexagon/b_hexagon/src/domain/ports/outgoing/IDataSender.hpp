/**
 * @file IDataSender.hpp
 * @brief Outgoing port interface for sending processed track data
 */

#pragma once

#include "domain/model/DelayCalculatedTrackData.hpp"

/**
 * @interface IDataSender
 * @brief Outbound port for sending processed DelayCalculatedTrackData
 * 
 * This interface is called by application services and implemented by
 * outgoing adapters (ZeroMQ, file, console, etc.) to send processed data.
 */
class IDataSender {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~IDataSender() = default;

    /**
     * @brief Send processed track data to external destination
     * @param data The processed track data with delay calculations
     */
    virtual void sendData(const DelayCalculatedTrackData& data) = 0;
};
