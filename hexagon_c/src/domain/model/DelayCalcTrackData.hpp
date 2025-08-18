#pragma once

#include <cstdint>
#include <cmath>
#include <string>
#include <chrono>

namespace hat::domain::model {

/**
 * DelayCalcTrackData - Gecikme hesaplama ve radar tracking verilerini birleştiren ana model
 * ECEF (Earth-Centered, Earth-Fixed) koordinat sistemi ve zaman analizi içerir
 */
class DelayCalcTrackData {
private:
    // Ana alanlar
    int id_;                          // ID
    double velocity_;                 // Velocity field
    double position_;                 // Position field  
    int64_t time_field_;              // Time field (milliseconds)
    
    // Track ID ve ECEF koordinatları
    int track_id_;                    // Track ID (*)
    
    // Velocity components in ECEF coordinate system
    double x_velocity_ecef_;          // XVelocityECEF
    double y_velocity_ecef_;          // YVelocityECEF
    double z_velocity_ecef_;          // ZVelocityECEF
    
    // Position components in ECEF coordinate system
    double x_position_ecef_;          // XPositionECEF
    double y_position_ecef_;          // YPositionECEF
    double z_position_ecef_;          // ZPositionECEF
    
    // Zaman alanları (long - milliseconds since epoch)
    int64_t update_time_;             // Update Time
    int64_t original_update_time_;    // OriginalUpdate Time
    int64_t first_hop_sent_time_;     // FirstHopSent Time
    int64_t first_hop_delay_time_;    // FirstHopDelay Time
    int64_t second_hop_sent_time_;    // SecondHopSent Time

public:

    // Constructor - tüm alanları alan
    DelayCalcTrackData(int id, double velocity, double position, int64_t time_field,
                       int track_id,
                       double x_vel_ecef, double y_vel_ecef, double z_vel_ecef,
                       double x_pos_ecef, double y_pos_ecef, double z_pos_ecef,
                       int64_t update_time, int64_t original_update_time,
                       int64_t first_hop_sent, int64_t first_hop_delay, 
                       int64_t second_hop_sent);

    // Default constructor
    DelayCalcTrackData();

    // Ana alanlar için getters
    int getId() const;
    double getVelocity() const;
    double getPosition() const;
    int64_t getTimeField() const;
    int getTrackId() const;
    double getXVelocityECEF() const;
    double getYVelocityECEF() const;
    double getZVelocityECEF() const;
    double getXPositionECEF() const;
    double getYPositionECEF() const;
    double getZPositionECEF() const;
    int64_t getUpdateTime() const;
    int64_t getOriginalUpdateTime() const;
    int64_t getFirstHopSentTime() const;
    int64_t getFirstHopDelayTime() const;
    int64_t getSecondHopSentTime() const;
    void setId(int id);
    void setVelocity(double velocity);
    void setPosition(double position);
    void setTimeField(int64_t time_field);
    void setTrackId(int track_id);
    void setVelocityECEF(double x_vel, double y_vel, double z_vel);
    void setPositionECEF(double x_pos, double y_pos, double z_pos);
    void setUpdateTime(int64_t time);
    void setOriginalUpdateTime(int64_t time);
    void setFirstHopSentTime(int64_t time);
    void setFirstHopDelayTime(int64_t time);
    void setSecondHopSentTime(int64_t time);
    double getECEFSpeed() const;
    double getDistanceFromOrigin() const;
    int64_t calculateFirstHopDelay() const;
    int64_t calculateTotalProcessingDelay() const;
    int64_t calculateDataAge() const;
    bool isDataFresh(int64_t threshold_ms = 1000) const;
    bool isHighVelocity(double threshold = 100.0) const;
    bool isValid() const;
    std::string toString() const;
    bool operator==(const DelayCalcTrackData& other) const;
    bool operator!=(const DelayCalcTrackData& other) const;
    bool operator<(const DelayCalcTrackData& other) const;
};

} // namespace hat::domain::model