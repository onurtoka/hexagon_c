// Derleyiciye DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH dahil) aktive etmesini söylüyoruz.
#define ZMQ_BUILD_DRAFT_API

#include <zmq.h> // Temel C başlık dosyasını kullanıyoruz
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <chrono>
#include <sstream>

// Hata kontrolünü basitleştirmek için bir yardımcı fonksiyon
void check_rc(int rc, const std::string& context_msg) {
    if (rc != 0) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

int main() {
    // Dinlenecek (Giriş) ve Yayınlanacak (Çıkış) endpoint'ler ve gruplar
    const char* input_endpoint = "udp://239.1.1.1:9001";
    const char* input_group = "SOURCE_DATA";

    const char* output_endpoint = "udp://239.1.1.1:9002"; // Farklı bir port
    const char* output_group = "PROCESSED_DATA";     // Farklı bir grup

    void* context = zmq_ctx_new();
    assert(context);

    // --- Giriş Soketi (DISH) ---
    void* dish_socket = zmq_socket(context, ZMQ_DISH);
    assert(dish_socket);
    std::cout << "Uygulama 2 (İşleyici) Başlatıldı." << std::endl;
    std::cout << "Dinleme Adresi: " << input_endpoint << ", Grup: " << input_group << std::endl;
    check_rc(zmq_bind(dish_socket, input_endpoint), "DISH zmq_bind");
    check_rc(zmq_join(dish_socket, input_group), "DISH zmq_join");

    // --- Çıkış Soketi (RADIO) ---
    void* radio_socket = zmq_socket(context, ZMQ_RADIO);
    assert(radio_socket);
    std::cout << "Yayın Adresi: " << output_endpoint << ", Grup: " << output_group << std::endl;
    check_rc(zmq_connect(radio_socket, output_endpoint), "RADIO zmq_connect");

    std::cout << "\nUygulama 1'den mesajlar bekleniyor..." << std::endl;

    while (true) {
        // --- Mesaj Alma ---
        zmq_msg_t received_msg;
        zmq_msg_init(&received_msg);
        int bytes = zmq_msg_recv(&received_msg, dish_socket, 0);
        if (bytes == -1) {
            zmq_msg_close(&received_msg);
            continue;
        }

        // --- GECİKME HESAPLAMA (Bölüm 1) ---
        // 1. Mesajı alır almaz mevcut zamanı kaydediyoruz.
        auto receive_time = std::chrono::high_resolution_clock::now();

        // 2. Gelen mesajı C++ string'ine çeviriyoruz.
        std::string received_payload(static_cast<char*>(zmq_msg_data(&received_msg)), zmq_msg_size(&received_msg));
        zmq_msg_close(&received_msg);

        // 3. Mesajı '|' karakterinden ayırarak orijinal metni ve gönderim zamanını buluyoruz.
        size_t separator_pos = received_payload.find_last_of('|');
        if (separator_pos == std::string::npos) {
            std::cerr << "Hata: Alınan mesajda zaman damgası ayıracı ('|') bulunamadı." << std::endl;
            continue;
        }

        std::string original_data = received_payload.substr(0, separator_pos);
        long long send_timestamp_ns = std::stoll(received_payload.substr(separator_pos + 1));

        // 4. Gecikmeyi hesaplıyoruz.
        auto receive_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(receive_time.time_since_epoch()).count();
        long long latency_ns = receive_time_ns - send_timestamp_ns;
        
        std::cout << "   -> Orijinal mesaj alındı: \"" << original_data << "\"" << std::endl;
        std::cout << "      Gecikme (App1 -> App2): " << latency_ns / 1000.0 << " mikrosaniye" << std::endl;
        // --- HESAPLAMA SONU ---

        // --- Mesajı Yeniden Yayınlama ---
        // 1. Yeni mesaj için yeni bir zaman damgası alıyoruz.
        auto forward_time = std::chrono::high_resolution_clock::now();
        auto forward_ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(forward_time.time_since_epoch()).count();

        // 2. Orijinal mesajın sonuna bu yeni zaman damgasını ekliyoruz.
        std::stringstream ss;
        ss << original_data << "|" << forward_ns_since_epoch;
        std::string processed_data = ss.str();

        zmq_msg_t processed_msg;
        check_rc(zmq_msg_init_size(&processed_msg, processed_data.size()), "zmq_msg_init_size (processed)");
        memcpy(zmq_msg_data(&processed_msg), processed_data.c_str(), processed_data.size());

        check_rc(zmq_msg_set_group(&processed_msg, output_group), "zmq_msg_set_group (processed)");

        int sent_bytes = zmq_msg_send(&processed_msg, radio_socket, 0);
        assert(sent_bytes == (int)processed_data.size());
        
        std::cout << "   <- İşlenmiş mesaj gönderildi: \"" << processed_data << "\"" << std::endl;

        zmq_msg_close(&processed_msg);
    }

    zmq_close(dish_socket);
    zmq_close(radio_socket);
    zmq_ctx_term(context);

    return 0;
}
