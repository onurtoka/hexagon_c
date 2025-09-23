/**
 * @file BinarySerializer.cpp
 * @brief Implementation of binary serialization utilities
 */

#include "common/BinarySerializer.hpp"
#include <stdexcept>

// Serialize ExtrapTrackData to binary format
std::vector<uint8_t> BinarySerializer::serialize(const ExtrapTrackData& data) noexcept {
    std::vector<uint8_t> buffer(EXTRAP_TRACK_DATA_SIZE);
    std::size_t offset = 0;

    // Serialize in fixed order
    writeToBuffer(buffer, offset, data.trackId);
    writeToBuffer(buffer, offset, data.xVelocityECEF);
    writeToBuffer(buffer, offset, data.yVelocityECEF);
    writeToBuffer(buffer, offset, data.zVelocityECEF);
    writeToBuffer(buffer, offset, data.xPositionECEF);
    writeToBuffer(buffer, offset, data.yPositionECEF);
    writeToBuffer(buffer, offset, data.zPositionECEF);
    writeToBuffer(buffer, offset, data.updateTime);
    writeToBuffer(buffer, offset, data.originalUpdateTime);
    writeToBuffer(buffer, offset, data.firstHopSentTime);

    return buffer;
}

// Deserialize binary data to ExtrapTrackData
ExtrapTrackData BinarySerializer::deserializeExtrapTrackData(const std::vector<uint8_t>& binaryData) {
    if (binaryData.size() != EXTRAP_TRACK_DATA_SIZE) {
        throw std::invalid_argument("BinarySerializer::deserializeExtrapTrackData: Invalid data size. Expected " +
                                   std::to_string(EXTRAP_TRACK_DATA_SIZE) + " bytes, got " +
                                   std::to_string(binaryData.size()) + " bytes");
    }

    return deserializeExtrapTrackData(binaryData.data(), binaryData.size());
}

// Deserialize binary data to ExtrapTrackData (from raw buffer)
ExtrapTrackData BinarySerializer::deserializeExtrapTrackData(const uint8_t* buffer, std::size_t size) {
    if (size != EXTRAP_TRACK_DATA_SIZE) {
        throw std::invalid_argument("BinarySerializer::deserializeExtrapTrackData: Invalid data size. Expected " +
                                   std::to_string(EXTRAP_TRACK_DATA_SIZE) + " bytes, got " +
                                   std::to_string(size) + " bytes");
    }

    ExtrapTrackData data;
    std::size_t offset = 0;

    // Deserialize in same order as serialization
    data.trackId = readFromBuffer<int>(buffer, offset);
    data.xVelocityECEF = readFromBuffer<double>(buffer, offset);
    data.yVelocityECEF = readFromBuffer<double>(buffer, offset);
    data.zVelocityECEF = readFromBuffer<double>(buffer, offset);
    data.xPositionECEF = readFromBuffer<double>(buffer, offset);
    data.yPositionECEF = readFromBuffer<double>(buffer, offset);
    data.zPositionECEF = readFromBuffer<double>(buffer, offset);
    data.updateTime = readFromBuffer<long>(buffer, offset);
    data.originalUpdateTime = readFromBuffer<long>(buffer, offset);
    data.firstHopSentTime = readFromBuffer<long>(buffer, offset);

    return data;
}

// Serialize DelayCalcTrackData to binary format
std::vector<uint8_t> BinarySerializer::serialize(const DelayCalcTrackData& data) noexcept {
    std::vector<uint8_t> buffer(DELAY_CALC_TRACK_DATA_SIZE);
    std::size_t offset = 0;

    // Serialize in fixed order
    writeToBuffer(buffer, offset, data.trackId);
    writeToBuffer(buffer, offset, data.xVelocityECEF);
    writeToBuffer(buffer, offset, data.yVelocityECEF);
    writeToBuffer(buffer, offset, data.zVelocityECEF);
    writeToBuffer(buffer, offset, data.xPositionECEF);
    writeToBuffer(buffer, offset, data.yPositionECEF);
    writeToBuffer(buffer, offset, data.zPositionECEF);
    writeToBuffer(buffer, offset, data.updateTime);
    writeToBuffer(buffer, offset, data.originalUpdateTime);
    writeToBuffer(buffer, offset, data.firstHopSentTime);
    writeToBuffer(buffer, offset, data.firstHopDelayTime);
    writeToBuffer(buffer, offset, data.secondHopSentTime);

    return buffer;
}

// Deserialize binary data to DelayCalcTrackData
DelayCalcTrackData BinarySerializer::deserializeDelayCalcTrackData(const std::vector<uint8_t>& binaryData) {
    if (binaryData.size() != DELAY_CALC_TRACK_DATA_SIZE) {
        throw std::invalid_argument("BinarySerializer::deserializeDelayCalcTrackData: Invalid data size. Expected " +
                                   std::to_string(DELAY_CALC_TRACK_DATA_SIZE) + " bytes, got " +
                                   std::to_string(binaryData.size()) + " bytes");
    }

    return deserializeDelayCalcTrackData(binaryData.data(), binaryData.size());
}

// Deserialize binary data to DelayCalcTrackData (from raw buffer)
DelayCalcTrackData BinarySerializer::deserializeDelayCalcTrackData(const uint8_t* buffer, std::size_t size) {
    if (size != DELAY_CALC_TRACK_DATA_SIZE) {
        throw std::invalid_argument("BinarySerializer::deserializeDelayCalcTrackData: Invalid data size. Expected " +
                                   std::to_string(DELAY_CALC_TRACK_DATA_SIZE) + " bytes, got " +
                                   std::to_string(size) + " bytes");
    }

    DelayCalcTrackData data;
    std::size_t offset = 0;

    // Deserialize in same order as serialization
    data.trackId = readFromBuffer<int>(buffer, offset);
    data.xVelocityECEF = readFromBuffer<double>(buffer, offset);
    data.yVelocityECEF = readFromBuffer<double>(buffer, offset);
    data.zVelocityECEF = readFromBuffer<double>(buffer, offset);
    data.xPositionECEF = readFromBuffer<double>(buffer, offset);
    data.yPositionECEF = readFromBuffer<double>(buffer, offset);
    data.zPositionECEF = readFromBuffer<double>(buffer, offset);
    data.updateTime = readFromBuffer<long>(buffer, offset);
    data.originalUpdateTime = readFromBuffer<long>(buffer, offset);
    data.firstHopSentTime = readFromBuffer<long>(buffer, offset);
    data.firstHopDelayTime = readFromBuffer<long>(buffer, offset);
    data.secondHopSentTime = readFromBuffer<long>(buffer, offset);

    return data;
}

// Template implementations for buffer read/write operations
template<typename T>
void BinarySerializer::writeToBuffer(std::vector<uint8_t>& buffer, std::size_t& offset, const T& value) noexcept {
    std::memcpy(buffer.data() + offset, &value, sizeof(T));
    offset += sizeof(T);
}

template<typename T>
T BinarySerializer::readFromBuffer(const uint8_t* buffer, std::size_t& offset) noexcept {
    T value;
    std::memcpy(&value, buffer + offset, sizeof(T));
    offset += sizeof(T);
    return value;
}

// Explicit template instantiations for the types we use
template void BinarySerializer::writeToBuffer<int>(std::vector<uint8_t>&, std::size_t&, const int&) noexcept;
template void BinarySerializer::writeToBuffer<double>(std::vector<uint8_t>&, std::size_t&, const double&) noexcept;
template void BinarySerializer::writeToBuffer<long>(std::vector<uint8_t>&, std::size_t&, const long&) noexcept;

template int BinarySerializer::readFromBuffer<int>(const uint8_t*, std::size_t&) noexcept;
template double BinarySerializer::readFromBuffer<double>(const uint8_t*, std::size_t&) noexcept;
template long BinarySerializer::readFromBuffer<long>(const uint8_t*, std::size_t&) noexcept;
