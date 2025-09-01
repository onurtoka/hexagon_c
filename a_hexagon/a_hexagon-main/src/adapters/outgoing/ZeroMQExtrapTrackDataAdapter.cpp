#include "ZeroMQExtrapTrackDataAdapter.h"
#include "../../utilities/JsonConfigParser.h"
#include <iostream>
#include <sstream>

namespace domain {
namespace adapters {
namespace outgoing {

ZeroMQExtrapTrackDataAdapter::ZeroMQExtrapTrackDataAdapter()
    : context(1), socket(context, ZMQ_RADIO) {
    loadConfiguration();
    socket.connect(endpoint);
    std::cout << "Outgoing adapter bağlandı: " << endpoint << " (protokol: " << protocol << ")" << std::endl;
}

void ZeroMQExtrapTrackDataAdapter::sendExtrapTrackData(const std::vector<domain::model::ExtrapTrackData>& data) {
    for (const auto& item : data) {
        std::string msg = serialize(item);
        zmq::message_t message(msg.begin(), msg.end());
        
        // RADIO socket için group ayarla
        message.set_group("SOURCE_DATA");
        
        socket.send(message, zmq::send_flags::none);
    }
}

void ZeroMQExtrapTrackDataAdapter::loadConfiguration() {
    try {
        auto config = utilities::JsonConfigParser::parseServiceMetadata("src/zmq_messages/ExtrapTrackData.json");
        
        // x-service-metadata'dan değerleri al
        protocol = config["protocol"];
        std::string address = config["multicast_address"];
        std::string port = config["port"];
        
        // Protocol'e göre endpoint ve socket type belirle
        if (protocol == "udp") {
            socketType = ZMQ_RADIO; // UDP multicast için RADIO socket
            endpoint = "udp://" + address + ":" + port; // UDP endpoint
            std::cout << "UDP protokolü kullanılıyor" << std::endl;
        } else if (protocol == "tcp") {
            socketType = ZMQ_PUSH; // TCP için PUSH socket
            endpoint = "tcp://" + address + ":" + port;
        } else {
            // Varsayılan olarak UDP kullan (RADIO/DISH pattern için)
            protocol = "udp";
            socketType = ZMQ_RADIO;
            endpoint = "udp://239.1.1.1:9001";
        }
        
        // Mevcut socket'i kapat ve yeni socket oluştur
        socket.close();
        socket = zmq::socket_t(context, socketType);
        
    } catch (const std::exception& e) {
        std::cerr << "Konfigürasyon yükleme hatası: " << e.what() << std::endl;
        std::cerr << "Varsayılan konfigürasyon kullanılıyor..." << std::endl;
        
        // Varsayılan değerler (UDP RADIO/DISH pattern)
        protocol = "udp";
        socketType = ZMQ_RADIO;
        endpoint = "udp://239.1.1.1:9001";
        socket.close();
        socket = zmq::socket_t(context, socketType);
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
