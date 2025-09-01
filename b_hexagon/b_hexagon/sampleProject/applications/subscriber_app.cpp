#include "messaging/messaging.h" // Ortak kütüphanemiz
#include <iostream>

int main() {
    // 1. Kontrat yöneticisini oluştur ve kontratları yükle
    ContractManager contracts;
    if (!contracts.load_contracts_from_directory("./zmq_messages")) {
        std::cerr << "Kontratlar yüklenemedi, program sonlandırılıyor." << std::endl;
        return 1;
    }

    // 2. ZeroMQ'yu kur
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5555");
    subscriber.set(zmq::sockopt::subscribe, ""); // Tüm konulara abone ol

    std::cout << "Abone başlatıldı, mesajlar bekleniyor..." << std::endl;

    while (true) {
        // 3. Ortak kütüphaneyi kullanarak mesajı al, çöz ve doğrula!
        //    Bu fonksiyon gelen mesajın tipini kendisi anlar.
        auto received_message = messaging::receive_and_deserialize(subscriber, contracts);

        if (received_message.has_value()) {
            std::cout << "\n--- Kontrattan Geçen Geçerli Mesaj Alındı ---" << std::endl;
            // Gelen veriyi güzel formatlanmış JSON olarak yazdır
            std::cout << received_message->dump(2) << std::endl;
        }
    }

    return 0;
}