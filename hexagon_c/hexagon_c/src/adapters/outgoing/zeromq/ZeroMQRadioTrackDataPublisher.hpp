#pragma once

#include "../../../domain/ports/outgoing/TrackDataPublisher.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include "../../../domain/model/FinalCalcDelayData.hpp"
#include <zmq.h>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <chrono>

// DRAFT API'leri etkinleştirmek için (ZMQ_RADIO için gerekli)
// Draft API gerekmiyor; standart PUB socket kullanılıyor

namespace hat::adapters::outgoing::zeromq {

/**
 * Standart PUB socket kullanan ZeroMQ Publisher Adapter
 * (Önceden RADIO/DISH draft API kullanıyordu; taşınabilirlik için PUB'a geçirildi)
 */
class ZeroMQRadioTrackDataPublisher : public domain::ports::outgoing::TrackDataPublisher {
private:
    // ZeroMQ context ve radio socket
    void* zmq_context_;
    void* radio_socket_;
    
    // Thread yönetimi
    std::thread publisher_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string multicast_endpoint_;  // UDP multicast adresi (örn: udp://239.1.1.1:9002)
    std::string group_name_;          // Grup adı (örn: "PROCESSED_DATA")
    
    // Async publishing için queue
    struct RadioPublishTask {
        enum Type { DELAY_CALC, FINAL_CALC } type;
        std::string serialized_data;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::queue<RadioPublishTask> publish_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    
    // Hata kontrolü için yardımcı fonksiyon
    void check_rc(int rc, const std::string& context_msg);

public:
    /**
     * Constructor
     * @param multicast_endpoint UDP multicast endpoint (örn: "udp://239.1.1.1:9002")
     * @param group_name Multicast grup adı (örn: "PROCESSED_DATA")
     */
    ZeroMQRadioTrackDataPublisher(
        const std::string& multicast_endpoint = "udp://239.1.1.1:9002",
        const std::string& group_name = "PROCESSED_DATA");

    ~ZeroMQRadioTrackDataPublisher();

    /**
     * Radio Publisher'ı başlatır
     */
    bool start();

    /**
     * Radio Publisher'ı durdurur
     */
    void stop();

    // TrackDataPublisher interface implementation

    /**
     * DelayCalcTrackData'yı radio ile yayınlar
     */
    bool publishDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) override;

    /**
     * FinalCalcDelayData'yı radio ile yayınlar
     */
    bool publishFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) override;

    /**
     * Publisher aktif durumu
     */
    bool isPublisherActive() const override;

private:
    /**
     * ZeroMQ radio socket'ini initialize eder
     */
    void initializeRadioSocket();

    /**
     * Publisher worker thread - asynchronous publishing
     */
    void publisherWorker();

    /**
     * Tek bir radio task'ını yayınlar
     */
    void publishRadioTask(const RadioPublishTask& task);

    /**
     * DelayCalcTrackData'yı zaman damgası ile birlikte serialize eder
     */
    std::string serializeDelayCalcTrackDataWithTimestamp(const domain::model::DelayCalcTrackData& data);

    /**
     * FinalCalcDelayData'yı zaman damgası ile birlikte serialize eder
     */
    std::string serializeFinalCalcDelayDataWithTimestamp(const domain::model::FinalCalcDelayData& data);
};

} // namespace hat::adapters::outgoing::zeromq
