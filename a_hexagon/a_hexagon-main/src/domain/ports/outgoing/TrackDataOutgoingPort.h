#ifndef TRACK_DATA_OUTGOING_PORT_H 
#define TRACK_DATA_OUTGOING_PORT_H 
#include "../../model/ExtrapTrackData.h"
#include <vector>
namespace domain {
namespace ports {
namespace outgoing {
class ExtrapTrackDataOutgoingPort {
public: 
    virtual ~ExtrapTrackDataOutgoingPort() = default;
    virtual void sendExtrapTrackData(const std::vector<model::ExtrapTrackData>& data) = 0;
};
}
}
}
#endif
