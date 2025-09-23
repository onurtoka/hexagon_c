/**
 * @file BinarySerializer.hpp
 * @brief Binary serialization utilities for domain objects
 */

#pragma once

#include "domain/model/ExtrapTrackData.hpp"
#include "domain/model/DelayCalcTrackData.hpp"
#include <vector>
#include <cstring>
#include <stdexcept>

/**
 * @class BinarySerializer
 * @brief Provides binary serialization/deserialization for domain objects
 * 
 * Implements efficient binary serialization for:
 * - ExtrapTrackData (input)
 * - DelayCalcTrackData (output)
 * 
 * Uses fixed-size binary format for network transmission efficiency.
 */
class BinarySerializer final {
public:
    // Fixed size constants for binary format
    static constexpr std::size_t EXTRAP_TRACK_DATA_SIZE = 
        sizeof(int) +           // trackId
        6 * sizeof(double) +    // velocities and positions (ECEF)
        3 * sizeof(long);       // timing fields

    static constexpr std::size_t DELAY_CALC_TRACK_DATA_SIZE = 
        sizeof(int) +           // trackId
        6 * sizeof(double) +    // velocities and positions (ECEF)
        5 * sizeof(long);       // timing fields including calculated values

    /**
     * @brief Serialize ExtrapTrackData to binary format
     * @param data The ExtrapTrackData object to serialize
     * @return Binary representation as byte vector
     */
    [[nodiscard]] static std::vector<uint8_t> serialize(const ExtrapTrackData& data) noexcept;

    /**
     * @brief Deserialize binary data to ExtrapTrackData
     * @param binaryData The binary data to deserialize
     * @return Deserialized ExtrapTrackData object
     * @throws std::invalid_argument if data size is incorrect
     */
    [[nodiscard]] static ExtrapTrackData deserializeExtrapTrackData(const std::vector<uint8_t>& binaryData);

    /**
     * @brief Deserialize binary data to ExtrapTrackData (from raw buffer)
     * @param buffer Pointer to binary data
     * @param size Size of the binary data
     * @return Deserialized ExtrapTrackData object
     * @throws std::invalid_argument if data size is incorrect
     */
    [[nodiscard]] static ExtrapTrackData deserializeExtrapTrackData(const uint8_t* buffer, std::size_t size);

    /**
     * @brief Serialize DelayCalcTrackData to binary format
     * @param data The DelayCalcTrackData object to serialize
     * @return Binary representation as byte vector
     */
    [[nodiscard]] static std::vector<uint8_t> serialize(const DelayCalcTrackData& data) noexcept;

    /**
     * @brief Deserialize binary data to DelayCalcTrackData
     * @param binaryData The binary data to deserialize
     * @return Deserialized DelayCalcTrackData object
     * @throws std::invalid_argument if data size is incorrect
     */
    [[nodiscard]] static DelayCalcTrackData deserializeDelayCalcTrackData(const std::vector<uint8_t>& binaryData);

    /**
     * @brief Deserialize binary data to DelayCalcTrackData (from raw buffer)
     * @param buffer Pointer to binary data
     * @param size Size of the binary data
     * @return Deserialized DelayCalcTrackData object
     * @throws std::invalid_argument if data size is incorrect
     */
    [[nodiscard]] static DelayCalcTrackData deserializeDelayCalcTrackData(const uint8_t* buffer, std::size_t size);

private:
    /**
     * @brief Write data to buffer with proper alignment
     * @tparam T Type of data to write
     * @param buffer Target buffer
     * @param offset Current offset in buffer (will be updated)
     * @param value Value to write
     */
    template<typename T>
    static void writeToBuffer(std::vector<uint8_t>& buffer, std::size_t& offset, const T& value) noexcept;

    /**
     * @brief Read data from buffer with proper alignment
     * @tparam T Type of data to read
     * @param buffer Source buffer
     * @param offset Current offset in buffer (will be updated)
     * @return Read value
     */
    template<typename T>
    static T readFromBuffer(const uint8_t* buffer, std::size_t& offset) noexcept;
};
