#pragma once

#include "../../../domain/ports/outgoing/TrackDataPublisher.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include "../../../domain/model/FinalCalcDelayData.hpp"
#include <zmq/zmq.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <locale>

namespace hat::adapters::outgoing::zeromq {

/**
 * Secondary adapter: İşlenmiş verileri ZeroMQ üzerinden yayınlayan adapter
 * Domain katmanından gelen verileri dış dünyaya ileten çıkış noktası
 */
class ZeroMQTrackDataPublisher : public domain::ports::outgoing::TrackDataPublisher {
private:
    // ZeroMQ context ve socket'ler
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> delay_calc_publisher_;
    std::unique_ptr<zmq::socket_t> final_calc_publisher_;
    
    // Thread yönetimi
    std::thread publisher_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string delay_calc_endpoint_;
    std::string final_calc_endpoint_;
    std::string topic_prefix_;
    
    // Async publishing için queue
    struct PublishTask {
        enum Type { DELAY_CALC, FINAL_CALC } type;
        std::vector<uint8_t> serialized_data;
        int track_id;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::queue<PublishTask> publish_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    
    // İstatistikler
    // İstatistikler kaldırıldı; gerekirse eklenecek

public:
    /**
     * Constructor
     * @param delay_calc_endpoint DelayCalcTrackData için ZeroMQ endpoint
     * @param final_calc_endpoint FinalCalcDelayData için ZeroMQ endpoint
     * @param topic_prefix Topic prefix (opsiyonel)
     */
    ZeroMQTrackDataPublisher(
        const std::string& delay_calc_endpoint = "tcp://*:5557",
        const std::string& final_calc_endpoint = "tcp://*:5558",
        const std::string& topic_prefix = "TRACK_DATA");

    ~ZeroMQTrackDataPublisher();

    /**
     * Publisher'ı başlatır
     */
    bool start();

    /**
     * Publisher'ı durdurur
     */
    void stop();

    // TrackDataPublisher interface implementation

    /**
     * DelayCalcTrackData yayınlar
     */
    bool publishDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) override;

    /**
     * FinalCalcDelayData yayınlar
     */
    bool publishFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) override;

    /**
     * Publisher aktif durumu
     */
    bool isPublisherActive() const override;

    // getPublisherStats removed - unnecessary complexity

private:
    void initializePublishers();

    /**
     * Publisher worker thread
     */
    void publisherWorker();

    /**
     * Tek bir task'ı yayınlar
     */
    void publishTask(const PublishTask& task);

    /**
     * DelayCalcTrackData'yı serialize eder
     */
    std::vector<uint8_t> serializeDelayCalcTrackData(const domain::model::DelayCalcTrackData& data);

    /**
     * FinalCalcDelayData'yı serialize eder
     */
    std::vector<uint8_t> serializeFinalCalcDelayData(const domain::model::FinalCalcDelayData& data);
};

} // namespace hat::adapters::outgoing::zeromq