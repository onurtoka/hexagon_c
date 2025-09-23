#pragma once

#include "../ports/incoming/TrackDataSubmission.hpp"
#include "../ports/outgoing/TrackDataPublisher.hpp"
#include "../model/DelayCalcTrackData.hpp"
#include "../model/FinalCalcTrackData.hpp"
#include <memory>

/**
 * @class FinalCalculatorService
 * @brief Core business logic for processing DelayCalcTrackData and generating FinalCalcTrackData
 * 
 * Receives DelayCalcTrackData from B_hexagon, performs final delay analysis,
 * and sends FinalCalcTrackData to external systems.
 */
class FinalCalculatorService final : public IDataReceiver {
private:
    std::unique_ptr<IDataSender> dataSender_;

public:
    explicit FinalCalculatorService(std::unique_ptr<IDataSender> dataSender);

    // IDataReceiver interface implementation
    void onDataReceived(const domain::model::DelayCalcTrackData& data) override;

private:
    /**
     * @brief Process DelayCalcTrackData and generate FinalCalcTrackData
     * @param input Incoming DelayCalcTrackData from B_hexagon
     * @return Final calculated track data with complete delay analysis
     */
    domain::model::FinalCalcTrackData calculateFinalDelay(const domain::model::DelayCalcTrackData& input);
};