
// Derleyiciye DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH dahil) aktive etmesini söylüyoruz.
//#define ZMQ_BUILD_DRAFT_API

//#include <../zmq-monitor/third_party/libzmq/include/zmq.h> // C++ (zmq.hpp) yerine temel C başlık dosyasını kullanıyoruz
#include <zmq.h> // C++ (zmq.hpp) yerine temel C başlık dosyasını kullanıyoruz
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <cstring>
#include <cassert> // Hata kontrolü için assert kullanacağız

// Hata kontrolünü basitleştirmek için bir yardımcı fonksiyon
void check_rc(int rc) {
    if (rc != 0) {
        std::cerr << "ZMQ Error: " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

int main() {
    const char* endpoint = "udp://239.255.0.1:7779";
    const char* group = "TRACK_DATA_UDP";
    
    // 1. Context oluşturma
    void* context = zmq_ctx_new();
    assert(context);

    // 2. RADIO soketi oluşturma
    void* publisher = zmq_socket(context, ZMQ_RADIO);
    assert(publisher);

    std::cout << "UDP RADIO Publisher (C API): Connecting to " << endpoint << "..." << std::endl;
    
    // 3. Soketi bağlama
    int rc = zmq_connect(publisher, endpoint);
    if (rc != 0) {
        std::cerr << "UDP RADIO Publisher Error: " << zmq_strerror(zmq_errno()) << std::endl;
        return 1;
    }
    std::cout << "UDP RADIO Publisher: Successfully connected. Starting to send messages..." << std::endl;

    long trackId = 800;
    while (true) {
        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto epoch_time = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        ss << "TrackID: " << trackId << ", Time: " << epoch_time;
        std::string payload_str = ss.str();

        // 4. Gönderilecek mesajı oluşturma
        zmq_msg_t message;
        rc = zmq_msg_init_size(&message, payload_str.size());
        check_rc(rc);
        memcpy(zmq_msg_data(&message), payload_str.c_str(), payload_str.size());

        // 5. Mesajın grubunu ayarlama
        rc = zmq_msg_set_group(&message, group);
        check_rc(rc);

        // 6. Mesajı gönderme
        rc = zmq_msg_send(&message, publisher, 0);
        assert(rc == (int)payload_str.size());

        std::cout << "Sent: '" << payload_str << "' to group '" << group << "'" << std::endl;
        trackId++;

        // 7. Mesajı kapatma (belleği serbest bırakma)
        zmq_msg_close(&message);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 8. Kaynakları temizleme
    zmq_close(publisher);
    zmq_ctx_term(context);

    return 0;
}