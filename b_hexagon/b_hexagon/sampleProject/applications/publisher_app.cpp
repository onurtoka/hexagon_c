#include "messaging/messaging.h" // Ortak kütüphanemiz
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // 1. Kontrat yöneticisini oluştur ve kontratları yükle
    ContractManager contracts;
    if (!contracts.load_contracts_from_directory("./zmq_messages")) {
        std::cerr << "Kontratlar yüklenemedi, program sonlandırılıyor." << std::endl;
        return 1;
    }

    // 2. ZeroMQ'yu kur
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5555");
    
    // 3. Gönderilecek "ProcessedTrackData" mesajını nlohmann::json olarak oluştur
    json track_msg = {
        {"trackId", 202},
        {"xVelocityECEF", 50.0},
        {"yVelocityECEF", 75.1},
        {"zVelocityECEF", -10.2},
        {"xPositionECEF", 887766.1},
        {"yPositionECEF", 554433.2},
        {"zPositionECEF", 12345.0},
        {"updateTime", 1723623745000000000L} // Örnek bir nanosaniye zaman damgası
    };
    
    // Alıcının bağlanabilmesi için kısa bir süre bekle
    std::cout << "Yayıncı başlatıldı, 1 saniye bekleniyor..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 4. Ortak kütüphaneyi kullanarak mesajı doğrula, serileştir ve gönder!
    //    Konu başlığını "ProcessedTrackData" olarak değiştiriyoruz.
    std::cout << "Kontrata uygun 'ProcessedTrackData' mesaji gonderiliyor..." << std::endl;
    if (messaging::serialize_and_send(publisher, "ProcessedTrackData", track_msg, contracts)) {
        std::cout << "Mesaj başarıyla gönderildi." << std::endl;
    } else {
        std::cout << "Mesaj gönderilemedi (Doğrulama hatası olabilir, logları kontrol edin)." << std::endl;
    }

    return 0;
}