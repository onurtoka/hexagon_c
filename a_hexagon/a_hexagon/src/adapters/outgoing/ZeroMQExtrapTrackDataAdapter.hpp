#ifndef ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H
#define ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H
#include <zmq.hpp>

// ZMQ_RADIO draft API fallback tanımı (IntelliSense için)
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif

#include <string>
#include <vector>
#include "../../domain/model/ExtrapTrackData.hpp"
#include "../../domain/ports/outgoing/TrackDataOutgoingPort.hpp"
namespace domain {
namespace adapters {
namespace outgoing {
class ZeroMQExtrapTrackDataAdapter : public domain::ports::outgoing::ExtrapTrackDataOutgoingPort {
public: 
    ZeroMQExtrapTrackDataAdapter(); // Parametresiz constructor
    void sendExtrapTrackData(const std::vector<domain::model::ExtrapTrackData>& data) override;
    void sendExtrapTrackData(const domain::model::ExtrapTrackData& data) override; // Tek veri için
private: 
    zmq::context_t context;
    zmq::socket_t socket;  
    std::string protocol;
    std::string endpoint;
    std::string group_name_;  // ZeroMQ grup adı (UDP RADIO için)
    int socketType;
    
    void loadConfiguration();
};
}
}
}
#endif
