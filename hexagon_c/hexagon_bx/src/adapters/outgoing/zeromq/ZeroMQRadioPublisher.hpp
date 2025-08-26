#pragma once

// DRAFT API'leri etkinleştirmek için
#define ZMQ_BUILD_DRAFT_API

#include "../../../domain/ports/outgoing/DataPublisher.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <sstream>
#include <chrono>
#include <queue>
#include <mutex>

namespace hat_b::adapters::outgoing::zeromq {

/**
 * RADIO pattern kullanan ZeroMQ Publisher Adapter
 * UDP multicast üzerinden grup tabanlı mesaj gönderimi sağlar
 * hexagon_c'ye DelayCalcTrackData gönderir
 */
class ZeroMQRadioPublisher : public domain::ports::outgoing::DataPublisher {
private:
    // ZeroMQ C++ context ve socket
    zmq::context_t zmq_context_;
    std::unique_ptr<zmq::socket_t> radio_socket_;
    
    // Thread yönetimi
    std::thread publisher_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string multicast_endpoint_;  // UDP multicast adresi (örn: udp://239.1.1.1:9001)
    std::string group_name_;          // Gönderilecek grup adı (örn: "SOURCE_DATA")
    
    // Mesaj kuyruğu
    std::queue<std::string> message_queue_;
    std::mutex queue_mutex_;
    
    // İstatistikler
    std::atomic<size_t> total_published_;
    std::atomic<size_t> failed_publications_;

public:
    /**
     * Constructor
     * @param multicast_endpoint UDP multicast endpoint (örn: "udp://239.1.1.1:9001")
     * @param group_name Gönderilecek multicast grup adı (örn: "SOURCE_DATA")
     */
    ZeroMQRadioPublisher(
        const std::string& multicast_endpoint = "udp://239.1.1.1:9001",
        const std::string& group_name = "SOURCE_DATA");

    ~ZeroMQRadioPublisher();

    // DataPublisher interface implementation
    bool publishDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) override;
    size_t publishBatchData(const std::vector<hat::domain::model::DelayCalcTrackData>& data_list) override;
    bool isPublisherActive() const override;
    PublisherStats getPublisherStats() const override;

    /**
     * Publisher'ı başlatır
     */
    bool start();

    /**
     * Publisher'ı durdurur
     */
    void stop();

private:
    /**
     * ZeroMQ radio socket'ini initialize eder
     */
    void initializeRadioSocket();

    /**
     * Publisher worker thread - asynchronous message sending
     */
    void publisherWorker();

    /**
     * DelayCalcTrackData'yı JSON string'e serialize eder
     */
    std::string serializeDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data);

    /**
     * Mesajı kuyruğa ekler
     */
    void enqueueMessage(const std::string& message);

    /**
     * Kuyruktan mesaj alır
     */
    bool dequeueMessage(std::string& message);
};

} // namespace hat_b::adapters::outgoing::zeromq
