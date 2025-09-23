#include "adapters/outgoing/ZeroMQExtrapTrackDataAdapter.hpp"
#include "utilities/JsonConfigParser.hpp"
#include <iostream>
#include <sstream>

namespace domain {
namespace adapters {
namespace outgoing {

ZeroMQExtrapTrackDataAdapter::ZeroMQExtrapTrackDataAdapter()
    : context(1), socket(context, ZMQ_PUSH) {
    loadConfiguration();
    socket.connect(endpoint);
    std::cout << "Outgoing adapter bağlandı: " << endpoint << " (protokol: " << protocol << ")" << std::endl;
}

void ZeroMQExtrapTrackDataAdapter::sendExtrapTrackData(const std::vector<domain::model::ExtrapTrackData>& data) {
    for (const auto& item : data) {
        std::string msg = serialize(item);
        zmq::message_t message(msg.begin(), msg.end());
        socket.send(message, zmq::send_flags::none);
    }
}

void ZeroMQExtrapTrackDataAdapter::loadConfiguration() {
    try {
        std::cout << "=== A_HEXAGON CONFIG DEBUG ===" << std::endl;
        std::cout << "Loading config from: src/zmq_messages/ExtrapTrackData.json" << std::endl;
        
        auto config = utilities::JsonConfigParser::parseServiceMetadata("src/zmq_messages/ExtrapTrackData.json");
        
        // x-service-metadata'dan değerleri al
        protocol = config["protocol"];
        std::string address = config["multicast_address"];
        std::string port = config["port"];
        
        std::cout << "Parsed config - Protocol: " << protocol << ", Address: " << address << ", Port: " << port << std::endl;
        
        // Protocol'e göre endpoint ve socket type belirle
        if (protocol == "udp") {
            socketType = ZMQ_RADIO; // UDP multicast için RADIO socket
            endpoint = "udp://" + address + ":" + port; // UDP endpoint
            std::cout << "UDP protokolü kullanılıyor - RADIO socket" << std::endl;
        } else if (protocol == "tcp") {
            socketType = ZMQ_PUSH; // TCP için PUSH socket
            endpoint = "tcp://" + address + ":" + port;
            std::cout << "TCP protokolü kullanılıyor - PUSH socket" << std::endl;
        } else {
            // Varsayılan olarak TCP kullan
            protocol = "tcp";
            socketType = ZMQ_PUSH;
            endpoint = "tcp://localhost:5556";
            std::cout << "Bilinmeyen protokol, TCP varsayılan kullanılıyor" << std::endl;
        }
        
        std::cout << "Final endpoint: " << endpoint << std::endl;
        
        // Mevcut socket'i kapat ve yeni socket oluştur
        socket.close();
        socket = zmq::socket_t(context, socketType);
        
        std::cout << "=== CONFIG LOADED SUCCESSFULLY ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "=== CONFIG ERROR ===" << std::endl;
        std::cerr << "Konfigürasyon yükleme hatası: " << e.what() << std::endl;
        std::cerr << "Varsayılan konfigürasyon kullanılıyor..." << std::endl;
        
        // Varsayılan değerler
        protocol = "tcp";
        socketType = ZMQ_PUSH;
        endpoint = "tcp://localhost:5556";
        socket.close();
        socket = zmq::socket_t(context, socketType);
        
        std::cout << "Fallback endpoint: " << endpoint << std::endl;
    }
}

std::string ZeroMQExtrapTrackDataAdapter::serialize(const domain::model::ExtrapTrackData& data) {
    std::ostringstream json;
    json << "{"
         << "\"trackId\":" << data.getTrackId() << ","
         << "\"xVelocityECEF\":" << data.getXVelocityECEF() << ","
         << "\"yVelocityECEF\":" << data.getYVelocityECEF() << ","
         << "\"zVelocityECEF\":" << data.getZVelocityECEF() << ","
         << "\"xPositionECEF\":" << data.getXPositionECEF() << ","
         << "\"yPositionECEF\":" << data.getYPositionECEF() << ","
         << "\"zPositionECEF\":" << data.getZPositionECEF() << ","
         << "\"originalUpdateTime\":" << data.getOriginalUpdateTime() << ","
         << "\"updateTime\":" << data.getUpdateTime() << ","
         << "\"firstHopSentTime\":" << data.getFirstHopSentTime()
         << "}";
    return json.str();
}

}
}
}
