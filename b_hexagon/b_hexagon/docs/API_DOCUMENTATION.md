# B_Hexagon API Documentation

## Overview
The B_Hexagon Track Delay Calculator provides a comprehensive API for processing track data with delay calculations. This document outlines all public interfaces, data structures, and usage patterns.

## Core Domain Models

### TrackData
Input data structure representing track information with mixed time units.

```cpp
struct TrackData {
    int trackId;                    ///< Unique track identifier
    double velocityX;               ///< X-axis velocity in m/s  
    double velocityY;               ///< Y-axis velocity in m/s
    double positionX;               ///< X-axis position in meters
    double positionY;               ///< Y-axis position in meters
    long updateTime;                ///< Current update time in nanoseconds since epoch
    long originalUpdateTime;        ///< Source timestamp in milliseconds since epoch
    long firstHopSentTime;          ///< Initial transmission time in nanoseconds since epoch
};
```

**Usage:**
```cpp
TrackData track{
    .trackId = 12345,
    .velocityX = 15.5,
    .velocityY = -8.2,
    .positionX = 1000.0,
    .positionY = 2000.0,
    .updateTime = 1672531200000000000L,  // ns
    .originalUpdateTime = 1672531200000L, // ms
    .firstHopSentTime = 1672531199000000000L // ns
};
```

### DelayCalculatedTrackData
Output data structure extending TrackData with calculated delay metrics.

```cpp
struct DelayCalculatedTrackData {
    // Inherits all TrackData fields
    long firstHopDelayTime;         ///< Calculated first hop delay in nanoseconds
    long secondHopSentTime;         ///< System time at forwarding in nanoseconds since epoch
};
```

**Usage:**
```cpp
DelayCalculatedTrackData result = calculator.processTrack(track);
// result.firstHopDelayTime contains calculated delay
// result.secondHopSentTime contains forwarding timestamp
```

## Calculation Logic

### TrackCalculations
Pure stateless calculation utilities for timing metrics.

```cpp
class TrackCalculations {
public:
    /// Calculate first hop delay: receiveTime - firstHopSentTime
    static long calculateFirstHopDelayTime(const TrackData& trackData, long receiveTimeNs);
    
    /// Get current system time for second hop timestamp
    static long calculateSecondHopSentTime();
};
```

**Example:**
```cpp
long receiveTime = getCurrentTimeNs();
long firstHopDelay = TrackCalculations::calculateFirstHopDelayTime(track, receiveTime);
long secondHopTime = TrackCalculations::calculateSecondHopSentTime();
```

## Port Interfaces

### IDataReceiver (Inbound Port)
Interface for receiving track data from external sources.

```cpp
class IDataReceiver {
public:
    virtual ~IDataReceiver() = default;
    
    /// Blocking call to receive next TrackData
    /// @throws std::runtime_error on receive failure
    virtual TrackData receive() = 0;
};
```

**Implementation Example:**
```cpp
class CustomReceiver : public IDataReceiver {
public:
    TrackData receive() override {
        // Custom implementation
        return TrackData{...};
    }
};
```

### IDataSender (Outbound Port)  
Interface for sending calculated results to external systems.

```cpp
class IDataSender {
public:
    virtual ~IDataSender() = default;
    
    /// Send calculated track data
    /// @throws std::runtime_error on send failure
    virtual void send(const DelayCalculatedTrackData& data) = 0;
};
```

**Implementation Example:**
```cpp
class CustomSender : public IDataSender {
public:
    void send(const DelayCalculatedTrackData& data) override {
        // Custom implementation
    }
};
```

## Application Service

### CalculatorService
Orchestrates the receive → compute → send workflow.

```cpp
class CalculatorService final {
public:
    /// Constructor with smart pointer injection
    explicit CalculatorService(std::unique_ptr<IDataReceiver> receiver, 
                               std::unique_ptr<IDataSender> sender);
    
    /// Constructor with raw pointer injection (takes ownership)
    explicit CalculatorService(IDataReceiver* receiver, IDataSender* sender);
    
    /// Start processing loop (blocking call)
    void start();
};
```

**Usage:**
```cpp
auto receiver = std::make_unique<ZeroMQDataReceiver>("tcp://localhost:5555", "receivedTrack");
auto sender = std::make_unique<ZeroMQDataSender>("tcp://*:5556", "calculatedTrack");
CalculatorService service(std::move(receiver), std::move(sender));
service.start(); // Infinite processing loop
```

## ZeroMQ Adapters

### ZeroMQDataReceiver
Subscribes to ZeroMQ messages and deserializes JSON to TrackData.

```cpp
class ZeroMQDataReceiver : public IDataReceiver {
public:
    /// Constructor
    /// @param endpoint ZeroMQ endpoint (e.g., "tcp://localhost:5555")
    /// @param topic Subscription topic filter
    explicit ZeroMQDataReceiver(const std::string& endpoint, const std::string& topic);
    
    TrackData receive() override;
};
```

**JSON Input Format:**
```json
{"trackId":12345,"velocityX":15.5,"velocityY":-8.2,"positionX":1000.0,"positionY":2000.0,"updateTime":1672531200000000000,"originalUpdateTime":1672531200000,"firstHopSentTime":1672531199000000000}
```

### ZeroMQDataSender
Publishes DelayCalculatedTrackData as JSON messages.

```cpp
class ZeroMQDataSender : public IDataSender {
public:
    /// Constructor  
    /// @param endpoint ZeroMQ bind endpoint (e.g., "tcp://*:5556")
    /// @param topic Publication topic prefix
    explicit ZeroMQDataSender(const std::string& endpoint, const std::string& topic);
    
    void send(const DelayCalculatedTrackData& data) override;
};
```

**JSON Output Format:**
```json
{"trackId":12345,"velocityX":15.5,"velocityY":-8.2,"positionX":1000.0,"positionY":2000.0,"updateTime":1672531200000000000,"originalUpdateTime":1672531200000,"firstHopSentTime":1672531199000000000,"firstHopDelayTime":1000000000,"secondHopSentTime":1672531201000000000}
```

## Error Handling

### Exception Types
All interfaces may throw `std::runtime_error` for operational failures:

```cpp
try {
    TrackData data = receiver->receive();
    // Process data...
} catch (const std::runtime_error& e) {
    // Handle receive failure
    std::cerr << "Receive error: " << e.what() << std::endl;
}
```

### MISRA Exception Safety
All components provide strong exception safety guarantees:
- **Basic Guarantee:** No resource leaks on exception
- **Strong Guarantee:** Transaction semantics (success or no change)
- **No-throw Guarantee:** Operations marked with `noexcept`

## Testing Support

### Mock Implementations
Test-friendly mock implementations for unit testing:

```cpp
class MockReceiver : public IDataReceiver {
    std::queue<TrackData> data_;
public:
    void addTestData(const TrackData& track) { data_.push(track); }
    TrackData receive() override {
        if (data_.empty()) throw std::runtime_error("No test data");
        auto result = data_.front();
        data_.pop();
        return result;
    }
};

class MockSender : public IDataSender {
    std::vector<DelayCalculatedTrackData> sent_;
public:
    void send(const DelayCalculatedTrackData& data) override {
        sent_.push_back(data);
    }
    const auto& getSentData() const { return sent_; }
};
```

### Unit Test Example
```cpp
void testCalculatorService() {
    auto mockReceiver = std::make_unique<MockReceiver>();
    auto mockSender = std::make_unique<MockSender>();
    
    // Setup test data
    TrackData testTrack{/* ... */};
    mockReceiver->addTestData(testTrack);
    
    // Test service
    CalculatorService service(std::move(mockReceiver), std::move(mockSender));
    // service.processOne(); // If such method existed
    
    // Verify results
    assert(!mockSender->getSentData().empty());
}
```

## Configuration

### ZeroMQ Endpoints
Default configuration for development:
- **Input:** `tcp://localhost:5555` subscribing to `receivedTrack`
- **Output:** `tcp://*:5556` publishing to `calculatedTrack`

### Production Configuration
```cpp
// High-availability setup
auto receiver = std::make_unique<ZeroMQDataReceiver>(
    "tcp://track-source.company.com:5555", 
    "receivedTrack"
);
auto sender = std::make_unique<ZeroMQDataSender>(
    "tcp://*:5556", 
    "calculatedTrack"
);
```

## Time Handling

### Units and Precision
- **Input Time Fields:**
  - `updateTime`: nanoseconds since epoch (long)
  - `originalUpdateTime`: milliseconds since epoch (long) 
  - `firstHopSentTime`: nanoseconds since epoch (long)
- **Calculated Fields:**
  - `firstHopDelayTime`: nanoseconds (long)
  - `secondHopSentTime`: nanoseconds since epoch (long)

### Time Conversion Utilities
```cpp
// Convert milliseconds to nanoseconds
long msToNs(long milliseconds) {
    return milliseconds * 1000000L;
}

// Convert nanoseconds to milliseconds  
long nsToMs(long nanoseconds) {
    return nanoseconds / 1000000L;
}

// Get current time in nanoseconds
long getCurrentTimeNs() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()
    ).count();
}
```

## Performance Considerations

### Memory Management
- **RAII:** All resources managed with smart pointers
- **Zero-copy:** Efficient message passing between layers
- **Stack Allocation:** Prefer stack over heap for temporary objects

### Threading
- **Single-threaded:** Current implementation uses blocking I/O
- **Thread Safety:** Components are not thread-safe by design
- **Future Enhancement:** Can be made thread-safe with proper synchronization

### Latency Optimization
- **Minimal Allocations:** Reuse objects where possible
- **Direct JSON Parsing:** No intermediate string copies
- **Fast Path:** Optimized calculation functions

## MISRA C++ 2023 Compliance

All public APIs comply with MISRA C++ 2023 guidelines:
- **Explicit Constructors:** Prevent implicit conversions
- **Const Correctness:** Immutable interfaces where appropriate
- **Exception Safety:** Strong guarantees with proper cleanup
- **Type Safety:** No C-style casts or implicit conversions
- **Resource Management:** RAII pattern throughout

This ensures the API is suitable for safety-critical applications and long-term maintenance.
