#include <iostream>
#include <chrono>
#include <csignal>
#include <atomic>
#include <memory>
#include <thread>

#include "../domain/logic/TrackDataProcessor.hpp"
#include "../adapters/incoming/zeromq/ZeroMQDishTrackDataSubscriber.hpp"

using namespace hat;

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\nShutdown signal received. Exiting..." << std::endl;
    running.store(false);
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== HEXAGON_C - Hexagonal Architecture Track Data Monitor ===" << std::endl;
        std::cout << "Starting hexagonal architecture components..." << std::endl;

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        auto data_processor = std::make_shared<domain::logic::TrackDataProcessor>();

        // Default UDP multicast configuration (udp://239.1.1.1:9001, SOURCE_DATA)
        auto subscriber = std::make_shared<adapters::incoming::zeromq::ZeroMQDishTrackDataSubscriber>(
            data_processor
        );

        if (data_processor && subscriber && data_processor->isReadyToReceive()) {
            std::cout << "✓ Hexagonal architecture active - Press Ctrl+C to stop" << std::endl;

            if (!subscriber->start()) {
                std::cerr << "❌ Failed to start ZeroMQ subscriber" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "❌ Failed to initialize hexagonal architecture components" << std::endl;
            return 1;
        }

        while (running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "\nShutting down hexagonal architecture components..." << std::endl;
        subscriber->stop();
        std::cout << "✓ All hexagonal components shut down successfully" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error in hexagonal architecture: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown fatal error occurred in hexagonal architecture" << std::endl;
        return 1;
    }
}