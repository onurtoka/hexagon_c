#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <map>
#include <set>
#include <mutex>
#include <limits>
#include <optional>

// Domain katmanı
#include "../domain/logic/TrackDataProcessor.hpp"

// Adapter katmanı
#include "../adapters/incoming/zeromq/ZeroMQTrackDataSubscriber.hpp"
#include "../adapters/outgoing/zeromq/ZeroMQTrackDataPublisher.hpp"

using namespace hat;

// Global değişkenler - sinyal yönetimi için
std::atomic<bool> running(true);

// Sinyal handler - Graceful shutdown için
void signalHandler(int signum) {
    std::cout << "\nShutdown signal received (" << signum << "). Stopping gracefully..." << std::endl;
    running.store(false);
}

/**
 * Basit in-memory repository implementasyonu
 * Gerçek uygulamada database, file system vs. kullanılacak
 */
class InMemoryTrackDataRepository : public domain::ports::outgoing::TrackDataRepository {
private:
    std::map<int, domain::model::DelayCalcTrackData> delay_calc_data_;
    std::map<int, domain::model::FinalCalcDelayData> final_calc_data_;
    mutable std::mutex data_mutex_;

public:
    // DelayCalcTrackData operations
    bool saveDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        delay_calc_data_[data.getId()] = data;
        return true;
    }

    std::optional<domain::model::DelayCalcTrackData> findDelayCalcTrackDataById(int id) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        auto it = delay_calc_data_.find(id);
        if (it != delay_calc_data_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<domain::model::DelayCalcTrackData> findDelayCalcTrackDataByTrackId(int track_id) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<domain::model::DelayCalcTrackData> result;
        for (const auto& pair : delay_calc_data_) {
            if (pair.second.getTrackId() == track_id) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    bool updateDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) override {
        return saveDelayCalcTrackData(data);
    }

    // FinalCalcDelayData operations
    bool saveFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        final_calc_data_[data.getTrackId()] = data;
        return true;
    }

    std::optional<domain::model::FinalCalcDelayData> findFinalCalcDelayDataByTrackId(int track_id) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        auto it = final_calc_data_.find(track_id);
        if (it != final_calc_data_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool updateFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) override {
        return saveFinalCalcDelayData(data);
    }

    // Query operations - basit implementasyonlar
    std::vector<domain::model::DelayCalcTrackData> findDelayCalcDataByTimeRange(
        int64_t start_time, int64_t end_time) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<domain::model::DelayCalcTrackData> result;
        for (const auto& pair : delay_calc_data_) {
            auto update_time = pair.second.getUpdateTime();
            if (update_time >= start_time && update_time <= end_time) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<domain::model::FinalCalcDelayData> findFinalCalcDataByTimeRange(
        int64_t start_time, int64_t end_time) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<domain::model::FinalCalcDelayData> result;
        for (const auto& pair : final_calc_data_) {
            auto update_time = pair.second.getUpdateTime();
            if (update_time >= start_time && update_time <= end_time) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<domain::model::FinalCalcDelayData> findHighDelayData(int64_t delay_threshold_ms) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<domain::model::FinalCalcDelayData> result;
        for (const auto& pair : final_calc_data_) {
            if (pair.second.getTotalDelayTime() > delay_threshold_ms) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<int> getActiveTrackIds() override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<int> result;
        for (const auto& pair : final_calc_data_) {
            result.push_back(pair.first);
        }
        return result;
    }

    size_t cleanupOldData(int64_t cutoff_time) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        size_t removed = 0;
        
        // DelayCalcTrackData cleanup
        for (auto it = delay_calc_data_.begin(); it != delay_calc_data_.end();) {
            if (it->second.getUpdateTime() < cutoff_time) {
                it = delay_calc_data_.erase(it);
                removed++;
            } else {
                ++it;
            }
        }
        
        // FinalCalcDelayData cleanup
        for (auto it = final_calc_data_.begin(); it != final_calc_data_.end();) {
            if (it->second.getUpdateTime() < cutoff_time) {
                it = final_calc_data_.erase(it);
                removed++;
            } else {
                ++it;
            }
        }
        
        return removed;
    }

    RepositoryStats getRepositoryStats() override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        RepositoryStats stats;
        stats.total_delay_calc_records = delay_calc_data_.size();
        stats.total_final_calc_records = final_calc_data_.size();
        
        // Active tracks sayısı
        std::set<int> track_ids;
        for (const auto& pair : delay_calc_data_) {
            track_ids.insert(pair.second.getTrackId());
        }
        for (const auto& pair : final_calc_data_) {
            track_ids.insert(pair.second.getTrackId());
        }
        stats.active_tracks = track_ids.size();
        
        // Zaman aralıkları
        stats.oldest_record_time = 0;
        stats.newest_record_time = 0;
        
        if (!delay_calc_data_.empty() || !final_calc_data_.empty()) {
            int64_t min_time = std::numeric_limits<int64_t>::max();
            int64_t max_time = 0;
            
            for (const auto& pair : delay_calc_data_) {
                min_time = std::min(min_time, pair.second.getUpdateTime());
                max_time = std::max(max_time, pair.second.getUpdateTime());
            }
            
            for (const auto& pair : final_calc_data_) {
                min_time = std::min(min_time, pair.second.getUpdateTime());
                max_time = std::max(max_time, pair.second.getUpdateTime());
            }
            
            stats.oldest_record_time = min_time;
            stats.newest_record_time = max_time;
        }
        
        return stats;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== HaT - Hexagonal Architecture Tracking System ===" << std::endl;
    std::cout << "C++ Implementation with ZeroMQ" << std::endl;
    std::cout << "=====================================================" << std::endl;

    // Sinyal handler'larını ayarla
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        // Konfigürasyon - hexagon_b'den veri alacak
        std::string delay_calc_sub_endpoint = "tcp://localhost:7777";  // hexagon_b'den alır
        std::string delay_calc_pub_endpoint = "tcp://*:5557";
        std::string final_calc_pub_endpoint = "tcp://*:5558";

        if (argc > 1) {
            // Basit komut satırı argüman işleme
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--help" || arg == "-h") {
                    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                    std::cout << "Options:" << std::endl;
                    std::cout << "  --help, -h     Show this help message" << std::endl;
                    std::cout << "  --sub-port P   DelayCalc subscriber port (default: 5555)" << std::endl;
                    std::cout << "  --pub-port1 P  DelayCalc publisher port (default: 5557)" << std::endl;
                    std::cout << "  --pub-port2 P  FinalCalc publisher port (default: 5558)" << std::endl;
                    return 0;
                }
            }
        }

        std::cout << "Configuration:" << std::endl;
        std::cout << "  DelayCalc Subscriber: " << delay_calc_sub_endpoint << " (from hexagon_b)" << std::endl;
        std::cout << "  DelayCalc Publisher:  " << delay_calc_pub_endpoint << std::endl;
        std::cout << "  FinalCalc Publisher:  " << final_calc_pub_endpoint << std::endl;
        std::cout << "  Note: Receives data from hexagon_b, generates FinalCalc internally" << std::endl;

        // Hexagonal Architecture bileşenlerini oluştur

        // 1. Repository (Secondary Port)
        auto repository = std::make_shared<InMemoryTrackDataRepository>();

        // 2. Publisher (Secondary Port)
        auto publisher = std::make_shared<adapters::outgoing::zeromq::ZeroMQTrackDataPublisher>(
            delay_calc_pub_endpoint, final_calc_pub_endpoint);

        // 3. Domain Logic (Hexagon'un kalbi)
        auto track_data_processor = std::make_shared<domain::logic::TrackDataProcessor>(
            publisher, repository);

        // 4. Subscriber (Primary Adapter) - sadece DelayCalc verisi alır
        auto subscriber = std::make_unique<adapters::incoming::zeromq::ZeroMQTrackDataSubscriber>(
            track_data_processor, delay_calc_sub_endpoint);

        std::cout << "\nStarting components..." << std::endl;

        // Publisher'ı başlat
        if (!publisher->start()) {
            std::cerr << "Failed to start publisher!" << std::endl;
            return 1;
        }
        std::cout << "✓ Publisher started" << std::endl;

        // Subscriber'ı başlat
        if (!subscriber->start()) {
            std::cerr << "Failed to start subscriber!" << std::endl;
            return 1;
        }
        std::cout << "✓ Subscriber started" << std::endl;

        std::cout << "\n=== System is running ===\n" << std::endl;
        std::cout << "Listening for DelayCalcTrackData on ZeroMQ..." << std::endl;
        std::cout << "Generating FinalCalcDelayData internally and publishing both..." << std::endl;
        std::cout << "Press Ctrl+C to stop gracefully." << std::endl;

        // Ana döngü - istatistikleri göster
        auto last_stats_time = std::chrono::steady_clock::now();
        
        while (running.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Her 10 saniyede bir istatistikleri göster
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - last_stats_time).count() >= 10) {
                
                std::cout << "\n--- Statistics ---" << std::endl;
                
                // Subscriber stats
                auto sub_stats = subscriber->getStats();
                std::cout << "Subscriber: Received=" << sub_stats.total_received 
                         << ", Processed=" << sub_stats.successful_processed
                         << ", Failed=" << sub_stats.failed_processed
                         << ", Success Rate=" << (sub_stats.success_rate * 100) << "%" << std::endl;

                // Publisher stats
                auto pub_stats = publisher->getPublisherStats();
                std::cout << "Publisher: Published=" << pub_stats.total_published
                         << ", Failed=" << pub_stats.failed_publications
                         << ", Avg Latency=" << pub_stats.average_latency_ms << "ms" << std::endl;

                // Repository stats
                auto repo_stats = repository->getRepositoryStats();
                std::cout << "Repository: DelayCalc=" << repo_stats.total_delay_calc_records
                         << ", FinalCalc=" << repo_stats.total_final_calc_records
                         << ", Active Tracks=" << repo_stats.active_tracks << std::endl;

                std::cout << "---" << std::endl;
                
                last_stats_time = now;
            }
        }

        std::cout << "\nShutting down components..." << std::endl;

        // Graceful shutdown
        subscriber->stop();
        std::cout << "✓ Subscriber stopped" << std::endl;

        publisher->stop();
        std::cout << "✓ Publisher stopped" << std::endl;

        // Son istatistikleri göster
        std::cout << "\n=== Final Statistics ===" << std::endl;
        auto final_sub_stats = subscriber->getStats();
        auto final_pub_stats = publisher->getPublisherStats();
        auto final_repo_stats = repository->getRepositoryStats();

        std::cout << "Total Messages Received: " << final_sub_stats.total_received << std::endl;
        std::cout << "Total Messages Published: " << final_pub_stats.total_published << std::endl;
        std::cout << "Final Success Rate: " << (final_sub_stats.success_rate * 100) << "%" << std::endl;
        std::cout << "Total Records in Repository: " 
                 << (final_repo_stats.total_delay_calc_records + final_repo_stats.total_final_calc_records) << std::endl;

        std::cout << "\n=== HaT System Shutdown Complete ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}