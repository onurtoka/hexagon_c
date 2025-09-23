// DRAFT API'leri etkinleştirmek için
#define ZMQ_BUILD_DRAFT_API

#include "ZeroMQRadioPublisher.hpp"
#include <iostream>

namespace hat_b::adapters::outgoing::zeromq {

ZeroMQRadioPublisher::ZeroMQRadioPublisher(
    const std::string& multicast_endpoint,
    const std::string& group_name)
    : running_(false)
    , multicast_endpoint_(multicast_endpoint)
    , group_name_(group_name)
    , zmq_context_(1)  // 1 I/O thread
    , radio_socket_(nullptr)
    , total_published_(0)
    , failed_publications_(0) {
    
    initializeRadioSocket();
}

ZeroMQRadioPublisher::~ZeroMQRadioPublisher() {
    stop();
    // C++ wrapper RAII ile otomatik cleanup yapar
}

void ZeroMQRadioPublisher::initializeRadioSocket() {
    try {
        // RADIO socket oluştur (C++ wrapper ile) - Draft API gerekli
        radio_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::radio);

        // Performance optimizations
        radio_socket_->set(zmq::sockopt::sndhwm, 0);      // Unlimited send buffer
        radio_socket_->set(zmq::sockopt::sndtimeo, 0);    // Non-blocking send
        radio_socket_->set(zmq::sockopt::linger, 0);      // No linger on close
        radio_socket_->set(zmq::sockopt::tcp_keepalive, 1); // Keep connections alive
        radio_socket_->set(zmq::sockopt::tcp_keepalive_idle, 60);
        radio_socket_->set(zmq::sockopt::immediate, 1);   // Connect immediately
        
        // Endpoint'e connect (RADIO için connect kullanılır)
        radio_socket_->connect(multicast_endpoint_);

    } catch (const zmq::error_t& e) {
        std::cerr << "[RadioPublisher] ZMQ Initialize hatası: " << e.what() << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] Initialize hatası: " << e.what() << std::endl;
        throw;
    }
}

bool ZeroMQRadioPublisher::start() {
    if (running_.load()) {
        return false; // Zaten çalışıyor
    }

    running_.store(true);

    // Publisher worker thread'ini başlat
    publisher_thread_ = std::thread([this]() {
        // Real-time thread priority ayarla
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = 94; // High priority
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        
        // CPU affinity - dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset); // Core 0'a bind et
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        publisherWorker();
    });

    return true;
}

void ZeroMQRadioPublisher::stop() {
    running_.store(false);

    if (publisher_thread_.joinable()) {
        publisher_thread_.join();
    }
}

bool ZeroMQRadioPublisher::publishDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) {
    try {
        // Veriyi serialize et
        std::string serialized_data = serializeDelayCalcTrackData(data);
        
        // Gecikme ölçümü için timestamp ekle (nanosecond precision)
        auto send_time = std::chrono::steady_clock::now();
        auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
            send_time.time_since_epoch()).count();

        std::stringstream ss;
        ss << serialized_data << "|" << ns_since_epoch;
        std::string payload_with_timestamp = ss.str();

        // Mesajı kuyruğa ekle
        enqueueMessage(payload_with_timestamp);
        
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] Publish hatası: " << e.what() << std::endl;
        failed_publications_.fetch_add(1);
        return false;
    }
}

size_t ZeroMQRadioPublisher::publishBatchData(const std::vector<hat::domain::model::DelayCalcTrackData>& data_list) {
    size_t published_count = 0;
    
    for (const auto& data : data_list) {
        if (publishDelayCalcTrackData(data)) {
            published_count++;
        }
    }
    
    return published_count;
}

bool ZeroMQRadioPublisher::isPublisherActive() const {
    return running_.load();
}

ZeroMQRadioPublisher::PublisherStats ZeroMQRadioPublisher::getPublisherStats() const {
    PublisherStats stats;
    stats.total_published = total_published_.load();
    stats.failed_publications = failed_publications_.load();
    stats.average_latency_ms = 0.0; // Basit implementasyon
    return stats;
}

void ZeroMQRadioPublisher::publisherWorker() {
    while (running_.load()) {
        try {
            std::string message;
            if (dequeueMessage(message)) {
                // C++ wrapper ile mesaj gönder
                zmq::message_t zmq_message(message.begin(), message.end());
                
                // Grup set et (RADIO için set_group kullanılır)
                zmq_message.set_group(group_name_.c_str());
                
                // Mesajı gönder
                auto send_result = radio_socket_->send(zmq_message, zmq::send_flags::none);
                
                if (send_result && *send_result == message.size()) {
                    total_published_.fetch_add(1);
                } else {
                    failed_publications_.fetch_add(1);
                }
            } else {
                // Kuyruk boş, çok kısa bekle (mikrosaniye)
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }

        } catch (const zmq::error_t& e) {
            std::cerr << "[RadioPublisher] ZMQ Worker thread hatası: " << e.what() << std::endl;
            failed_publications_.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        } catch (const std::exception& e) {
            std::cerr << "[RadioPublisher] Worker thread hatası: " << e.what() << std::endl;
            failed_publications_.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
    }
}

std::string ZeroMQRadioPublisher::serializeDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) {
    // Basit JSON-like serialization
    std::stringstream ss;
    ss << "{\"type\":\"DelayCalcTrackData\"";
    ss << ",\"track_id\":" << data.getTrackId();
    ss << ",\"x\":" << data.getXPositionECEF();
    ss << ",\"y\":" << data.getYPositionECEF();
    ss << ",\"z\":" << data.getZPositionECEF();
    ss << ",\"vx\":" << data.getXVelocityECEF();
    ss << ",\"vy\":" << data.getYVelocityECEF();
    ss << ",\"vz\":" << data.getZVelocityECEF();
    ss << ",\"update_time\":" << data.getUpdateTime();
    ss << "}";
    
    return ss.str();
}

void ZeroMQRadioPublisher::enqueueMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    message_queue_.push(message);
}

bool ZeroMQRadioPublisher::dequeueMessage(std::string& message) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (message_queue_.empty()) {
        return false;
    }
    
    message = message_queue_.front();
    message_queue_.pop();
    return true;
}

} // namespace hat_b::adapters::outgoing::zeromq
