#pragma once

#include "../ports/incoming/TrackDataSubmission.hpp"
#include "../model/DelayCalcTrackData.hpp"
#include "../model/FinalCalcDelayData.hpp"

namespace hat::domain::logic {

// Basit konfigürasyon - sadece gerekli
struct ProcessorConfig {
    // Minimal config - sadece gerekli olanlar
};

/**
 * Core business logic - Hexagon'un kalbi
 * Track data işleme, gecikme analizi ve console output
 * Sadece domain modelleri ve incoming port'a bağımlı, publisher yok
 */
class TrackDataProcessor : public ports::incoming::TrackDataSubmission {
    
public:
    TrackDataProcessor();

    // TrackDataSubmission interface implementation
    
    /**
     * DelayCalcTrackData işleme - Ana iş mantığı
     */
    bool submitDelayCalcTrackData(const model::DelayCalcTrackData& data) override;

    /**
     * Sistem hazır durumu kontrolü
     */
    bool isReadyToReceive() const override;

private:
    /**
     * DelayCalcTrackData'dan FinalCalcDelayData üretir
     * Basit gecikme hesaplaması - gereksiz karmaşıklık kaldırıldı
     */
    model::FinalCalcDelayData createFinalCalcDelayData(const model::DelayCalcTrackData& input_data);
};

} // namespace hat::domain::logic