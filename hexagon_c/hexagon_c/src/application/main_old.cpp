#include <iostream>
#include <chrono>
#include <csignal>
#include <atomic>
#include <memory>
#include <thread>

#include "../domain/logic/TrackDataProcessor.hpp"
#include "../domain/ports/incoming/TrackDataSubmission.hpp"
#include "../domain/ports/outgoing/TrackDataPublisher.hpp"
#include "../adapters/incoming/zeromq/ZeroMQDishTrackDataSubscriber.hpp"

// Using declarations for convenience
using domain::model::DelayCalcTrackData;

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\nShutdown signal received. Exiting..." << std::endl;
    running.store(false);
}

// Simple ZeroMQ DISH adapter for receiving DelayCalcTrackData  
class ZeroMQDataHandler {
private:
    IDataReceiver* dataReceiver_;
    
public:
    explicit ZeroMQDataHandler(IDataReceiver* receiver) : dataReceiver_(receiver) {}
    
    void startReceiving() {
        // Mock implementation - will be replaced with actual ZeroMQ DISH socket
        std::cout << "🔄 C_hexagon ready to receive DelayCalcTrackData from B_hexagon" << std::endl;
        std::cout << "📡 Listening on udp://239.1.1.1:9002 (DISH socket)" << std::endl;
        
        while (running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // Mock: Would receive and deserialize DelayCalcTrackData here
            // dataReceiver_->onDataReceived(deserializedData);
        }
    }
};

// Simple ZeroMQ RADIO adapter for sending FinalCalcTrackData
class ZeroMQDataSender : public IDataSender {
public:
    void sendData(const domain::model::FinalCalcTrackData& data) override {
        // Mock implementation - would use binary serialization here
        std::cout << "📤 Sending FinalCalcTrackData[" << data.getTrackId() 
                  << "] via RADIO socket to external systems" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== C_Hexagon - Final Track Data Processing System ===" << std::endl;
        std::cout << "Architecture: Hexagonal (Ports & Adapters)" << std::endl;
        std::cout << "Messaging: ZeroMQ RADIO/DISH UDP multicast" << std::endl;
        std::cout << "Models: Binary serialization with domain::model namespace" << std::endl;

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        // Create outgoing adapter
        auto dataSender = std::make_unique<ZeroMQDataSender>();
        
        // Create domain service
        auto calculator = std::make_unique<FinalCalculatorService>(std::move(dataSender));
        
        // Create incoming adapter
        ZeroMQDataHandler dataHandler(calculator.get());

        std::cout << "✓ System initialized - Ready to process DelayCalcTrackData" << std::endl;
        
        // Start receiving (would use actual ZeroMQ in production)
        dataHandler.startReceiving();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }
}