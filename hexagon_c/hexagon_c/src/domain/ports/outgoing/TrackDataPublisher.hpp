#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include "../../model/FinalCalcDelayData.hpp"
#include <vector>
#include <memory>

namespace hat::domain::ports::outgoing {

/**
 * SECONDARY PORT INTERFACE - Hexagonal Architecture'da Secondary Port
 * 
 * HEXAGONAL ARCHITECTURE AÇIKLAMASI:
 * - Secondary Port = Domain'in dış dünyayı kullanması için tanımladığı interface
 * - Bu port DOMAIN LOGIC tarafından çağrılır (outgoing direction)
 * - ADAPTER'lar bu port'u implement eder (ZeroMQTrackDataPublisher)
 * - İşlenen verileri ZeroMQ üzerinden dışarı gönderen arayüz
 */
class TrackDataPublisher {
public:
    virtual ~TrackDataPublisher() = default;

    /**
     * DELAY CALC TRACK DATA PUBLISHING - Ana veri yayınlama fonksiyonu
     */
    virtual bool publishDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * FINAL CALC DELAY DATA PUBLISHING - Final veri yayınlama fonksiyonu
     */
    virtual bool publishFinalCalcDelayData(const model::FinalCalcDelayData& data) = 0;

    /**
     * PUBLISHER STATUS CHECK - Yayınlayıcı durum kontrolü
     */
    virtual bool isPublisherActive() const = 0;

}; // class TrackDataPublisher

} // namespace hat::domain::ports::outgoing
