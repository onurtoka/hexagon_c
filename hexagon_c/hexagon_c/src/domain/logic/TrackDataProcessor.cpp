#include "TrackDataProcessor.hpp"
#include <iostream>
#include <chrono>

namespace hat::domain::logic {

TrackDataProcessor::TrackDataProcessor() {
}

bool TrackDataProcessor::submitDelayCalcTrackData(const model::DelayCalcTrackData& data) {
    try {
        if (!data.isValid()) {
            return false;
        }

        // FinalCalcDelayData oluştur ve performance metrics hesapla
        model::FinalCalcDelayData final_data = createFinalCalcDelayData(data);
        auto metrics = final_data.calculatePerformanceMetrics();

        auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        long long total_delay = now - data.getFirstHopSentTime();
        long long b_to_c_delay = now - data.getSecondHopSentTime();
        
        std::cout << "Track[" << data.getTrackId() << "] FirstSent:" << data.getFirstHopSentTime()
                  << " TotalDelay:" << total_delay << "μs B->C:" << b_to_c_delay << "μs"
                  << " LatencyScore:" << metrics.latency_score << std::endl;

        return true;

    } catch (const std::exception& e) {
        return false;
    }
}

bool TrackDataProcessor::isReadyToReceive() const {
    return true;
}

model::FinalCalcDelayData TrackDataProcessor::createFinalCalcDelayData(
    const model::DelayCalcTrackData& input_data) {
    
    model::FinalCalcDelayData final_data;

    final_data.setTrackId(input_data.getTrackId());

    auto current_time = std::chrono::system_clock::now();
    auto epoch_time = current_time.time_since_epoch();
    int64_t current_time_us = std::chrono::duration_cast<std::chrono::microseconds>(epoch_time).count();

    int64_t processing_delay = current_time_us - input_data.getOriginalUpdateTime();
    
    final_data.setTotalDelayTime(processing_delay);
    final_data.setFirstHopSentTime(input_data.getOriginalUpdateTime());
    final_data.setThirdHopSentTime(current_time_us);

    return final_data;
}

}