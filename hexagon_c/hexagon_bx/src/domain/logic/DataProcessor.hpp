#pragma once

#include "../ports/incoming/DataSubmission.hpp"
#include "../ports/outgoing/DataPublisher.hpp"
#include "../ports/outgoing/DataRepository.hpp"
#include "../model/DelayCalcTrackData.hpp"
#include <memory>
#include <chrono>
#include <vector>
#include <optional>
#include <cmath>

namespace hat_b::domain::logic {

/**
 * Core business logic - hexagon_b'nin hexagon kalbi
 * DelayCalcTrackData üretir ve cpp_hat'e gönderir
 * Sadece domain modelleri ve port'lara bağımlı, teknik detaylardan bağımsız
 */
class DataProcessor : public ports::incoming::DataSubmission {
private:
    std::shared_ptr<ports::outgoing::DataPublisher> publisher_;
    std::shared_ptr<ports::outgoing::DataRepository> repository_;
    
    // İş mantığı konfigürasyonu
    bool enable_batch_processing_;
    size_t batch_size_;

public:
    DataProcessor(
        std::shared_ptr<ports::outgoing::DataPublisher> publisher,
        std::shared_ptr<ports::outgoing::DataRepository> repository,
        bool enable_batch_processing = true,
        size_t batch_size = 100)
        : publisher_(publisher)
        , repository_(repository)
        , enable_batch_processing_(enable_batch_processing)
        , batch_size_(batch_size) {}

    // DataSubmission interface implementation
    
    /**
     * DelayCalcTrackData işleme - hexagon_b'de üretilir ve gönderilir
     */
    bool submitDelayCalcTrackData(const hat::domain::model::DelayCalcTrackData& data) override {
        try {
            // İş kuralı: Veri validasyonu
            if (!data.isValid()) {
                return false;
            }

            // İş kuralı: Veri yaşı kontrolü
            if (!data.isDataFresh(5000)) {
                // Eski veri uyarısı - yine de işle
            }

            // Normal işlem akışı
            return processDelayCalcData(data);

        } catch (const std::exception& e) {
            return false;
        }
    }

    /**
     * Batch veri işleme
     */
    size_t submitBatchData(
        const std::vector<hat::domain::model::DelayCalcTrackData>& data_list) override {
        
        if (!enable_batch_processing_) {
            // Tek tek işle
            size_t processed = 0;
            for (const auto& data : data_list) {
                if (submitDelayCalcTrackData(data)) processed++;
            }
            return processed;
        }

        return processBatchData(data_list);
    }

    /**
     * Sistem hazır durumu kontrolü
     */
    bool isReadyToProcess() const override {
        // RADIO Publisher ve repository kontrolü
        return publisher_ != nullptr && publisher_->isPublisherActive() && repository_ != nullptr;
    }

    /**
     * DelayCalcTrackData üretir ve sisteme gönderir
     * hexagon_b'nin ana fonksiyonu
     */
    bool generateAndSendDelayCalcData(int track_id, 
                                      double x_vel, double y_vel, double z_vel,
                                      double x_pos, double y_pos, double z_pos) {
        try {
            // KRITIK: Gerçek gönderim zamanını al (hexagon_b'de)
            auto send_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            // DelayCalcTrackData oluştur - GERÇEK GECİKME ÖLÇÜMÜ İÇİN
            hat::domain::model::DelayCalcTrackData data(
                track_id,               // track_id
                x_vel, y_vel, z_vel,    // velocity ECEF
                x_pos, y_pos, z_pos,    // position ECEF
                send_time,              // update_time (gönderim zamanı)
                send_time - 100,        // original_update_time
                send_time,              // first_hop_sent_time (GERÇEK GÖNDERİM ZAMANI)
                0,                      // first_hop_delay_time (henüz gecikme yok)
                send_time               // second_hop_sent_time (gönderim zamanı)
            );

            // İş kuralları uygula
            return submitDelayCalcTrackData(data);

        } catch (const std::exception& e) {
            return false;
        }
    }

    // Yardımcı struct'lar
    struct TrackInfo {
        int track_id;
        double x_vel, y_vel, z_vel;
        double x_pos, y_pos, z_pos;
    };

    /**
     * Toplu veri üretimi ve gönderimi
     */
    size_t generateAndSendBatchData(const std::vector<TrackInfo>& track_infos) {
        std::vector<hat::domain::model::DelayCalcTrackData> data_list;
        
        for (const auto& info : track_infos) {
            if (auto data = createDelayCalcData(info)) {
                data_list.push_back(*data);
            }
        }

        return submitBatchData(data_list);
    }

private:
    /**
     * Normal DelayCalcTrackData işleme
     */
    bool processDelayCalcData(const hat::domain::model::DelayCalcTrackData& data) {
        // 1. Veriyi sakla
        if (!repository_->saveDelayCalcTrackData(data)) {
            return false;
        }

        // 2. hexagon_c'ye RADIO ile gönder
        if (!publisher_->publishDelayCalcTrackData(data)) {
            return false;
        }

        return true;
    }

    /**
     * Batch veri işleme optimizasyonu
     */
    size_t processBatchData(
        const std::vector<hat::domain::model::DelayCalcTrackData>& data_list) {
        
        size_t processed = 0;
        std::vector<hat::domain::model::DelayCalcTrackData> valid_data;

        // Validation
        for (const auto& data : data_list) {
            if (data.isValid()) {
                valid_data.push_back(data);
                
                // Repository'ye kaydet
                repository_->saveDelayCalcTrackData(data);
                processed++;
            }
        }

        // Batch publishing - RADIO ile gönder
        publisher_->publishBatchData(valid_data);

        return processed;
    }

    // Yardımcı metodlar
    int generateId() {
        static int counter = 1;
        return counter++;
    }

    double calculateVelocity(double x_vel, double y_vel, double z_vel) {
        return std::sqrt(x_vel * x_vel + y_vel * y_vel + z_vel * z_vel);
    }

    double calculatePosition(double x_pos, double y_pos, double z_pos) {
        return std::sqrt(x_pos * x_pos + y_pos * y_pos + z_pos * z_pos);
    }

    std::optional<hat::domain::model::DelayCalcTrackData> createDelayCalcData(
        const TrackInfo& info) {
        try {
            auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            return hat::domain::model::DelayCalcTrackData(
                info.track_id,
                info.x_vel, info.y_vel, info.z_vel,
                info.x_pos, info.y_pos, info.z_pos,
                current_time,
                current_time - 100,
                current_time - 50,
                current_time - 30,
                current_time - 10
            );
        } catch (const std::exception& e) {
            return std::nullopt;
        }
    }
};

} // namespace hat_b::domain::logic