#pragma once

#include "../../../domain/ports/outgoing/DataPublisher.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
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

namespace hat_b::adapters::outgoing::zeromq {

/**
 * Secondary adapter: hexagon_b'den cpp_hat'e DelayCalcTrackData gönderen ZeroMQ adapter
 * Domain katmanından gelen verileri cpp_hat'e ileten çıkış noktası
 */
class ZeroMQDataPublisher : public domain::ports::outgoing::DataPublisher {
private:
    // ZeroMQ context ve socket
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> publisher_;
    
    // Thread yönetimi
    std::thread publisher_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string endpoint_;
    std::string topic_prefix_;
    
    // Async publishing için queue
    struct PublishTask {
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
     * @param endpoint cpp_hat'e gönderim için ZeroMQ endpoint
     * @param topic_prefix Topic prefix (opsiyonel)
     */
    ZeroMQDataPublisher(
         const std::string& endpoint = "tcp://*:7777",  // cpp_hat port 7777'den dinliyor
        const std::string& topic_prefix = "DELAY_CALC_DATA")
        : context_(1)
        , running_(false)
        , endpoint_(endpoint)
        , topic_prefix_(topic_prefix)
        , total_published_(0)
        , failed_publications_(0)
        , active_connections_(0)
        , total_latency_ms_(0.0) {
        
        initializePublisher();
    }

    ~ZeroMQDataPublisher() {
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

    // DataPublisher interface implementation

     /**
      * DelayCalcTrackData'yı cpp_hat'e gönderir
      */
    bool publishDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) override {
        try {
            auto serialized = serializeDelayCalcTrackData(data);
            if (serialized.empty()) {
                failed_publications_++;
                return false;
            }

            PublishTask task;
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
     * Batch veri gönderimi
     */
    size_t publishBatchData(
        const std::vector<hat::domain::model::DelayCalcTrackData>& data_list) override {
        
        size_t published = 0;

        for (const auto& data : data_list) {
            if (publishDelayCalcTrackData(data)) {
                published++;
            }
        }

        return published;
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
     * ZeroMQ publisher'ı initialize eder
     */
    void initializePublisher() {
        // DelayCalcTrackData publisher - cpp_hat'e gönderir
        publisher_ = std::make_unique<zmq::socket_t>(context_, ZMQ_PUB);
        publisher_->bind(endpoint_);

        // PERFORMANS OPTİMİZASYONU - Düşük gecikme için
        int hwm = 10;  // Düşük buffer - hızlı gönderim
        publisher_->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
        
        // Linger time - hızlı kapanma
        int linger = 0;
        publisher_->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
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

            // Topic mesajını gönder
            zmq::message_t topic_msg(topic.size());
            memcpy(topic_msg.data(), topic.c_str(), topic.size());
            publisher_->send(topic_msg, zmq::send_flags::sndmore);

            // Data mesajını gönder
            zmq::message_t data_msg(task.serialized_data.size());
            memcpy(data_msg.data(), task.serialized_data.data(), task.serialized_data.size());
            publisher_->send(data_msg, zmq::send_flags::dontwait);

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
    std::vector<uint8_t> serializeDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) {
        try {
            // Basit ve tutarlı serileştirme: '|' ile ayrılmış alanlar (UTF-8)
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
                << data.getSecondHopSentTime();

            const std::string s = oss.str();
            return std::vector<uint8_t>(s.begin(), s.end());
        } catch (const std::exception& e) {
            return {};
        }
    }
};

} // namespace hat_b::adapters::outgoing::zeromq