#include "DelayCalcTrackData.hpp"

namespace hat::domain::model {

DelayCalcTrackData::DelayCalcTrackData(int id, double velocity, double position, int64_t time_field,
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

DelayCalcTrackData::DelayCalcTrackData()
	: id_(0), velocity_(0.0), position_(0.0), time_field_(0)
	, track_id_(0)
	, x_velocity_ecef_(0.0), y_velocity_ecef_(0.0), z_velocity_ecef_(0.0)
	, x_position_ecef_(0.0), y_position_ecef_(0.0), z_position_ecef_(0.0)
	, update_time_(0), original_update_time_(0)
	, first_hop_sent_time_(0), first_hop_delay_time_(0), second_hop_sent_time_(0) {}

} // namespace hat::domain::model
