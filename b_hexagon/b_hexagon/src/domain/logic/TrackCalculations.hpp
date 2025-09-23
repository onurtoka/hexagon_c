#pragma once
#include "domain/model/ExtrapTrackData.hpp"   // Input data structure
#include <chrono>                        // Time utilities for nanosecond precision

/**
 * @brief Pure stateless calculation utilities for `ExtrapTrackData` timing metrics.
 *
 * @dot
 * digraph timing_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *   
 *   input [label="Input: ExtrapTrackData\n+ firstHopSentTime", fillcolor=lightblue];
 *   receive [label="System captures\nReceive Time", fillcolor=yellow];
 *   calc1 [label="Calculate First Hop Delay\n= receiveTime - firstHopSentTime", fillcolor=orange];
 *   calc2 [label="Calculate Second Hop Sent Time\n= current system time", fillcolor=orange];
 *   output [label="Output: DelayCalculated\nTrackData", fillcolor=lightgreen];
 *   
 *   input -> receive;
 *   receive -> calc1;
 *   calc1 -> calc2;
 *   calc2 -> output;
 *   
 *   {rank=same; calc1; calc2;}
 * }
 * @enddot
 *
 * Centralizes arithmetic logic to keep the application service (orchestrator) lightweight and
 * focused on workflow. Facilitates targeted unit testing of business rules. All returned / consumed
 * time values are expressed as nanoseconds since epoch, represented as `double` for convenience.
 */
class TrackCalculations {
public:
    /**
     * @brief Calculates first hop delay time in nanoseconds.
     * @param trackData The input ExtrapTrackData
     * @param receiveTimeNs The time when data was received (nanoseconds)
     * @return Delay time in nanoseconds (receive_time - first_hop_sent_time)
     */
    static long calculateFirstHopDelayTime(const ExtrapTrackData& trackData, long receiveTimeNs) {
        return receiveTimeNs - trackData.firstHopSentTime;
    }

    /**
     * @brief Calculates second hop sent time as the current system time in nanoseconds.
     * @return Second hop sent time (nanoseconds)
     */
    static long calculateSecondHopSentTime() {
        auto now = std::chrono::system_clock::now();
        return static_cast<long>(std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
    }
};
