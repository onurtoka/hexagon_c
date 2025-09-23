#include <iostream>
#include <chrono>
#include <csignal>
#include <atomic>
#include <memory>
#include <thread>

#include "../domain/model/DelayCalcTrackData.hpp"
#include "../domain/model/FinalCalcTrackData.hpp"

// Enable ZeroMQ DRAFT API for RADIO/DISH - must be defined before zmq.hpp
#define ZMQ_BUILD_DRAFT_API 1
#include "zmq.hpp"

// Using declarations for convenience
using domain::model::DelayCalcTrackData;
using domain::model::FinalCalcTrackData;

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\nShutdown signal received. Exiting..." << std::endl;
    running.store(false);
}

class ZeroMQDishTrackDataSubscriber {
private:
    zmq::context_t context_;
    zmq::socket_t socket_;
    
public:
    explicit ZeroMQDishTrackDataSubscriber(const std::string& endpoint) 
        : context_(1), socket_(context_, ZMQ_DISH) {
        socket_.bind(endpoint);  // DISH socket should bind, not connect
        socket_.join("DelayCalcTrackData");
        std::cout << "Bound to " << endpoint << " and joined group 'DelayCalcTrackData'" << std::endl;
    }
    
    bool receiveDelayCalcTrackData(DelayCalcTrackData& trackData) {
        zmq::message_t message;
        auto result = socket_.recv(message, zmq::recv_flags::dontwait);
        
        if (result.has_value() && message.size() > 0) {
            try {
                const uint8_t* data = static_cast<const uint8_t*>(message.data());
                size_t dataSize = message.size();
                
                std::vector<uint8_t> dataVector(data, data + dataSize);
                
                if (trackData.deserialize(dataVector)) {
                    std::cout << "Successfully received and deserialized DelayCalcTrackData" << std::endl;
                    std::cout << "Track ID: " << trackData.getTrackId() 
                              << ", Update Time: " << trackData.getUpdateTime() << std::endl;
                    return true;
                } else {
                    std::cerr << "Failed to deserialize DelayCalcTrackData" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception during deserialization: " << e.what() << std::endl;
            }
        }
        
        return false;
    }
};

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== C_Hexagon - Final Track Data Processing System ===" << std::endl;
        std::cout << "Architecture: Hexagonal (Ports & Adapters)" << std::endl;
        std::cout << "Messaging: ZeroMQ RADIO/DISH UDP multicast" << std::endl;
        std::cout << "Input: DelayCalcTrackData from B_hexagon via DISH socket" << std::endl;
        std::cout << "Group: DelayCalcTrackData" << std::endl;
        std::cout << "Endpoint: udp://239.1.1.5:9595" << std::endl;
        std::cout << "Models: Binary serialization with domain::model namespace" << std::endl;
        std::cout << "========================================================" << std::endl;

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        ZeroMQDishTrackDataSubscriber subscriber("udp://239.1.1.5:9595");
        DelayCalcTrackData delayCalcData;
        
        std::cout << "🚀 Starting DelayCalcTrackData reception from B_hexagon..." << std::endl;
        std::cout << "📡 Listening on udp://239.1.1.5:9595 (DelayCalcTrackData group)" << std::endl;
        std::cout << "⏹️  Press Ctrl+C to stop..." << std::endl;

        // Main loop - keep application running
        while (running.load()) {
            if (subscriber.receiveDelayCalcTrackData(delayCalcData)) {
                // Process received DelayCalcTrackData
                FinalCalcTrackData finalData;
                
                // Copy basic track data
                finalData.setTrackId(delayCalcData.getTrackId());
                finalData.setXPositionECEF(delayCalcData.getXPositionECEF());
                finalData.setYPositionECEF(delayCalcData.getYPositionECEF());
                finalData.setZPositionECEF(delayCalcData.getZPositionECEF());
                finalData.setXVelocityECEF(delayCalcData.getXVelocityECEF());
                finalData.setYVelocityECEF(delayCalcData.getYVelocityECEF());
                finalData.setZVelocityECEF(delayCalcData.getZVelocityECEF());
                
                // Set timing information
                auto currentTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                
                finalData.setThirdHopSentTime(currentTime);
                finalData.setSecondHopSentTime(delayCalcData.getSecondHopSentTime());
                finalData.setFirstHopDelayTime(delayCalcData.getFirstHopDelayTime());
                finalData.setSecondHopDelayTime(currentTime - delayCalcData.getSecondHopSentTime());
                finalData.setTotalDelayTime(currentTime - (delayCalcData.getOriginalUpdateTime() * 1000));
                
                std::cout << "Created FinalCalcTrackData for Track ID: " << finalData.getTrackId() << std::endl
                          << " FirstHopDelayTime: " << finalData.getFirstHopDelayTime() << " microseconds" << std::endl
                          << " SecondHopDelayTime: " << finalData.getSecondHopDelayTime() << " microseconds" << std::endl
                          << " Total ZeroMQ Delay: " << finalData.getFirstHopDelayTime() + finalData.getSecondHopDelayTime() << " microseconds" << std::endl
                          << " Total Delay: " << finalData.getTotalDelayTime() << " microseconds" << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }

        std::cout << "✅ C_hexagon shutdown complete." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ FATAL ERROR in C_hexagon main: " << e.what() << std::endl;
        return 1;
    }
}