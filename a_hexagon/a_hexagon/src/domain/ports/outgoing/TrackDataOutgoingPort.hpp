#ifndef TRACK_DATA_OUTGOING_PORT_H 
#define TRACK_DATA_OUTGOING_PORT_H 
#include "domain/model/ExtrapTrackData.hpp"
#include <vector>
namespace domain {
namespace ports {
namespace outgoing {
class ExtrapTrackDataOutgoingPort {
public: 
    virtual ~ExtrapTrackDataOutgoingPort() = default;
    virtual void sendExtrapTrackData(const std::vector<model::ExtrapTrackData>& data) = 0;
    virtual void sendExtrapTrackData(const model::ExtrapTrackData& data) = 0; // Tek veri için
};
}
}
}
#endif
