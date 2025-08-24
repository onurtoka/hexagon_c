#pragma once

#include <cstdint>
#include <string>

namespace hat::domain::model {

/**
 * DelayCalcTrackData - Gecikme hesaplama ve radar tracking verilerini birleştiren ana model
 * ECEF (Earth-Centered, Earth-Fixed) koordinat sistemi ve zaman analizi içerir
 */
class DelayCalcTrackData {
private:
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
    // Constructors
    DelayCalcTrackData(int track_id,
                       double x_vel_ecef, double y_vel_ecef, double z_vel_ecef,
                       double x_pos_ecef, double y_pos_ecef, double z_pos_ecef,
                       int64_t update_time, int64_t original_update_time,
                       int64_t first_hop_sent, int64_t first_hop_delay, 
                       int64_t second_hop_sent);

    DelayCalcTrackData();

    // Getters - Ana alanlar
    int getTrackId() const { return track_id_; }
    
    // Getters - ECEF Velocity
    double getXVelocityECEF() const { return x_velocity_ecef_; }
    double getYVelocityECEF() const { return y_velocity_ecef_; }
    double getZVelocityECEF() const { return z_velocity_ecef_; }
    
    // Getters - ECEF Position
    double getXPositionECEF() const { return x_position_ecef_; }
    double getYPositionECEF() const { return y_position_ecef_; }
    double getZPositionECEF() const { return z_position_ecef_; }
    
    // Getters - Zaman alanları
    int64_t getUpdateTime() const { return update_time_; }
    int64_t getOriginalUpdateTime() const { return original_update_time_; }
    int64_t getFirstHopSentTime() const { return first_hop_sent_time_; }
    int64_t getFirstHopDelayTime() const { return first_hop_delay_time_; }
    int64_t getSecondHopSentTime() const { return second_hop_sent_time_; }

    // Setters - Ana alanlar
    void setTrackId(int track_id) { track_id_ = track_id; }
    
    // Setters - ECEF
    void setVelocityECEF(double x_vel, double y_vel, double z_vel);
    void setPositionECEF(double x_pos, double y_pos, double z_pos);
    
    // Setters - Zaman alanları
    void setUpdateTime(int64_t time) { update_time_ = time; }
    void setOriginalUpdateTime(int64_t time) { original_update_time_ = time; }
    void setFirstHopSentTime(int64_t time) { first_hop_sent_time_ = time; }
    void setFirstHopDelayTime(int64_t time) { first_hop_delay_time_ = time; }
    void setSecondHopSentTime(int64_t time) { second_hop_sent_time_ = time; }

    // İş mantığı metodları - basitleştirildi
    int64_t calculateFirstHopDelay() const;
    int64_t calculateDataAge() const;
    bool isDataFresh(int64_t threshold_ms = 1000) const;

    // Validation
    bool isValid() const;

    // Utility
    std::string toString() const;

    // Comparison operators
    bool operator==(const DelayCalcTrackData& other) const;
    bool operator!=(const DelayCalcTrackData& other) const;
    bool operator<(const DelayCalcTrackData& other) const;
};

} // namespace hat::domain::model