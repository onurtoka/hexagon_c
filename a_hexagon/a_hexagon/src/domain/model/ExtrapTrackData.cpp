#include "domain/model/ExtrapTrackData.hpp"

namespace domain {
namespace model {

// MISRA C++ 2023 compliant constructor implementation
ExtrapTrackData::ExtrapTrackData() noexcept {
    trackId_ = static_cast<int32_t>(0);
    xVelocityECEF_ = static_cast<double>(0);
    yVelocityECEF_ = static_cast<double>(0);
    zVelocityECEF_ = static_cast<double>(0);
    xPositionECEF_ = static_cast<double>(0);
    yPositionECEF_ = static_cast<double>(0);
    zPositionECEF_ = static_cast<double>(0);
    originalUpdateTime_ = static_cast<int64_t>(0);
    updateTime_ = static_cast<int64_t>(0);
    firstHopSentTime_ = static_cast<int64_t>(0);
}

    void ExtrapTrackData::validateTrackId(int32_t value) const {
        if (value < 1LL || value > 4294967295LL) {
            throw std::out_of_range("TrackId value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateXVelocityECEF(double value) const {
        if (std::isnan(value) || value < -1.0E+6 || value > 1.0E+6) {
            throw std::out_of_range("XVelocityECEF value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateYVelocityECEF(double value) const {
        if (std::isnan(value) || value < -1.0E+6 || value > 1.0E+6) {
            throw std::out_of_range("YVelocityECEF value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateZVelocityECEF(double value) const {
        if (std::isnan(value) || value < -1.0E+6 || value > 1.0E+6) {
            throw std::out_of_range("ZVelocityECEF value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateXPositionECEF(double value) const {
        if (std::isnan(value) || value < -9.9E+10 || value > 9.9E+10) {
            throw std::out_of_range("XPositionECEF value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateYPositionECEF(double value) const {
        if (std::isnan(value) || value < -9.9E+10 || value > 9.9E+10) {
            throw std::out_of_range("YPositionECEF value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateZPositionECEF(double value) const {
        if (std::isnan(value) || value < -9.9E+10 || value > 9.9E+10) {
            throw std::out_of_range("ZPositionECEF value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateOriginalUpdateTime(int64_t value) const {
        if (value < 0LL || value > 9223372036854775LL) {  // Microsecond epoch time için yeterli
            throw std::out_of_range("OriginalUpdateTime value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateUpdateTime(int64_t value) const {
        if (value < 0LL || value > 9223372036854775LL) {  // Microsecond epoch time için yeterli
            throw std::out_of_range("UpdateTime value is out of valid range: " + std::to_string(value));
        }
    }

    void ExtrapTrackData::validateFirstHopSentTime(int64_t value) const {
        if (value < 0LL || value > 9223372036854775LL) {  // Microsecond epoch time için yeterli
            throw std::out_of_range("FirstHopSentTime value is out of valid range: " + std::to_string(value));
        }
    }

int32_t ExtrapTrackData::getTrackId() const noexcept {
    return trackId_;
}

void ExtrapTrackData::setTrackId(const int32_t& value) {
    validateTrackId(value);
    trackId_ = value;
}

double ExtrapTrackData::getXVelocityECEF() const noexcept {
    return xVelocityECEF_;
}

void ExtrapTrackData::setXVelocityECEF(const double& value) {
    validateXVelocityECEF(value);
    xVelocityECEF_ = value;
}

double ExtrapTrackData::getYVelocityECEF() const noexcept {
    return yVelocityECEF_;
}

void ExtrapTrackData::setYVelocityECEF(const double& value) {
    validateYVelocityECEF(value);
    yVelocityECEF_ = value;
}

double ExtrapTrackData::getZVelocityECEF() const noexcept {
    return zVelocityECEF_;
}

void ExtrapTrackData::setZVelocityECEF(const double& value) {
    validateZVelocityECEF(value);
    zVelocityECEF_ = value;
}

double ExtrapTrackData::getXPositionECEF() const noexcept {
    return xPositionECEF_;
}

void ExtrapTrackData::setXPositionECEF(const double& value) {
    validateXPositionECEF(value);
    xPositionECEF_ = value;
}

double ExtrapTrackData::getYPositionECEF() const noexcept {
    return yPositionECEF_;
}

void ExtrapTrackData::setYPositionECEF(const double& value) {
    validateYPositionECEF(value);
    yPositionECEF_ = value;
}

double ExtrapTrackData::getZPositionECEF() const noexcept {
    return zPositionECEF_;
}

void ExtrapTrackData::setZPositionECEF(const double& value) {
    validateZPositionECEF(value);
    zPositionECEF_ = value;
}

int64_t ExtrapTrackData::getOriginalUpdateTime() const noexcept {
    return originalUpdateTime_;
}

void ExtrapTrackData::setOriginalUpdateTime(const int64_t& value) {
    validateOriginalUpdateTime(value);
    originalUpdateTime_ = value;
}

int64_t ExtrapTrackData::getUpdateTime() const noexcept {
    return updateTime_;
}

void ExtrapTrackData::setUpdateTime(const int64_t& value) {
    validateUpdateTime(value);
    updateTime_ = value;
}

int64_t ExtrapTrackData::getFirstHopSentTime() const noexcept {
    return firstHopSentTime_;
}

void ExtrapTrackData::setFirstHopSentTime(const int64_t& value) {
    validateFirstHopSentTime(value);
    firstHopSentTime_ = value;
}

bool ExtrapTrackData::isValid() const noexcept {
    try {
        validateTrackId(trackId_);
        validateXVelocityECEF(xVelocityECEF_);
        validateYVelocityECEF(yVelocityECEF_);
        validateZVelocityECEF(zVelocityECEF_);
        validateXPositionECEF(xPositionECEF_);
        validateYPositionECEF(yPositionECEF_);
        validateZPositionECEF(zPositionECEF_);
        validateOriginalUpdateTime(originalUpdateTime_);
        validateUpdateTime(updateTime_);
        validateFirstHopSentTime(firstHopSentTime_);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// MISRA C++ 2023 compliant Binary Serialization Implementation
std::vector<uint8_t> ExtrapTrackData::serialize() const {
    std::vector<uint8_t> buffer;
    buffer.reserve(getSerializedSize());
    
    // Serialize trackId_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&trackId_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(trackId_));
    }
    
    // Serialize xVelocityECEF_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&xVelocityECEF_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(xVelocityECEF_));
    }
    
    // Serialize yVelocityECEF_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&yVelocityECEF_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(yVelocityECEF_));
    }
    
    // Serialize zVelocityECEF_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&zVelocityECEF_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(zVelocityECEF_));
    }
    
    // Serialize xPositionECEF_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&xPositionECEF_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(xPositionECEF_));
    }
    
    // Serialize yPositionECEF_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&yPositionECEF_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(yPositionECEF_));
    }
    
    // Serialize zPositionECEF_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&zPositionECEF_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(zPositionECEF_));
    }
    
    // Serialize originalUpdateTime_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&originalUpdateTime_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(originalUpdateTime_));
    }
    
    // Serialize updateTime_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&updateTime_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(updateTime_));
    }
    
    // Serialize firstHopSentTime_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&firstHopSentTime_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(firstHopSentTime_));
    }
    
    return buffer;
}

bool ExtrapTrackData::deserialize(const std::vector<uint8_t>& data) noexcept {
    if (data.size() < getSerializedSize()) {
        return false;
    }
    
    std::size_t offset = 0U;
    
    // Deserialize trackId_
    if (offset + sizeof(trackId_) <= data.size()) {
        std::memcpy(&trackId_, &data[offset], sizeof(trackId_));
        offset += sizeof(trackId_);
    } else {
        return false;
    }
    
    // Deserialize xVelocityECEF_
    if (offset + sizeof(xVelocityECEF_) <= data.size()) {
        std::memcpy(&xVelocityECEF_, &data[offset], sizeof(xVelocityECEF_));
        offset += sizeof(xVelocityECEF_);
    } else {
        return false;
    }
    
    // Deserialize yVelocityECEF_
    if (offset + sizeof(yVelocityECEF_) <= data.size()) {
        std::memcpy(&yVelocityECEF_, &data[offset], sizeof(yVelocityECEF_));
        offset += sizeof(yVelocityECEF_);
    } else {
        return false;
    }
    
    // Deserialize zVelocityECEF_
    if (offset + sizeof(zVelocityECEF_) <= data.size()) {
        std::memcpy(&zVelocityECEF_, &data[offset], sizeof(zVelocityECEF_));
        offset += sizeof(zVelocityECEF_);
    } else {
        return false;
    }
    
    // Deserialize xPositionECEF_
    if (offset + sizeof(xPositionECEF_) <= data.size()) {
        std::memcpy(&xPositionECEF_, &data[offset], sizeof(xPositionECEF_));
        offset += sizeof(xPositionECEF_);
    } else {
        return false;
    }
    
    // Deserialize yPositionECEF_
    if (offset + sizeof(yPositionECEF_) <= data.size()) {
        std::memcpy(&yPositionECEF_, &data[offset], sizeof(yPositionECEF_));
        offset += sizeof(yPositionECEF_);
    } else {
        return false;
    }
    
    // Deserialize zPositionECEF_
    if (offset + sizeof(zPositionECEF_) <= data.size()) {
        std::memcpy(&zPositionECEF_, &data[offset], sizeof(zPositionECEF_));
        offset += sizeof(zPositionECEF_);
    } else {
        return false;
    }
    
    // Deserialize originalUpdateTime_
    if (offset + sizeof(originalUpdateTime_) <= data.size()) {
        std::memcpy(&originalUpdateTime_, &data[offset], sizeof(originalUpdateTime_));
        offset += sizeof(originalUpdateTime_);
    } else {
        return false;
    }
    
    // Deserialize updateTime_
    if (offset + sizeof(updateTime_) <= data.size()) {
        std::memcpy(&updateTime_, &data[offset], sizeof(updateTime_));
        offset += sizeof(updateTime_);
    } else {
        return false;
    }
    
    // Deserialize firstHopSentTime_
    if (offset + sizeof(firstHopSentTime_) <= data.size()) {
        std::memcpy(&firstHopSentTime_, &data[offset], sizeof(firstHopSentTime_));
        offset += sizeof(firstHopSentTime_);
    } else {
        return false;
    }
    
    return true;
}

std::size_t ExtrapTrackData::getSerializedSize() const noexcept {
    std::size_t size = 0U;
    
    size += sizeof(trackId_);  // int32_t
    size += sizeof(xVelocityECEF_);  // double
    size += sizeof(yVelocityECEF_);  // double
    size += sizeof(zVelocityECEF_);  // double
    size += sizeof(xPositionECEF_);  // double
    size += sizeof(yPositionECEF_);  // double
    size += sizeof(zPositionECEF_);  // double
    size += sizeof(originalUpdateTime_);  // int64_t
    size += sizeof(updateTime_);  // int64_t
    size += sizeof(firstHopSentTime_);  // int64_t
    
    return size;
}

}  // namespace model
}  // namespace domain
