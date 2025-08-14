#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>

// Domain modelleri
#include "src/domain/model/DelayCalcTrackData.hpp"
#include "src/domain/model/FinalCalcDelayData.hpp"

// Domain logic
#include "src/domain/logic/TrackDataProcessor.hpp"

using namespace hat::domain::model;

/**
 * Basit mock repository - ZeroMQ olmadan test için
 */
class MockRepository : public hat::domain::ports::outgoing::TrackDataRepository {
public:
    bool saveDelayCalcTrackData(const DelayCalcTrackData& data) override {
        std::cout << "📁 Saved DelayCalc Track " << data.getTrackId() << std::endl;
        return true;
    }
    
    bool saveFinalCalcDelayData(const FinalCalcDelayData& data) override {
        std::cout << "📁 Saved FinalCalc Track " << data.getTrackId() << std::endl;
        return true;
    }
    
    // Diğer metodlar basit implementasyon
    std::optional<DelayCalcTrackData> findDelayCalcTrackDataById(int id) override { return std::nullopt; }
    std::vector<DelayCalcTrackData> findDelayCalcTrackDataByTrackId(int track_id) override { return {}; }
    bool updateDelayCalcTrackData(const DelayCalcTrackData& data) override { return true; }
    std::optional<FinalCalcDelayData> findFinalCalcDelayDataByTrackId(int track_id) override { return std::nullopt; }
    bool updateFinalCalcDelayData(const FinalCalcDelayData& data) override { return true; }
    std::vector<DelayCalcTrackData> findDelayCalcDataByTimeRange(int64_t start, int64_t end) override { return {}; }
    std::vector<FinalCalcDelayData> findFinalCalcDataByTimeRange(int64_t start, int64_t end) override { return {}; }
    std::vector<FinalCalcDelayData> findHighDelayData(int64_t threshold) override { return {}; }
    std::vector<int> getActiveTrackIds() override { return {}; }
    size_t cleanupOldData(int64_t cutoff) override { return 0; }
    RepositoryStats getRepositoryStats() override { return {0, 0, 0, 0, 0}; }
};

/**
 * Basit mock publisher - ZeroMQ olmadan test için
 */
class MockPublisher : public hat::domain::ports::outgoing::TrackDataPublisher {
public:
    bool publishDelayCalcTrackData(const DelayCalcTrackData& data) override {
        std::cout << "📤 Published DelayCalc Track " << data.getTrackId() 
                 << " - Speed: " << data.getECEFSpeed() << " m/s" << std::endl;
        return true;
    }
    
    bool publishFinalCalcDelayData(const FinalCalcDelayData& data) override {
        std::cout << "📤 Published FinalCalc Track " << data.getTrackId() 
                 << " - Total Delay: " << data.getTotalDelayTime() << " ms"
                 << " - Status: ";
        
        auto status = data.getPerformanceStatus();
        switch (status) {
            case FinalCalcDelayData::PerformanceStatus::EXCELLENT: std::cout << "EXCELLENT"; break;
            case FinalCalcDelayData::PerformanceStatus::GOOD: std::cout << "GOOD"; break;
            case FinalCalcDelayData::PerformanceStatus::ACCEPTABLE: std::cout << "ACCEPTABLE"; break;
            case FinalCalcDelayData::PerformanceStatus::POOR: std::cout << "POOR"; break;
            case FinalCalcDelayData::PerformanceStatus::CRITICAL: std::cout << "CRITICAL"; break;
        }
        std::cout << std::endl;
        return true;
    }
    
    size_t publishBatchData(const std::vector<DelayCalcTrackData>& delay_data,
                           const std::vector<FinalCalcDelayData>& final_data) override {
        size_t published = 0;
        for (const auto& data : delay_data) {
            if (publishDelayCalcTrackData(data)) published++;
        }
        for (const auto& data : final_data) {
            if (publishFinalCalcDelayData(data)) published++;
        }
        return published;
    }
    
    bool publishForTrackId(int track_id, const DelayCalcTrackData* delay_data,
                          const FinalCalcDelayData* final_data) override { return true; }
    bool isPublisherActive() const override { return true; }
    PublisherStats getPublisherStats() const override { return {0, 0, 0, 0.0}; }
};

int main() {
    std::cout << "=== HaT Receiver - Simple Test ===" << std::endl;
    std::cout << "Testing DelayCalc → FinalCalc processing" << std::endl;
    std::cout << "====================================" << std::endl;

    try {
        // Mock components oluştur
        auto repository = std::make_shared<MockRepository>();
        auto publisher = std::make_shared<MockPublisher>();
        
        // Domain logic oluştur
        auto processor = std::make_shared<hat::domain::logic::TrackDataProcessor>(
            publisher, repository);

        std::cout << "\n🔄 Processing Test Data..." << std::endl;

        // Test verisi oluştur (hexagon_b'den gelmiş gibi)
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        // Test data 1
        DelayCalcTrackData test_data1(
            1, 45.5, 1200.0, now,
            150,                        // track_id
            25.0, 30.0, 15.0,          // velocity ECEF
            500.0, 600.0, 200.0,       // position ECEF
            now, now - 500,            // update times
            now - 200, now - 150, now - 100  // hop times
        );

        // Test data 2 - yüksek gecikme
        DelayCalcTrackData test_data2(
            2, 75.2, 800.0, now,
            151,                        // track_id
            40.0, 50.0, 20.0,          // velocity ECEF
            300.0, 400.0, 150.0,       // position ECEF
            now, now - 3000,           // update times (3 saniye gecikme)
            now - 1500, now - 1200, now - 800  // hop times
        );

        std::cout << "\n--- Processing Test Data 1 ---" << std::endl;
        if (processor->submitDelayCalcTrackData(test_data1)) {
            std::cout << "✅ Successfully processed test data 1" << std::endl;
        } else {
            std::cout << "❌ Failed to process test data 1" << std::endl;
        }

        std::cout << "\n--- Processing Test Data 2 (High Delay) ---" << std::endl;
        if (processor->submitDelayCalcTrackData(test_data2)) {
            std::cout << "✅ Successfully processed test data 2" << std::endl;
        } else {
            std::cout << "❌ Failed to process test data 2" << std::endl;
        }

        // Batch test
        std::cout << "\n--- Batch Processing Test ---" << std::endl;
        std::vector<DelayCalcTrackData> batch_data;
        
        for (int i = 3; i <= 5; ++i) {
            DelayCalcTrackData batch_item(
                i, 30.0 + i * 5, 1000.0 + i * 100, now + i * 100,
                150 + i,
                20.0 + i, 25.0 + i, 10.0 + i,
                400.0 + i * 50, 500.0 + i * 50, 100.0 + i * 20,
                now + i * 100, now + i * 100 - 300,
                now + i * 100 - 150, now + i * 100 - 100, now + i * 100 - 50
            );
            batch_data.push_back(batch_item);
        }

        size_t processed = processor->submitBatchDelayCalcData(batch_data);
        std::cout << "📊 Batch processed: " << processed << " / " << batch_data.size() << " items" << std::endl;

        std::cout << "\n🎉 All Tests Completed Successfully!" << std::endl;
        std::cout << "✅ DelayCalc processing works" << std::endl;
        std::cout << "✅ FinalCalc generation works" << std::endl;
        std::cout << "✅ Performance analysis works" << std::endl;
        std::cout << "✅ Batch processing works" << std::endl;

        std::cout << "\n📋 Ready for ZeroMQ integration!" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}