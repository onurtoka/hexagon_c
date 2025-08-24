#pragma once

#include "../../../domain/ports/incoming/TrackDataSubmission.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include <zmq.h>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <sstream>
#include <locale>
#include <optional>
#include <cstring>

namespace hat::adapters::incoming::zeromq {

class ZeroMQTrackDataSubscriber {
private:
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission_;
    
    void* zmq_context_;
    void* delay_calc_subscriber_;
    
    std::thread delay_calc_thread_;
    std::atomic<bool> running_;
    
    std::string delay_calc_endpoint_;
    std::string topic_filter_;

public:
    ZeroMQTrackDataSubscriber(
        std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission);

    ~ZeroMQTrackDataSubscriber();

    bool start();
    void stop();
    bool isRunning() const;

private:
    void initializeSubscribers();
    void subscribeDelayCalcTrackData();
    std::optional<domain::model::DelayCalcTrackData> deserializeDelayCalcTrackData(const char* data, size_t size);
};

}