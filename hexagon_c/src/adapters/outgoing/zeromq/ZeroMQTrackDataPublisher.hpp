#pragma once

#include "../../../domain/ports/outgoing/TrackDataPublisher.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include "../../../domain/model/FinalCalcDelayData.hpp"
#include "../../../../include/zmq/zmq.hpp"
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
    std::atomic<size_t> total_published_;
    std::atomic<size_t> failed_publications_;
    std::atomic<size_t> active_connections_;
    std::atomic<double> total_latency_ms_;

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
        const std::string& topic_prefix = "TRACK_DATA")
        : context_(1)
        , running_(false)
        , delay_calc_endpoint_(delay_calc_endpoint)
        , final_calc_endpoint_(final_calc_endpoint)
        , topic_prefix_(topic_prefix)
        , total_published_(0)
        , failed_publications_(0)
        , active_connections_(0)
        , total_latency_ms_(0.0) {
        
        initializePublishers();
    }

    ~ZeroMQTrackDataPublisher() {
        stop();
    }

    /**
     * Publisher'ı başlatır
     */
    bool start() {
        if (running_.load()) {
            return false; // Zaten çalışıyor
        }

        running_.store(true);

        // Publisher thread'ini başlat
        publisher_thread_ = std::thread([this]() {
            publisherWorker();
        });

        return true;
    }

    /**
     * Publisher'ı durdurur
     */
    void stop() {
        running_.store(false);
        queue_cv_.notify_all();

        if (publisher_thread_.joinable()) {
            publisher_thread_.join();
        }
    }

    // TrackDataPublisher interface implementation

    /**
     * DelayCalcTrackData yayınlar
     */
    bool publishDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) override {
        try {
            auto serialized = serializeDelayCalcTrackData(data);
            if (serialized.empty()) {
                failed_publications_++;
                return false;
            }

            PublishTask task;
            task.type = PublishTask::DELAY_CALC;
            task.serialized_data = std::move(serialized);
            task.track_id = data.getTrackId();
            task.timestamp = std::chrono::steady_clock::now();

            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                publish_queue_.push(std::move(task));
            }
            queue_cv_.notify_one();

            return true;

        } catch (const std::exception& e) {
            failed_publications_++;
            return false;
        }
    }

    /**
     * FinalCalcDelayData yayınlar
     */
    bool publishFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) override {
        try {
            auto serialized = serializeFinalCalcDelayData(data);
            if (serialized.empty()) {
                failed_publications_++;
                return false;
            }

            PublishTask task;
            task.type = PublishTask::FINAL_CALC;
            task.serialized_data = std::move(serialized);
            task.track_id = data.getTrackId();
            task.timestamp = std::chrono::steady_clock::now();

            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                publish_queue_.push(std::move(task));
            }
            queue_cv_.notify_one();

            return true;

        } catch (const std::exception& e) {
            failed_publications_++;
            return false;
        }
    }

    /**
     * Batch veri yayınlama
     */
    size_t publishBatchData(
        const std::vector<domain::model::DelayCalcTrackData>& delay_calc_data,
        const std::vector<domain::model::FinalCalcDelayData>& final_calc_data) override {
        
        size_t published = 0;

        // DelayCalcTrackData batch
        for (const auto& data : delay_calc_data) {
            if (publishDelayCalcTrackData(data)) {
                published++;
            }
        }

        // FinalCalcDelayData batch
        for (const auto& data : final_calc_data) {
            if (publishFinalCalcDelayData(data)) {
                published++;
            }
        }

        return published;
    }

    /**
     * Belirli track ID için yayınlama
     */
    bool publishForTrackId(
        int track_id,
        const domain::model::DelayCalcTrackData* delay_calc_data,
        const domain::model::FinalCalcDelayData* final_calc_data) override {
        
        bool success = true;

        if (delay_calc_data != nullptr) {
            success &= publishDelayCalcTrackData(*delay_calc_data);
        }

        if (final_calc_data != nullptr) {
            success &= publishFinalCalcDelayData(*final_calc_data);
        }

        return success;
    }

    /**
     * Publisher aktif durumu
     */
    bool isPublisherActive() const override {
        return running_.load();
    }

    /**
     * Publisher istatistikleri
     */
    PublisherStats getPublisherStats() const override {
        PublisherStats stats;
        stats.total_published = total_published_.load();
        stats.failed_publications = failed_publications_.load();
        stats.active_connections = active_connections_.load();
        
        auto total = total_published_.load();
        stats.average_latency_ms = total > 0 ? total_latency_ms_.load() / total : 0.0;
        
        return stats;
    }

private:
    /**
     * ZeroMQ publisher'ları initialize eder
     */
    void initializePublishers() {
        // DelayCalcTrackData publisher
        delay_calc_publisher_ = std::make_unique<zmq::socket_t>(context_, ZMQ_PUB);
        delay_calc_publisher_->bind(delay_calc_endpoint_);

        // FinalCalcDelayData publisher
        final_calc_publisher_ = std::make_unique<zmq::socket_t>(context_, ZMQ_PUB);
        final_calc_publisher_->bind(final_calc_endpoint_);

        // High Water Mark ayarla (memory kontrolü için)
        int hwm = 10000;
        delay_calc_publisher_->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
        final_calc_publisher_->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
    }

    /**
     * Publisher worker thread
     */
    void publisherWorker() {
        while (running_.load()) {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            // Queue'da task bekle
            queue_cv_.wait(lock, [this]() {
                return !publish_queue_.empty() || !running_.load();
            });

            if (!running_.load()) {
                break;
            }

            // Task'ları işle
            while (!publish_queue_.empty()) {
                auto task = std::move(publish_queue_.front());
                publish_queue_.pop();
                lock.unlock();

                // Task'ı yayınla
                publishTask(task);

                lock.lock();
            }
        }
    }

    /**
     * Tek bir task'ı yayınlar
     */
    void publishTask(const PublishTask& task) {
        try {
            auto start_time = std::chrono::steady_clock::now();

            // Topic oluştur
            std::string topic = topic_prefix_ + "." + std::to_string(task.track_id);

            zmq::socket_t* socket = nullptr;
            if (task.type == PublishTask::DELAY_CALC) {
                socket = delay_calc_publisher_.get();
            } else {
                socket = final_calc_publisher_.get();
            }

            // Topic mesajını gönder
            zmq::message_t topic_msg(topic.size());
            memcpy(topic_msg.data(), topic.c_str(), topic.size());
            socket->send(topic_msg, zmq::send_flags::sndmore);

            // Data mesajını gönder
            zmq::message_t data_msg(task.serialized_data.size());
            memcpy(data_msg.data(), task.serialized_data.data(), task.serialized_data.size());
            socket->send(data_msg, zmq::send_flags::dontwait);

            // İstatistikleri güncelle
            total_published_++;
            
            auto end_time = std::chrono::steady_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
            total_latency_ms_ = total_latency_ms_.load() + latency;

        } catch (const std::exception& e) {
            failed_publications_++;
        }
    }

    /**
     * DelayCalcTrackData'yı serialize eder
     * Bu implementasyon basit binary format kullanır
     * Gerçek uygulamada JSON, Protocol Buffers, vs. kullanılabilir
     */
    std::vector<uint8_t> serializeDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) {
        try {
            std::ostringstream oss;
            oss.imbue(std::locale::classic());
            oss 
                << data.getId() << '|'
                << data.getVelocity() << '|'
                << data.getPosition() << '|'
                << data.getTimeField() << '|'
                << data.getTrackId() << '|'
                << data.getXVelocityECEF() << '|'
                << data.getYVelocityECEF() << '|'
                << data.getZVelocityECEF() << '|'
                << data.getXPositionECEF() << '|'
                << data.getYPositionECEF() << '|'
                << data.getZPositionECEF() << '|'
                << data.getUpdateTime() << '|'
                << data.getOriginalUpdateTime() << '|'
                << data.getFirstHopSentTime() << '|'
                << data.getFirstHopDelayTime() << '|'
                << data.getSecondHopSentTime();
            const std::string s = oss.str();
            return std::vector<uint8_t>(s.begin(), s.end());
        } catch (...) {
            return {};
        }
    }

    /**
     * FinalCalcDelayData'yı serialize eder
     */
    std::vector<uint8_t> serializeFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) {
        try {
            std::ostringstream oss;
            oss.imbue(std::locale::classic());
            oss 
                << data.getTrackId() << '|'
                << data.getXVelocityECEF() << '|'
                << data.getYVelocityECEF() << '|'
                << data.getZVelocityECEF() << '|'
                << data.getXPositionECEF() << '|'
                << data.getYPositionECEF() << '|'
                << data.getZPositionECEF() << '|'
                << data.getUpdateTime() << '|'
                << data.getOriginalUpdateTime() << '|'
                << data.getFirstHopSentTime() << '|'
                << data.getFirstHopDelayTime() << '|'
                << data.getSecondHopSentTime() << '|'
                << data.getSecondHopDelayTime() << '|'
                << data.getTotalDelayTime() << '|'
                << data.getThirdHopSentTime();
            const std::string s = oss.str();
            return std::vector<uint8_t>(s.begin(), s.end());
        } catch (...) {
            return {};
        }
    }
};

} // namespace hat::adapters::outgoing::zeromq