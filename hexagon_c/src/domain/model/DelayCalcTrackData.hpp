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
                       int64_t second_hop_sent)
        : id_(id), velocity_(velocity), position_(position), time_field_(time_field)
        , track_id_(track_id)
        , x_velocity_ecef_(x_vel_ecef), y_velocity_ecef_(y_vel_ecef), z_velocity_ecef_(z_vel_ecef)
        , x_position_ecef_(x_pos_ecef), y_position_ecef_(y_pos_ecef), z_position_ecef_(z_pos_ecef)
        , update_time_(update_time), original_update_time_(original_update_time)
        , first_hop_sent_time_(first_hop_sent), first_hop_delay_time_(first_hop_delay)
        , second_hop_sent_time_(second_hop_sent) {}

    // Default constructor
    DelayCalcTrackData() 
        : id_(0), velocity_(0.0), position_(0.0), time_field_(0)
        , track_id_(0)
        , x_velocity_ecef_(0.0), y_velocity_ecef_(0.0), z_velocity_ecef_(0.0)
        , x_position_ecef_(0.0), y_position_ecef_(0.0), z_position_ecef_(0.0)
        , update_time_(0), original_update_time_(0)
        , first_hop_sent_time_(0), first_hop_delay_time_(0), second_hop_sent_time_(0) {}

    // Ana alanlar için getters
    int getId() const { return id_; }
    double getVelocity() const { return velocity_; }
    double getPosition() const { return position_; }
    int64_t getTimeField() const { return time_field_; }
    
    // Track ID
    int getTrackId() const { return track_id_; }
    
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

    // Ana alanlar için setters
    void setId(int id) { id_ = id; }
    void setVelocity(double velocity) { velocity_ = velocity; }
    void setPosition(double position) { position_ = position; }
    void setTimeField(int64_t time_field) { time_field_ = time_field; }
    
    // Track ID setter
    void setTrackId(int track_id) { track_id_ = track_id; }
    
    // ECEF Velocity setters
    void setVelocityECEF(double x_vel, double y_vel, double z_vel) {
        x_velocity_ecef_ = x_vel;
        y_velocity_ecef_ = y_vel;
        z_velocity_ecef_ = z_vel;
    }
    
    // ECEF Position setters
    void setPositionECEF(double x_pos, double y_pos, double z_pos) {
        x_position_ecef_ = x_pos;
        y_position_ecef_ = y_pos;
        z_position_ecef_ = z_pos;
    }
    
    // Zaman alanları setters
    void setUpdateTime(int64_t time) { update_time_ = time; }
    void setOriginalUpdateTime(int64_t time) { original_update_time_ = time; }
    void setFirstHopSentTime(int64_t time) { first_hop_sent_time_ = time; }
    void setFirstHopDelayTime(int64_t time) { first_hop_delay_time_ = time; }
    void setSecondHopSentTime(int64_t time) { second_hop_sent_time_ = time; }

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
     * İlk hop gecikmesini hesaplar
     */
    int64_t calculateFirstHopDelay() const {
        return first_hop_delay_time_ - first_hop_sent_time_;
    }

    /**
     * Toplam işlem gecikmesini hesaplar
     */
    int64_t calculateTotalProcessingDelay() const {
        return update_time_ - original_update_time_;
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

    // Validation
    bool isValid() const {
        return id_ > 0 && 
               track_id_ > 0 && 
               update_time_ > 0 && 
               original_update_time_ > 0;
    }

    // Utility
    std::string toString() const {
        return "DelayCalcTrackData{id=" + std::to_string(id_) + 
               ", track_id=" + std::to_string(track_id_) + 
               ", velocity=" + std::to_string(velocity_) + 
               ", position=" + std::to_string(position_) + 
               ", ecef_speed=" + std::to_string(getECEFSpeed()) + "}";
    }

    // Comparison operators
    bool operator==(const DelayCalcTrackData& other) const {
        return id_ == other.id_ && track_id_ == other.track_id_;
    }

    bool operator!=(const DelayCalcTrackData& other) const {
        return !(*this == other);
    }

    bool operator<(const DelayCalcTrackData& other) const {
        return update_time_ < other.update_time_;
    }
};

} // namespace hat::domain::model