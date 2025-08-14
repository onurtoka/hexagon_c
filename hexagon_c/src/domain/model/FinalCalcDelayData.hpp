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
    // Constructor - tüm alanları alan
    FinalCalcDelayData(int track_id,
                       double x_vel_ecef, double y_vel_ecef, double z_vel_ecef,
                       double x_pos_ecef, double y_pos_ecef, double z_pos_ecef,
                       int64_t update_time, int64_t original_update_time,
                       int64_t first_hop_sent, int64_t first_hop_delay,
                       int64_t second_hop_sent, int64_t second_hop_delay,
                       int64_t total_delay, int64_t third_hop_sent)
        : track_id_(track_id)
        , x_velocity_ecef_(x_vel_ecef), y_velocity_ecef_(y_vel_ecef), z_velocity_ecef_(z_vel_ecef)
        , x_position_ecef_(x_pos_ecef), y_position_ecef_(y_pos_ecef), z_position_ecef_(z_pos_ecef)
        , update_time_(update_time), original_update_time_(original_update_time)
        , first_hop_sent_time_(first_hop_sent), first_hop_delay_time_(first_hop_delay)
        , second_hop_sent_time_(second_hop_sent), second_hop_delay_time_(second_hop_delay)
        , total_delay_time_(total_delay), third_hop_sent_time_(third_hop_sent) {}

    // Default constructor
    FinalCalcDelayData() 
        : track_id_(0)
        , x_velocity_ecef_(0.0), y_velocity_ecef_(0.0), z_velocity_ecef_(0.0)
        , x_position_ecef_(0.0), y_position_ecef_(0.0), z_position_ecef_(0.0)
        , update_time_(0), original_update_time_(0)
        , first_hop_sent_time_(0), first_hop_delay_time_(0)
        , second_hop_sent_time_(0), second_hop_delay_time_(0)
        , total_delay_time_(0), third_hop_sent_time_(0) {}

    // Copy constructor
    FinalCalcDelayData(const FinalCalcDelayData& other) 
        : track_id_(other.track_id_)
        , x_velocity_ecef_(other.x_velocity_ecef_), y_velocity_ecef_(other.y_velocity_ecef_), z_velocity_ecef_(other.z_velocity_ecef_)
        , x_position_ecef_(other.x_position_ecef_), y_position_ecef_(other.y_position_ecef_), z_position_ecef_(other.z_position_ecef_)
        , update_time_(other.update_time_), original_update_time_(other.original_update_time_)
        , first_hop_sent_time_(other.first_hop_sent_time_), first_hop_delay_time_(other.first_hop_delay_time_)
        , second_hop_sent_time_(other.second_hop_sent_time_), second_hop_delay_time_(other.second_hop_delay_time_)
        , total_delay_time_(other.total_delay_time_), third_hop_sent_time_(other.third_hop_sent_time_) {}

    // Assignment operator
    FinalCalcDelayData& operator=(const FinalCalcDelayData& other) {
        if (this != &other) {
            track_id_ = other.track_id_;
            x_velocity_ecef_ = other.x_velocity_ecef_;
            y_velocity_ecef_ = other.y_velocity_ecef_;
            z_velocity_ecef_ = other.z_velocity_ecef_;
            x_position_ecef_ = other.x_position_ecef_;
            y_position_ecef_ = other.y_position_ecef_;
            z_position_ecef_ = other.z_position_ecef_;
            update_time_ = other.update_time_;
            original_update_time_ = other.original_update_time_;
            first_hop_sent_time_ = other.first_hop_sent_time_;
            first_hop_delay_time_ = other.first_hop_delay_time_;
            second_hop_sent_time_ = other.second_hop_sent_time_;
            second_hop_delay_time_ = other.second_hop_delay_time_;
            total_delay_time_ = other.total_delay_time_;
            third_hop_sent_time_ = other.third_hop_sent_time_;
        }
        return *this;
    }

    // Track ID getter/setter
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

    // ECEF Velocity setters
    void setVelocityECEF(double x_vel, double y_vel, double z_vel) {
        x_velocity_ecef_ = x_vel;
        y_velocity_ecef_ = y_vel;
        z_velocity_ecef_ = z_vel;
    }
    
    void setXVelocityECEF(double x_vel) { x_velocity_ecef_ = x_vel; }
    void setYVelocityECEF(double y_vel) { y_velocity_ecef_ = y_vel; }
    void setZVelocityECEF(double z_vel) { z_velocity_ecef_ = z_vel; }
    
    // ECEF Position setters
    void setPositionECEF(double x_pos, double y_pos, double z_pos) {
        x_position_ecef_ = x_pos;
        y_position_ecef_ = y_pos;
        z_position_ecef_ = z_pos;
    }
    
    void setXPositionECEF(double x_pos) { x_position_ecef_ = x_pos; }
    void setYPositionECEF(double y_pos) { y_position_ecef_ = y_pos; }
    void setZPositionECEF(double z_pos) { z_position_ecef_ = z_pos; }
    
    // Zaman alanları setters
    void setUpdateTime(int64_t time) { update_time_ = time; }
    void setOriginalUpdateTime(int64_t time) { original_update_time_ = time; }
    void setFirstHopSentTime(int64_t time) { first_hop_sent_time_ = time; }
    void setFirstHopDelayTime(int64_t time) { first_hop_delay_time_ = time; }
    void setSecondHopSentTime(int64_t time) { second_hop_sent_time_ = time; }
    void setSecondHopDelayTime(int64_t time) { second_hop_delay_time_ = time; }
    void setTotalDelayTime(int64_t time) { total_delay_time_ = time; }
    void setThirdHopSentTime(int64_t time) { third_hop_sent_time_ = time; }

    // İş mantığı metodları
    
    /**
     * ECEF koordinatlarından 3D hız büyüklüğünü hesaplar
     */
    double getECEFSpeed() const {
        return std::sqrt(x_velocity_ecef_ * x_velocity_ecef_ + 
                        y_velocity_ecef_ * y_velocity_ecef_ + 
                        z_velocity_ecef_ * z_velocity_ecef_);
    }
    
    /**
     * ECEF koordinatlarından orijine uzaklığı hesaplar
     */
    double getDistanceFromOrigin() const {
        return std::sqrt(x_position_ecef_ * x_position_ecef_ + 
                        y_position_ecef_ * y_position_ecef_ + 
                        z_position_ecef_ * z_position_ecef_);
    }

    /**
     * İlk hop gecikmesini hesaplar (gerçek gecikme)
     */
    int64_t calculateFirstHopActualDelay() const {
        return first_hop_delay_time_ - first_hop_sent_time_;
    }

    /**
     * İkinci hop gecikmesini hesaplar (gerçek gecikme)
     */
    int64_t calculateSecondHopActualDelay() const {
        return second_hop_delay_time_ - second_hop_sent_time_;
    }

    /**
     * Hop'lar arası süreyi hesaplar
     */
    int64_t calculateInterHopTime() const {
        return second_hop_sent_time_ - first_hop_sent_time_;
    }

    /**
     * Üçüncü hop'a kadar geçen toplam süreyi hesaplar
     */
    int64_t calculateTotalProcessingTime() const {
        return third_hop_sent_time_ - original_update_time_;
    }

    /**
     * Sistem performans metrikleri hesaplar
     */
    struct PerformanceMetrics {
        int64_t first_hop_delay;
        int64_t second_hop_delay;
        int64_t inter_hop_time;
        int64_t total_processing_time;
        int64_t end_to_end_delay;
        double average_hop_delay;
    };

    PerformanceMetrics calculatePerformanceMetrics() const {
        PerformanceMetrics metrics;
        metrics.first_hop_delay = calculateFirstHopActualDelay();
        metrics.second_hop_delay = calculateSecondHopActualDelay();
        metrics.inter_hop_time = calculateInterHopTime();
        metrics.total_processing_time = calculateTotalProcessingTime();
        metrics.end_to_end_delay = update_time_ - original_update_time_;
        metrics.average_hop_delay = (metrics.first_hop_delay + metrics.second_hop_delay) / 2.0;
        return metrics;
    }

    /**
     * Mevcut zamana göre veri yaşını hesaplar
     */
    int64_t calculateDataAge() const {
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        return now - update_time_;
    }

    /**
     * Verinin güncel olup olmadığını kontrol eder
     */
    bool isDataFresh(int64_t threshold_ms = 1000) const {
        return calculateDataAge() <= threshold_ms;
    }

    /**
     * Yüksek hız uyarısı
     */
    bool isHighVelocity(double threshold = 100.0) const {
        return getECEFSpeed() > threshold;
    }

    /**
     * Gecikme uyarısı - kritik eşik kontrolü
     */
    bool isDelayExcessive(int64_t threshold_ms = 5000) const {
        return total_delay_time_ > threshold_ms;
    }

    /**
     * Sistem performans durumu analizi
     */
    enum class PerformanceStatus {
        EXCELLENT,    // < 100ms toplam gecikme
        GOOD,         // 100-500ms
        ACCEPTABLE,   // 500-1000ms
        POOR,         // 1000-5000ms
        CRITICAL      // > 5000ms
    };

    PerformanceStatus getPerformanceStatus() const {
        if (total_delay_time_ < 100) return PerformanceStatus::EXCELLENT;
        if (total_delay_time_ < 500) return PerformanceStatus::GOOD;
        if (total_delay_time_ < 1000) return PerformanceStatus::ACCEPTABLE;
        if (total_delay_time_ < 5000) return PerformanceStatus::POOR;
        return PerformanceStatus::CRITICAL;
    }

    // Validation
    bool isValid() const {
        return track_id_ > 0 && 
               update_time_ > 0 && 
               original_update_time_ > 0 &&
               first_hop_sent_time_ > 0 &&
               second_hop_sent_time_ > 0;
    }

    // Utility
    std::string toString() const {
        return "FinalCalcDelayData{track_id=" + std::to_string(track_id_) + 
               ", ecef_speed=" + std::to_string(getECEFSpeed()) + 
               ", total_delay=" + std::to_string(total_delay_time_) + "ms" +
               ", data_age=" + std::to_string(calculateDataAge()) + "ms}";
    }

    // Comparison operators
    bool operator==(const FinalCalcDelayData& other) const {
        return track_id_ == other.track_id_;
    }

    bool operator!=(const FinalCalcDelayData& other) const {
        return !(*this == other);
    }

    bool operator<(const FinalCalcDelayData& other) const {
        return update_time_ < other.update_time_;
    }
};

} // namespace hat::domain::model