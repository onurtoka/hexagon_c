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
#include <sstream>

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

int main() {
    // TCP endpoints using different ports
    const char* input_endpoint = "tcp://127.0.0.1:9001";
    const char* output_endpoint = "tcp://127.0.0.1:9002";

    void* context = zmq_ctx_new();
    assert(context);

    // Use SUB socket instead of DISH for compatibility
    void* sub_socket = zmq_socket(context, ZMQ_SUB);
    assert(sub_socket);
    std::cout << "Uygulama 2 (İşleyici) Başlatıldı." << std::endl;
    std::cout << "Dinleme Adresi: " << input_endpoint << std::endl;
    check_rc(zmq_connect(sub_socket, input_endpoint), "SUB zmq_connect");
    check_rc(zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "SOURCE_DATA:", 12), "SUB zmq_setsockopt");

    // Use PUB socket instead of RADIO for compatibility
    void* pub_socket = zmq_socket(context, ZMQ_PUB);
    assert(pub_socket);
    std::cout << "Yayın Adresi: " << output_endpoint << std::endl;
    check_rc(zmq_bind(pub_socket, output_endpoint), "PUB zmq_bind");

    std::cout << "\nUygulama 1'den mesajlar bekleniyor..." << std::endl;

    while (true) {
        // Mesaj alma
        zmq_msg_t received_msg;
        zmq_msg_init(&received_msg);
        int bytes = zmq_msg_recv(&received_msg, sub_socket, 0);
        if (bytes == -1) {
            zmq_msg_close(&received_msg);
            continue;
        }

        // Gecikme hesaplama
        auto receive_time = std::chrono::high_resolution_clock::now();

        std::string received_payload(static_cast<char*>(zmq_msg_data(&received_msg)), zmq_msg_size(&received_msg));
        zmq_msg_close(&received_msg);

        // "SOURCE_DATA:" prefix'ini kaldır
        if (received_payload.find("SOURCE_DATA:") == 0) {
            received_payload = received_payload.substr(12);
        }

        size_t separator_pos = received_payload.find_last_of('|');
        if (separator_pos == std::string::npos) {
            std::cerr << "Hata: Alınan mesajda zaman damgası ayıracı ('|') bulunamadı." << std::endl;
            continue;
        }

        std::string original_data = received_payload.substr(0, separator_pos);
        long long send_timestamp_ns = std::stoll(received_payload.substr(separator_pos + 1));

        auto receive_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(receive_time.time_since_epoch()).count();
        long long latency_ns = receive_time_ns - send_timestamp_ns;
        
        std::cout << "   -> Orijinal mesaj alındı: \"" << original_data << "\"" << std::endl;
        std::cout << "      Gecikme (App1 -> App2): " << latency_ns / 1000.0 << " mikrosaniye" << std::endl;

        // Mesajı yeniden yayınlama
        auto forward_time = std::chrono::high_resolution_clock::now();
        auto forward_ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(forward_time.time_since_epoch()).count();

        std::stringstream ss;
        ss << "PROCESSED_DATA:" << original_data << "|" << forward_ns_since_epoch;
        std::string processed_data = ss.str();

        zmq_msg_t processed_msg;
        check_rc(zmq_msg_init_size(&processed_msg, processed_data.size()), "zmq_msg_init_size (processed)");
        memcpy(zmq_msg_data(&processed_msg), processed_data.c_str(), processed_data.size());

        int sent_bytes = zmq_msg_send(&processed_msg, pub_socket, 0);
        assert(sent_bytes == (int)processed_data.size());
        
        std::cout << "   <- İşlenmiş mesaj gönderildi: \"" << processed_data << "\"" << std::endl;

        zmq_msg_close(&processed_msg);
    }

    zmq_close(sub_socket);
    zmq_close(pub_socket);
    zmq_ctx_term(context);

    return 0;
}
