/**
 * @file CalculatorService.cpp
 * @brief Implementation of CalculatorService for track data processing
 */

#include "domain/logic/CalculatorService.hpp"
#include "common/Logger.hpp"

DelayCalculatedTrackData CalculatorService::calculateDelay(const TrackData& trackData) const {
    Logger::debug("Processing track ", trackData.trackId, " - calculating delay metrics");
    
    // Get current processing time for second hop
    long currentTime = getCurrentTimeMicroseconds();
    
    // Validate input timestamp
    if (trackData.firstHopSentTime <= 0) {
        Logger::warn("Invalid firstHopSentTime for track ", trackData.trackId, ": ", trackData.firstHopSentTime);
    }
    
    // Create result with calculated values
    DelayCalculatedTrackData result;
    
    // Copy all original track data
    result.trackId = trackData.trackId;
    result.xVelocityECEF = trackData.xVelocityECEF;
    result.yVelocityECEF = trackData.yVelocityECEF;
    result.zVelocityECEF = trackData.zVelocityECEF;
    result.xPositionECEF = trackData.xPositionECEF;
    result.yPositionECEF = trackData.yPositionECEF;
    result.zPositionECEF = trackData.zPositionECEF;
    result.updateTime = trackData.updateTime;
    result.originalUpdateTime = trackData.originalUpdateTime;
    result.firstHopSentTime = trackData.firstHopSentTime;
    
    // Calculate first hop delay (current time - first hop sent time)
    result.firstHopDelayTime = calculateTimeDelta(trackData.firstHopSentTime, currentTime);
    
    // Set second hop sent time as current time
    result.secondHopSentTime = currentTime;
    
    // Logger::info("Track ", trackData.trackId, " delay calculation complete - first hop delay: ", 
    //              result.firstHopDelayTime, " μs, second hop time: ", result.secondHopSentTime);
    
    return result;
}

long CalculatorService::getCurrentTimeMicroseconds() const noexcept {
    try {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    } catch (...) {
        return 0L;  // Fallback value if time acquisition fails
    }
}

long CalculatorService::calculateTimeDelta(long originalTime, long currentTime) const noexcept {
    if (originalTime <= 0 || currentTime <= 0) {
        return 0L;  // Invalid timestamps
    }
    
    if (currentTime <= originalTime) {
        return 0L;  // No delay if current time is not later
    }
    
    return currentTime - originalTime;
}
