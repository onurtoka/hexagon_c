#include "../zmq-monitor/third_party/cppzmq/zmq.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>

int main() {
    // DEĞİŞİKLİK: 'lo' yerine doğrudan loopback IP'sini kullanıyoruz.
    const std::string endpoint = "epgm://127.0.0.1;239.255.0.1:7777";
    
    zmq::context_t context(1);
    zmq::socket_t publisher(context, zmq::socket_type::pub);

    std::cout << "EPGM Publisher (Simple Text): Binding to " << endpoint << "..." << std::endl;
    
    try {
        publisher.bind(endpoint);
        std::cout << "EPGM Publisher: Successfully bound." << std::endl;
        std::cout << "EPGM Publisher: Waiting 1 second for subscribers to connect..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "EPGM Publisher: Starting to send messages..." << std::endl;
    } catch (const zmq::error_t& e) {
        std::cerr << "EPGM Publisher Error: " << e.what() << std::endl;
        return 1;
    }

    long trackId = 202;
    while (true) {
        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto epoch_time = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        ss << "TrackID: " << trackId << ", Time: " << epoch_time;
        std::string payload_str = ss.str();

        zmq::message_t message(payload_str.data(), payload_str.size());
        publisher.send(message, zmq::send_flags::none);

        std::cout << "Sent: " << payload_str << std::endl;
        trackId++;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}