#include "FinalCalcDelayData.hpp"

namespace hat::domain::model {

// Constructor with parameters
FinalCalcDelayData::FinalCalcDelayData(int track_id,
                                     double x, double y, double z,
                                     int64_t original_update_time,
                                     int64_t first_hop_sent_time,
                                     int64_t second_hop_sent_time)
    : track_id_(track_id), x_(x), y_(y), z_(z),
      original_update_time_(original_update_time),
      first_hop_sent_time_(first_hop_sent_time),
      second_hop_sent_time_(second_hop_sent_time),
      third_hop_sent_time_(0),
      total_delay_time_(0),
      velocity_x_(0.0), velocity_y_(0.0), velocity_z_(0.0) {
}

// Default constructor
FinalCalcDelayData::FinalCalcDelayData() 
    : track_id_(0), x_(0.0), y_(0.0), z_(0.0),
      original_update_time_(0), first_hop_sent_time_(0),
      second_hop_sent_time_(0), third_hop_sent_time_(0),
      total_delay_time_(0),
      velocity_x_(0.0), velocity_y_(0.0), velocity_z_(0.0) {
}

// Copy constructor
FinalCalcDelayData::FinalCalcDelayData(const FinalCalcDelayData& other) 
    : track_id_(other.track_id_), x_(other.x_), y_(other.y_), z_(other.z_),
      original_update_time_(other.original_update_time_),
      first_hop_sent_time_(other.first_hop_sent_time_),
      second_hop_sent_time_(other.second_hop_sent_time_),
      third_hop_sent_time_(other.third_hop_sent_time_),
      total_delay_time_(other.total_delay_time_),
      velocity_x_(other.velocity_x_), velocity_y_(other.velocity_y_), velocity_z_(other.velocity_z_) {
}

// Assignment operator
FinalCalcDelayData& FinalCalcDelayData::operator=(const FinalCalcDelayData& other) {
    if (this != &other) {
        track_id_ = other.track_id_;
        x_ = other.x_;
        y_ = other.y_;
        z_ = other.z_;
        original_update_time_ = other.original_update_time_;
        first_hop_sent_time_ = other.first_hop_sent_time_;
        second_hop_sent_time_ = other.second_hop_sent_time_;
        third_hop_sent_time_ = other.third_hop_sent_time_;
        total_delay_time_ = other.total_delay_time_;
        velocity_x_ = other.velocity_x_;
        velocity_y_ = other.velocity_y_;
        velocity_z_ = other.velocity_z_;
    }
    return *this;
}

// Enhanced setters
void FinalCalcDelayData::setVelocityECEF(double x_vel, double y_vel, double z_vel) {
    velocity_x_ = x_vel;
    velocity_y_ = y_vel;
    velocity_z_ = z_vel;
}

void FinalCalcDelayData::setPositionECEF(double x_pos, double y_pos, double z_pos) {
    x_ = x_pos;
    y_ = y_pos;
    z_ = z_pos;
}

// Calculated properties
double FinalCalcDelayData::getECEFSpeed() const {
    return std::sqrt(velocity_x_ * velocity_x_ + 
                    velocity_y_ * velocity_y_ + 
                    velocity_z_ * velocity_z_);
}

double FinalCalcDelayData::getDistanceFromOrigin() const {
    return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

// Delay calculations
int64_t FinalCalcDelayData::calculateFirstHopActualDelay() const {
    return second_hop_sent_time_ - first_hop_sent_time_;
}

int64_t FinalCalcDelayData::calculateSecondHopActualDelay() const {
    return third_hop_sent_time_ - second_hop_sent_time_;
}

int64_t FinalCalcDelayData::calculateInterHopTime() const {
    return second_hop_sent_time_ - first_hop_sent_time_;
}

int64_t FinalCalcDelayData::calculateTotalProcessingTime() const {
    return third_hop_sent_time_ - original_update_time_;
}

// Performance metrics calculation
FinalCalcDelayData::PerformanceMetrics FinalCalcDelayData::calculatePerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Simple performance calculations
    int64_t total_time = calculateTotalProcessingTime();
    
    // Latency score (lower is better, scale 0-100)
    metrics.latency_score = std::min(100.0, std::max(0.0, 100.0 - (total_time / 1000000.0))); // nanoseconds to score
    
    // Throughput rating (simple calculation)
    metrics.throughput_rating = metrics.latency_score;
    
    // Efficiency index
    metrics.efficiency_index = (metrics.latency_score + metrics.throughput_rating) / 2.0;
    
    // Processing overhead
    metrics.processing_overhead = total_time;
    
    return metrics;
}

} // namespace hat::domain::model
