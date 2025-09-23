#include "TrackDataProcessor.hpp"
#include <iostream>
#include <chrono>

FinalCalculatorService::FinalCalculatorService(std::unique_ptr<IDataSender> dataSender)
    : dataSender_(std::move(dataSender)) {
}

void FinalCalculatorService::onDataReceived(const domain::model::DelayCalcTrackData& data) {
    try {
        if (!data.isValid()) {
            std::cout << "Invalid DelayCalcTrackData received" << std::endl;
            return;
        }

        // Calculate final delay analysis
        domain::model::FinalCalcTrackData finalData = calculateFinalDelay(data);
        
        // Send processed data via outgoing adapter
        if (dataSender_) {
            dataSender_->sendData(finalData);
        }

        // Log processing results
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        
        long long totalDelay = now - data.getFirstHopSentTime();
        long long bToCDelay = now - data.getSecondHopSentTime();
        
        std::cout << "📊 Track[" << data.getTrackId() 
                  << "] Total:" << totalDelay << "μs"
                  << " B→C:" << bToCDelay << "μs"
                  << " Final:" << finalData.getTotalDelayTime() << "μs" << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Error processing DelayCalcTrackData: " << e.what() << std::endl;
    }
}

domain::model::FinalCalcTrackData FinalCalculatorService::calculateFinalDelay(
    const domain::model::DelayCalcTrackData& input) {
    
    domain::model::FinalCalcTrackData finalData;

    // Copy basic track information
    finalData.setTrackId(static_cast<uint16_t>(input.getTrackId()));
    finalData.setXVelocityECEF(input.getXVelocityECEF());
    finalData.setYVelocityECEF(input.getYVelocityECEF());
    finalData.setZVelocityECEF(input.getZVelocityECEF());
    finalData.setXPositionECEF(input.getXPositionECEF());
    finalData.setYPositionECEF(input.getYPositionECEF());
    finalData.setZPositionECEF(input.getZPositionECEF());

    // Copy timing information from previous hops
    finalData.setOriginalUpdateTime(static_cast<uint32_t>(input.getOriginalUpdateTime()));
    finalData.setUpdateTime(static_cast<uint32_t>(input.getUpdateTime()));
    finalData.setFirstHopSentTime(static_cast<uint32_t>(input.getFirstHopSentTime()));
    finalData.setFirstHopDelayTime(static_cast<uint32_t>(input.getFirstHopDelayTime()));
    finalData.setSecondHopSentTime(static_cast<uint32_t>(input.getSecondHopSentTime()));
    finalData.setSecondHopDelayTime(static_cast<uint32_t>(input.getSecondHopSentTime() - input.getFirstHopSentTime()));

    // Calculate final hop timing
    auto currentTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    
    uint32_t thirdHopSentTime = static_cast<uint32_t>(currentTime);
    uint32_t totalDelayTime = thirdHopSentTime - static_cast<uint32_t>(input.getFirstHopSentTime());

    finalData.setThirdHopSentTime(thirdHopSentTime);
    finalData.setTotalDelayTime(totalDelayTime);

    return finalData;
}