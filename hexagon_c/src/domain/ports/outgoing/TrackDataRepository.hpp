#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include "../../model/FinalCalcDelayData.hpp"
#include <vector>
#include <optional>
#include <memory>

namespace hat::domain::ports::outgoing {

/**
 * Secondary port: Track verilerini kalıcı olarak saklayan arayüz
 * Bu port domain logic tarafından çağrılır ve persistence adapter'ı tarafından implement edilir
 */
class TrackDataRepository {
public:
    virtual ~TrackDataRepository() = default;

    // DelayCalcTrackData operations
    
    /**
     * DelayCalcTrackData'yı saklar
     * @param data Saklanacak veri
     * @return Saklama başarılı mı?
     */
    virtual bool saveDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * ID ile DelayCalcTrackData bulur
     * @param id Aranacak ID
     * @return Bulunan veri (varsa)
     */
    virtual std::optional<model::DelayCalcTrackData> findDelayCalcTrackDataById(int id) = 0;

    /**
     * Track ID ile DelayCalcTrackData bulur
     * @param track_id Aranacak track ID
     * @return Bulunan veri listesi
     */
    virtual std::vector<model::DelayCalcTrackData> findDelayCalcTrackDataByTrackId(int track_id) = 0;

    /**
     * DelayCalcTrackData'yı günceller
     * @param data Güncellenecek veri
     * @return Güncelleme başarılı mı?
     */
    virtual bool updateDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    // FinalCalcDelayData operations

    /**
     * FinalCalcDelayData'yı saklar
     * @param data Saklanacak final veri
     * @return Saklama başarılı mı?
     */
    virtual bool saveFinalCalcDelayData(const model::FinalCalcDelayData& data) = 0;

    /**
     * Track ID ile FinalCalcDelayData bulur
     * @param track_id Aranacak track ID
     * @return Bulunan veri (varsa)
     */
    virtual std::optional<model::FinalCalcDelayData> findFinalCalcDelayDataByTrackId(int track_id) = 0;

    /**
     * FinalCalcDelayData'yı günceller
     * @param data Güncellenecek veri
     * @return Güncelleme başarılı mı?
     */
    virtual bool updateFinalCalcDelayData(const model::FinalCalcDelayData& data) = 0;

    // Query operations

    /**
     * Belirli zaman aralığındaki verileri bulur
     * @param start_time Başlangıç zamanı (ms)
     * @param end_time Bitiş zamanı (ms)
     * @return Zaman aralığındaki DelayCalcTrackData listesi
     */
    virtual std::vector<model::DelayCalcTrackData> findDelayCalcDataByTimeRange(
        int64_t start_time, int64_t end_time) = 0;

    /**
     * Belirli zaman aralığındaki final verileri bulur
     * @param start_time Başlangıç zamanı (ms)
     * @param end_time Bitiş zamanı (ms)
     * @return Zaman aralığındaki FinalCalcDelayData listesi
     */
    virtual std::vector<model::FinalCalcDelayData> findFinalCalcDataByTimeRange(
        int64_t start_time, int64_t end_time) = 0;

    /**
     * Yüksek gecikme olan verileri bulur
     * @param delay_threshold_ms Gecikme eşiği (ms)
     * @return Eşik üzerinde gecikme olan FinalCalcDelayData listesi
     */
    virtual std::vector<model::FinalCalcDelayData> findHighDelayData(
        int64_t delay_threshold_ms) = 0;

    /**
     * Aktif track ID'leri döner
     * @return Aktif track ID listesi
     */
    virtual std::vector<int> getActiveTrackIds() = 0;

    // Maintenance operations

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
        size_t total_delay_calc_records;
        size_t total_final_calc_records;
        size_t active_tracks;
        int64_t oldest_record_time;
        int64_t newest_record_time;
    };

    virtual RepositoryStats getRepositoryStats() = 0;
};

} // namespace hat::domain::ports::outgoing