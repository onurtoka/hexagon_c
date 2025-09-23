// #pragma once = Header guard modern syntax'ı
// Bu header dosyasının aynı compilation unit'te sadece bir kez include edilmesini sağlar
// Geleneksel #ifndef/#define/#endif yerine kullanılan modern yaklaşım
#pragma once

// DOMAIN MODEL INCLUDES - İş mantığı modelleri
#include "../../model/DelayCalcTrackData.hpp"    // Incoming DelayCalcTrackData from B_hexagon

// STL (Standard Template Library) INCLUDES
#include <memory>      // Smart pointer'lar için (std::shared_ptr, std::unique_ptr)

/**
 * @interface IDataReceiver
 * @brief Hexagonal Architecture Primary Port for incoming track data
 * 
 * Receives DelayCalcTrackData from B_hexagon via ZeroMQ DISH socket.
 * This is the primary port interface that domain logic implements.
 */
class IDataReceiver {
public:
    virtual ~IDataReceiver() = default;

    /**
     * @brief Process incoming DelayCalcTrackData from B_hexagon
     * @param data DelayCalcTrackData containing delay calculations from B_hexagon
     */
    virtual void onDataReceived(const domain::model::DelayCalcTrackData& data) = 0;
};

// HEADER GUARD NOTES:
// Bu dosya #pragma once ile korunmuş durumda
// Aynı compilation unit'te birden fazla include edilse bile
// Sadece bir kez işlenir, compilation time optimize edilir

// HEXAGONAL ARCHITECTURE FLOW:
// 1. External System (hexagon_b) -> ZeroMQ message gönderir
// 2. ZeroMQTrackDataSubscriber (Primary Adapter) -> message'ı alır
// 3. TrackDataSubmission interface (Primary Port) -> domain'e giriş noktası
// 4. TrackDataProcessor (Domain Logic) -> iş mantığını uygular
// 5. TrackDataRepository & TrackDataPublisher (Secondary Ports) -> dış dünyaya çıkış