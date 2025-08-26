#pragma once

// DRAFT API'leri etkinleştirmek için (ZMQ_DISH için gerekli)
#define ZMQ_BUILD_DRAFT_API

#include "../../../domain/ports/incoming/TrackDataSubmission.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <sstream>
#include <optional>
#include <cstring>
#include <chrono>

namespace hat::adapters::incoming::zeromq {

/**
 * Dish pattern kullanan ZeroMQ Subscriber Adapter
 * UDP multicast üzerinden grup tabanlı mesaj alma sağlar
 * app2_processor.cpp'deki dish pattern'ini hexagonal architecture'a entegre eder
 */
class ZeroMQDishTrackDataSubscriber {
private:
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission_;
    
    // ZeroMQ C++ context ve socket
    zmq::context_t zmq_context_;
    std::unique_ptr<zmq::socket_t> dish_socket_;
    
    // Thread yönetimi
    std::thread subscriber_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string multicast_endpoint_;  // UDP multicast adresi (örn: udp://239.1.1.1:9001)
    std::string group_name_;          // Dinlenecek grup adı (örn: "SOURCE_DATA")
    
    // Gecikme hesaplama için
    struct LatencyMeasurement {
        std::chrono::steady_clock::time_point receive_time;
        long long send_timestamp_ns;
        long long latency_ns;
        std::string original_data;
    };
    
    // Hata kontrolü için yardımcı fonksiyon (C++ wrapper ile artık gereksiz)
    // void check_rc(int rc, const std::string& context_msg);

public:
    /**
     * Constructor
     * @param track_data_submission Domain katmanına veri göndermek için port
     * @param multicast_endpoint UDP multicast endpoint (örn: "udp://239.1.1.1:9001")
     * @param group_name Dinlenecek multicast grup adı (örn: "SOURCE_DATA")
     */
    ZeroMQDishTrackDataSubscriber(
        std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
        const std::string& multicast_endpoint = "udp://239.1.1.1:9001",
        const std::string& group_name = "SOURCE_DATA");

    ~ZeroMQDishTrackDataSubscriber();

    /**
     * Dish Subscriber'ı başlatır
     */
    bool start();

    /**
     * Dish Subscriber'ı durdurur
     */
    void stop();

    /**
     * Subscriber aktif durumu
     */
    bool isRunning() const;

private:
    /**
     * ZeroMQ dish socket'ini initialize eder
     */
    void initializeDishSocket();

    /**
     * Subscriber worker thread - asynchronous message receiving
     */
    void subscriberWorker();

    /**
     * Alınan mesajı işler ve gecikme hesaplar (app2_processor.cpp pattern'i)
     */
    LatencyMeasurement processReceivedMessage(const std::string& received_payload);

    /**
     * Alınan veriyi DelayCalcTrackData'ya deserialize eder
     */
    std::optional<domain::model::DelayCalcTrackData> deserializeDelayCalcTrackData(
        const std::string& original_data, 
        const LatencyMeasurement& latency_info);
};

} // namespace hat::adapters::incoming::zeromq
