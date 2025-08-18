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

            public:
                ZeroMQTrackDataSubscriber(
                    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
                    const std::string& delay_calc_endpoint = "tcp://localhost:5555",
                    const std::string& topic_filter = "");
                ~ZeroMQTrackDataSubscriber();
                bool start();
                void stop();
                bool isRunning() const;
                struct SubscriberStats {
                    size_t total_received;
                    size_t successful_processed;
                    size_t failed_processed;
                    double success_rate;
                };
                SubscriberStats getStats() const;

            private:
                void initializeSubscribers();
                void subscribeDelayCalcTrackData();
                std::optional<domain::model::DelayCalcTrackData> deserializeDelayCalcTrackData(const zmq::message_t& message);
            };