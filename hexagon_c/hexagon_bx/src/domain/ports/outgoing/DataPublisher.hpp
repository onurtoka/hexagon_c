#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include <vector>
#include <memory>

namespace hat_b::domain::ports::outgoing {

/**
 * Secondary port: hexagon_b'den cpp_hat'e veri gönderen arayüz
 * Bu port domain logic tarafından çağrılır ve adapter'lar tarafından implement edilir
 */
class DataPublisher {
public:
    virtual ~DataPublisher() = default;

     /**
      * DelayCalcTrackData'yı cpp_hat'e gönderir
      * @param data Gönderilecek veri
      * @return Gönderim başarılı mı?
      */
    virtual bool publishDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) = 0;

    /**
     * Batch olarak veri gönderir
     * @param data_list DelayCalcTrackData listesi
     * @return Gönderilen veri sayısı
     */
    virtual size_t publishBatchData(
        const std::vector<hat::domain::model::DelayCalcTrackData>& data_list) = 0;

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

} // namespace hat_b::domain::ports::outgoing