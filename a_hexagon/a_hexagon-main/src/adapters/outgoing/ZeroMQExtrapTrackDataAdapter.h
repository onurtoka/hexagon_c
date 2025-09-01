#ifndef ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H
#define ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H
#include <zmq.hpp>

// ZMQ_RADIO draft API fallback tanımı (IntelliSense için)
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif

#include <string>
#include <vector>
#include "../../domain/model/ExtrapTrackData.h"
#include "../../domain/ports/outgoing/TrackDataOutgoingPort.h"
namespace domain {
namespace adapters {
namespace outgoing {
class ZeroMQExtrapTrackDataAdapter : public domain::ports::outgoing::ExtrapTrackDataOutgoingPort {
public: 
    ZeroMQExtrapTrackDataAdapter(); // Parametresiz constructor
    void sendExtrapTrackData(const std::vector<domain::model::ExtrapTrackData>& data) override;
private: 
    zmq::context_t context;
    zmq::socket_t socket;  
    std::string protocol;
    std::string endpoint;
    int socketType;
    
    void loadConfiguration();
    std::string serialize(const domain::model::ExtrapTrackData& data);
};
}
}
}
#endif
