#define ZMQ_BUILD_DRAFT_API

#include <zmq.hpp>         // C++ ZeroMQ binding
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <optional>

// Yardımcı hata kontrol fonksiyonu
void check_rc(bool condition, const std::string& context_msg) {
    if (!condition) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

int main() {
    const std::string input_endpoint = "udp://239.1.1.1:9001";
    const std::string input_group = "SOURCE_DATA";

    const std::string output_endpoint = "udp://239.1.1.1:9002";
    const std::string output_group = "PROCESSED_DATA";

    zmq::context_t context(1);

    // Giriş soketi (DISH)
    zmq::socket_t dish_socket(context, zmq::socket_type::dish);
    std::cout << "Uygulama 2 (İşleyici) Başlatıldı." << std::endl;
    std::cout << "Dinleme Adresi: " << input_endpoint << ", Grup: " << input_group << std::endl;

    dish_socket.bind(input_endpoint);
    dish_socket.join(input_group.c_str());

    // Çıkış soketi (RADIO)
    zmq::socket_t radio_socket(context, zmq::socket_type::radio);
    std::cout << "Yayın Adresi: " << output_endpoint << ", Grup: " << output_group << std::endl;
    radio_socket.connect(output_endpoint);

    std::cout << "\nUygulama 1'den mesajlar bekleniyor..." << std::endl;

    while (true) {
        zmq::message_t received_msg;
        if (!dish_socket.recv(received_msg, zmq::recv_flags::none)) {
            continue; // Alım başarısızsa geç
        }

        auto receive_time = std::chrono::high_resolution_clock::now();

        std::string received_payload(static_cast<char*>(received_msg.data()), received_msg.size());

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

        // Yeni zaman damgası
        auto forward_time = std::chrono::high_resolution_clock::now();
        auto forward_ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(forward_time.time_since_epoch()).count();

        std::stringstream ss;
        ss << original_data << "|" << forward_ns_since_epoch;
        std::string processed_data = ss.str();

        zmq::message_t processed_msg(processed_data.begin(), processed_data.end());

        processed_msg.set_group(output_group.c_str());
        
        auto send_result = radio_socket.send(processed_msg, zmq::send_flags::none);
        if (!send_result || *send_result != processed_data.size()) {
            std::cerr << "Mesaj gönderilemedi veya boyut uyuşmuyor." << std::endl;
        } else {
            std::cout << "   <- İşlenmiş mesaj gönderildi: \"" << processed_data << "\"" << std::endl;
        }
    }

    return 0;
}


//g++ app2_processor.cpp -o app2 -std=c++17   -I/workspaces/hexagons/zmq_cpp_test/lib/libzmq/include   -L/workspaces/hexagons/zmq_cpp_test/lib/libzmq/build/lib   -lzmq