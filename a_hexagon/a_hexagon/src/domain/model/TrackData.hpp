/**
 * @file TrackData.h
 * @brief Header file for TrackData model class
 * @author A Hexagon Team
 * @date 2025
 */

#ifndef TRACK_DATA_H 
#define TRACK_DATA_H 

namespace domain {
namespace model {

/**
 * @brief Input data model for track information at 5Hz frequency
 * 
 * This class represents incoming track data containing position,
 * velocity, and timing information in ECEF (Earth-Centered, Earth-Fixed)
 * coordinate system. Data is typically received from radar systems,
 * sensors, or other tracking devices at 5Hz frequency.
 */
class TrackData {
public: 
    /** @brief Default constructor */
    TrackData();
    
    /** @brief Virtual destructor */
    virtual ~TrackData();
    
    // Track ID accessors
    /** @brief Get track identifier @return Track ID */
    int getTrackId() const { return TrackId; }
    /** @brief Set track identifier @param id Track ID */
    void setTrackId(int id) { TrackId = id; }
    
    // Velocity accessors in ECEF coordinates
    /** @brief Get X velocity in ECEF coordinates @return X velocity (m/s) */
    double getXVelocityECEF() const { return XVelocityECEF; }
    /** @brief Set X velocity in ECEF coordinates @param v X velocity (m/s) */
    void setXVelocityECEF(double v) { XVelocityECEF = v; }
    
    /** @brief Get Y velocity in ECEF coordinates @return Y velocity (m/s) */
    double getYVelocityECEF() const { return YVelocityECEF; }
    /** @brief Set Y velocity in ECEF coordinates @param v Y velocity (m/s) */
    void setYVelocityECEF(double v) { YVelocityECEF = v; }
    
    /** @brief Get Z velocity in ECEF coordinates @return Z velocity (m/s) */
    double getZVelocityECEF() const { return ZVelocityECEF; }
    /** @brief Set Z velocity in ECEF coordinates @param v Z velocity (m/s) */
    void setZVelocityECEF(double v) { ZVelocityECEF = v; }
    
    // Position accessors in ECEF coordinates
    /** @brief Get X position in ECEF coordinates @return X position (m) */
    double getXPositionECEF() const { return XPositionECEF; }
    /** @brief Set X position in ECEF coordinates @param p X position (m) */
    void setXPositionECEF(double p) { XPositionECEF = p; }
    
    /** @brief Get Y position in ECEF coordinates @return Y position (m) */
    double getYPositionECEF() const { return YPositionECEF; }
    /** @brief Set Y position in ECEF coordinates @param p Y position (m) */
    void setYPositionECEF(double p) { YPositionECEF = p; }
    
    /** @brief Get Z position in ECEF coordinates @return Z position (m) */
    double getZPositionECEF() const { return ZPositionECEF; }
    /** @brief Set Z position in ECEF coordinates @param p Z position (m) */
    void setZPositionECEF(double p) { ZPositionECEF = p; }
    
    // Timing accessors
    /** @brief Get original update timestamp @return Original timestamp (nanoseconds) */
    long getOriginalUpdateTime() const { return originalUpdateTime; }
    /** @brief Set original update timestamp @param t Original timestamp (nanoseconds) */
    void setOriginalUpdateTime(long t) { originalUpdateTime = t; }

private: 
    /** @brief Unique track identifier */
    int TrackId;
    
    /** @brief X velocity component in ECEF coordinates (m/s) */
    double XVelocityECEF;
    /** @brief Y velocity component in ECEF coordinates (m/s) */
    double YVelocityECEF;
    /** @brief Z velocity component in ECEF coordinates (m/s) */
    double ZVelocityECEF;
    
    /** @brief X position component in ECEF coordinates (m) */
    double XPositionECEF;
    double YPositionECEF;
    double ZPositionECEF;
    long originalUpdateTime;
};
}
}
#endif 