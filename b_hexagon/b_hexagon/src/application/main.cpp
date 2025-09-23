/**
 * @file main.cpp
 * @brief Application entry point demonstrating Hexagonal Architecture assembly
 */

#include "domain/logic/CalculatorService.hpp"
#include "domain/ports/incoming/IDataHandler.hpp"
#include "domain/ports/outgoing/IDataWriter.hpp"
#include "adapters/incoming/ZeroMQDataHandler.hpp"
#include "adapters/outgoing/ZeroMQDataWriter.hpp"
#include "common/Logger.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

// Using declarations for convenience
using domain::model::ExtrapTrackData;
using domain::model::DelayCalcTrackData;

/**
 * @class ProcessTrackUseCase
 * @brief Domain use case for processing track data
 */
class ProcessTrackUseCase final : public IDataHandler {
private:
    std::unique_ptr<CalculatorService> calculator_;
    std::unique_ptr<IDataWriter> dataSender_;

public:
    explicit ProcessTrackUseCase(std::unique_ptr<CalculatorService> calculator,
                       std::unique_ptr<IDataWriter> dataSender)
        : calculator_(std::move(calculator)),
          dataSender_(std::move(dataSender)) {
        Logger::info("ProcessTrackUseCase initialized with CalculatorService and DataSender");
    }

    void onDataReceived(const ExtrapTrackData& data) override {
        Logger::info("=== RECEIVED DATA FROM A_HEXAGON ===");
        Logger::info("Track ID: ", data.getTrackId());
        Logger::info("Position ECEF: (", data.getXPositionECEF(), ", ", data.getYPositionECEF(), ", ", data.getZPositionECEF(), ")");
        Logger::info("Velocity ECEF: (", data.getXVelocityECEF(), ", ", data.getYVelocityECEF(), ", ", data.getZVelocityECEF(), ")");
        Logger::info("Update Time: ", data.getUpdateTime());
        Logger::info("=====================================");
        
        if (!data.isValid()) {
            Logger::warn("Invalid track data received: ID=", data.getTrackId());
            return;
        }
        
        try {
            // Process the track data through domain logic
            DelayCalcTrackData processedData = calculator_->calculateDelay(data);
            
            Logger::info("Processed track ", data.getTrackId(), 
                        " -> Delay: ", processedData.getFirstHopDelayTime(), "μs, ",
                        "SecondHop: ", processedData.getSecondHopSentTime(), "μs");
            
            // Send processed data via outgoing adapter
            dataSender_->sendData(processedData);
            
            Logger::debug("Successfully sent processed track data for ID=", data.getTrackId());
            
        } catch (const std::exception& e) {
            Logger::error("Error processing track ", data.getTrackId(), ": ", e.what());
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
        Logger::debug("Creating ZeroMQDataWriter (RADIO socket)...");
        auto dataSender = std::make_unique<ZeroMQDataWriter>();
        
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
        Logger::info("Endpoint: udp://239.255.0.1:7779");
        Logger::info("Group: TRACK_DATA_UDP");
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
