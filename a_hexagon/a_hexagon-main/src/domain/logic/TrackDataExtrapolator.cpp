#include "TrackDataExtrapolator.h"
#include <chrono>
namespace domain {
namespace logic {
using std::vector;
using namespace domain::model;
TrackDataExtrapolator::TrackDataExtrapolator(domain::ports::outgoing::ExtrapTrackDataOutgoingPort* outgoingPort) 
    : outgoingPort_(outgoingPort) {
}
vector<ExtrapTrackData> TrackDataExtrapolator::extrapolateTo200Hz(const TrackData& trackData) {
    vector<ExtrapTrackData> result;
    int numSamples = 25;
    double dt = 0.005;
    for (int i = 0; i < numSamples; ++i) {
        ExtrapTrackData extrap;
        double t = i * dt;
        extrap.setTrackId(trackData.getTrackId());
        extrap.setXVelocityECEF(trackData.getXVelocityECEF());
        extrap.setYVelocityECEF(trackData.getYVelocityECEF());
        extrap.setZVelocityECEF(trackData.getZVelocityECEF());
        extrap.setXPositionECEF(trackData.getXPositionECEF() + trackData.getXVelocityECEF() * t);
        extrap.setYPositionECEF(trackData.getYPositionECEF() + trackData.getYVelocityECEF() * t);
        extrap.setZPositionECEF(trackData.getZPositionECEF() + trackData.getZVelocityECEF() * t);
        extrap.setUpdateTime(trackData.getOriginalUpdateTime() + static_cast<long>(t * 1000000)); // mikrosaniye
        extrap.setOriginalUpdateTime(trackData.getOriginalUpdateTime());
        auto now = std::chrono::high_resolution_clock::now();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        extrap.setFirstHopSentTime(micros);
        result.push_back(extrap);
    }
    return result;
}
void TrackDataExtrapolator::processAndForwardTrackData(const TrackData& trackData) {
    vector<ExtrapTrackData> extrapolated = extrapolateTo200Hz(trackData);
    if (outgoingPort_) {
        outgoingPort_->sendExtrapTrackData(extrapolated);
    }
}
}
}