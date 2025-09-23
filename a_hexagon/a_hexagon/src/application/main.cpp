#include "domain/logic/TrackDataExtrapolator.hpp"
#include "adapters/outgoing/ZeroMQExtrapTrackDataAdapter.hpp"
#include "domain/model/TrackData.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <vector>

// Test data generator function - 8Hz ile TrackData üretir, 200Hz'ye extrapole eder
void generateTestData() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Gerçekçi test verisi aralıkları
    std::uniform_real_distribution<double> velocity_dist(-1000.0, 1000.0);  // m/s
    std::uniform_real_distribution<double> position_dist(-10000000.0, 10000000.0);  // meters ECEF
    std::uniform_int_distribution<int> track_id_dist(1000, 9999);
    
    int testTrackId = track_id_dist(gen);
    
    std::cout << "=== A_HEXAGON 8Hz→200Hz EXTRAPOLATION SYSTEM ===" << std::endl;
    std::cout << "8Hz TrackData üretiliyor → TrackDataExtrapolator → 200Hz ExtrapTrackData" << std::endl;
    std::cout << "Test Track ID: " << testTrackId << std::endl;
    std::cout << "B_hexagon'a UDP RADIO/DISH ile gönderiliyor..." << std::endl;
    
    // Outgoing adapter ve extrapolator kurulumu
    domain::adapters::outgoing::ZeroMQExtrapTrackDataAdapter outgoingAdapter;
    domain::logic::TrackDataExtrapolator extrapolator(&outgoingAdapter);
    
    int messageCount = 0;
    
    while (true) {
        try {
            // 8Hz TrackData oluştur (ham veri)
            domain::model::TrackData rawTrackData;
            
            // Gerçekçi test değerleri ayarla
            rawTrackData.setTrackId(testTrackId);
            rawTrackData.setXVelocityECEF(velocity_dist(gen));
            rawTrackData.setYVelocityECEF(velocity_dist(gen));
            rawTrackData.setZVelocityECEF(velocity_dist(gen));
            rawTrackData.setXPositionECEF(position_dist(gen));
            rawTrackData.setYPositionECEF(position_dist(gen));
            rawTrackData.setZPositionECEF(position_dist(gen));
            
            // Güncel timestamp'ler ayarla
            auto nowMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            rawTrackData.setOriginalUpdateTime(nowMilliseconds);
            
            // Domain logic'e gönder (8Hz → 200Hz extrapolation)
            extrapolator.processAndForwardTrackData(rawTrackData);
            
            messageCount++;
            std::cout << "[" << messageCount << "] 8Hz TrackData → Extrapolator → 200Hz ExtrapTrackData sent" 
                      << " - TrackId: " << rawTrackData.getTrackId() << std::endl;
            
            // 8Hz için 125ms bekle (1000ms / 8Hz = 125ms)
            std::this_thread::sleep_for(std::chrono::milliseconds(125));
            
        } catch (const std::exception& e) {
            std::cerr << "HATA - Test verisi oluşturulurken: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(125));
        }
    }
}

int main() {
    try {
        std::cout << "A_hexagon 8Hz→200Hz extrapolation system başlatılıyor..." << std::endl;
        
        // Test verisi gönderimini başlat (sonsuz döngü)
        generateTestData();
        
    } catch (const std::exception& e) {
        std::cerr << "ÖLÜMCÜL HATA main'de: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}