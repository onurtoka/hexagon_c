#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include "../../model/FinalCalcDelayData.hpp"
#include <vector>
#include <memory>

namespace hat::domain::ports::outgoing {

/**
 * Secondary port: İşlenen verileri ZeroMQ üzerinden dışarı gönderen arayüz
 * Bu port domain logic tarafından çağrılır ve adapter'lar tarafından implement edilir
 */
class TrackDataPublisher {
public:
    virtual ~TrackDataPublisher() = default;

    /**
     * İşlenen DelayCalcTrackData'yı yayınlar
     * @param data Yayınlanacak veri
     * @return Yayınlama başarılı mı?
     */
    virtual bool publishDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * İşlenen FinalCalcDelayData'yı yayınlar
     * @param data Yayınlanacak final veri
     * @return Yayınlama başarılı mı?
     */
    virtual bool publishFinalCalcDelayData(const model::FinalCalcDelayData& data) = 0;

    /**
     * Batch olarak veri yayınlar (performans için)
     * @param delay_calc_data DelayCalcTrackData listesi
     * @param final_calc_data FinalCalcDelayData listesi
     * @return Yayınlanan veri sayısı
     */
    virtual size_t publishBatchData(
        const std::vector<model::DelayCalcTrackData>& delay_calc_data,
        const std::vector<model::FinalCalcDelayData>& final_calc_data) = 0;

    /**
     * Belirli bir track ID'ye ait verileri yayınlar
     * @param track_id Hedef track ID
     * @param delay_calc_data DelayCalcTrackData (opsiyonel)
     * @param final_calc_data FinalCalcDelayData (opsiyonel)
     * @return Yayınlama başarılı mı?
     */
    virtual bool publishForTrackId(
        int track_id,
        const model::DelayCalcTrackData* delay_calc_data = nullptr,
        const model::FinalCalcDelayData* final_calc_data = nullptr) = 0;

    /**
     * Publisher durumunu kontrol eder
     * @return Publisher aktif mi?
     */
    virtual bool isPublisherActive() const = 0;

    /**
     * Publisher istatistiklerini döner
     */
    struct PublisherStats {
        size_t total_published;
        size_t failed_publications;
        size_t active_connections;
        double average_latency_ms;
    };

    virtual PublisherStats getPublisherStats() const = 0;
};

} // namespace hat::domain::ports::outgoing