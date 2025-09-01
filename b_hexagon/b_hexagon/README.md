
# C++17 Hexagonal Architecture Track Delay Calculator

## Project Introduction
This project is a C++17 application designed using SOLID principles and the hexagonal (ports and adapters) architecture. The application receives `TrackData` data, performs two delay calculations, and sends the results as `DelayCalculatedTrackData` to another application. The system is modular, testable, and easily extensible with new adapters (e.g., for ZeroMQ or console I/O).

### Key Features
- **Hexagonal Architecture:** Clear separation between domain logic, application services, and infrastructure adapters.
- **SOLID Principles:** Interfaces and abstractions for all I/O, enabling easy testing and extension.
- **MISRA C++ 2023 Compliance:** All source files adhere to MISRA C++ 2023 coding standards for safety-critical software development.
- **ZeroMQ PUB/SUB:** High-performance messaging with topic-based routing (`receivedTrack` → `calculatedTrack`).
- **Comprehensive Documentation:** Line-by-line explanations with Doxygen integration and architectural diagrams.

### Architecture Summary
The solution implements classic Hexagonal (Ports & Adapters) architecture:
1. Domain Layer: Pure models (`TrackData`, `DelayCalculatedTrackData`) and deterministic logic (`TrackCalculations`).
2. Application Layer: `CalculatorService` orchestrates the use case without technology concerns.
3. Ports: `IDataReceiver` (inbound) and `IDataSender` (outbound) invert dependencies.
4. Adapters: ZeroMQ implementations translate external representations (JSON messages) to domain objects.

Benefits:
- Technology Agnostic Core
- High Testability (domain logic isolated, ports mockable)
- Ease of Extension (add new transport by implementing interfaces)

### Timing & Calculations
Timing fields now use mixed units per specification:
- `updateTime` (nanoseconds since epoch)
- `originalUpdateTime` (milliseconds since epoch)
- `firstHopSentTime` (nanoseconds since epoch)

Derived (nanoseconds):
- `firstHopDelayTime = receiveTime - firstHopSentTime`
- `secondHopSentTime = system time at forwarding`

### JSON Transport
ZeroMQ PUB/SUB adapters use a compact single-line JSON (numeric only) schema for evolvability and readability.

## Data Flow
1. The application receives a `TrackData` object (TrackId int; velocities double m/s; positions double m; UpdateTime long ns; OriginalUpdateTime long ms; FirstHopSentTime long ns).
2. On receipt, it records the system time as `ReceiveTime` (in nanoseconds).
3. It calculates:
   - **FirstHopDelayTime = ReceiveTime - FirstHopSentTime**
   - **SecondHopSentTime = system time in nanoseconds at send**
4. It constructs a `DelayCalculatedTrackData` object containing all original fields plus the two calculated times.
5. The result is sent to another application via the selected output adapter (e.g., ZeroMQ).

## Project Structure
- `src/domain/model/TrackData.hpp` — Domain model for input data
- `src/domain/model/DelayCalculatedTrackData.hpp` — Domain model for output data
- `src/domain/ports/IDataReceiver.hpp` — Input port interface
- `src/domain/ports/IDataSender.hpp` — Output port interface
- `src/domain/logic/TrackCalculations.hpp` — All calculation logic
- `src/application/CalculatorService.hpp/cpp` — Application service orchestrating the workflow
- `src/adapters/ZeroMQDataSender.hpp` — ZeroMQ output adapter
- `src/main.cpp` — Application entry point
- `tests/` — Unit tests

## Build & Test Instructions

### Prerequisites
- **C++17 Compiler:** GCC 8.5+ or Clang with C++17 support
- **CMake:** Version 3.16 or higher
- **ZeroMQ:** External installation (recommended) or vendored version available

### Building the Project

#### Option 1: Using External libzmq (Recommended)
```bash
# First, compile and install libzmq with draft API support
./compile_libzmq.sh /opt/zmq

# Configure build with external libzmq (default)
cmake -S . -B build

# Build all targets
cmake --build build
```

#### Option 2: Using Vendored libzmq
```bash
# Configure build with vendored ZeroMQ
cmake -S . -B build -DUSE_VENDORED_ZMQ=ON

# Build all targets
cmake --build build
```

#### Build Targets
```bash
cmake --build build --target b_hexagon_app    # Main application
cmake --build build --target test_calculations # Logic tests
cmake --build build --target test_service     # Service tests
```

### External libzmq Installation

For detailed instructions on compiling libzmq separately, see [External libzmq Compilation Guide](docs/EXTERNAL_LIBZMQ_COMPILATION.md).

**Quick installation:**
```bash
# Using the provided script (installs to /opt/zmq by default)
./compile_libzmq.sh

# Or specify custom installation directory
./compile_libzmq.sh /usr/local
```

### Running Tests
```bash
# Run all tests
ctest --test-dir build

# Run specific tests
./build/test_calculations  # Domain logic tests
./build/test_service      # Application service tests
```

### Running the Application
```bash
# Launch the track delay calculator
./build/b_hexagon_app

# The application will:
# 1. Subscribe to ZeroMQ topic "receivedTrack" on tcp://localhost:5555
# 2. Publish calculated results to topic "calculatedTrack" on tcp://*:5556
# 3. Process incoming TrackData and calculate delay metrics
```

### MISRA C++ 2023 Compliance
This project fully complies with MISRA C++ 2023 guidelines for safety-critical software:
- **Rule 16-2-1:** Include guards and header protection
- **Rule 8-5-1:** Explicit constructors to prevent implicit conversions
- **Rule 15-3-4:** Exception safety specifications with noexcept
- **Rule 5-0-5:** Type safety and explicit type conversions
- **Rule 6-4-1:** Proper initialization of variables
- **Rule 8-4-2:** Const correctness throughout
- **Rule 7-1-2:** Reference parameters for non-trivial types
- **Rule 12-1-4:** Resource management with RAII

See `docs/MISRA_*` files for detailed compliance analysis.

## Extending the Application
- To add new input/output methods, implement `IDataReceiver` or `IDataSender` in a new adapter.
- The domain and application logic remain unchanged.

## License
MIT

## Documentation

This project features **enhanced Doxygen documentation** with advanced visualization capabilities and comprehensive code flow diagrams:

### 🎯 Key Documentation Features
- **172 Interactive SVG Diagrams** - Clickable, zoomable, and scalable
- **UML Class Diagrams** - Automatic inheritance and relationship visualization  
- **Call Graphs** - Function call dependencies and reverse caller graphs
- **Include Dependency Graphs** - File relationship visualization
- **Javadoc-Style Comments** - Comprehensive API documentation
- **Directory Dependencies** - Architecture layer visualization

### 📊 Architecture Visualization

This project includes comprehensive architectural diagrams in the `docs/` directory:

#### **🏗️ Hexagonal Architecture Overview**
- **`hexagonal_architecture_generic.svg`** - Complete architectural overview showing all layers
- **`hexagonal_architecture_code_flow.svg`** - Detailed code flow through architecture components

#### **⚡ Code Execution Flow Diagrams**
- **`b_hexagon_detailed_code_flow.svg`** - Class and function level execution flow (16 steps)
- **`data_processing_flow.svg`** - RADIO-DISH pattern data processing visualization

These SVG diagrams provide:
- **Visual Consistency** - Hexagons for adapters, diamonds for ports, distinct colors per layer
- **Flow Analysis** - Step-by-step execution from main() through runtime processing
- **Technical Details** - File paths, function names, and architectural relationships
- **Interactive Elements** - Scalable vector graphics with clear legends

### 🚀 Quick Start

```bash
# Generate enhanced documentation
doxygen Doxyfile

# Or use the convenient build script
chmod +x build_docs.sh
./build_docs.sh

# Open in browser
$BROWSER docs/html/index.html
```

### 📚 Documentation Includes

1. **Main Project Documentation**
   - Hexagonal architecture visualization
   - ZeroMQ adapter implementation details
   - Domain logic and calculations
   - Interface contracts and ports

2. **Example Code** (`src/examples/example_documented_code.cpp`)
   - Factory pattern implementation
   - Shape hierarchy with polymorphism
   - Comprehensive documentation examples
   - Best practices demonstration

3. **Setup Guide** (`docs/DOXYGEN_SETUP.md`)
   - Complete configuration reference
   - Advanced features explanation
   - Troubleshooting guide
   - Integration instructions

### 🔧 Requirements
- **Doxygen** 1.9.0+ (for full feature support)
- **Graphviz** (for UML and call graphs)
- Modern browser (for interactive SVG diagrams)

### Key Documented Files
- `src/domain/model/TrackData.hpp`
- `src/domain/model/DelayCalculatedTrackData.hpp`
- `src/domain/logic/TrackCalculations.hpp`
- `src/domain/logic/CalculatorService.hpp`
- `src/domain/ports/incoming/IDataReceiver.hpp`
- `src/domain/ports/outgoing/IDataSender.hpp`
- `src/adapters/incoming/ZeroMQDataReceiver.hpp`
- `src/adapters/outgoing/ZeroMQDataSender.hpp`

Testing examples live under `tests/` for both pure calculations and service orchestration with mocks.

## Detailed Architecture & Build Reference

### Layer Breakdown
- Domain Model: Plain structs (`TrackData`, `DelayCalculatedTrackData`) only hold data (no behavior) for clarity and serialization friendliness.
- Domain Logic: `TrackCalculations` encapsulates pure functions (deterministic, side-effect free, trivially testable).
- Application Service: `CalculatorService` sequences use-case steps (receive -> timestamp -> compute -> assemble -> send -> log) without any transport details.
- Ports: `IDataReceiver` and `IDataSender` invert control; domain and application see only these abstractions.
- Adapters: ZeroMQ-based implementations perform translation between transport layer (JSON messages) and domain DTOs.

### ZeroMQ PUB/SUB Topics
- `ExtrapTrackData`: raw inbound `TrackData` messages published by an upstream component.
- `DelayCalcTrackData`: enriched `DelayCalculatedTrackData` produced by this service.
Subscribers can filter by topic prefix to consume only relevant streams.

### Time & Units Rationale
- `originalUpdateTime` kept in milliseconds to preserve upstream fidelity.
- All derived metrics (delay, second hop timestamp) expressed in nanoseconds for precision comparability.

### Build System (CMake) Overview
- Option `USE_VENDORED_ZMQ` (default ON) builds bundled ZeroMQ (`third_party/libzmq`) to avoid ABI/libstdc++ mismatch.
- Targets:
  - `b_hexagon`: main application binary.
  - `test_calculations`: unit tests for pure calculation logic.
  - `test_service`: unit / integration style tests for service orchestration (mock based).

### Build Steps Explained
1. Configure: `cmake -S b_hexagon -B b_hexagon/build`
   - Detects compiler, configures vendored libzmq, sets include paths.
2. Build: `cmake --build b_hexagon/build --target b_hexagon`
   - Compiles domain, adapters, application, links with freshly built `libzmq` shared library.
3. (Optional) Run tests: `cmake --build b_hexagon/build --target test_calculations && ./b_hexagon/build/test_calculations`

### Vendored libzmq Justification
System libzmq referenced GLIBCXX symbols newer than those provided by the available libstdc++ (GCC 8.5). Vendoring ensures the library is compiled against the same standard library ABI, eliminating linker errors (`GLIBCXX_3.4.29/30`).

### Extending / Replacing Transports
Implement a new adapter by deriving from `IDataReceiver` or `IDataSender` and wiring it in `main.cpp`. No domain or logic changes required.

### Potential Enhancements
- Graceful shutdown (atomic flag + signal handlers).
- Structured logging & metrics (latency histograms, message counts).
- Robust JSON validation or introduction of a small JSON library.
- Backpressure / queueing abstraction if throughput increases.
- Versioned topics (e.g., `calculatedTrack.v2`) for schema evolution.

---
Generated extended documentation to fulfill request for line-by-line and architectural explanation.
