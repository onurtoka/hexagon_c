#include <gtest/gtest.h>
#include "domain/logic/TrackDataProcessor.hpp"
#include "domain/model/DelayCalcTrackData.hpp"
#include "domain/model/FinalCalcTrackData.hpp"
#include "domain/ports/outgoing/TrackDataPublisher.hpp"
#include <memory>
#include <iostream>
#include <chrono>

class TrackDataProcessorMock : public IDataSender {
public:
    void sendData(const domain::model::FinalCalcTrackData& data) override {
        lastSentData = data;
        std::cout << "Mock Sender: Veri gönderildi - Track ID: " << data.getTrackId() << std::endl;
    }
    
    domain::model::FinalCalcTrackData lastSentData;
};

class TrackDataProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        dataSender = std::make_unique<TrackDataProcessorMock>();
        mockSender = dataSender.get();
        processor = std::make_unique<FinalCalculatorService>(std::move(dataSender));

        // Test verisi için simüle edilmiş zaman değerleri
        baseTime = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        // Test mesajını hazırla (B_hexagon'dan geliyormuş gibi)
        inputData.setTrackId(1234);
        inputData.setXVelocityECEF(100.0);
        inputData.setYVelocityECEF(200.0);
        inputData.setZVelocityECEF(300.0);
        inputData.setXPositionECEF(1000.0);
        inputData.setYPositionECEF(2000.0);
        inputData.setZPositionECEF(3000.0);
        
        // Zaman değerlerini ayarla (gerçek sistemi simüle et)
        inputData.setOriginalUpdateTime(baseTime - 10000);    // A'dan gelen orijinal zaman
        inputData.setUpdateTime(baseTime - 8000);            // B'de güncellenen zaman
        inputData.setFirstHopSentTime(baseTime - 5000);      // A→B gönderim zamanı
        inputData.setFirstHopDelayTime(3000);                // A→B gecikmesi
        inputData.setSecondHopSentTime(baseTime - 2000);     // B→C gönderim zamanı
    }

    std::unique_ptr<TrackDataProcessorMock> dataSender;
    TrackDataProcessorMock* mockSender;
    std::unique_ptr<FinalCalculatorService> processor;
    int64_t baseTime;
    domain::model::DelayCalcTrackData inputData;
};

TEST_F(TrackDataProcessorTest, SimulateRealSystemTest) {
    std::cout << "\n=== B_hexagon'dan Gelen Veri Simülasyonu ===" << std::endl;
    std::cout << "Track ID: " << inputData.getTrackId() << std::endl;
    std::cout << "A→B Gecikmesi: " << inputData.getFirstHopDelayTime() << " μs" << std::endl;
    std::cout << "B İşlem Süresi: " << 
        (inputData.getSecondHopSentTime() - inputData.getFirstHopSentTime()) << " μs" << std::endl;
    
    // Veriyi gerçek işleyiciye gönder (normal sistemdeki gibi)
    processor->onDataReceived(inputData);
    
    std::cout << "\n=== Gerçek Sistem Test Sonuçları ===" << std::endl;
    std::cout << "1. B_hexagon'dan Gelen Mesaj:" << std::endl;
    std::cout << "   - Track ID: " << inputData.getTrackId() << std::endl;
    std::cout << "   - A→B Gecikmesi: " << inputData.getFirstHopDelayTime() << " μs" << std::endl;
    std::cout << "   - B İşlem Süresi: " << 
        (inputData.getSecondHopSentTime() - inputData.getFirstHopSentTime()) << " μs\n" << std::endl;
    
    // FinalCalculatorService'i kullanarak gerçek işlemi yap
    std::cout << "2. C_hexagon İşlemi Başlıyor:" << std::endl;
    processor->onDataReceived(inputData);
    
    // İşlenmiş sonuçları al
    const auto& finalData = mockSender->lastSentData;
    
    std::cout << "\n3. C_hexagon İşlem Sonuçları:" << std::endl;
    std::cout << "   - İlk Atlama (A→B): " << finalData.getFirstHopDelayTime() << " μs" << std::endl;
    std::cout << "   - İkinci Atlama (B): " << 
        (finalData.getSecondHopSentTime() - finalData.getFirstHopSentTime()) << " μs" << std::endl;
    std::cout << "   - Son Atlama (B→C): " << 
        (finalData.getThirdHopSentTime() - finalData.getSecondHopSentTime()) << " μs" << std::endl;
    std::cout << "   - Toplam Gecikme (A→B→C): " << finalData.getTotalDelayTime() << " μs\n" << std::endl;
    
    // Temel doğrulamalar
    EXPECT_EQ(finalData.getTrackId(), 1234);
    EXPECT_GT(finalData.getTotalDelayTime(), 0);
    EXPECT_GT(finalData.getSecondHopSentTime(), finalData.getFirstHopSentTime());
    EXPECT_GT(finalData.getThirdHopSentTime(), finalData.getSecondHopSentTime());
}