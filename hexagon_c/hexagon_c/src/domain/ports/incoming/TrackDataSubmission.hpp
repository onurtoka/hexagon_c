// #pragma once = Header guard modern syntax'ı
// Bu header dosyasının aynı compilation unit'te sadece bir kez include edilmesini sağlar
// Geleneksel #ifndef/#define/#endif yerine kullanılan modern yaklaşım
#pragma once

// DOMAIN MODEL INCLUDES - İş mantığı modelleri
// ../../ = iki seviye yukarı çık (ports -> domain -> src)
// model/ = domain model'lerin bulunduğu dizin
#include "../../model/DelayCalcTrackData.hpp"    // Gecikme hesaplama track verisi
#include "../../model/FinalCalcDelayData.hpp"    // Final gecikme hesaplama verisi

// STL (Standard Template Library) INCLUDES
#include <memory>      // Smart pointer'lar için (std::shared_ptr, std::unique_ptr)
#include <vector>      // Dynamic array için (std::vector<T>)

// NAMESPACE DEFINITION - Hat projesinin domain katmanı namespace'i
// :: = namespace separator (scope resolution operator)
// Nested namespace: hat -> domain -> ports -> incoming
namespace hat::domain::ports::incoming {

/**
 * PRIMARY PORT INTERFACE - Hexagonal Architecture'da Primary Port
 * 
 * HEXAGONAL ARCHITECTURE AÇIKLAMASI:
 * - Primary Port = Uygulamanın dış dünyaya sunduğu interface (API)
 * - Bu port DOMAIN LOGIC tarafından implement edilir
 * - ADAPTER'lar bu port'u çağırır (dependency inversion)
 * - ZeroMQ üzerinden gelen track data'ları sistem içine alan arayüz
 * 
 * DESIGN PATTERN: Port & Adapter Pattern
 * - Port = Interface definition (bu dosya)
 * - Adapter = External system integration (ZeroMQTrackDataSubscriber)
 * - Domain Logic = Business rules implementation (TrackDataProcessor)
 */
class TrackDataSubmission {
public:    // Public section - dışarıdan erişilebilir members
    
    // VIRTUAL DESTRUCTOR - Polymorphism için kritik
    // virtual = bu fonksiyon derived class'larda override edilebilir
    // ~ = destructor operator (object yok edilirken çağrılır)
    // = default = compiler'ın default implementation'ını kullan
    // 
    // NEDEN VIRTUAL DESTRUCTOR GEREKLİ:
    // - Base class pointer ile derived object silinirken
    // - Derived class'ın destructor'ının çağrılması için
    // - Memory leak'leri önlemek için
    virtual ~TrackDataSubmission() = default;

    /**
     * CORE BUSINESS METHOD - Ana iş mantığı fonksiyonu
     * DelayCalcTrackData verisi işleme alır
     * 
     * PURE VIRTUAL FUNCTION (= 0):
     * - Bu class'da implementation yok, abstract class yapıyor
     * - Derived class'larda mutlaka implement edilmeli
     * - Interface contract'ı tanımlar
     * 
     * @param data Gelen track data
     * @return İşlem sonucu (başarılı/başarısız)
     */
    // virtual = polymorphic behavior (runtime'da hangi implementation çağrılacağı belirlenir)
    // bool = return type, true/false değer döner
    // const model::DelayCalcTrackData& = referans parametresi
    //   - const = fonksiyon içinde parametre değiştirilemez (immutable)
    //   - & = reference, kopyalama yapmaz (performance optimization)
    //   - model::DelayCalcTrackData = domain model type
    // = 0 = pure virtual function, implementation yok
    virtual bool submitDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * SYSTEM STATUS CHECK METHOD - Sistem durumu kontrolü
     * Sistem veri almaya hazır mı kontrol eder
     * 
     * CONST MEMBER FUNCTION:
     * - const = bu fonksiyon object'in state'ini değiştirmez
     * - Read-only operation garantisi
     * - Thread-safety için önemli ipucu
     * - Compiler optimization'a yardım eder
     * 
     * @return Sistem veri almaya hazır mı?
     */
    // const = member function modifier, object state'ini değiştirmez
    // Bu fonksiyon sadece okuma yapar, yazma yapmaz
    virtual bool isReadyToReceive() const = 0;

    // CLASS DESIGN NOTES:
    // 
    // 1. INTERFACE SEGREGATION PRINCIPLE (ISP):
    //    - Sadece gerekli metodlar var
    //    - Client'lar kullanmadıkları interface'lere depend etmez
    // 
    // 2. DEPENDENCY INVERSION PRINCIPLE (DIP):
    //    - High-level modules (Domain Logic) low-level modules'a (Adapters) depend etmez
    //    - Her ikisi de abstraction'lara (bu interface) depend eder
    // 
    // 3. SINGLE RESPONSIBILITY PRINCIPLE (SRP):
    //    - Sadece track data submission sorumluluğu
    //    - Başka sorumluluklar yok
    // 
    // 4. OPEN/CLOSED PRINCIPLE (OCP):
    //    - Extension'a açık (yeni implementation'lar eklenebilir)
    //    - Modification'a kapalı (mevcut interface değişmez)

}; // class TrackDataSubmission sonu

// NAMESPACE CLOSING - Hat projesinin domain katmanı namespace'i kapanışı
} // namespace hat::domain::ports::incoming

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