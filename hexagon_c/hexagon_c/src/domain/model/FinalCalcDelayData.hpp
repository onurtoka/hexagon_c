#pragma once

#include <chrono>
#include <cmath>

namespace hat::domain::model {

/**
 * FINAL CALCULATION DELAY DATA - Enhanced track data with performance metrics
 * 
 * Bu class DelayCalcTrackData'dan türetilen ve ek gecikme analizleri içeren
 * gelişmiş veri modelidir. Performance monitoring ve reporting için kullanılır.
 */
class FinalCalcDelayData {
public:
    // Performance metrics yapısı
    struct PerformanceMetrics {
        double latency_score;          // Gecikme skoru (0-100)
        double throughput_rating;      // Verim oranı
        double efficiency_index;       // Verimlilik indeksi
        int64_t processing_overhead;   // İşleme ek yükü (microseconds)
    };

    // Constructors
    FinalCalcDelayData(int track_id, 
                      double x, double y, double z,
                      int64_t original_update_time,
                      int64_t first_hop_sent_time,
                      int64_t second_hop_sent_time);
    
    FinalCalcDelayData(); // Default constructor
    FinalCalcDelayData(const FinalCalcDelayData& other); // Copy constructor
    FinalCalcDelayData& operator=(const FinalCalcDelayData& other); // Assignment operator

    // Basic getters
    int getTrackId() const { return track_id_; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    double getZ() const { return z_; }
    
    // Timing getters
    int64_t getOriginalUpdateTime() const { return original_update_time_; }
    int64_t getFirstHopSentTime() const { return first_hop_sent_time_; }
    int64_t getSecondHopSentTime() const { return second_hop_sent_time_; }
    int64_t getThirdHopSentTime() const { return third_hop_sent_time_; }
    int64_t getTotalDelayTime() const { return total_delay_time_; }

    // Velocity getters
    double getVelocityX() const { return velocity_x_; }
    double getVelocityY() const { return velocity_y_; }
    double getVelocityZ() const { return velocity_z_; }

    // Basic setters
    void setTrackId(int track_id) { track_id_ = track_id; }
    void setX(double x) { x_ = x; }
    void setY(double y) { y_ = y; }
    void setZ(double z) { z_ = z; }
    
    // Timing setters
    void setOriginalUpdateTime(int64_t time) { original_update_time_ = time; }
    void setFirstHopSentTime(int64_t time) { first_hop_sent_time_ = time; }
    void setSecondHopSentTime(int64_t time) { second_hop_sent_time_ = time; }
    void setThirdHopSentTime(int64_t time) { third_hop_sent_time_ = time; }
    void setTotalDelayTime(int64_t delay) { total_delay_time_ = delay; }

    // Enhanced setters
    void setVelocityECEF(double x_vel, double y_vel, double z_vel);
    void setPositionECEF(double x_pos, double y_pos, double z_pos);

    // Calculated properties
    double getECEFSpeed() const;
    double getDistanceFromOrigin() const;
    
    // Delay calculations
    int64_t calculateFirstHopActualDelay() const;
    int64_t calculateSecondHopActualDelay() const;
    int64_t calculateInterHopTime() const;
    int64_t calculateTotalProcessingTime() const;
    
    // Performance metrics
    PerformanceMetrics calculatePerformanceMetrics() const;

private:
    // Core data
    int track_id_;
    double x_, y_, z_;
    
    // Timing information
    int64_t original_update_time_;    // İlk güncelleme zamanı
    int64_t first_hop_sent_time_;     // A -> B gönderim zamanı
    int64_t second_hop_sent_time_;    // B -> C gönderim zamanı
    int64_t third_hop_sent_time_;     // C'de işlem zamanı
    int64_t total_delay_time_;        // Toplam gecikme
    
    // Enhanced data
    double velocity_x_, velocity_y_, velocity_z_;
};

} // namespace hat::domain::model
