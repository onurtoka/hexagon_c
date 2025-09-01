#include "../domain/logic/TrackDataExtrapolator.h"
#include "../adapters/outgoing/ZeroMQExtrapTrackDataAdapter.h"
#include "../domain/model/ExtrapTrackData.h"               
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\nShutdown signal received. Stopping data generation..." << std::endl;
    running.store(false);
}

// Test verisi üretme fonksiyonu
domain::model::ExtrapTrackData generateTestData(int trackId) {
    domain::model::ExtrapTrackData data;
    
    // Test değerleri (gerçek implementasyonda sensör verisi gelir)
    data.setTrackId(trackId);
    data.setXPositionECEF(100000.0 + (trackId * 10));
    data.setYPositionECEF(200000.0 + (trackId * 20));
    data.setZPositionECEF(300000.0 + (trackId * 30));
    data.setXVelocityECEF(50.0 + (trackId * 0.1));
    data.setYVelocityECEF(75.0 + (trackId * 0.2));
    data.setZVelocityECEF(25.0 + (trackId * 0.3));
    
    auto now = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    data.setOriginalUpdateTime(now - 1000); // 1ms önce (1000 µs)
    data.setUpdateTime(now);
    data.setFirstHopSentTime(now);
    
    return data;
}

int main() {
    std::cout << "=== A_Hexagon ExtrapTrackData Generator Starting ===" << std::endl;
    
    // Signal handler'ı kur
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    domain::adapters::outgoing::ZeroMQExtrapTrackDataAdapter outgoingAdapter;
    domain::logic::TrackDataExtrapolator extrapolator(&outgoingAdapter);
    
    std::cout << "✓ ZeroMQ Outgoing adapter hazır (Port 9001)" << std::endl;
    std::cout << "✓ Starting ExtrapTrackData generation (5 tracks, 2 sec interval)" << std::endl;
    std::cout << "✓ Press Ctrl+C to stop" << std::endl;
    
    int cycle = 0;
    while (running.load()) {
        try {
            std::vector<domain::model::ExtrapTrackData> dataList;
            
            // 5 farklı track için veri üret
            for (int trackId = 100; trackId < 105; trackId++) {
                domain::model::ExtrapTrackData data = generateTestData(trackId + cycle);
                dataList.push_back(data);
            }
            
            // Veriyi gönder
            outgoingAdapter.sendExtrapTrackData(dataList);
            
            // std::cout << "📡 Sent " << dataList.size() 
            //           << " ExtrapTrackData messages (cycle " << cycle << ")" << std::endl;
            
            cycle++;
            
            // 2 saniye bekle
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
        } catch (const std::exception& e) {
            std::cerr << "❌ Error during data generation: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    std::cout << "✓ A_Hexagon data generator stopped gracefully" << std::endl;
    return 0;
}
