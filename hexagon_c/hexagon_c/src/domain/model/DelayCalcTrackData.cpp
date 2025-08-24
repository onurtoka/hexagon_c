#include "DelayCalcTrackData.hpp"
#include <cmath>
#include <chrono>

namespace hat::domain::model {

DelayCalcTrackData::DelayCalcTrackData(int track_id,
                   double x_vel_ecef, double y_vel_ecef, double z_vel_ecef,
                   double x_pos_ecef, double y_pos_ecef, double z_pos_ecef,
                   int64_t update_time, int64_t original_update_time,
                   int64_t first_hop_sent, int64_t first_hop_delay, 
                   int64_t second_hop_sent)
    : track_id_(track_id)
    , x_velocity_ecef_(x_vel_ecef), y_velocity_ecef_(y_vel_ecef), z_velocity_ecef_(z_vel_ecef)
    , x_position_ecef_(x_pos_ecef), y_position_ecef_(y_pos_ecef), z_position_ecef_(z_pos_ecef)
    , update_time_(update_time), original_update_time_(original_update_time)
    , first_hop_sent_time_(first_hop_sent), first_hop_delay_time_(first_hop_delay)
    , second_hop_sent_time_(second_hop_sent) {
}

DelayCalcTrackData::DelayCalcTrackData() 
    : track_id_(0)
    , x_velocity_ecef_(0.0), y_velocity_ecef_(0.0), z_velocity_ecef_(0.0)
    , x_position_ecef_(0.0), y_position_ecef_(0.0), z_position_ecef_(0.0)
    , update_time_(0), original_update_time_(0)
    , first_hop_sent_time_(0), first_hop_delay_time_(0), second_hop_sent_time_(0) {
}

void DelayCalcTrackData::setVelocityECEF(double x_vel, double y_vel, double z_vel) {
    x_velocity_ecef_ = x_vel;
    y_velocity_ecef_ = y_vel;
    z_velocity_ecef_ = z_vel;
}

void DelayCalcTrackData::setPositionECEF(double x_pos, double y_pos, double z_pos) {
    x_position_ecef_ = x_pos;
    y_position_ecef_ = y_pos;
    z_position_ecef_ = z_pos;
}

// Velocity calculation methods removed - unnecessary complexity

int64_t DelayCalcTrackData::calculateFirstHopDelay() const {
    return first_hop_delay_time_ - first_hop_sent_time_;
}

int64_t DelayCalcTrackData::calculateDataAge() const {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return now - update_time_;
}

bool DelayCalcTrackData::isDataFresh(int64_t threshold_ms) const {
    return calculateDataAge() <= threshold_ms;
}

// isHighVelocity removed - unnecessary complexity

bool DelayCalcTrackData::isValid() const {
    return track_id_ > 0 && 
           update_time_ > 0 && 
           original_update_time_ > 0;
}

std::string DelayCalcTrackData::toString() const {
    return "DelayCalcTrackData{track_id=" + std::to_string(track_id_) + 
           ", update_time=" + std::to_string(update_time_) + "}";
}

bool DelayCalcTrackData::operator==(const DelayCalcTrackData& other) const {
    return track_id_ == other.track_id_;
}

bool DelayCalcTrackData::operator!=(const DelayCalcTrackData& other) const {
    return !(*this == other);
}

bool DelayCalcTrackData::operator<(const DelayCalcTrackData& other) const {
    return update_time_ < other.update_time_;
}

} // namespace hat::domain::model
