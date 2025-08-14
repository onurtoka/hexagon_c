#pragma once

#include "../ports/incoming/TrackDataSubmission.hpp"
#include "../ports/outgoing/TrackDataPublisher.hpp"
#include "../ports/outgoing/TrackDataRepository.hpp"
#include "../model/DelayCalcTrackData.hpp"
#include "../model/FinalCalcDelayData.hpp"
#include <memory>
#include <chrono>

namespace hat::domain::logic {

/**
 * Core business logic - Hexagon'un kalbi
 * Track data işleme, gecikme analizi ve veri akışı yönetimi
 * Sadece domain modelleri ve port'lara bağımlı, teknik detaylardan bağımsız
 */
class TrackDataProcessor : public ports::incoming::TrackDataSubmission {
private:
    std::shared_ptr<ports::outgoing::TrackDataPublisher> publisher_;
    std::shared_ptr<ports::outgoing::TrackDataRepository> repository_;
    
    // İş mantığı konfigürasyonu
    int64_t max_acceptable_delay_ms_;
    double high_velocity_threshold_;
    bool enable_batch_processing_;

public:
    TrackDataProcessor(
        std::shared_ptr<ports::outgoing::TrackDataPublisher> publisher,
        std::shared_ptr<ports::outgoing::TrackDataRepository> repository,
        int64_t max_acceptable_delay_ms = 5000,
        double high_velocity_threshold = 100.0,
        bool enable_batch_processing = true)
        : publisher_(publisher)
        , repository_(repository)
        , max_acceptable_delay_ms_(max_acceptable_delay_ms)
        , high_velocity_threshold_(high_velocity_threshold)
        , enable_batch_processing_(enable_batch_processing) {}

    // TrackDataSubmission interface implementation
    
    /**
     * DelayCalcTrackData işleme - Ana iş mantığı
     */
    bool submitDelayCalcTrackData(const model::DelayCalcTrackData& data) override {
        try {
            // İş kuralı: Veri validasyonu
            if (!data.isValid()) {
                return false;
            }

            // İş kuralı: Veri yaşı kontrolü
            if (!data.isDataFresh(max_acceptable_delay_ms_)) {
                // Eski veri uyarısı - yine de işle ama logla
                // Gerçek uygulamada logging yapılacak
            }

            // İş kuralı: Yüksek hız kontrolü
            if (data.isHighVelocity(high_velocity_threshold_)) {
                // Yüksek hız uyarısı - öncelikli işlem
                return processHighVelocityData(data);
            }

            // Normal işlem akışı
            return processNormalDelayCalcData(data);

        } catch (const std::exception& e) {
            // Hata yönetimi
            return false;
        }
    }

    /**
     * FinalCalcDelayData üretir ve gönderir
     * DelayCalcTrackData'dan hesaplanır
     */
    bool generateAndPublishFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
        try {
            // FinalCalcDelayData'yı DelayCalcTrackData'dan üret
            auto final_data = createFinalCalcDelayData(input_data);
            
            if (!final_data.isValid()) {
                return false;
            }

            // İş kuralı: Performans analizi
            auto performance_status = final_data.getPerformanceStatus();
            
            switch (performance_status) {
                case model::FinalCalcDelayData::PerformanceStatus::CRITICAL:
                    return processCriticalPerformanceData(final_data);
                
                case model::FinalCalcDelayData::PerformanceStatus::POOR:
                    return processPoorPerformanceData(final_data);
                
                default:
                    return processNormalFinalCalcData(final_data);
            }

        } catch (const std::exception& e) {
            return false;
        }
    }

    /**
     * Batch DelayCalcTrackData işleme - Performans optimizasyonu
     */
    size_t submitBatchDelayCalcData(
        const std::vector<model::DelayCalcTrackData>& delay_calc_data) override {
        
        if (!enable_batch_processing_) {
            // Tek tek işle
            size_t processed = 0;
            for (const auto& data : delay_calc_data) {
                if (submitDelayCalcTrackData(data)) processed++;
            }
            return processed;
        }

        return processBatchDelayCalcData(delay_calc_data);
    }

    /**
     * Sistem hazır durumu kontrolü
     */
    bool isReadyToReceive() const override {
        return publisher_ && publisher_->isPublisherActive() && 
               repository_;
    }

private:
    /**
     * Normal DelayCalcTrackData işleme
     */
    bool processNormalDelayCalcData(const model::DelayCalcTrackData& data) {
        // 1. Veriyi sakla
        if (!repository_->saveDelayCalcTrackData(data)) {
            return false;
        }

        // 2. İş mantığı: Gecikme analizi yap
        auto delay_analysis = performDelayAnalysis(data);

        // 3. FinalCalcDelayData üret ve gönder
        if (!generateAndPublishFinalCalcDelayData(data)) {
            return false;
        }

        // 4. Orijinal DelayCalcTrackData'yı da yayınla (opsiyonel)
        return publisher_->publishDelayCalcTrackData(data);
    }

    /**
     * Yüksek hız verisi işleme - Öncelikli
     */
    bool processHighVelocityData(const model::DelayCalcTrackData& data) {
        // Yüksek hızlı veriler için özel işlem
        
        // 1. Öncelikli saklama
        if (!repository_->saveDelayCalcTrackData(data)) {
            return false;
        }

        // 2. Acil yayınlama
        if (!publisher_->publishDelayCalcTrackData(data)) {
            return false;
        }

        // 3. İlgili track'leri kontrol et
        checkRelatedTracks(data.getTrackId());
        
        return true;
    }

    /**
     * Normal FinalCalcDelayData işleme
     */
    bool processNormalFinalCalcData(const model::FinalCalcDelayData& data) {
        // 1. Final veriyi sakla
        if (!repository_->saveFinalCalcDelayData(data)) {
            return false;
        }

        // 2. Performans metrikleri hesapla
        auto metrics = data.calculatePerformanceMetrics();

        // 3. Final veriyi yayınla
        return publisher_->publishFinalCalcDelayData(data);
    }

    /**
     * Kritik performans verisi işleme
     */
    bool processCriticalPerformanceData(const model::FinalCalcDelayData& data) {
        // Kritik durum - acil müdahale
        
        // 1. Acil saklama
        repository_->saveFinalCalcDelayData(data);

        // 2. Acil yayınlama
        publisher_->publishFinalCalcDelayData(data);

        // 3. Sistem uyarısı tetikle
        triggerSystemAlert(data);

        return true;
    }

    /**
     * Zayıf performans verisi işleme
     */
    bool processPoorPerformanceData(const model::FinalCalcDelayData& data) {
        // Performans düşüklüğü - izleme artır
        
        repository_->saveFinalCalcDelayData(data);
        publisher_->publishFinalCalcDelayData(data);

        // İzleme seviyesini artır
        increaseMonitoringLevel(data.getTrackId());

        return true;
    }

    /**
     * DelayCalcTrackData'dan FinalCalcDelayData üretir
     * GERÇEK GECİKME HESAPLAMALARI - User Specified Formulas
     */
    model::FinalCalcDelayData createFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
        // KRITIK: Alım zamanını al (hexagon_c'de işleme zamanı)
        auto receive_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // GERÇEK GECİKME HESAPLAMALARI:
        // hexagon_b'den gelen gerçek gönderim zamanı
        int64_t sent_time = input_data.getFirstHopSentTime();
        
        // User Specified Formulas:
        // SecondHopDelayTime = ReceivedTime - FirstHopSentTime
        int64_t second_hop_delay_time = receive_time - sent_time;
        
        // TotalDelayTime = ReceivedTime - FirstHopSentTime  
        int64_t total_delay_time = receive_time - sent_time;
        
        // ThirdHopSentTime = CurrentTime (işleme zamanı)
        int64_t third_hop_sent_time = receive_time;
        
        // FinalCalcDelayData oluştur
        model::FinalCalcDelayData final_data(
            input_data.getTrackId(),                    // track_id
            input_data.getXVelocityECEF(),             // x_vel_ecef
            input_data.getYVelocityECEF(),             // y_vel_ecef  
            input_data.getZVelocityECEF(),             // z_vel_ecef
            input_data.getXPositionECEF(),             // x_pos_ecef
            input_data.getYPositionECEF(),             // y_pos_ecef
            input_data.getZPositionECEF(),             // z_pos_ecef
            input_data.getUpdateTime(),                 // update_time
            input_data.getOriginalUpdateTime(),         // original_update_time
            input_data.getFirstHopSentTime(),          // first_hop_sent
            input_data.getFirstHopDelayTime(),         // first_hop_delay
            input_data.getSecondHopSentTime(),         // second_hop_sent
            second_hop_delay_time,                      // second_hop_delay (HESAPLANAN)
            total_delay_time,                           // total_delay (HESAPLANAN)
            third_hop_sent_time                         // third_hop_sent (HESAPLANAN)
        );
        
        // GERÇEK GECİKME CONSOLE ÇIKTISI
        std::cout << "📨 Received Track " << input_data.getTrackId() 
                  << " - 📤 Sent: " << sent_time << " ms"
                  << " - 📨 Received: " << receive_time << " ms"
                  << " - ⏱️  DELAY: " << total_delay_time << " ms";
        
        // Gecikme kategorisi
        if (total_delay_time < 10) std::cout << " 🟢 EXCELLENT";
        else if (total_delay_time < 50) std::cout << " 🟢 GOOD";
        else if (total_delay_time < 100) std::cout << " 🟡 ACCEPTABLE";
        else if (total_delay_time < 500) std::cout << " 🟠 POOR";
        else std::cout << " 🔴 CRITICAL";
        std::cout << std::endl;
        
        return final_data;
    }

    /**
     * Batch DelayCalcTrackData işleme optimizasyonu
     */
    size_t processBatchDelayCalcData(
        const std::vector<model::DelayCalcTrackData>& delay_calc_data) {
        
        size_t processed = 0;

        // Batch validation ve processing
        std::vector<model::DelayCalcTrackData> valid_delay_data;
        std::vector<model::FinalCalcDelayData> generated_final_data;

        for (const auto& data : delay_calc_data) {
            if (data.isValid()) {
                valid_delay_data.push_back(data);
                
                // Her DelayCalcTrackData için FinalCalcDelayData üret
                auto final_data = createFinalCalcDelayData(data);
                generated_final_data.push_back(final_data);
                
                // Repository'ye kaydet
                repository_->saveDelayCalcTrackData(data);
                repository_->saveFinalCalcDelayData(final_data);
                
                processed++;
            }
        }

        // Batch publishing - hem orijinal hem de üretilen verileri gönder
        publisher_->publishBatchData(valid_delay_data, generated_final_data);

        return processed;
    }

    // Yardımcı iş mantığı metodları

    struct DelayAnalysisResult {
        bool is_acceptable;
        int64_t calculated_delay;
        std::string analysis_notes;
    };

    DelayAnalysisResult performDelayAnalysis(const model::DelayCalcTrackData& data) {
        DelayAnalysisResult result;
        result.calculated_delay = data.calculateDataAge();
        result.is_acceptable = result.calculated_delay <= max_acceptable_delay_ms_;
        result.analysis_notes = "Delay analysis completed for track " + std::to_string(data.getTrackId());
        return result;
    }

    void checkRelatedTracks(int track_id) {
        // İlgili track'leri kontrol et - iş mantığı
        auto related_data = repository_->findDelayCalcTrackDataByTrackId(track_id);
        // İlgili track analizi...
    }

    void triggerSystemAlert(const model::FinalCalcDelayData& data) {
        // Sistem uyarısı tetikle - iş mantığı
        // Gerçek uygulamada alert sistemi çağrılacak
    }

    void increaseMonitoringLevel(int track_id) {
        // İzleme seviyesini artır - iş mantığı
        // Gerçek uygulamada monitoring sistemi güncellenecek
    }
};

} // namespace hat::domain::logic