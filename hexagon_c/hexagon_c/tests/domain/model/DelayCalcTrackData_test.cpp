#include <gtest/gtest.h>
#include "domain/model/DelayCalcTrackData.hpp"

using namespace domain::model;

class DelayCalcTrackDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        trackData = DelayCalcTrackData();
    }

    DelayCalcTrackData trackData;
};

TEST_F(DelayCalcTrackDataTest, DelayTimeComparison) {
    // Test verilerini ayarla
    const int64_t originalTime = 1000;
    const int64_t firstHopTime = 1200;
    const int64_t firstHopDelay = 50;
    const int64_t secondHopTime = 1300;

    trackData.setOriginalUpdateTime(originalTime);
    trackData.setFirstHopSentTime(firstHopTime);
    trackData.setFirstHopDelayTime(firstHopDelay);
    trackData.setSecondHopSentTime(secondHopTime);

    // Gecikme sürelerini hesapla ve karşılaştır
    const int64_t firstHopLatency = trackData.getFirstHopSentTime() - trackData.getOriginalUpdateTime();
    const int64_t secondHopLatency = trackData.getSecondHopSentTime() - trackData.getFirstHopSentTime();
    const int64_t totalLatency = trackData.getSecondHopSentTime() - trackData.getOriginalUpdateTime();

    // Sonuçları yazdır ve doğrula
    std::cout << "\nGecikme Süreleri Analizi:" << std::endl;
    std::cout << "İlk Atlama Gecikmesi: " << firstHopLatency << " ms" << std::endl;
    std::cout << "İkinci Atlama Gecikmesi: " << secondHopLatency << " ms" << std::endl;
    std::cout << "Toplam Gecikme: " << totalLatency << " ms" << std::endl;
    std::cout << "Bildirilen İlk Atlama Gecikmesi: " << trackData.getFirstHopDelayTime() << " ms\n" << std::endl;

    // Beklenen değerlerle karşılaştır
    EXPECT_EQ(firstHopLatency, 200);  // 1200 - 1000
    EXPECT_EQ(secondHopLatency, 100); // 1300 - 1200
    EXPECT_EQ(totalLatency, 300);     // 1300 - 1000
    EXPECT_EQ(trackData.getFirstHopDelayTime(), firstHopDelay);
}