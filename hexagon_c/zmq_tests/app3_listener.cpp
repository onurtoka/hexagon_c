// Derleyiciye DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH dahil) aktive etmesini söylüyoruz.
#define ZMQ_BUILD_DRAFT_API

#include <zmq.h> // Temel C başlık dosyasını kullanıyoruz
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <chrono>

// Hata kontrolünü basitleştirmek için bir yardımcı fonksiyon
void check_rc(int rc, const std::string& context_msg) {
    if (rc != 0) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

// Gelen mesajı ayrıştırıp gecikmeyi hesaplayan ve yazdıran bir yardımcı fonksiyon
void process_message(zmq_msg_t* msg, const std::string& source_name) {
    // Mesajı alır almaz mevcut zamanı kaydediyoruz.
    auto receive_time = std::chrono::high_resolution_clock::now();

    // Mesaj içeriğini C++ string'ine çeviriyoruz.
    std::string payload(static_cast<char*>(zmq_msg_data(msg)), zmq_msg_size(msg));

    // Mesajı '|' karakterinden ayırarak orijinal metni ve gönderim zamanını buluyoruz.
    size_t separator_pos = payload.find_last_of('|');
    if (separator_pos == std::string::npos) {
        std::cerr << "[" << source_name << "] Hata: Alınan mesajda zaman damgası ayıracı ('|') bulunamadı." << std::endl;
        return;
    }

    std::string original_data = payload.substr(0, separator_pos);
    long long send_timestamp_ns = 0;
    try {
        send_timestamp_ns = std::stoll(payload.substr(separator_pos + 1));
    } catch (const std::invalid_argument& e) {
        std::cerr << "[" << source_name << "] Hata: Geçersiz zaman damgası formatı." << std::endl;
        return;
    }


    // Gecikmeyi hesaplıyoruz.
    auto receive_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(receive_time.time_since_epoch()).count();
    long long latency_ns = receive_time_ns - send_timestamp_ns;

    // Sonuçları ekrana yazdırıyoruz.
    std::cout << "\n---[ " << source_name << " MESAJI ALINDI ]---" << std::endl;
    std::cout << "  Orijinal Mesaj: \"" << original_data << "\"" << std::endl;
    std::cout << "  Gecikme: " << latency_ns / 1000.0 << " mikrosaniye" << std::endl;
}


int main() {
    // Dinlenecek endpoint'ler ve gruplar
    const char* source_endpoint = "udp://239.1.1.1:9001";
    const char* source_group = "SOURCE_DATA";

    const char* processed_endpoint = "udp://239.1.1.1:9002";
    const char* processed_group = "PROCESSED_DATA";

    void* context = zmq_ctx_new();
    assert(context);

    // --- Soket 1: Kaynak Veriyi Dinlemek İçin (App1'den) ---
    void* source_dish = zmq_socket(context, ZMQ_DISH);
    assert(source_dish);
    std::cout << "Uygulama 3 (Dinleyici) Başlatıldı." << std::endl;
    std::cout << "Kaynak dinleme adresi: " << source_endpoint << std::endl;
    check_rc(zmq_bind(source_dish, source_endpoint), "Bind Source DISH");
    check_rc(zmq_join(source_dish, source_group), "Join Source Group");

    // --- Soket 2: İşlenmiş Veriyi Dinlemek İçin (App2'den) ---
    void* processed_dish = zmq_socket(context, ZMQ_DISH);
    assert(processed_dish);
    std::cout << "İşlenmiş veri dinleme adresi: " << processed_endpoint << std::endl;
    check_rc(zmq_bind(processed_dish, processed_endpoint), "Bind Processed DISH");
    check_rc(zmq_join(processed_dish, processed_group), "Join Processed Group");

    // --- Poller Kurulumu ---
    zmq_pollitem_t items[] = {
        { source_dish,    0, ZMQ_POLLIN, 0 }, 
        { processed_dish, 0, ZMQ_POLLIN, 0 }
    };

    std::cout << "\nTüm yayıncılardan mesajlar bekleniyor..." << std::endl;

    while (true) {
        int rc = zmq_poll(items, 2, -1);
        assert(rc >= 0);

        // Kaynak soketinde mesaj var mı?
        if (items[0].revents & ZMQ_POLLIN) {
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            zmq_msg_recv(&msg, source_dish, 0);
            process_message(&msg, "KAYNAK (App1 -> App3)");
            zmq_msg_close(&msg);
        }

        // İşlenmiş veri soketinde mesaj var mı?
        if (items[1].revents & ZMQ_POLLIN) {
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            zmq_msg_recv(&msg, processed_dish, 0);
            process_message(&msg, "İŞLENMİŞ (App2 -> App3)");
            zmq_msg_close(&msg);
        }
    }

    zmq_close(source_dish);
    zmq_close(processed_dish);
    zmq_ctx_term(context);

    return 0;
}
