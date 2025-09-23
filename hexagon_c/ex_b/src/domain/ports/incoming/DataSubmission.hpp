#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include <memory>
#include <vector>

namespace hat_b::domain::ports::incoming {

/**
 * Primary port: hexagon_b sistemine veri girişi için arayüz
 * Bu port domain logic tarafından implement edilir ve adapter'lar tarafından çağrılır
 */
class DataSubmission {
public:
    virtual ~DataSubmission() = default;

     /**
      * DelayCalcTrackData verisi işleme alır (hexagon_b'de üretilir)
      * @param data İşlenecek track data
      * @return İşlem sonucu (başarılı/başarısız)
      */
    virtual bool submitDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) = 0;

    /**
     * Batch olarak veri işleme alır
     * @param data_list DelayCalcTrackData listesi
     * @return İşlenen veri sayısı
     */
    virtual size_t submitBatchData(
        const std::vector<hat::domain::model::DelayCalcTrackData>& data_list) = 0;

    /**
     * Sistem durumunu kontrol eder
     * @return Sistem veri işlemeye hazır mı?
     */
    virtual bool isReadyToProcess() const = 0;
};

} // namespace hat_b::domain::ports::incoming