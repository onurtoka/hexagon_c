/**
 * @file CalculatorService.hpp
 * @brief Application service for track data processing and delay calculations
 */

#pragma once

#include "domain/model/TrackData.hpp"
#include "domain/model/DelayCalculatedTrackData.hpp"
#include <chrono>

/**
 * @class CalculatorService
 * @brief Domain service for calculating timing delays and processing track data
 */
class CalculatorService final {
public:
    /**
     * @brief Default constructor
     */
    CalculatorService() = default;

    /**
     * @brief Destructor
     */
    ~CalculatorService() = default;

    // Disable copy operations
    CalculatorService(const CalculatorService&) = delete;
    CalculatorService& operator=(const CalculatorService&) = delete;

    // Enable move operations
    CalculatorService(CalculatorService&&) noexcept = default;
    CalculatorService& operator=(CalculatorService&&) noexcept = default;

    /**
     * @brief Calculate delay between original update time and current time
     * @param trackData Input track data with timing information
     * @return DelayCalculatedTrackData with computed delay value
     */
    DelayCalculatedTrackData calculateDelay(const TrackData& trackData) const;

private:
    /**
     * @brief Get current time in microseconds since epoch
     * @return Current timestamp in microseconds
     */
    long getCurrentTimeMicroseconds() const noexcept;

    /**
     * @brief Calculate processing delay from timestamp
     * @param originalTime Original update time in microseconds
     * @param currentTime Current time in microseconds
     * @return Calculated delay in microseconds
     */
    long calculateTimeDelta(long originalTime, long currentTime) const noexcept;
};
