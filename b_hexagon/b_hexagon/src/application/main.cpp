/**
 * @file main.cpp
 * @brief Application entry point demonstrating Hexagonal Architecture assembly
 */

#include "domain/logic/CalculatorService.hpp"
#include "domain/ports/incoming/IDataReceiver.hpp"
#include "domain/ports/outgoing/IDataSender.hpp"
#include "adapters/incoming/ZeroMQDataHandler.hpp"
#include "adapters/outgoing/ZeroMQDataSender.hpp"
#include "common/Logger.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @class ProcessTrackUseCase
 * @brief Domain use case for processing track data
 */
class ProcessTrackUseCase final : public IDataReceiver {
private:
    std::unique_ptr<CalculatorService> calculatorService_;
    std::unique_ptr<IDataSender> dataSender_;

public:
    ProcessTrackUseCase(std::unique_ptr<CalculatorService> calculatorService,
                       std::unique_ptr<IDataSender> dataSender)
        : calculatorService_(std::move(calculatorService)),
          dataSender_(std::move(dataSender)) {
        Logger::info("ProcessTrackUseCase initialized with CalculatorService and DataSender");
    }

    void onDataReceived(const TrackData& data) override {
        Logger::debug("Received track data: ID=", data.trackId, 
                     ", Position=(", data.xPositionECEF, ",", data.yPositionECEF, ",", data.zPositionECEF, ")");
        
        if (!data.isValid()) {
            Logger::warn("Invalid track data received: ID=", data.trackId);
            return;
        }
        
        try {
            // Process the track data through domain logic
            DelayCalculatedTrackData processedData = calculatorService_->calculateDelay(data);
            
            // Logger::info("Processed track ", data.trackId, 
            //             " -> Delay: ", processedData.firstHopDelayTime, "μs, ",
            //             "SecondHop: ", processedData.secondHopSentTime, "μs");
            
            // Send processed data via outgoing adapter
            dataSender_->sendData(processedData);
            
            Logger::debug("Successfully sent processed track data for ID=", data.trackId);
            
        } catch (const std::exception& e) {
            Logger::error("Error processing track ", data.trackId, ": ", e.what());
        }
    }
};

/**
 * @brief Application entry point
 * 
 * Demonstrates composition root pattern where all dependencies are wired together.
 * Uses ZeroMQ RADIO/DISH pattern for UDP multicast messaging.
 */
int main() {
    Logger::info("=== B_Hexagon Track Processing System Starting ===");
    
    try {
        Logger::info("Initializing application components...");
        
        // Create domain services
        Logger::debug("Creating CalculatorService...");
        auto calculatorService = std::make_unique<CalculatorService>();
        
        // Create outgoing adapter (RADIO socket)
        Logger::debug("Creating ZeroMQDataSender (RADIO socket)...");
        auto dataSender = std::make_unique<ZeroMQDataSender>();
        
        // Create use case with dependencies
        Logger::debug("Creating ProcessTrackUseCase with dependencies...");
        auto useCase = std::make_unique<ProcessTrackUseCase>(
            std::move(calculatorService),
            std::move(dataSender)
        );
        
        // Create incoming adapter (DISH socket) and wire to use case
        Logger::info("Creating ZeroMQDataHandler (DISH socket)...");
        ZeroMQDataHandler dataHandler(useCase.get());
        
        Logger::info("=== System Configuration ===");
        Logger::info("Architecture: Hexagonal (Ports & Adapters)");
        Logger::info("Messaging: ZeroMQ RADIO/DISH UDP multicast");
        Logger::info("Endpoint: udp://239.1.1.1:9001");
        Logger::info("Group: SOURCE_DATA");
        Logger::info("Status: Ready to receive track data");
        Logger::info("===============================");
        
        // Start receiving messages (blocks indefinitely)
        Logger::info("Starting message reception loop...");
        dataHandler.startReceiving();
        
    } catch (const std::exception& ex) {
        Logger::error("Application error: ", ex.what());
        return 1;
    }
    
    Logger::info("Application shutting down normally");
    return 0;
}
