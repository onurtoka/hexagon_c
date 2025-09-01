/**
 * @file TrackDataExtrapolator.h
 * @brief Header file for TrackDataExtrapolator class
 * @author A Hexagon Team
 * @date 2025
 */

#ifndef TRACK_DATA_EXTRAPOLATOR_H 
#define TRACK_DATA_EXTRAPOLATOR_H 

#include <vector>
#include "../model/TrackData.h"          
#include "../model/ExtrapTrackData.h"    
#include "../ports/outgoing/TrackDataOutgoingPort.h" 

namespace domain {
namespace logic {

using domain::model::TrackData;     
using domain::model::ExtrapTrackData;
using std::vector; 

/**
 * @brief Main business logic class for track data extrapolation
 * 
 * This class implements the core extrapolation algorithm that converts
 * incoming track data from 5Hz frequency to 200Hz frequency using
 * constant velocity model. It follows the Hexagonal Architecture pattern
 * by implementing the TrackDataIncomingPort interface.
 */
class TrackDataExtrapolator {
private:
    /** @brief Pointer to outgoing port for sending extrapolated data */
    domain::ports::outgoing::ExtrapTrackDataOutgoingPort* outgoingPort_;

public: 
    /**
     * @brief Constructor for TrackDataExtrapolator
     * @param outgoingPort Pointer to the outgoing port implementation
     */
    TrackDataExtrapolator(domain::ports::outgoing::ExtrapTrackDataOutgoingPort* outgoingPort);
    
    /**
     * @brief Processes incoming track data and forwards extrapolated results
     * 
     * This method receives track data at 5Hz, extrapolates it to 200Hz
     * for a 125ms window (25 data points), and forwards the results
     * through the configured outgoing port.
     * 
     * @param trackData Input track data containing position, velocity, and timing
     */
    void processAndForwardTrackData(const domain::model::TrackData& trackData);
    
    /**
     * @brief Static method to extrapolate track data to 200Hz frequency
     * 
     * Generates 25 extrapolated data points covering 125ms window
     * using constant velocity model. Each point is spaced 5ms apart.
     * 
     * @param trackData Input track data to extrapolate
     * @return vector<ExtrapTrackData> Vector of 25 extrapolated data points
     */
    static vector<ExtrapTrackData> extrapolateTo200Hz(const TrackData& trackData);
};
}
}
#endif