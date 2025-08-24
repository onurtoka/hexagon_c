// Derleyiciye DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH dahil) aktive etmesini söylüyoruz.
//#define ZMQ_BUILD_DRAFT_API

#include <zmq.h> // Temel C başlık dosyasını kullanıyoruz
#include <cstring> // strcmp ve strlen için
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

// Hata kontrolünü basitleştirmek için bir yardımcı fonksiyon
void check_rc(int rc, const std::string& context_msg) {
    if (rc != 0) {
        std::cerr << context_msg << " - ZMQ Error: " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

int main() {
    // Dinlenecek endpoint ve grup
    const char* endpoint_udp = "udp://239.255.0.1:7779";
    const char* group_udp = "TRACK_DATA_UDP";

    void* context = zmq_ctx_new();
    assert(context);

    // UDP için DISH soketi oluşturma ve ayarlama
    void* sub_udp = zmq_socket(context, ZMQ_DISH);
    assert(sub_udp);
    std::cout << "DISH UDP Subscriber: Binding to " << endpoint_udp << "..." << std::endl;
    check_rc(zmq_bind(sub_udp, endpoint_udp), "UDP Bind");
    check_rc(zmq_join(sub_udp, group_udp), "UDP Join Group");
    std::cout << "  - Joined group '" << group_udp << "'" << std::endl;


    std::cout << "\nWaiting for messages from RADIO UDP publisher..." << std::endl;

    while (true) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        
        // Mesajı al
        int bytes = zmq_msg_recv(&msg, sub_udp, 0);
        if (bytes != -1) {
            // Gelen mesajın grubunu al
            const char* group_str = zmq_msg_group(&msg);
            // Mesaj içeriğini string'e çevir
            std::string data_str(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
            
            std::cout << "\n---[ MESSAGE RECEIVED on RADIO/DISH UDP ]---" << std::endl;
            if (group_str) {
                std::cout << "  Group: " << group_str << std::endl;
            }
            std::cout << "  Message: " << data_str << std::endl;
        }

        zmq_msg_close(&msg);
    }

    zmq_close(sub_udp);
    zmq_ctx_term(context);

    return 0;
}