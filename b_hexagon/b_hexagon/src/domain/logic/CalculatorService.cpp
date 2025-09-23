/**
 * @file CalculatorService.cpp
 * @brief Implementation of CalculatorService for track data processing
 */

#include "domain/logic/CalculatorService.hpp"
#include "common/Logger.hpp"

DelayCalcTrackData CalculatorService::calculateDelay(const ExtrapTrackData& trackData) const {
    Logger::debug("Processing track ", trackData.getTrackId(), " - calculating delay metrics");
    
    // Get current processing time for second hop
    long currentTime = getCurrentTimeMicroseconds();
    
    // Validate input timestamp
    if (trackData.getFirstHopSentTime() <= 0) {
        Logger::warn("Invalid firstHopSentTime for track ", trackData.getTrackId(), ": ", trackData.getFirstHopSentTime());
    }
    
    // Create result with calculated values
    DelayCalcTrackData result;
    
    // Copy all original track data
    result.setTrackId(trackData.getTrackId());
    result.setXVelocityECEF(trackData.getXVelocityECEF());
    result.setYVelocityECEF(trackData.getYVelocityECEF());
    result.setZVelocityECEF(trackData.getZVelocityECEF());
    result.setXPositionECEF(trackData.getXPositionECEF());
    result.setYPositionECEF(trackData.getYPositionECEF());
    result.setZPositionECEF(trackData.getZPositionECEF());
    result.setUpdateTime(trackData.getUpdateTime());
    result.setOriginalUpdateTime(trackData.getOriginalUpdateTime());
    result.setFirstHopSentTime(trackData.getFirstHopSentTime());
    
    // Calculate first hop delay (current time - first hop sent time)
    result.setFirstHopDelayTime(calculateTimeDelta(trackData.getFirstHopSentTime(), currentTime));
    
    // Set second hop sent time as current time
    result.setSecondHopSentTime(currentTime);
    
    Logger::info("Track ", trackData.getTrackId(), " delay calculation complete - first hop delay: ", 
                 result.getFirstHopDelayTime(), " μs, second hop time: ", result.getSecondHopSentTime());
    Logger::info("CURRENT TIME <>>>>>>>  ",currentTime);
    Logger::info("getFirstHopSentTime TIME <>>>>>>>  ",trackData.getFirstHopSentTime());
    
    return result;
}

long CalculatorService::getCurrentTimeMicroseconds() const noexcept {
    try {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        // Microsecond precision kullan (nanosecond yerine)
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
