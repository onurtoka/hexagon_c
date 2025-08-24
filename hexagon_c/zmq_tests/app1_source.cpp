// Derleyiciye DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH dahil) aktive etmesini söylüyoruz.
#define ZMQ_BUILD_DRAFT_API

#include <zmq.h> // Temel C başlık dosyasını kullanıyoruz
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <cassert>
#include <cstring>

// Hata kontrolünü basitleştirmek için bir yardımcı fonksiyon
void check_rc(int rc, const std::string& context_msg) {
    if (rc != 0) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

int main() {
    const char* endpoint = "udp://239.1.1.1:9001";
    const char* group = "SOURCE_DATA";

    void* context = zmq_ctx_new();
    assert(context);

    void* radio_socket = zmq_socket(context, ZMQ_RADIO);
    assert(radio_socket);

    std::cout << "Uygulama 1 (Kaynak) Başlatıldı." << std::endl;
    std::cout << "Yayın Adresi: " << endpoint << ", Grup: " << group << std::endl;
    
    int rc = zmq_connect(radio_socket, endpoint);
    check_rc(rc, "zmq_connect");
    
    std::cout << "Göndermek için bir mesaj yazıp Enter'a basın ('exit' yazarak çıkabilirsiniz)..." << std::endl;

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            break;
        }

        // --- GECİKME ÖLÇÜMÜ İÇİN DEĞİŞİKLİK ---
        // 1. Yüksek hassasiyetli mevcut zamanı alıyoruz (nanosaniye).
        auto send_time = std::chrono::high_resolution_clock::now();
        auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(send_time.time_since_epoch()).count();

        // 2. Mesajın sonuna zaman damgasını ekliyoruz.
        std::stringstream ss;
        ss << line << "|" << ns_since_epoch;
        std::string payload_str = ss.str();
        // --- DEĞİŞİKLİK SONU ---

        zmq_msg_t message;
        rc = zmq_msg_init_size(&message, payload_str.size());
        check_rc(rc, "zmq_msg_init_size");
        memcpy(zmq_msg_data(&message), payload_str.c_str(), payload_str.size());

        rc = zmq_msg_set_group(&message, group);
        check_rc(rc, "zmq_msg_set_group");

        rc = zmq_msg_send(&message, radio_socket, 0);
        assert(rc == (int)payload_str.size());

        std::cout << "   -> Mesaj gönderildi: \"" << payload_str << "\"" << std::endl;

        zmq_msg_close(&message);
    }

    std::cout << "Uygulama kapatılıyor..." << std::endl;
    zmq_close(radio_socket);
    zmq_ctx_term(context);

    return 0;
}
