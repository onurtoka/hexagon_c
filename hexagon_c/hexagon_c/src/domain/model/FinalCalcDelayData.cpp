#include "FinalCalcDelayData.hpp"
#include <cmath>
#include <chrono>

namespace hat::domain::model {

FinalCalcDelayData::FinalCalcDelayData(int track_id,
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
    , total_delay_time_(total_delay), third_hop_sent_time_(third_hop_sent) {
}

FinalCalcDelayData::FinalCalcDelayData() 
    : track_id_(0)
    , x_velocity_ecef_(0.0), y_velocity_ecef_(0.0), z_velocity_ecef_(0.0)
    , x_position_ecef_(0.0), y_position_ecef_(0.0), z_position_ecef_(0.0)
    , update_time_(0), original_update_time_(0)
    , first_hop_sent_time_(0), first_hop_delay_time_(0)
    , second_hop_sent_time_(0), second_hop_delay_time_(0)
    , total_delay_time_(0), third_hop_sent_time_(0) {
}

FinalCalcDelayData::FinalCalcDelayData(const FinalCalcDelayData& other) 
    : track_id_(other.track_id_)
    , x_velocity_ecef_(other.x_velocity_ecef_), y_velocity_ecef_(other.y_velocity_ecef_), z_velocity_ecef_(other.z_velocity_ecef_)
    , x_position_ecef_(other.x_position_ecef_), y_position_ecef_(other.y_position_ecef_), z_position_ecef_(other.z_position_ecef_)
    , update_time_(other.update_time_), original_update_time_(other.original_update_time_)
    , first_hop_sent_time_(other.first_hop_sent_time_), first_hop_delay_time_(other.first_hop_delay_time_)
    , second_hop_sent_time_(other.second_hop_sent_time_), second_hop_delay_time_(other.second_hop_delay_time_)
    , total_delay_time_(other.total_delay_time_), third_hop_sent_time_(other.third_hop_sent_time_) {
}

FinalCalcDelayData& FinalCalcDelayData::operator=(const FinalCalcDelayData& other) {
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

void FinalCalcDelayData::setVelocityECEF(double x_vel, double y_vel, double z_vel) {
    x_velocity_ecef_ = x_vel;
    y_velocity_ecef_ = y_vel;
    z_velocity_ecef_ = z_vel;
}

void FinalCalcDelayData::setPositionECEF(double x_pos, double y_pos, double z_pos) {
    x_position_ecef_ = x_pos;
    y_position_ecef_ = y_pos;
    z_position_ecef_ = z_pos;
}

double FinalCalcDelayData::getECEFSpeed() const {
    return std::sqrt(x_velocity_ecef_ * x_velocity_ecef_ + 
                    y_velocity_ecef_ * y_velocity_ecef_ + 
                    z_velocity_ecef_ * z_velocity_ecef_);
}

double FinalCalcDelayData::getDistanceFromOrigin() const {
    return std::sqrt(x_position_ecef_ * x_position_ecef_ + 
                    y_position_ecef_ * y_position_ecef_ + 
                    z_position_ecef_ * z_position_ecef_);
}

int64_t FinalCalcDelayData::calculateFirstHopActualDelay() const {
    return first_hop_delay_time_ - first_hop_sent_time_;
}

int64_t FinalCalcDelayData::calculateSecondHopActualDelay() const {
    return second_hop_delay_time_ - second_hop_sent_time_;
}

int64_t FinalCalcDelayData::calculateInterHopTime() const {
    return second_hop_sent_time_ - first_hop_sent_time_;
}

int64_t FinalCalcDelayData::calculateTotalProcessingTime() const {
    return third_hop_sent_time_ - original_update_time_;
}

FinalCalcDelayData::PerformanceMetrics FinalCalcDelayData::calculatePerformanceMetrics() const {
    PerformanceMetrics metrics;
    metrics.first_hop_delay = calculateFirstHopActualDelay();
    metrics.second_hop_delay = calculateSecondHopActualDelay();
    metrics.inter_hop_time = calculateInterHopTime();
    metrics.total_processing_time = calculateTotalProcessingTime();
    metrics.end_to_end_delay = update_time_ - original_update_time_;
    metrics.average_hop_delay = (metrics.first_hop_delay + metrics.second_hop_delay) / 2.0;
    return metrics;
}

int64_t FinalCalcDelayData::calculateDataAge() const {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return now - update_time_;
}

bool FinalCalcDelayData::isDataFresh(int64_t threshold_ms) const {
    return calculateDataAge() <= threshold_ms;
}

bool FinalCalcDelayData::isHighVelocity(double threshold) const {
    return getECEFSpeed() > threshold;
}

bool FinalCalcDelayData::isDelayExcessive(int64_t threshold_ms) const {
    return total_delay_time_ > threshold_ms;
}

FinalCalcDelayData::PerformanceStatus FinalCalcDelayData::getPerformanceStatus() const {
    if (total_delay_time_ < 100) return PerformanceStatus::EXCELLENT;
    if (total_delay_time_ < 500) return PerformanceStatus::GOOD;
    if (total_delay_time_ < 1000) return PerformanceStatus::ACCEPTABLE;
    if (total_delay_time_ < 5000) return PerformanceStatus::POOR;
    return PerformanceStatus::CRITICAL;
}

bool FinalCalcDelayData::isValid() const {
    return track_id_ > 0 && 
           update_time_ > 0 && 
           original_update_time_ > 0 &&
           first_hop_sent_time_ > 0 &&
           second_hop_sent_time_ > 0;
}

std::string FinalCalcDelayData::toString() const {
    return "FinalCalcDelayData{track_id=" + std::to_string(track_id_) + 
           ", ecef_speed=" + std::to_string(getECEFSpeed()) + 
           ", total_delay=" + std::to_string(total_delay_time_) + "ms" +
           ", data_age=" + std::to_string(calculateDataAge()) + "ms}";
}

bool FinalCalcDelayData::operator==(const FinalCalcDelayData& other) const {
    return track_id_ == other.track_id_;
}

bool FinalCalcDelayData::operator!=(const FinalCalcDelayData& other) const {
    return !(*this == other);
}

bool FinalCalcDelayData::operator<(const FinalCalcDelayData& other) const {
    return update_time_ < other.update_time_;
}

} // namespace hat::domain::model
