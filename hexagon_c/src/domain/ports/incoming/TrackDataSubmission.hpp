#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include "../../model/FinalCalcDelayData.hpp"
#include <memory>
#include <vector>

namespace hat::domain::ports::incoming {

/**
 * Primary port: ZeroMQ üzerinden gelen track data'larını sistem içine alan arayüz
 * Bu port domain logic tarafından implement edilir ve adapter'lar tarafından çağrılır
 */
class TrackDataSubmission {
public:
    virtual ~TrackDataSubmission() = default;

    /**
     * DelayCalcTrackData verisi işleme alır
     * @param data Gelen track data
     * @return İşlem sonucu (başarılı/başarısız)
     */
    virtual bool submitDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * Batch olarak DelayCalcTrackData işleme alır (performans için)
     * @param delay_calc_data DelayCalcTrackData listesi
     * @return İşlenen veri sayısı
     */
    virtual size_t submitBatchDelayCalcData(
        const std::vector<model::DelayCalcTrackData>& delay_calc_data) = 0;

    /**
     * Sistem durumunu kontrol eder
     * @return Sistem veri almaya hazır mı?
     */
    virtual bool isReadyToReceive() const = 0;
};

} // namespace hat::domain::ports::incoming