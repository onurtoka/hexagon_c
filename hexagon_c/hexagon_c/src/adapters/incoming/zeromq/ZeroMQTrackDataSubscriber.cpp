#include "ZeroMQTrackDataSubscriber.hpp"
#include <cstring>
#include <iostream>

namespace hat::adapters::incoming::zeromq {

ZeroMQTrackDataSubscriber::ZeroMQTrackDataSubscriber(
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission)
    : track_data_submission_(track_data_submission)
    , zmq_context_(nullptr)
    , delay_calc_subscriber_(nullptr)
    , running_(false)
    , delay_calc_endpoint_("tcp://localhost:7777")
    , topic_filter_("DELAY_CALC_DATA") {
    
    try {
        initializeSubscribers();
    } catch (const std::exception& e) {
        throw;
    }
}

ZeroMQTrackDataSubscriber::~ZeroMQTrackDataSubscriber() {
    stop();
    
    if (delay_calc_subscriber_) {
        zmq_close(delay_calc_subscriber_);
        delay_calc_subscriber_ = nullptr;
    }
    
    if (zmq_context_) {
        zmq_ctx_term(zmq_context_);
        zmq_context_ = nullptr;
    }
}

bool ZeroMQTrackDataSubscriber::start() {
    if (running_.load()) {
        return false;
    }

    running_.store(true);

    delay_calc_thread_ = std::thread([this]() {
        subscribeDelayCalcTrackData();
    });

    return true;
}

void ZeroMQTrackDataSubscriber::stop() {
    running_.store(false);

    if (delay_calc_thread_.joinable()) {
        delay_calc_thread_.join();
    }
}

bool ZeroMQTrackDataSubscriber::isRunning() const {
    return running_.load();
}

void ZeroMQTrackDataSubscriber::initializeSubscribers() {
    zmq_context_ = zmq_ctx_new();
    
    if (!zmq_context_) {
        throw std::runtime_error("Failed to create ZeroMQ context");
    }
    
    delay_calc_subscriber_ = zmq_socket(zmq_context_, ZMQ_SUB);
    
    if (!delay_calc_subscriber_) {
        zmq_ctx_term(zmq_context_);
        zmq_context_ = nullptr;
        throw std::runtime_error("Failed to create ZeroMQ subscriber socket");
    }
    
    int rc = zmq_connect(delay_calc_subscriber_, delay_calc_endpoint_.c_str());
    
    if (rc != 0) {
        zmq_close(delay_calc_subscriber_);
        zmq_ctx_term(zmq_context_);
        delay_calc_subscriber_ = nullptr;
        zmq_context_ = nullptr;
        throw std::runtime_error("Failed to connect to " + delay_calc_endpoint_);
    }
    
    if (!topic_filter_.empty()) {
        rc = zmq_setsockopt(delay_calc_subscriber_, ZMQ_SUBSCRIBE, 
                           topic_filter_.c_str(), topic_filter_.length());
    } else {
        rc = zmq_setsockopt(delay_calc_subscriber_, ZMQ_SUBSCRIBE, "", 0);
    }
    
    if (rc != 0) {
        zmq_close(delay_calc_subscriber_);
        zmq_ctx_term(zmq_context_);
        delay_calc_subscriber_ = nullptr;
        zmq_context_ = nullptr;
        throw std::runtime_error("Failed to set subscription options");
    }

    int timeout = 1;
    zmq_setsockopt(delay_calc_subscriber_, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    
    int rcv_hwm = 10;
    zmq_setsockopt(delay_calc_subscriber_, ZMQ_RCVHWM, &rcv_hwm, sizeof(rcv_hwm));
    
    int linger = 0;
    zmq_setsockopt(delay_calc_subscriber_, ZMQ_LINGER, &linger, sizeof(linger));
}

void ZeroMQTrackDataSubscriber::subscribeDelayCalcTrackData() {
    char buffer[4096];
    
    while (running_.load()) {
        try {
            int topic_size = zmq_recv(delay_calc_subscriber_, buffer, sizeof(buffer) - 1, ZMQ_DONTWAIT);
            
            if (topic_size > 0) {
                int more;
                size_t more_size = sizeof(more);
                
                zmq_getsockopt(delay_calc_subscriber_, ZMQ_RCVMORE, &more, &more_size);
                
                if (more) {
                    int data_size = zmq_recv(delay_calc_subscriber_, buffer, sizeof(buffer) - 1, 0);
                    
                    if (data_size > 0) {
                        buffer[data_size] = '\0';
                        
                        auto data = deserializeDelayCalcTrackData(buffer, static_cast<size_t>(data_size));
                        
                        if (data.has_value()) {
                            track_data_submission_->submitDelayCalcTrackData(data.value());
                        }
                    }
                }
            } else if (zmq_errno() == EAGAIN) {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            } else {
                throw std::runtime_error("ZeroMQ receive error: " + std::string(zmq_strerror(zmq_errno())));
            }
            
        } catch (const std::exception& e) {
        }
    }
}

std::optional<domain::model::DelayCalcTrackData> ZeroMQTrackDataSubscriber::deserializeDelayCalcTrackData(const char* data, size_t size) {
    try {
        if (data == nullptr || size == 0) {
            return std::nullopt;
        }
        
        std::string s(data, size);

        std::istringstream iss(s);
        iss.imbue(std::locale::classic());
        
        std::string token;
        std::vector<std::string> parts;

        while (std::getline(iss, token, '|')) {
            parts.push_back(token);
        }
        
        if (parts.size() != 12) {
            return std::nullopt;
        }

        auto to_i = [](const std::string& v) { return std::stoi(v); };
        auto to_d = [](const std::string& v) { return std::stod(v); };
        auto to_i64 = [](const std::string& v) { return static_cast<int64_t>(std::stoll(v)); };

        domain::model::DelayCalcTrackData data(
            to_i(parts[0]),
            to_d(parts[1]),
            to_d(parts[2]),
            to_d(parts[3]),
            to_d(parts[4]),
            to_d(parts[5]),
            to_d(parts[6]),
            to_i64(parts[7]),
            to_i64(parts[8]),
            to_i64(parts[9]),
            to_i64(parts[10]),
            to_i64(parts[11])
        );

        return data;
        
    } catch (...) {
        return std::nullopt;
    }
}

}