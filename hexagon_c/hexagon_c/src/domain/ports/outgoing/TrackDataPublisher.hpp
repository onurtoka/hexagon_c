#pragma once

#include "../../model/FinalCalcTrackData.hpp"  // Outgoing FinalCalcTrackData

/**
 * @interface IDataSender  
 * @brief Hexagonal Architecture Secondary Port for outgoing track data
 * 
 * Sends FinalCalcTrackData to external systems via ZeroMQ RADIO socket.
 * This is the secondary port interface that domain logic calls.
 */
class IDataSender {
public:
    virtual ~IDataSender() = default;

    /**
     * @brief Send final calculated track data to external systems
     * @param data FinalCalcTrackData containing complete delay analysis
     */
    virtual void sendData(const domain::model::FinalCalcTrackData& data) = 0;
};
