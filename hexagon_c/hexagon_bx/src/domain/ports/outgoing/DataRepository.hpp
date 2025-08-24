#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include <vector>
#include <optional>
#include <memory>

namespace hat_b::domain::ports::outgoing {

/**
 * Secondary port: hexagon_b'de veri saklama arayüzü
 * Bu port domain logic tarafından çağrılır ve persistence adapter'ı tarafından implement edilir
 */
class DataRepository {
public:
    virtual ~DataRepository() = default;

    /**
     * DelayCalcTrackData'yı saklar
     * @param data Saklanacak veri
     * @return Saklama başarılı mı?
     */
    virtual bool saveDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) = 0;

    /**
     * ID ile DelayCalcTrackData bulur
     * @param id Aranacak ID
     * @return Bulunan veri (varsa)
     */
    virtual std::optional<hat::domain::model::DelayCalcTrackData> findById(int id) = 0;

    /**
     * Track ID ile DelayCalcTrackData bulur
     * @param track_id Aranacak track ID
     * @return Bulunan veri listesi
     */
    virtual std::vector<hat::domain::model::DelayCalcTrackData> findByTrackId(int track_id) = 0;

    /**
     * DelayCalcTrackData'yı günceller
     * @param data Güncellenecek veri
     * @return Güncelleme başarılı mı?
     */
    virtual bool updateDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) = 0;

    /**
     * Belirli zaman aralığındaki verileri bulur
     * @param start_time Başlangıç zamanı (ms)
     * @param end_time Bitiş zamanı (ms)
     * @return Zaman aralığındaki veri listesi
     */
    virtual std::vector<hat::domain::model::DelayCalcTrackData> findByTimeRange(
        int64_t start_time, int64_t end_time) = 0;

    /**
     * Aktif track ID'leri döner
     * @return Aktif track ID listesi
     */
    virtual std::vector<int> getActiveTrackIds() = 0;

    /**
     * Eski verileri temizler
     * @param cutoff_time Bu zamandan eski veriler silinir (ms)
     * @return Silinen veri sayısı
     */
    virtual size_t cleanupOldData(int64_t cutoff_time) = 0;

    /**
     * Repository istatistiklerini döner
     */
    struct RepositoryStats {
        size_t total_records;
        size_t active_tracks;
        int64_t oldest_record_time;
        int64_t newest_record_time;
    };

    virtual RepositoryStats getRepositoryStats() = 0;
};

} // namespace hat_b::domain::ports::outgoing