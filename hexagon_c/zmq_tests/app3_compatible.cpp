// ZMQ Draft API manual definition for Windows
#ifndef ZMQ_BUILD_DRAFT_API
#define ZMQ_BUILD_DRAFT_API
#endif

#include <zmq.h>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <chrono>

// Manual ZMQ Draft API definitions if not available
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif

#ifndef ZMQ_DISH
#define ZMQ_DISH 15
#endif

// Hata kontrolünü basitleştirmek için bir yardımcı fonksiyon
void check_rc(int rc, const std::string& context_msg) {
    if (rc != 0) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

// Gelen mesajı ayrıştırıp gecikmeyi hesaplayan ve yazdıran bir yardımcı fonksiyon
void process_message(zmq_msg_t* msg, const std::string& source_name, const std::string& prefix) {
    auto receive_time = std::chrono::high_resolution_clock::now();

    std::string payload(static_cast<char*>(zmq_msg_data(msg)), zmq_msg_size(msg));

    // Prefix'i kaldır
    if (payload.find(prefix) == 0) {
        payload = payload.substr(prefix.length());
    }

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

    auto receive_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(receive_time.time_since_epoch()).count();
    long long latency_ns = receive_time_ns - send_timestamp_ns;

    std::cout << "\n---[ " << source_name << " MESAJI ALINDI ]---" << std::endl;
    std::cout << "  Orijinal Mesaj: \"" << original_data << "\"" << std::endl;
    std::cout << "  Gecikme: " << latency_ns / 1000.0 << " mikrosaniye" << std::endl;
}

int main() {
    // TCP endpoints
    const char* source_endpoint = "tcp://127.0.0.1:9001";
    const char* processed_endpoint = "tcp://127.0.0.1:9002";

    void* context = zmq_ctx_new();
    assert(context);

    // Use SUB sockets instead of DISH for compatibility
    void* source_sub = zmq_socket(context, ZMQ_SUB);
    assert(source_sub);
    std::cout << "Uygulama 3 (Dinleyici) Başlatıldı." << std::endl;
    std::cout << "Kaynak dinleme adresi: " << source_endpoint << std::endl;
    check_rc(zmq_connect(source_sub, source_endpoint), "Connect Source SUB");
    check_rc(zmq_setsockopt(source_sub, ZMQ_SUBSCRIBE, "SOURCE_DATA:", 12), "Subscribe Source");

    void* processed_sub = zmq_socket(context, ZMQ_SUB);
    assert(processed_sub);
    std::cout << "İşlenmiş veri dinleme adresi: " << processed_endpoint << std::endl;
    check_rc(zmq_connect(processed_sub, processed_endpoint), "Connect Processed SUB");
    check_rc(zmq_setsockopt(processed_sub, ZMQ_SUBSCRIBE, "PROCESSED_DATA:", 15), "Subscribe Processed");

    // Poller kurulumu
    zmq_pollitem_t items[] = {
        { source_sub,    0, ZMQ_POLLIN, 0 }, 
        { processed_sub, 0, ZMQ_POLLIN, 0 }
    };

    std::cout << "\nTüm yayıncılardan mesajlar bekleniyor..." << std::endl;

    while (true) {
        int rc = zmq_poll(items, 2, -1);
        assert(rc >= 0);

        // Kaynak soketinde mesaj var mı?
        if (items[0].revents & ZMQ_POLLIN) {
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            zmq_msg_recv(&msg, source_sub, 0);
            process_message(&msg, "KAYNAK (App1 -> App3)", "SOURCE_DATA:");
            zmq_msg_close(&msg);
        }

        // İşlenmiş veri soketinde mesaj var mı?
        if (items[1].revents & ZMQ_POLLIN) {
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            zmq_msg_recv(&msg, processed_sub, 0);
            process_message(&msg, "İŞLENMİŞ (App2 -> App3)", "PROCESSED_DATA:");
            zmq_msg_close(&msg);
        }
    }

    zmq_close(source_sub);
    zmq_close(processed_sub);
    zmq_ctx_term(context);

    return 0;
}
