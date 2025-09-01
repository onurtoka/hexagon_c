#ifndef TRACK_DATA_INCOMING_PORT_H 
#define TRACK_DATA_INCOMING_PORT_H 
#include "../../model/TrackData.h"
namespace domain {
namespace ports {
namespace incoming {
class TrackDataIncomingPort {
public: 
    virtual ~TrackDataIncomingPort() = default;
    virtual void processAndForwardTrackData(const model::TrackData& trackData) = 0;
};
}
}
}
#endif
