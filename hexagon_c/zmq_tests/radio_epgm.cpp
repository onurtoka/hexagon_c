#define ZMQ_BUILD_DRAFT_API
#include "../zmq-monitor/third_party/cppzmq/zmq.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <../zmq-monitor/third_party/cppzmq/zmq.h>  // C API'yi kullanabilmek için ekliyoruz

int main() {
    const std::string endpoint = "epgm://eth0;239.255.0.1:7780";
    const std::string group = "TRACK_DATA_EPGM";
    
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_RADIO);

    std::cout << "EPGM RADIO Publisher (Simple Text): Connecting to " << endpoint << "..." << std::endl;
    
    try {
        publisher.connect(endpoint);
        std::cout << "EPGM RADIO Publisher: Successfully connected. Starting to send messages..." << std::endl;
    } catch (const zmq::error_t& e) {
        std::cerr << "EPGM RADIO Publisher Error: " << e.what() << std::endl;
        return 1;
    }

    // C API üzerinden soket üzerine grup bilgisi set ediliyor
    // C API'yi kullanabilmek için 'publisher' objesini alıyoruz
    void *pub_socket = publisher;
    
    // C API ile ZMQ_GROUP opsiyonunu ayarla
    int group_size = group.size();
    if (zmq_setsockopt(pub_socket, ZMQ_GROUP, group.c_str(), group_size) != 0) {
        std::cerr << "Error setting group: " << zmq_strerror(zmq_errno()) << std::endl;
        return 1;
    }

    long trackId = 1100;
    while (true) {
        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto epoch_time = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        ss << "TrackID: " << trackId << ", Time: " << epoch_time;
        std::string payload_str = ss.str();

        zmq::message_t message(payload_str.data(), payload_str.size());

        // Mesajı gönderiyoruz
        publisher.send(message, zmq::send_flags::none);

        std::cout << "Sent: '" << payload_str << "' to group '" << group << "'" << std::endl;
        trackId++;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
