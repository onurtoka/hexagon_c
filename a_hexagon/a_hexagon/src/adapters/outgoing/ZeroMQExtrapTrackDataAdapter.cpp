#include "ZeroMQExtrapTrackDataAdapter.hpp"
#include "../../utilities/JsonConfigParser.hpp"
#include <iostream>
#include <sstream>

namespace domain {
namespace adapters {
namespace outgoing {

ZeroMQExtrapTrackDataAdapter::ZeroMQExtrapTrackDataAdapter()
    : context(1), socket(context, ZMQ_RADIO), group_name_("ExtrapTrackData") {
    loadConfiguration();
    socket.connect(endpoint);
    std::cout << "Outgoing adapter bağlandı: " << endpoint << " (protokol: " << protocol << ", grup: " << group_name_ << ")" << std::endl;
}

void ZeroMQExtrapTrackDataAdapter::sendExtrapTrackData(const std::vector<domain::model::ExtrapTrackData>& data) {
    for (const auto& item : data) {
        // Modelin kendi binary serialization'ını kullan
        std::vector<uint8_t> binaryData = item.serialize();
        zmq::message_t message(binaryData.begin(), binaryData.end());
        
        // RADIO socket için group belirleme
        message.set_group(group_name_.c_str());
        
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
            socketType = ZMQ_RADIO; // TCP için de RADIO socket (RADIO/DISH pattern)
            endpoint = "tcp://" + address + ":" + port;
        } else {
            // Varsayılan olarak UDP kullan
            protocol = "udp";
            socketType = ZMQ_RADIO;
            endpoint = "udp://239.255.0.1:7779";
        }
        
        // Mevcut socket'i kapat ve yeni socket oluştur
        socket.close();
        socket = zmq::socket_t(context, socketType);
        
    } catch (const std::exception& e) {
        std::cerr << "Konfigürasyon yükleme hatası: " << e.what() << std::endl;
        std::cerr << "Varsayılan konfigürasyon kullanılıyor..." << std::endl;
        
        // Varsayılan değerler
        protocol = "udp";
        socketType = ZMQ_RADIO;
        endpoint = "udp://239.255.0.1:7779";
        socket.close();
        socket = zmq::socket_t(context, socketType);
    }
}

// Tek veri gönderimi için yeni metod
void ZeroMQExtrapTrackDataAdapter::sendExtrapTrackData(const domain::model::ExtrapTrackData& data) {
    std::vector<domain::model::ExtrapTrackData> singleData = {data};
    sendExtrapTrackData(singleData); // Mevcut vector metodunu kullan
}

}
}
}
