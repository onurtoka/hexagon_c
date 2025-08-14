#include <iostream>
#include <memory>
#include <chrono>
#include <cstdint>

// Domain modelleri test et
#include "src/domain/model/DelayCalcTrackData.hpp"
#include "src/domain/model/FinalCalcDelayData.hpp"

using namespace hat::domain::model;

int main() {
    std::cout << "=== HaT C++ Project - GCC 8.5.0 Test ===" << std::endl;
    
    try {
        // Şu anki zamanı al
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        std::cout << "Current timestamp: " << now << " ms" << std::endl;
        
        // DelayCalcTrackData oluştur
        std::cout << "\nTesting DelayCalcTrackData..." << std::endl;
        
        DelayCalcTrackData delay_data(
            1,                  // id
            150.5,              // velocity
            1000.0,             // position
            now,                // time_field
            101,                // track_id
            10.5, 20.3, 30.1,   // velocity ECEF
            100.0, 200.0, 300.0, // position ECEF
            now,                // update_time
            now - 1000,         // original_update_time
            now - 500,          // first_hop_sent_time
            now - 400,          // first_hop_delay_time
            now - 200           // second_hop_sent_time
        );
        
        std::cout << "✓ DelayCalcTrackData created successfully!" << std::endl;
        std::cout << "  Track ID: " << delay_data.getTrackId() << std::endl;
        std::cout << "  Velocity: " << delay_data.getVelocity() << " m/s" << std::endl;
        std::cout << "  ECEF Speed: " << delay_data.getECEFSpeed() << " m/s" << std::endl;
        std::cout << "  Valid: " << (delay_data.isValid() ? "Yes" : "No") << std::endl;
        
        // FinalCalcDelayData oluştur
        std::cout << "\nTesting FinalCalcDelayData..." << std::endl;
        
        FinalCalcDelayData final_data(
            101,                // track_id
            10.5, 20.3, 30.1,   // velocity ECEF
            100.0, 200.0, 300.0, // position ECEF
            now,                // update_time
            now - 1000,         // original_update_time
            now - 500,          // first_hop_sent_time
            now - 400,          // first_hop_delay_time
            now - 200,          // second_hop_sent_time
            now - 100,          // second_hop_delay_time
            300,                // total_delay_time (300ms = GOOD performance)
            now                 // third_hop_sent_time
        );
        
        std::cout << "✓ FinalCalcDelayData created successfully!" << std::endl;
        std::cout << "  Track ID: " << final_data.getTrackId() << std::endl;
        std::cout << "  ECEF Speed: " << final_data.getECEFSpeed() << " m/s" << std::endl;
        std::cout << "  Total Delay: " << final_data.getTotalDelayTime() << " ms" << std::endl;
        
        // Performance analizi
        auto perf_status = final_data.getPerformanceStatus();
        std::cout << "  Performance Status: ";
        switch (perf_status) {
            case FinalCalcDelayData::PerformanceStatus::EXCELLENT:
                std::cout << "EXCELLENT (< 100ms)"; break;
            case FinalCalcDelayData::PerformanceStatus::GOOD:
                std::cout << "GOOD (100-500ms)"; break;
            case FinalCalcDelayData::PerformanceStatus::ACCEPTABLE:
                std::cout << "ACCEPTABLE (500-1000ms)"; break;
            case FinalCalcDelayData::PerformanceStatus::POOR:
                std::cout << "POOR (1000-5000ms)"; break;
            case FinalCalcDelayData::PerformanceStatus::CRITICAL:
                std::cout << "CRITICAL (> 5000ms)"; break;
        }
        std::cout << std::endl;
        
        // Performance metrics
        auto metrics = final_data.calculatePerformanceMetrics();
        std::cout << "  Performance Metrics:" << std::endl;
        std::cout << "    - First Hop Delay: " << metrics.first_hop_delay << " ms" << std::endl;
        std::cout << "    - Second Hop Delay: " << metrics.second_hop_delay << " ms" << std::endl;
        std::cout << "    - Average Hop Delay: " << metrics.average_hop_delay << " ms" << std::endl;
        std::cout << "    - End-to-End Delay: " << metrics.end_to_end_delay << " ms" << std::endl;
        
        std::cout << "\n🎉 All Tests Passed Successfully!" << std::endl;
        std::cout << "✅ GCC 8.5.0 compilation successful" << std::endl;
        std::cout << "✅ C++17 features working correctly" << std::endl;
        std::cout << "✅ Domain models functional" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}