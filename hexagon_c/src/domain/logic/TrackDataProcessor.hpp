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
    int64_t max_acceptable_delay_ms_;
    double high_velocity_threshold_;
    bool enable_batch_processing_;

public:
    TrackDataProcessor(
        std::shared_ptr<ports::outgoing::TrackDataPublisher> publisher,
        std::shared_ptr<ports::outgoing::TrackDataRepository> repository,
        int64_t max_acceptable_delay_ms = 5000,
        double high_velocity_threshold = 100.0,
        bool enable_batch_processing = true);

    bool submitDelayCalcTrackData(const model::DelayCalcTrackData& data) override;
    bool generateAndPublishFinalCalcDelayData(const model::DelayCalcTrackData& input_data);
    size_t submitBatchDelayCalcData(const std::vector<model::DelayCalcTrackData>& delay_calc_data) override;
    bool isReadyToReceive() const override;

private:
    bool processNormalDelayCalcData(const model::DelayCalcTrackData& data);
    bool processHighVelocityData(const model::DelayCalcTrackData& data);
    bool processNormalFinalCalcData(const model::FinalCalcDelayData& data);
    bool processCriticalPerformanceData(const model::FinalCalcDelayData& data);
    bool processPoorPerformanceData(const model::FinalCalcDelayData& data);
    model::FinalCalcDelayData createFinalCalcDelayData(const model::DelayCalcTrackData& input_data);
    size_t processBatchDelayCalcData(const std::vector<model::DelayCalcTrackData>& delay_calc_data);

    struct DelayAnalysisResult {
        bool is_acceptable;
        int64_t calculated_delay;
        std::string analysis_notes;
    };
    DelayAnalysisResult performDelayAnalysis(const model::DelayCalcTrackData& data);
    void checkRelatedTracks(int track_id);
    void triggerSystemAlert(const model::FinalCalcDelayData& data);
    void increaseMonitoringLevel(int track_id);
};

} // namespace hat::domain::logic