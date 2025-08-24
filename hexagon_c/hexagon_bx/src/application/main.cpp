#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <map>
#include <set>
#include <mutex>
#include <random>
#include <limits>
#include <optional>

// Domain katmanı
#include "../domain/logic/DataProcessor.hpp"

// Adapter katmanı
#include "../adapters/outgoing/zeromq/ZeroMQDataPublisher.hpp"

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
 * hexagon_b için veri saklama
 */
class InMemoryDataRepository : public hat_b::domain::ports::outgoing::DataRepository {
private:
    std::map<int, hat::domain::model::DelayCalcTrackData> data_storage_;
    mutable std::mutex data_mutex_;

public:
    bool saveDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        data_storage_[data.getTrackId()] = data;
        return true;
    }

    std::optional<hat::domain::model::DelayCalcTrackData> findById(int id) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        auto it = data_storage_.find(id);
        if (it != data_storage_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<hat::domain::model::DelayCalcTrackData> findByTrackId(int track_id) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<hat::domain::model::DelayCalcTrackData> result;
        for (const auto& pair : data_storage_) {
            if (pair.second.getTrackId() == track_id) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    bool updateDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) override {
        return saveDelayCalcTrackData(data);
    }

    std::vector<hat::domain::model::DelayCalcTrackData> findByTimeRange(
        int64_t start_time, int64_t end_time) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<hat::domain::model::DelayCalcTrackData> result;
        for (const auto& pair : data_storage_) {
            auto update_time = pair.second.getUpdateTime();
            if (update_time >= start_time && update_time <= end_time) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<int> getActiveTrackIds() override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        std::vector<int> result;
        std::set<int> track_ids;
        for (const auto& pair : data_storage_) {
            track_ids.insert(pair.second.getTrackId());
        }
        for (int id : track_ids) {
            result.push_back(id);
        }
        return result;
    }

    size_t cleanupOldData(int64_t cutoff_time) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        size_t removed = 0;
        for (auto it = data_storage_.begin(); it != data_storage_.end();) {
            if (it->second.getUpdateTime() < cutoff_time) {
                it = data_storage_.erase(it);
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
        stats.total_records = data_storage_.size();
        
        std::set<int> track_ids;
        int64_t min_time = std::numeric_limits<int64_t>::max();
        int64_t max_time = 0;
        
        for (const auto& pair : data_storage_) {
            track_ids.insert(pair.second.getTrackId());
            min_time = std::min(min_time, pair.second.getUpdateTime());
            max_time = std::max(max_time, pair.second.getUpdateTime());
        }
        
        stats.active_tracks = track_ids.size();
        stats.oldest_record_time = min_time == std::numeric_limits<int64_t>::max() ? 0 : min_time;
        stats.newest_record_time = max_time;
        
        return stats;
    }
};

/**
 * Test veri üretici - gerçek uygulamada sensörlerden gelecek
 */
class TestDataGenerator {
private:
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> vel_dist_;
    std::uniform_real_distribution<> pos_dist_;
    std::uniform_int_distribution<> track_dist_;

public:
    TestDataGenerator() 
        : gen_(rd_())
        , vel_dist_(-50.0, 50.0)    // -50 to 50 m/s velocity
        , pos_dist_(-1000.0, 1000.0) // -1000 to 1000 m position
        , track_dist_(100, 199) {}   // Track IDs 100-199

    hat_b::domain::logic::DataProcessor::TrackInfo generateRandomTrack() {
        hat_b::domain::logic::DataProcessor::TrackInfo info;
        info.track_id = track_dist_(gen_);
        info.x_vel = vel_dist_(gen_);
        info.y_vel = vel_dist_(gen_);
        info.z_vel = vel_dist_(gen_);
        info.x_pos = pos_dist_(gen_);
        info.y_pos = pos_dist_(gen_);
        info.z_pos = pos_dist_(gen_);
        return info;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== HaT-B - Data Generator & Sender ===" << std::endl;
    std::cout << "C++ Implementation with ZeroMQ" << std::endl;
    std::cout << "Sends DelayCalcTrackData to cpp_hat" << std::endl;
    std::cout << "=====================================" << std::endl;

    // Sinyal handler'larını ayarla
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        // Konfigürasyon
        std::string output_endpoint = "tcp://*:7777";  // cpp_hat port 7777'den dinliyor
        int generation_interval_ms = 1000;  // Her 1 saniyede veri üret
        
        if (argc > 1) {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--help" || arg == "-h") {
                    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                    std::cout << "Options:" << std::endl;
                    std::cout << "  --help, -h      Show this help message" << std::endl;
                    std::cout << "  --port P        Output port (default: 7777)" << std::endl;
                    std::cout << "  --interval MS   Data generation interval in ms (default: 1000)" << std::endl;
                    return 0;
                }
            }
        }

        std::cout << "Configuration:" << std::endl;
        std::cout << "  Output Endpoint: " << output_endpoint << std::endl;
        std::cout << "  Generation Interval: " << generation_interval_ms << " ms" << std::endl;
    std::cout << "  Target: cpp_hat system" << std::endl;

        // Hexagonal Architecture bileşenlerini oluştur

        // 1. Repository (Secondary Port)
        auto repository = std::make_shared<InMemoryDataRepository>();

        // 2. Publisher (Secondary Port) - cpp_hat'e gönderir
        auto publisher = std::make_shared<hat_b::adapters::outgoing::zeromq::ZeroMQDataPublisher>(
            output_endpoint);

        // 3. Domain Logic (Hexagon'un kalbi)
        auto data_processor = std::make_shared<hat_b::domain::logic::DataProcessor>(
            publisher, repository);

        // 4. Test Data Generator
        TestDataGenerator generator;

        std::cout << "\nStarting components..." << std::endl;

        // Publisher'ı başlat
        if (!publisher->start()) {
            std::cerr << "Failed to start publisher!" << std::endl;
            return 1;
        }
        std::cout << "✓ Publisher started" << std::endl;

        std::cout << "\n=== Data Generation Started ===\n" << std::endl;
        std::cout << "Generating DelayCalcTrackData every " << generation_interval_ms << "ms" << std::endl;
        std::cout << "Sending to cpp_hat on " << output_endpoint << std::endl;
        std::cout << "Press Ctrl+C to stop gracefully." << std::endl;

        // Ana döngü - veri üretimi ve gönderimi
        auto last_stats_time = std::chrono::steady_clock::now();
        size_t total_generated = 0;
        
        while (running.load()) {
            // Test verisi üret ve gönder
            auto track_info = generator.generateRandomTrack();
            
            if (data_processor->generateAndSendDelayCalcData(
                track_info.track_id,
                track_info.x_vel, track_info.y_vel, track_info.z_vel,
                track_info.x_pos, track_info.y_pos, track_info.z_pos)) {
                total_generated++;
                
                // GERÇEK GECİKME ÖLÇÜMÜ İÇİN - Gönderim zamanını göster
                auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                
                std::cout << "📤 Generated Track " << track_info.track_id 
                         << " - Speed: " << std::sqrt(track_info.x_vel*track_info.x_vel + 
                                                     track_info.y_vel*track_info.y_vel + 
                                                     track_info.z_vel*track_info.z_vel) 
                         << " m/s - 🕒 Sent Time: " << current_time << " ms" << std::endl;
            }

            // Her 10 saniyede bir istatistikleri göster
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - last_stats_time).count() >= 10) {
                
                std::cout << "\n--- Statistics ---" << std::endl;
                
                // Publisher stats
                auto pub_stats = publisher->getPublisherStats();
                std::cout << "Publisher: Published=" << pub_stats.total_published
                         << ", Failed=" << pub_stats.failed_publications
                         << ", Avg Latency=" << pub_stats.average_latency_ms << "ms" << std::endl;

                // Repository stats
                auto repo_stats = repository->getRepositoryStats();
                std::cout << "Repository: Records=" << repo_stats.total_records
                         << ", Active Tracks=" << repo_stats.active_tracks << std::endl;

                std::cout << "Total Generated: " << total_generated << std::endl;
                std::cout << "---" << std::endl;
                
                last_stats_time = now;
            }

            // Belirtilen interval kadar bekle
            std::this_thread::sleep_for(std::chrono::milliseconds(generation_interval_ms));
        }

        std::cout << "\nShutting down components..." << std::endl;

        // Graceful shutdown
        publisher->stop();
        std::cout << "✓ Publisher stopped" << std::endl;

        // Son istatistikleri göster
        std::cout << "\n=== Final Statistics ===" << std::endl;
        auto final_pub_stats = publisher->getPublisherStats();
        auto final_repo_stats = repository->getRepositoryStats();

        std::cout << "Total Generated: " << total_generated << std::endl;
        std::cout << "Total Published: " << final_pub_stats.total_published << std::endl;
        std::cout << "Total Stored: " << final_repo_stats.total_records << std::endl;
        std::cout << "Success Rate: " << (total_generated > 0 ? 
            (double)final_pub_stats.total_published / total_generated * 100 : 0) << "%" << std::endl;

        std::cout << "\n=== HaT-B Data Generator Shutdown Complete ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}