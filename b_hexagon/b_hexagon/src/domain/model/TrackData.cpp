#include "domain/model/TrackData.hpp"
#include <cmath>
#include <limits>

bool TrackData::isValid() const noexcept {
    // MISRA Rule 7-1-2: Const member function doesn't modify state
    // MISRA Rule 15-3-4: Function guaranteed not to throw
    
    // Check for reasonable trackId (positive)
    if (trackId <= 0) {
        return false;
    }
    
    // Check for finite velocity values (not NaN or infinite)
    if (!std::isfinite(xVelocityECEF) || !std::isfinite(yVelocityECEF) || !std::isfinite(zVelocityECEF)) {
        return false;
    }
    
    // Check for finite position values (not NaN or infinite)
    if (!std::isfinite(xPositionECEF) || !std::isfinite(yPositionECEF) || !std::isfinite(zPositionECEF)) {
        return false;
    }
    
    // Check for reasonable time values (positive)
    if (updateTime < 0L || originalUpdateTime < 0L || firstHopSentTime < 0L) {
        return false;
    }
    
    // Check velocity magnitude is reasonable (less than speed of light * 1000 for safety)
    const double velocityMagnitude = std::sqrt(xVelocityECEF * xVelocityECEF + 
                                              yVelocityECEF * yVelocityECEF + 
                                              zVelocityECEF * zVelocityECEF);
    const double maxReasonableVelocity = 299792458000.0; // Speed of light * 1000 m/s
    if (velocityMagnitude > maxReasonableVelocity) {
        return false;
    }
    
    return true;
}
