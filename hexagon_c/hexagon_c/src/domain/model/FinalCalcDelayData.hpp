#pragma once

#include <cstdint>
#include <string>

namespace hat::domain::model {

/**
 * FinalCalcDelayData - Gelişmiş gecikme hesaplama ve radar tracking verilerini içeren final model
 * ECEF (Earth-Centered, Earth-Fixed) koordinat sistemi ve kapsamlı zaman analizi içerir
 */
class FinalCalcDelayData {
private:
    // Track ID
    int track_id_;                    // Track ID (*)
    
    // Velocity components in ECEF coordinate system
    double x_velocity_ecef_;          // XVelocity ECEF
    double y_velocity_ecef_;          // YVelocity ECEF
    double z_velocity_ecef_;          // ZVelocity ECEF
    
    // Position components in ECEF coordinate system
    double x_position_ecef_;          // XPositionECEF
    double y_position_ecef_;          // YPositionECEF
    double z_position_ecef_;          // ZPositionECEF
    
    // Zaman alanları (long - milliseconds since epoch)
    int64_t update_time_;             // Update Time
    int64_t original_update_time_;    // Original UpdateTime
    int64_t first_hop_sent_time_;     // FirstHopSentTime
    int64_t first_hop_delay_time_;    // FirstHopDelay Time
    int64_t second_hop_sent_time_;    // SecondHopSentTime
    int64_t second_hop_delay_time_;   // SecondHopDelayTime
    int64_t total_delay_time_;        // Total Delay Time
    int64_t third_hop_sent_time_;     // ThirdHopSentTime

public:
    // Constructors
    FinalCalcDelayData(int track_id,
                       double x_vel_ecef, double y_vel_ecef, double z_vel_ecef,
                       double x_pos_ecef, double y_pos_ecef, double z_pos_ecef,
                       int64_t update_time, int64_t original_update_time,
                       int64_t first_hop_sent, int64_t first_hop_delay,
                       int64_t second_hop_sent, int64_t second_hop_delay,
                       int64_t total_delay, int64_t third_hop_sent);

    FinalCalcDelayData();
    FinalCalcDelayData(const FinalCalcDelayData& other);
    FinalCalcDelayData& operator=(const FinalCalcDelayData& other);

    // Getters
    int getTrackId() const { return track_id_; }
    void setTrackId(int track_id) { track_id_ = track_id; }
    
    // ECEF Velocity getters
    double getXVelocityECEF() const { return x_velocity_ecef_; }
    double getYVelocityECEF() const { return y_velocity_ecef_; }
    double getZVelocityECEF() const { return z_velocity_ecef_; }
    
    // ECEF Position getters
    double getXPositionECEF() const { return x_position_ecef_; }
    double getYPositionECEF() const { return y_position_ecef_; }
    double getZPositionECEF() const { return z_position_ecef_; }
    
    // Zaman alanları getters
    int64_t getUpdateTime() const { return update_time_; }
    int64_t getOriginalUpdateTime() const { return original_update_time_; }
    int64_t getFirstHopSentTime() const { return first_hop_sent_time_; }
    int64_t getFirstHopDelayTime() const { return first_hop_delay_time_; }
    int64_t getSecondHopSentTime() const { return second_hop_sent_time_; }
    int64_t getSecondHopDelayTime() const { return second_hop_delay_time_; }
    int64_t getTotalDelayTime() const { return total_delay_time_; }
    int64_t getThirdHopSentTime() const { return third_hop_sent_time_; }

    // ECEF Setters
    void setVelocityECEF(double x_vel, double y_vel, double z_vel);
    void setPositionECEF(double x_pos, double y_pos, double z_pos);
    void setXVelocityECEF(double x_vel) { x_velocity_ecef_ = x_vel; }
    void setYVelocityECEF(double y_vel) { y_velocity_ecef_ = y_vel; }
    void setZVelocityECEF(double z_vel) { z_velocity_ecef_ = z_vel; }
    void setXPositionECEF(double x_pos) { x_position_ecef_ = x_pos; }
    void setYPositionECEF(double y_pos) { y_position_ecef_ = y_pos; }
    void setZPositionECEF(double z_pos) { z_position_ecef_ = z_pos; }
    
    // Zaman setters
    void setUpdateTime(int64_t time) { update_time_ = time; }
    void setOriginalUpdateTime(int64_t time) { original_update_time_ = time; }
    void setFirstHopSentTime(int64_t time) { first_hop_sent_time_ = time; }
    void setFirstHopDelayTime(int64_t time) { first_hop_delay_time_ = time; }
    void setSecondHopSentTime(int64_t time) { second_hop_sent_time_ = time; }
    void setSecondHopDelayTime(int64_t time) { second_hop_delay_time_ = time; }
    void setTotalDelayTime(int64_t time) { total_delay_time_ = time; }
    void setThirdHopSentTime(int64_t time) { third_hop_sent_time_ = time; }

    // İş mantığı metodları
    double getECEFSpeed() const;
    double getDistanceFromOrigin() const;
    int64_t calculateFirstHopActualDelay() const;
    int64_t calculateSecondHopActualDelay() const;
    int64_t calculateInterHopTime() const;
    int64_t calculateTotalProcessingTime() const;

    // Performance metrics
    struct PerformanceMetrics {
        int64_t first_hop_delay;
        int64_t second_hop_delay;
        int64_t inter_hop_time;
        int64_t total_processing_time;
        int64_t end_to_end_delay;
        double average_hop_delay;
    };

    PerformanceMetrics calculatePerformanceMetrics() const;
    int64_t calculateDataAge() const;
    bool isDataFresh(int64_t threshold_ms = 1000) const;
    bool isHighVelocity(double threshold = 100.0) const;
    bool isDelayExcessive(int64_t threshold_ms = 5000) const;

    // Performance status
    enum class PerformanceStatus {
        EXCELLENT,    // < 100ms toplam gecikme
        GOOD,         // 100-500ms
        ACCEPTABLE,   // 500-1000ms
        POOR,         // 1000-5000ms
        CRITICAL      // > 5000ms
    };

    PerformanceStatus getPerformanceStatus() const;

    // Validation
    bool isValid() const;

    // Utility
    std::string toString() const;

    // Comparison operators
    bool operator==(const FinalCalcDelayData& other) const;
    bool operator!=(const FinalCalcDelayData& other) const;
    bool operator<(const FinalCalcDelayData& other) const;
};

} // namespace hat::domain::model