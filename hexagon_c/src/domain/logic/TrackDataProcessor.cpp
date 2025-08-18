#include "TrackDataProcessor.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <exception>

using namespace hat::domain::logic;

TrackDataProcessor::TrackDataProcessor(
	std::shared_ptr<ports::outgoing::TrackDataPublisher> publisher,
	std::shared_ptr<ports::outgoing::TrackDataRepository> repository,
	int64_t max_acceptable_delay_ms,
	double high_velocity_threshold,
	bool enable_batch_processing)
	: publisher_(publisher)
	, repository_(repository)
	, max_acceptable_delay_ms_(max_acceptable_delay_ms)
	, high_velocity_threshold_(high_velocity_threshold)
	, enable_batch_processing_(enable_batch_processing) {}

bool TrackDataProcessor::submitDelayCalcTrackData(const model::DelayCalcTrackData& data) {
	try {
		if (!data.isValid()) {
			return false;
		}
		if (!data.isDataFresh(max_acceptable_delay_ms_)) {
			// Eski veri uyarısı - yine de işle ama logla
		}
		if (data.isHighVelocity(high_velocity_threshold_)) {
			return processHighVelocityData(data);
		}
		return processNormalDelayCalcData(data);
	} catch (const std::exception& e) {
		return false;
	}
}

bool TrackDataProcessor::generateAndPublishFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
	try {
		auto final_data = createFinalCalcDelayData(input_data);
		if (!final_data.isValid()) {
			return false;
		}
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

size_t TrackDataProcessor::submitBatchDelayCalcData(const std::vector<model::DelayCalcTrackData>& delay_calc_data) {
	if (!enable_batch_processing_) {
		size_t processed = 0;
		for (const auto& data : delay_calc_data) {
			if (submitDelayCalcTrackData(data)) {
				processed++;
			}
		}
		return processed;
	}
	return processBatchDelayCalcData(delay_calc_data);
}

bool TrackDataProcessor::isReadyToReceive() const {
	// Burada gerçek bir kontrol yapılabilir
	return true;
}

bool TrackDataProcessor::processNormalDelayCalcData(const model::DelayCalcTrackData& data) {
	repository_->saveDelayCalcTrackData(data);
	return generateAndPublishFinalCalcDelayData(data);
}

bool TrackDataProcessor::processHighVelocityData(const model::DelayCalcTrackData& data) {
	// Yüksek hızda özel işlem yapılabilir
	repository_->saveDelayCalcTrackData(data);
	return generateAndPublishFinalCalcDelayData(data);
}

bool TrackDataProcessor::processNormalFinalCalcData(const model::FinalCalcDelayData& data) {
	repository_->saveFinalCalcDelayData(data);
	publisher_->publishFinalCalcDelayData(data);
	return true;
}

bool TrackDataProcessor::processCriticalPerformanceData(const model::FinalCalcDelayData& data) {
	triggerSystemAlert(data);
	repository_->saveFinalCalcDelayData(data);
	publisher_->publishFinalCalcDelayData(data);
	return true;
}

bool TrackDataProcessor::processPoorPerformanceData(const model::FinalCalcDelayData& data) {
	increaseMonitoringLevel(data.getTrackId());
	repository_->saveFinalCalcDelayData(data);
	publisher_->publishFinalCalcDelayData(data);
	return true;
}

hat::domain::model::FinalCalcDelayData TrackDataProcessor::createFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
	auto receive_time = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	int64_t sent_time = input_data.getFirstHopSentTime();
	int64_t second_hop_delay_time = receive_time - sent_time;
	int64_t total_delay_time = receive_time - sent_time;
	int64_t third_hop_sent_time = receive_time;
	model::FinalCalcDelayData final_data(
		input_data.getTrackId(),
		input_data.getXVelocityECEF(),
		input_data.getYVelocityECEF(),
		input_data.getZVelocityECEF(),
		input_data.getXPositionECEF(),
		input_data.getYPositionECEF(),
		input_data.getZPositionECEF(),
		input_data.getUpdateTime(),
		input_data.getOriginalUpdateTime(),
		input_data.getFirstHopSentTime(),
		input_data.getFirstHopDelayTime(),
		input_data.getSecondHopSentTime(),
		second_hop_delay_time,
		total_delay_time,
		third_hop_sent_time
	);
	std::cout << "📨 Received Track " << input_data.getTrackId() 
			  << " - 📤 Sent: " << sent_time << " ms"
			  << " - 📨 Received: " << receive_time << " ms"
			  << " - ⏱️  DELAY: " << total_delay_time << " ms";
	if (total_delay_time < 10) std::cout << " 🟢 EXCELLENT";
	else if (total_delay_time < 50) std::cout << " 🟢 GOOD";
	else if (total_delay_time < 100) std::cout << " 🟡 ACCEPTABLE";
	else if (total_delay_time < 500) std::cout << " 🟠 POOR";
	else std::cout << " 🔴 CRITICAL";
	std::cout << std::endl;
	return final_data;
}

size_t TrackDataProcessor::processBatchDelayCalcData(const std::vector<model::DelayCalcTrackData>& delay_calc_data) {
	size_t processed = 0;
	std::vector<model::DelayCalcTrackData> valid_delay_data;
	std::vector<model::FinalCalcDelayData> generated_final_data;
	for (const auto& data : delay_calc_data) {
		if (data.isValid()) {
			valid_delay_data.push_back(data);
			auto final_data = createFinalCalcDelayData(data);
			generated_final_data.push_back(final_data);
			repository_->saveDelayCalcTrackData(data);
			repository_->saveFinalCalcDelayData(final_data);
			processed++;
		}
	}
	publisher_->publishBatchData(valid_delay_data, generated_final_data);
	return processed;
}

TrackDataProcessor::DelayAnalysisResult TrackDataProcessor::performDelayAnalysis(const model::DelayCalcTrackData& data) {
	DelayAnalysisResult result;
	result.calculated_delay = data.calculateDataAge();
	result.is_acceptable = result.calculated_delay <= max_acceptable_delay_ms_;
	result.analysis_notes = "Delay analysis completed for track " + std::to_string(data.getTrackId());
	return result;
}

void TrackDataProcessor::checkRelatedTracks(int track_id) {
	auto related_data = repository_->findDelayCalcTrackDataByTrackId(track_id);
	// İlgili track analizi...
}

void TrackDataProcessor::triggerSystemAlert(const model::FinalCalcDelayData& data) {
	// Gerçek uygulamada alert sistemi çağrılacak
}

void TrackDataProcessor::increaseMonitoringLevel(int track_id) {
	// Gerçek uygulamada monitoring sistemi güncellenecek
}
