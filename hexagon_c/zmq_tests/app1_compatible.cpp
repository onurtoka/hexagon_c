// ZMQ Draft API manual definition for Windows
#ifndef ZMQ_BUILD_DRAFT_API
#define ZMQ_BUILD_DRAFT_API
#endif

#include <zmq.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <cassert>
#include <cstring>

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

// Manual implementation of zmq_msg_set_group if not available
int manual_zmq_msg_set_group(zmq_msg_t *msg, const char *group) {
    // This is a placeholder - we'll use regular PUB socket instead
    return 0;
}

int main() {
    const char* endpoint = "tcp://127.0.0.1:9001";

    void* context = zmq_ctx_new();
    assert(context);

    // Use PUB socket instead of RADIO for compatibility
    void* pub_socket = zmq_socket(context, ZMQ_PUB);
    assert(pub_socket);

    std::cout << "Uygulama 1 (Kaynak) Başlatıldı." << std::endl;
    std::cout << "Yayın Adresi: " << endpoint << std::endl;
    
    int rc = zmq_bind(pub_socket, endpoint);
    check_rc(rc, "zmq_bind");
    
    // Give time for subscribers to connect
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    std::cout << "Göndermek için bir mesaj yazıp Enter'a basın ('exit' yazarak çıkabilirsiniz)..." << std::endl;

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            break;
        }

        // Zaman damgası ekleme
        auto send_time = std::chrono::high_resolution_clock::now();
        auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(send_time.time_since_epoch()).count();

        std::stringstream ss;
        ss << "SOURCE_DATA:" << line << "|" << ns_since_epoch;
        std::string payload_str = ss.str();

        zmq_msg_t message;
        rc = zmq_msg_init_size(&message, payload_str.size());
        check_rc(rc, "zmq_msg_init_size");
        memcpy(zmq_msg_data(&message), payload_str.c_str(), payload_str.size());

        rc = zmq_msg_send(&message, pub_socket, 0);
        assert(rc == (int)payload_str.size());

        std::cout << "   -> Mesaj gönderildi: \"" << payload_str << "\"" << std::endl;

        zmq_msg_close(&message);
    }

    std::cout << "Uygulama kapatılıyor..." << std::endl;
    zmq_close(pub_socket);
    zmq_ctx_term(context);

    return 0;
}
