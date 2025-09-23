#pragma once

// MISRA C++ 2023 compliant includes
#include <string>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <cstring>

/**
 * @brief Bir izin çok adımlı (multi-hop) gecikme verilerinin istatistiksel analizini (ortalama, standart sapma, min/max) içerir.
 * Auto-generated from TrackStatics.json
 * MISRA C++ 2023 compliant implementation
 * Direction: outgoing
 */
class TrackStatics final {
public:
    // Network configuration constants
    static constexpr const char* MULTICAST_ADDRESS = "239.1.1.5";
    static constexpr int PORT = 9599;
    
    // ZeroMQ RADIO socket configuration (outgoing)
    static constexpr const char* ZMQ_SOCKET_TYPE = "RADIO";
    static constexpr bool IS_PUBLISHER = true;

    // MISRA C++ 2023 compliant constructors
    explicit TrackStatics() noexcept;
    
    // Copy constructor
    TrackStatics(const TrackStatics& other) = default;
    
    // Move constructor
    TrackStatics(TrackStatics&& other) noexcept = default;
    
    // Copy assignment operator
    TrackStatics& operator=(const TrackStatics& other) = default;
    
    // Move assignment operator
    TrackStatics& operator=(TrackStatics&& other) noexcept = default;
    
    // Destructor
    ~TrackStatics() = default;
    
    // Getters and Setters
    int32_t getTrackId() const noexcept;
    void setTrackId(const int32_t& value);

    double getFirstHopDelayDataMean() const noexcept;
    void setFirstHopDelayDataMean(const double& value);

    double getFirstHopDelayDataStd() const noexcept;
    void setFirstHopDelayDataStd(const double& value);

    double getFirstHopDelayDataMin() const noexcept;
    void setFirstHopDelayDataMin(const double& value);

    double getFirstHopDelayDataMax() const noexcept;
    void setFirstHopDelayDataMax(const double& value);

    double getSecondHopDelayDataMean() const noexcept;
    void setSecondHopDelayDataMean(const double& value);

    double getSecondHopDelayDataStd() const noexcept;
    void setSecondHopDelayDataStd(const double& value);

    double getSecondHopDelayDataMin() const noexcept;
    void setSecondHopDelayDataMin(const double& value);

    double getSecondHopDelayDataMax() const noexcept;
    void setSecondHopDelayDataMax(const double& value);

    double getTotalHopDelayDataMean() const noexcept;
    void setTotalHopDelayDataMean(const double& value);

    double getTotalHopDelayDataStd() const noexcept;
    void setTotalHopDelayDataStd(const double& value);

    double getTotalHopDelayDataMin() const noexcept;
    void setTotalHopDelayDataMin(const double& value);

    double getTotalHopDelayDataMax() const noexcept;
    void setTotalHopDelayDataMax(const double& value);

    int64_t getUpdateTime() const noexcept;
    void setUpdateTime(const int64_t& value);

    // Validation - MISRA compliant
    [[nodiscard]] bool isValid() const noexcept;

    // Binary Serialization - MISRA compliant
    [[nodiscard]] std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data) noexcept;
    [[nodiscard]] std::size_t getSerializedSize() const noexcept;

private:
    // Member variables
    /// İz için benzersiz tam sayı kimliği
    int32_t trackId_;
    /// İlk atlama gecikme verisinin ortalaması.
    double firstHopDelayDataMean_;
    /// İlk atlama gecikme verisinin standart sapması.
    double firstHopDelayDataStd_;
    /// İlk atlama gecikme verisinin minimum değeri.
    double firstHopDelayDataMin_;
    /// İlk atlama gecikme verisinin maksimum değeri.
    double firstHopDelayDataMax_;
    /// İkinci atlama gecikme verisinin ortalaması.
    double secondHopDelayDataMean_;
    /// İkinci atlama gecikme verisinin standart sapması.
    double secondHopDelayDataStd_;
    /// İkinci atlama gecikme verisinin minimum değeri.
    double secondHopDelayDataMin_;
    /// İkinci atlama gecikme verisinin maksimum değeri.
    double secondHopDelayDataMax_;
    /// Toplam gecikme verisinin ortalaması.
    double totalHopDelayDataMean_;
    /// Toplam gecikme verisinin standart sapması.
    double totalHopDelayDataStd_;
    /// Toplam gecikme verisinin minimum değeri.
    double totalHopDelayDataMin_;
    /// Toplam gecikme verisinin maksimum değeri.
    double totalHopDelayDataMax_;
    /// Son güncelleme zamanı (mikrosaniye)
    int64_t updateTime_;

    // Validation functions - MISRA compliant
    void validateTrackId(int32_t value) const;
    void validateFirstHopDelayDataMean(double value) const;
    void validateFirstHopDelayDataStd(double value) const;
    void validateFirstHopDelayDataMin(double value) const;
    void validateFirstHopDelayDataMax(double value) const;
    void validateSecondHopDelayDataMean(double value) const;
    void validateSecondHopDelayDataStd(double value) const;
    void validateSecondHopDelayDataMin(double value) const;
    void validateSecondHopDelayDataMax(double value) const;
    void validateTotalHopDelayDataMean(double value) const;
    void validateTotalHopDelayDataStd(double value) const;
    void validateTotalHopDelayDataMin(double value) const;
    void validateTotalHopDelayDataMax(double value) const;
    void validateUpdateTime(int64_t value) const;
};
