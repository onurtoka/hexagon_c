#pragma once

/**
 * @brief Output domain value object aggregating original TrackData fields plus derived delay metrics.
 * 
 * All timing values are in microseconds.
 */
class DelayCalculatedTrackData final {
public:
    int trackId{0};                ///< Unique identifier for the TrackData
    double xVelocityECEF{0.0};     ///< X velocity in ECEF coordinates (m/s)
    double yVelocityECEF{0.0};     ///< Y velocity in ECEF coordinates (m/s)
    double zVelocityECEF{0.0};     ///< Z velocity in ECEF coordinates (m/s)
    double xPositionECEF{0.0};     ///< X position in ECEF coordinates (meters)
    double yPositionECEF{0.0};     ///< Y position in ECEF coordinates (meters)
    double zPositionECEF{0.0};     ///< Z position in ECEF coordinates (meters)
    long updateTime{0L};           ///< Time of the last update (microseconds)
    long originalUpdateTime{0L};   ///< Original update time (microseconds) - matches DelayCalcTrackData.json schema
    long firstHopSentTime{0L};     ///< Time when first hop was sent (microseconds)
    long firstHopDelayTime{0L};    ///< Calculated delay for first hop (microseconds)
    long secondHopSentTime{0L};    ///< Calculated time for second hop (microseconds)

    /**
     * @brief Default constructor. Initializes all fields to zero.
     * MISRA Rule 15-3-4: Guaranteed not to throw exceptions.
     */
    DelayCalculatedTrackData() noexcept = default;

    /**
     * @brief Parameterized constructor.
     * @param id TrackData ID
     * @param xv X velocity (ECEF)
     * @param yv Y velocity (ECEF)
     * @param zv Z velocity (ECEF)
     * @param xp X position (ECEF)
     * @param yp Y position (ECEF)
     * @param zp Z position (ECEF)
     * @param ut Update time (microseconds)
     * @param out Original update time (microseconds)
     * @param fhst First hop sent time (microseconds)
     * @param fhd First hop delay time (microseconds)
     * @param shst Second hop sent time (microseconds)
     * MISRA Rule 15-3-4: Constructor may throw std::bad_alloc.
     */
    DelayCalculatedTrackData(
        int id, double xv, double yv, double zv, double xp, double yp, double zp,
        long ut, long out, long fhst, long fhd, long shst) noexcept
        : trackId(id), xVelocityECEF(xv), yVelocityECEF(yv), zVelocityECEF(zv),
          xPositionECEF(xp), yPositionECEF(yp), zPositionECEF(zp),
          updateTime(ut), originalUpdateTime(out), firstHopSentTime(fhst),
          firstHopDelayTime(fhd), secondHopSentTime(shst) {}

    /**
     * @brief Copy constructor.
     * MISRA Rule 15-3-4: Guaranteed not to throw exceptions for POD-like data.
     */
    DelayCalculatedTrackData(const DelayCalculatedTrackData& other) noexcept = default;

    /**
     * @brief Move constructor.
     * MISRA Rule 15-3-4: Move operations are noexcept for simple data types.
     */
    DelayCalculatedTrackData(DelayCalculatedTrackData&& other) noexcept = default;

    /**
     * @brief Assignment operator.
     * MISRA Rule 15-3-4: Assignment is noexcept for POD-like data.
     */
    DelayCalculatedTrackData& operator=(const DelayCalculatedTrackData& other) noexcept = default;

    /**
     * @brief Move assignment operator.
     * MISRA Rule 15-3-4: Move assignment is noexcept for simple data types.
     */
    DelayCalculatedTrackData& operator=(DelayCalculatedTrackData&& other) noexcept = default;

    /**
     * @brief Destructor.
     * MISRA Rule 15-3-4: Trivial destructor guaranteed not to throw.
     */
    ~DelayCalculatedTrackData() noexcept = default;

    /**
     * @brief Validates if all required fields have reasonable values.
     * @return true if the track data appears valid, false otherwise
     * MISRA Rule 7-1-2: Const member function as it doesn't modify state.
     */
    [[nodiscard]] bool isValid() const noexcept;
};
