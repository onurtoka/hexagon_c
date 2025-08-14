#pragma once

#include "../../../domain/ports/incoming/TrackDataSubmission.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include "../../../domain/model/FinalCalcDelayData.hpp"
#include "../../../../include/zmq/zmq.hpp"
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <functional>
#include <sstream>
#include <locale>

namespace hat::adapters::incoming::zeromq {

/**
 * Primary adapter: ZeroMQ üzerinden veri alan ve domain'e ileten adapter
 * Hexagonal mimaride dış dünyadan sisteme gelen veriler için giriş noktası
 */
class ZeroMQTrackDataSubscriber {
private:
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission_;
    
    // ZeroMQ context ve socket
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> delay_calc_subscriber_;
    
    // Thread yönetimi
    std::thread delay_calc_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string delay_calc_endpoint_;
    std::string topic_filter_;
    
    // İstatistikler
    std::atomic<size_t> total_received_;
    std::atomic<size_t> successful_processed_;
    std::atomic<size_t> failed_processed_;

public:
    /**
     * Constructor
     * @param track_data_submission Domain katmanına veri gönderecek port
     * @param delay_calc_endpoint DelayCalcTrackData için ZeroMQ endpoint
     * @param topic_filter Topic filtresi (opsiyonel)
     */
    ZeroMQTrackDataSubscriber(
        std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
        const std::string& delay_calc_endpoint = "tcp://localhost:5555",
        const std::string& topic_filter = "")
        : track_data_submission_(track_data_submission)
        , context_(1)
        , running_(false)
        , delay_calc_endpoint_(delay_calc_endpoint)
        , topic_filter_(topic_filter)
        , total_received_(0)
        , successful_processed_(0)
        , failed_processed_(0) {
        
        initializeSubscribers();
    }

    ~ZeroMQTrackDataSubscriber() {
        stop();
    }

    /**
     * Subscriber'ı başlatır
     */
    bool start() {
        if (running_.load()) {
            return false; // Zaten çalışıyor
        }

        if (!track_data_submission_->isReadyToReceive()) {
            return false; // Domain hazır değil
        }

        running_.store(true);

        // DelayCalcTrackData için thread başlat
        delay_calc_thread_ = std::thread([this]() {
            subscribeDelayCalcTrackData();
        });

        return true;
    }

    /**
     * Subscriber'ı durdurur
     */
    void stop() {
        running_.store(false);

        if (delay_calc_thread_.joinable()) {
            delay_calc_thread_.join();
        }
    }

    /**
     * Subscriber durumunu kontrol eder
     */
    bool isRunning() const {
        return running_.load();
    }

    /**
     * İstatistikleri döner
     */
    struct SubscriberStats {
        size_t total_received;
        size_t successful_processed;
        size_t failed_processed;
        double success_rate;
    };

    SubscriberStats getStats() const {
        SubscriberStats stats;
        stats.total_received = total_received_.load();
        stats.successful_processed = successful_processed_.load();
        stats.failed_processed = failed_processed_.load();
        stats.success_rate = stats.total_received > 0 ? 
            (double)stats.successful_processed / stats.total_received : 0.0;
        return stats;
    }

private:
    /**
     * ZeroMQ subscriber'ı initialize eder
     */
    void initializeSubscribers() {
        // DelayCalcTrackData subscriber
        delay_calc_subscriber_ = std::make_unique<zmq::socket_t>(context_, ZMQ_SUB);
        delay_calc_subscriber_->connect(delay_calc_endpoint_);
        
        // Topic filter ayarla
        if (!topic_filter_.empty()) {
            delay_calc_subscriber_->setsockopt(ZMQ_SUBSCRIBE, topic_filter_.c_str(), topic_filter_.length());
        } else {
            const char* default_prefix = "DELAY_CALC_DATA"; // B tarafındaki default prefix ile eşleşir
            delay_calc_subscriber_->setsockopt(ZMQ_SUBSCRIBE, default_prefix, (int)strlen(default_prefix));
        }

        // PERFORMANS OPTİMİZASYONU - Düşük gecikme için
        int timeout = 10; // Çok kısa timeout - 10ms
        delay_calc_subscriber_->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
        
        // Receive High Water Mark - düşük buffer
        int rcv_hwm = 10;
        delay_calc_subscriber_->setsockopt(ZMQ_RCVHWM, &rcv_hwm, sizeof(rcv_hwm));
        
        // Linger time - hızlı kapanma
        int linger = 0;
        delay_calc_subscriber_->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    }

    /**
     * DelayCalcTrackData mesajlarını dinler
     */
    void subscribeDelayCalcTrackData() {
        while (running_.load()) {
            try {
                // Multipart: önce topic, sonra data
                zmq::message_t topic_msg;
                auto r1 = delay_calc_subscriber_->recv(topic_msg, zmq::recv_flags::dontwait);
                if (r1) {
                    zmq::message_t data_msg;
                    auto r2 = delay_calc_subscriber_->recv(data_msg, zmq::recv_flags::none);
                    if (!r2) {
                        failed_processed_++;
                        continue;
                    }
                    total_received_++;

                    // Veriyi DelayCalcTrackData'ya deserialize et
                    auto data = deserializeDelayCalcTrackData(data_msg);
                    
                    if (data.has_value()) {
                        // Domain katmanına gönder
                        if (track_data_submission_->submitDelayCalcTrackData(data.value())) {
                            successful_processed_++;
                        } else {
                            failed_processed_++;
                        }
                    } else {
                        failed_processed_++;
                    }
                }
                
                // CPU'yu rahatlatmak için kısa bekleme
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                
            } catch (const std::exception& e) {
                failed_processed_++;
                // Hata loglaması yapılabilir
            }
        }
    }



    /**
     * ZeroMQ mesajını DelayCalcTrackData'ya deserialize eder
     * Bu implementasyon basit binary format kullanır
     * Gerçek uygulamada JSON, Protocol Buffers, vs. kullanılabilir
     */
    std::optional<domain::model::DelayCalcTrackData> deserializeDelayCalcTrackData(const zmq::message_t& message) {
        try {
            const char* begin = static_cast<const char*>(message.data());
            std::string s(begin, begin + message.size());

            std::istringstream iss(s);
            iss.imbue(std::locale::classic());
            std::string token;
            std::vector<std::string> parts;
            while (std::getline(iss, token, '|')) {
                parts.push_back(token);
            }
            if (parts.size() != 16) {
                return std::nullopt;
            }

            auto to_i = [](const std::string& v) { return std::stoi(v); };
            auto to_d = [](const std::string& v) { return std::stod(v); };
            auto to_i64 = [](const std::string& v) { return static_cast<int64_t>(std::stoll(v)); };

            domain::model::DelayCalcTrackData data(
                to_i(parts[0]),        // id
                to_d(parts[1]),        // velocity
                to_d(parts[2]),        // position
                to_i64(parts[3]),      // time_field
                to_i(parts[4]),        // track_id
                to_d(parts[5]),        // x_vel_ecef
                to_d(parts[6]),        // y_vel_ecef
                to_d(parts[7]),        // z_vel_ecef
                to_d(parts[8]),        // x_pos_ecef
                to_d(parts[9]),        // y_pos_ecef
                to_d(parts[10]),       // z_pos_ecef
                to_i64(parts[11]),     // update_time
                to_i64(parts[12]),     // original_update_time
                to_i64(parts[13]),     // first_hop_sent
                to_i64(parts[14]),     // first_hop_delay
                to_i64(parts[15])      // second_hop_sent
            );

            return data;
        } catch (...) {
            return std::nullopt;
        }
    }


};

} // namespace hat::adapters::incoming::zeromq