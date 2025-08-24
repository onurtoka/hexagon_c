#include "../zmq-monitor/third_party/cppzmq/zmq.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream> // Metin oluşturmak için

int main() {
    // DEĞİŞİKLİK: Adres protokolünü 'udp' ve port numarasını '7778' olarak güncelledik.
    const std::string endpoint = "udp://239.255.0.1:7778";
    
    // ZeroMQ context ve PUB soketini oluşturuyoruz.
    zmq::context_t context(1);
    zmq::socket_t publisher(context, zmq::socket_type::pub);

    std::cout << "UDP Publisher (Simple Text): Binding to " << endpoint << "..." << std::endl;
    
    try {
        // Soketi UDP multicast adresine 'bind' ediyoruz.
        publisher.bind(endpoint);
        std::cout << "UDP Publisher: Successfully bound. Starting to send messages..." << std::endl;
    } catch (const zmq::error_t& e) {
        std::cerr << "UDP Publisher Error: " << e.what() << std::endl;
        return 1;
    }

    long trackId = 500; // Farklı bir başlangıç ID'si kullanalım
    while (true) {
        // 1. std::stringstream kullanarak mesaj metnini oluşturuyoruz.
        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto epoch_time = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        ss << "TrackID: " << trackId << ", Time: " << epoch_time;
        std::string payload_str = ss.str();

        // 2. Metni bir ZMQ mesajına dönüştürüyoruz.
        zmq::message_t message(payload_str.data(), payload_str.size());

        // 3. Tek çerçeveli mesajı gönderiyoruz.
        publisher.send(message, zmq::send_flags::none);

        std::cout << "Sent: " << payload_str << std::endl;
        trackId++;
        
        // Her saniye bir mesaj gönder.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}