#include "domain/logic/TrackDataExtrapolator.hpp"
#include "adapters/outgoing/ZeroMQExtrapTrackDataAdapter.hpp"
#include "domain/model/ExtrapTrackData.hpp"               
#include <iostream>

int main() {
    domain::adapters::outgoing::ZeroMQExtrapTrackDataAdapter outgoingAdapter;
    domain::logic::TrackDataExtrapolator extrapolator(&outgoingAdapter);
    
    std::cout << "ZeroMQ Outgoing adapter hazır. Incoming adapter implementasyonu bekleniyor..." << std::endl;
    
    // TODO: Incoming adapter teknolojisi belirlendikten sonra eklenecek
    // incomingAdapter.listenAndProcess(extrapolator);
    
    return 0;
}
