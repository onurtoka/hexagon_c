# A Hexagon - Track Data Extrapolation System

## Overview

This project implements a **Hexagonal Architecture (Ports & Adapters Pattern)** for a Track Data Extrapolation System that converts incoming track data from 5Hz to 200Hz frequency.

## Architecture

The system follows the Hexagonal Architecture pattern with clear separation of concerns:

### Domain Core
- **Logic Layer**: `TrackDataExtrapolator` - Main business logic for extrapolation
- **Model Layer**: Data models (`TrackData`, `ExtrapTrackData`)
- **Ports**: Interfaces defining contracts for incoming and outgoing data

### Adapters
- **Incoming Adapters**: Handle external data sources (placeholder for future implementation)
- **Outgoing Adapters**: `ZeroMQExtrapTrackDataAdapter` - Sends extrapolated data via ZeroMQ

### External Systems
- **Data Source**: Produces track data at 5Hz (radar, sensors, etc.)
- **Data Consumer**: Consumes extrapolated data at 200Hz (display systems, analytics, etc.)

## Key Components

### TrackDataExtrapolator
Main business logic class that:
- Receives `TrackData` at 5Hz
- Extrapolates to 25 data points (200Hz for 125ms window)
- Uses constant velocity model for prediction
- Forwards results through outgoing adapter

### Data Models
- **TrackData**: Input data with position, velocity, and timestamp
- **ExtrapTrackData**: Output data with extrapolated position and timing information

### ZeroMQ Integration
- TCP-based messaging using ZeroMQ PUSH socket
- JSON serialization format
- Configurable endpoint (tcp://localhost:9596)

## Testing

Comprehensive unit tests using mock adapters:
- Logic validation for various velocity scenarios
- Time progression verification
- Data integrity checks
- Boundary condition testing

## Build & Run

```bash
# Compile the project
g++ -std=c++17 -I. src/**/*.cpp -lzmq -o main

# Run tests
g++ -std=c++17 -I. test/**/*.cpp src/domain/**/*.cpp -lzmq -o test_extrapolator
./test_extrapolator
```

## Dependencies

- C++17 or later
- ZeroMQ library
- Standard C++ libraries (chrono, vector, memory)
