#pragma once

#include <cstdint>
#include <cmath>
#include <string>
#include <chrono>

namespace hat::domain::model {

/**
 * FinalCalcDelayData - Gelişmiş gecikme hesaplama ve radar tracking verilerini içeren final model
 * ECEF (Earth-Centered, Earth-Fixed) koordinat sistemi ve kapsamlı zaman analizi içerir
 */
class FinalCalcDelayData {
private:
    // Track ID
    int track_id_;                    // Track ID (*)
    int getTrackId() const;
    void setTrackId(int track_id);
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
    int64_t getSecondHopDelayTime() const;
    int64_t getTotalDelayTime() const;
    int64_t getThirdHopSentTime() const;
    void setVelocityECEF(double x_vel, double y_vel, double z_vel);
    void setXVelocityECEF(double x_vel);
    void setYVelocityECEF(double y_vel);
    void setZVelocityECEF(double z_vel);
    void setPositionECEF(double x_pos, double y_pos, double z_pos);
    void setXPositionECEF(double x_pos);
    void setYPositionECEF(double y_pos);
    void setZPositionECEF(double z_pos);
    void setUpdateTime(int64_t time);
    void setOriginalUpdateTime(int64_t time);
    void setFirstHopSentTime(int64_t time);
    void setFirstHopDelayTime(int64_t time);
    void setSecondHopSentTime(int64_t time);
    void setSecondHopDelayTime(int64_t time);
    void setTotalDelayTime(int64_t time);
    void setThirdHopSentTime(int64_t time);
    double getECEFSpeed() const;
    double getDistanceFromOrigin() const;
    int64_t calculateFirstHopActualDelay() const;
    int64_t calculateSecondHopActualDelay() const;
    int64_t calculateInterHopTime() const;
    int64_t calculateTotalProcessingTime() const;
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
    enum class PerformanceStatus {
        EXCELLENT,    // < 100ms toplam gecikme
        GOOD,         // 100-500ms
        ACCEPTABLE,   // 500-1000ms
        POOR,         // 1000-5000ms
        CRITICAL      // > 5000ms
    };
    PerformanceStatus getPerformanceStatus() const;
    bool isValid() const;
    std::string toString() const;
    bool operator==(const FinalCalcDelayData& other) const;
    bool operator!=(const FinalCalcDelayData& other) const;
    bool operator<(const FinalCalcDelayData& other) const;
    void setFirstHopDelayTime(int64_t time) { first_hop_delay_time_ = time; }
    void setSecondHopSentTime(int64_t time) { second_hop_sent_time_ = time; }
    void setSecondHopDelayTime(int64_t time) { second_hop_delay_time_ = time; }
    void setTotalDelayTime(int64_t time) { total_delay_time_ = time; }
    void setThirdHopSentTime(int64_t time) { third_hop_sent_time_ = time; }

    // İş mantığı metodları
    
    /**
     * ECEF koordinatlarından 3D hız büyüklüğünü hesaplar
     */
    double getECEFSpeed() const;
    double getDistanceFromOrigin() const;
    int64_t calculateFirstHopActualDelay() const;
    int64_t calculateSecondHopActualDelay() const;
    int64_t calculateInterHopTime() const;
    int64_t calculateTotalProcessingTime() const;
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
    enum class PerformanceStatus {
        EXCELLENT,    // < 100ms toplam gecikme
        GOOD,         // 100-500ms
        ACCEPTABLE,   // 500-1000ms
        POOR,         // 1000-5000ms
        CRITICAL      // > 5000ms
    };
    PerformanceStatus getPerformanceStatus() const;
    bool isValid() const;
    std::string toString() const;
    bool operator==(const FinalCalcDelayData& other) const;
    bool operator!=(const FinalCalcDelayData& other) const;
    bool operator<(const FinalCalcDelayData& other) const;
};

} // namespace hat::domain::model