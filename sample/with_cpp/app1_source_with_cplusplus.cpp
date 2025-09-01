#define ZMQ_BUILD_DRAFT_API

#include <zmq.hpp>         // C++ binding
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <cassert>

// Yardımcı hata kontrol fonksiyonu
void check_rc(bool condition, const std::string& context_msg) {
    if (!condition) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": "
                  << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

int main() {
    const std::string endpoint = "udp://239.1.1.1:9001";
    const std::string group = "SOURCE_DATA";

    zmq::context_t context(1);

    zmq::socket_t radio_socket(context, zmq::socket_type::radio);
    std::cout << "Uygulama 1 (Kaynak) Başlatıldı." << std::endl;
    std::cout << "Yayın Adresi: " << endpoint << ", Grup: " << group << std::endl;

    radio_socket.connect(endpoint);

    std::cout << "Göndermek için bir mesaj yazıp Enter'a basın ('exit' yazarak çıkabilirsiniz)..." << std::endl;

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            break;
        }

        // --- GECİKME ÖLÇÜMÜ ---
        auto send_time = std::chrono::high_resolution_clock::now();
        auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
            send_time.time_since_epoch()).count();

        std::stringstream ss;
        ss << line << "|" << ns_since_epoch;
        std::string payload_str = ss.str();
        // --- DEĞİŞİKLİK SONU ---

        zmq::message_t message(payload_str.begin(), payload_str.end());

        message.set_group(group.c_str());

        auto send_result = radio_socket.send(message, zmq::send_flags::none);
        check_rc(send_result && *send_result == payload_str.size(), "Mesaj gönderilemedi");

        std::cout << "   -> Mesaj gönderildi: \"" << payload_str << "\"" << std::endl;
    }

    std::cout << "Uygulama kapatılıyor..." << std::endl;
    // socket ve context RAII sayesinde otomatik kapanır

    return 0;
}
