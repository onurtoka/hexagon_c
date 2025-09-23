# a_hexagon_app - ZeroMQ Track Data Extrapolation System

## Project Overview
This project implements a hexagonal architecture-based application that receives TrackData via ZeroMQ, performs 200Hz extrapolation, and sends ExtrapTrackData to another endpoint. The system uses the port-adapter pattern to maintain clean separation between business logic and external communication.

**🚀 NEW: Complete Air-gapped Deployment Solution** - Now includes offline installation for RedHat 8 systems without internet connectivity, perfect for secure enterprise environments.

## Key Features
- **Hexagonal Architecture**: Clean separation of concerns with port-adapter pattern
- **High-Performance Messaging**: ZeroMQ-based communication
- **200Hz Extrapolation**: Real-time data processing with 5ms intervals
- **🔒 Air-gapped Deployment**: Complete offline installation for isolated networks
- **🛡️ Enterprise Ready**: Security hardening and systemd service integration

## Architecture
- **Hexagonal Architecture**: Core business logic is isolated from external dependencies
- **Port-Adapter Pattern**: Interfaces (ports) define contracts, adapters implement external communication
- **ZeroMQ Communication**: High-performance messaging for data input/output
- **200Hz Extrapolation**: Converts input data to 25 samples (5ms intervals) for 200Hz output

## File Structure

### Domain Layer (Core Business Logic)
- **src/domain/model/TrackData.h/.cpp**: Input track data model with:
  - `TrackId`: Identifier for the track
  - `XVelocityECEF`: ECEF X velocity in m/s
  - `YVelocityECEF`: ECEF Y velocity in m/s  
  - `ZVelocityECEF`: ECEF Z velocity in m/s
  - `XPositionECEF`: ECEF X position in meters
  - `YPositionECEF`: ECEF Y position in meters
  - `ZPositionECEF`: ECEF Z position in meters
  - `originalUpdateTime`: Update time in milliseconds

- **src/domain/model/ExtrapTrackData.h/.cpp**: Extrapolated track data model with additional fields:
  - All TrackData fields plus extrapolated positions and timing
  - `firstHopSentTime`: Nanosecond timestamp for processing metrics

- **src/domain/logic/TrackDataExtrapolator.h/.cpp**: Core extrapolation logic
  - Implements `TrackDataIncomingPort` interface
  - Provides `extrapolateTo200Hz()` static method
  - Generates 25 samples at 5ms intervals

### Ports (Interfaces)
- **src/domain/ports/incoming/TrackDataIncomingPort.h**: Input port interface
- **src/domain/ports/outgoing/TrackDataOutgoingPort.h**: Output port interface

### Adapters (External Communication)
- **src/adapters/incoming/ZeroMQTrackDataAdapter.h/.cpp**: ZeroMQ input adapter
  - Connects to `tcp://*:5555` (PULL socket)
  - Parses CSV format: `TrackId,XVel,YVel,ZVel,XPos,YPos,ZPos,Time`
  - Handles parsing errors gracefully

- **src/adapters/outgoing/ZeroMQExtrapTrackDataAdapter.h/.cpp**: ZeroMQ output adapter  
  - Connects to `tcp://localhost:5556` (PUSH socket)
  - Serializes ExtrapTrackData to CSV format
  - Sends 25 extrapolated samples per input

### Application Layer
- **src/application/main.cpp**: Application entry point
  - Wires adapters and business logic together
  - Starts ZeroMQ listener loop

## Dependencies

### Standard Installation
- **ZeroMQ**: High-performance messaging library
- **cppzmq**: C++ bindings for ZeroMQ
- **GCC 8.5+**: C++17 compiler support

### 🚀 Air-gapped Installation (NEW!)
For air-gapped RedHat 8 systems, this project includes **complete offline installation** with:

- **✅ GCC 8.5**: Compiled from source for C++17 support
- **✅ ZeroMQ 4.3.2**: Built from source with optimal configuration
- **✅ All RPM Dependencies**: Pre-downloaded system packages
- **✅ No Internet Required**: Target system needs no connectivity

**Quick Start for Air-gapped:**
```bash
# On internet-connected system (same OS/arch):
./prepare-offline.sh

# On air-gapped target system:
./check-offline-requirements.sh
sudo ./install-offline.sh
```

See **[Air-gapped Deployment](#-air-gapped-deployment-for-redhat-8-systems)** section below for complete details.

## ZeroMQ Integration Guide for GCC 8.5

### Problem Description
When using ZeroMQ library with GCC 8.5, you may encounter GLIBCXX version incompatibilities:
```
undefined reference to `std::condition_variable::wait(std::unique_lock<std::mutex>&)@GLIBCXX_3.4.30'
undefined reference to `std::__throw_bad_array_new_length()@GLIBCXX_3.4.29'
```

### Solution Steps

#### 1. Install ZeroMQ Libraries
```bash
sudo apt-get update
sudo apt-get install -y libzmq3-dev cppzmq-dev
```

**Explanation:**
- `libzmq3-dev`: ZeroMQ core library and header files
- `cppzmq-dev`: C++ wrapper header files for ZeroMQ (`zmq.hpp`)

#### 2. Resolve Version Incompatibility
If ZeroMQ library requires a newer GLIBCXX version than available on your system, use **static linking**:

```bash
g++ -std=c++17 -I./src -o main \
src/application/main.cpp \
src/adapters/incoming/ZeroMQTrackDataAdapter.cpp \
src/adapters/outgoing/ZeroMQExtrapTrackDataAdapter.cpp \
src/domain/logic/TrackDataExtrapolator.cpp \
src/domain/model/TrackData.cpp \
src/domain/model/ExtrapTrackData.cpp \
-static-libgcc -static-libstdc++ -lzmq -lpthread
```

**Critical Parameters:**
- `-static-libgcc`: Statically links GCC runtime
- `-static-libstdc++`: Statically links C++ standard library  
- `-lzmq`: Links ZeroMQ library
- `-lpthread`: Required for thread support

#### 3. Test Compilation Steps

**Step 1:** Try simple compilation (if error occurs, proceed to step 2)
```bash
g++ -std=c++17 -I./src -o main \
src/application/main.cpp \
[other .cpp files] \
-lzmq -lpthread
```

**Step 2:** If GLIBCXX error occurs, use static linking
```bash
g++ -std=c++17 -I./src -o main \
src/application/main.cpp \
[other .cpp files] \
-static-libgcc -static-libstdc++ -lzmq -lpthread
```

#### 4. Common Errors and Solutions

**Error:** `zmq.hpp: No such file or directory`
```bash
# Solution: Install cppzmq-dev package
sudo apt-get install -y cppzmq-dev
```

**Error:** `undefined reference to zmq_xxx functions`
```bash
# Solution: Add -lzmq parameter
g++ ... -lzmq
```

**Error:** `GLIBCXX_3.4.XX not found`
```bash
# Solution: Use static linking
g++ ... -static-libgcc -static-libstdc++ -lzmq
```

#### 5. Header Include Order
When using ZeroMQ, include header files in this order:

```cpp
#include <zmq.hpp>        // ZeroMQ C++ bindings
#include <string>         // std::string
#include <vector>         // std::vector  
#include <iostream>       // For debug output
#include <sstream>        // For string parsing
```

#### 6. Verification
After successful compilation, test the system:

```bash
# Start the application
./main

# In another terminal, test ZeroMQ connection
echo "test" | nc localhost 5555
```

### Static vs Dynamic Linking Explanation

#### What is Static Linking?
Static linking embeds library code directly into the executable file during compilation.

**Dynamic Linking (Default):**
```bash
g++ -o main main.cpp -lzmq
# Creates small executable (25KB)
# Depends on system libraries at runtime
# May fail if library versions don't match
```

**Static Linking:**
```bash
g++ -static-libgcc -static-libstdc++ -o main main.cpp -lzmq
# Creates larger executable (2.5MB)
# Includes required library code inside
# Independent of system library versions
```

#### Why We Use Static Linking:
- **System GLIBCXX version**: 3.4.25 (older)
- **ZeroMQ requires**: 3.4.30 (newer)
- **Solution**: Embed newer C++ runtime into our program

#### Benefits:
- ✅ **Portability**: Runs on different systems
- ✅ **No version conflicts**: Carries its own libraries
- ✅ **Easy deployment**: Single file distribution

#### Drawbacks:
- ❌ **Larger file size**: Libraries embedded in executable
- ❌ **Update issues**: Must recompile for library updates
- ❌ **Memory usage**: Each program carries its own copy

### Alternative: Docker Isolated Environment
If you continue having issues with system libraries:

```dockerfile
FROM ubuntu:20.04
RUN apt-get update && apt-get install -y \
    gcc-8 g++-8 libzmq3-dev cppzmq-dev
# Build your project inside Docker
```

### Summary Checklist
- ✅ `libzmq3-dev` and `cppzmq-dev` installed
- ✅ `-static-libgcc -static-libstdc++` parameters added  
- ✅ `-lzmq -lpthread` linking parameters present
- ✅ `#include <zmq.hpp>` header file found
- ✅ Compilation completed without errors
- ✅ `./main` executable created successfully

Follow these steps to achieve seamless ZeroMQ integration with GCC 8.5.

## Building the Application

### 1. Install Dependencies
```bash
sudo apt-get update
sudo apt-get install -y libzmq3-dev cppzmq-dev
```

### 2. Compile the Application
```bash
g++ -std=c++17 -I./src -o main \
src/application/main.cpp \
src/adapters/incoming/ZeroMQTrackDataAdapter.cpp \
src/adapters/outgoing/ZeroMQExtrapTrackDataAdapter.cpp \
src/domain/logic/TrackDataExtrapolator.cpp \
src/domain/model/TrackData.cpp \
src/domain/model/ExtrapTrackData.cpp \
-static-libgcc -static-libstdc++ -lzmq -lpthread
```

### 3. Run the Application
```bash
./main
```
The application will start listening on `tcp://*:5555` for TrackData input.

## Testing the System

### Test Files
- **test_publisher.py**: Sends sample TrackData to the application
- **test_subscriber.py**: Receives ExtrapTrackData from the application

### End-to-End Testing

#### Terminal 1: Start the main application
```bash
./main
```
Output: `Dinleniyor: tcp://*:5555`

#### Terminal 2: Start the data receiver (subscriber)
```bash
python3 test_subscriber.py
```
This will listen on `tcp://*:5556` for ExtrapTrackData.

#### Terminal 3: Send test data (publisher)
```bash
python3 test_publisher.py
```
This will send 5 TrackData samples to the application.

### Expected Results
- **Input**: 5 TrackData samples
- **Output**: 125 ExtrapTrackData samples (25 per input)
- **Timing**: Each input generates 25 samples at 5ms intervals
- **Data Flow**: TrackData → Extrapolation → ExtrapTrackData

### Sample Output Format
```
Mesaj #1: TrackID=1
  Velocity: (74.030000, -75.590000, -2.720000)
  Position: (2992.830000, 7874.460000, 820.120000)
  Time: 1755003626798
```

## Data Flow
1. **test_publisher.py** → TrackData (CSV) → **Main App** (`tcp://localhost:5555`)
2. **Main App** → Extrapolation (25 samples) → **test_subscriber.py** (`tcp://localhost:5556`)

## Unit Testing
To run the original unit tests:
```bash
g++ -std=c++17 -I./src test/domain/logic/TrackDataExtrapolator_test.cpp \
src/domain/logic/TrackDataExtrapolator.cpp \
src/domain/model/TrackData.cpp \
src/domain/model/ExtrapTrackData.cpp -o test_exe
./test_exe
```

## CSV Data Format
**Input (TrackData)**: `TrackId,XVelocityECEF,YVelocityECEF,ZVelocityECEF,XPositionECEF,YPositionECEF,ZPositionECEF,OriginalUpdateTime`

**Output (ExtrapTrackData)**: `TrackId,XVelocityECEF,YVelocityECEF,ZVelocityECEF,XPositionECEF,YPositionECEF,ZPositionECEF,UpdateTime`

## 🚀 Air-gapped Deployment for RedHat 8 Systems

### Overview
This project includes a complete offline installation solution for air-gapped RedHat 8 environments. The system can be deployed on isolated networks without internet connectivity, making it suitable for secure enterprise environments.

### 📦 Offline Installation Features

#### ✅ Complete Air-gapped Support
- **No Internet Required**: Target system needs no internet connectivity
- **Source Compilation**: GCC 8.5 and ZeroMQ 4.3.2 compiled from source
- **Self-contained**: All dependencies included in offline packages
- **Enterprise Ready**: Suitable for production environments

#### ✅ Automated Installation Process
- **System Requirements Check**: Automated hardware/software validation
- **One-click Installation**: Single script deployment
- **Service Integration**: Systemd service configuration
- **Security Hardening**: Best practices implementation

#### ✅ Comprehensive Package Management
- **RPM Dependencies**: Automated system package installation
- **Source Code Compilation**: GCC 8.5 from source for C++17 support
- **ZeroMQ Integration**: Custom ZeroMQ 4.3.2 build with GCC 8.5
- **Binary Distribution**: Ready-to-run executables

### 🛠️ Installation Scripts

#### 1. **prepare-offline.sh** - Internet-connected Preparation
```bash
# Run on internet-connected system with same OS/architecture
./prepare-offline.sh
```
- Downloads all required RPM packages
- Downloads GCC 8.5.0 and ZeroMQ 4.3.2 source code
- Creates offline packages archive
- Generates checksums and documentation

#### 2. **check-offline-requirements.sh** - System Validation
```bash
# Run on target air-gapped system
./check-offline-requirements.sh
```
- Validates OS compatibility (RHEL/CentOS/Rocky/AlmaLinux 8.x)
- Checks hardware requirements (CPU, RAM, disk space)
- Verifies offline packages availability
- Estimates installation time
- Reports system readiness

#### 3. **install-offline.sh** - Complete Installation
```bash
# Run on target air-gapped system as root
sudo ./install-offline.sh
```
- Installs RPM dependencies
- Compiles GCC 8.5 from source (60-180 minutes)
- Compiles ZeroMQ 4.3.2 with GCC 8.5
- Builds hexagonal architecture application
- Configures systemd services
- Sets up security policies

### 📋 System Requirements

#### Minimum Requirements
- **OS**: RedHat Enterprise Linux 8.x, CentOS 8, Rocky Linux 8, AlmaLinux 8
- **CPU**: 2 cores minimum (4+ recommended)
- **RAM**: 4GB minimum (8GB+ recommended) 
- **Disk**: 10GB root + 15GB /tmp (for GCC compilation)
- **Network**: Interface for UDP multicast (runtime)
- **Privileges**: Root or sudo access

#### Installation Time Estimates
- **Fast System** (8+ cores, 16GB+ RAM): ~90 minutes
- **Standard System** (4 cores, 8GB RAM): ~120 minutes
- **Minimum System** (2 cores, 4GB RAM): ~180 minutes

### 🔧 Quick Start for Air-gapped Deployment

#### Step 1: Prepare Offline Packages (Internet-connected system)
```bash
git clone <repository-url> hexagon-project
cd hexagon-project
chmod +x prepare-offline.sh
./prepare-offline.sh

# Create transfer archive
tar -czf hexagon-offline.tar.gz .
```

#### Step 2: Transfer to Air-gapped System
```bash
# Extract on target system
tar -xzf hexagon-offline.tar.gz
cd hexagon-project
```

#### Step 3: Validate and Install
```bash
# Check system requirements
chmod +x check-offline-requirements.sh
./check-offline-requirements.sh

# If requirements pass, install
chmod +x install-offline.sh
sudo ./install-offline.sh
```

#### Step 4: Verify Installation
```bash
# Check services
systemctl status hexagon-extrapolator
systemctl status hexagon-publisher

# Test application
cd /opt/hexagon
./bin/test_extrapolator

# View logs
journalctl -u hexagon-extrapolator -f
```

### 🔒 Security Features

#### Enterprise Security
- **Dedicated User Account**: Non-privileged hexagon user
- **File Permissions**: Restricted access to configuration files
- **Service Isolation**: Systemd service containment
- **Firewall Configuration**: Minimal port exposure
- **Log Security**: Secure logging with rotation

#### Network Security
- **Minimal Ports**: Only required UDP multicast ports
- **Local Binding**: Services bind to specific interfaces
- **No Remote Access**: No SSH or remote management by default

### 📚 Documentation

#### Complete Installation Guide
- **[INSTALL_OFFLINE.md](INSTALL_OFFLINE.md)**: Comprehensive offline installation guide
- **Step-by-step instructions**: Detailed deployment process
- **Troubleshooting guide**: Common issues and solutions
- **Security hardening**: Enterprise security recommendations
- **Maintenance procedures**: System monitoring and updates

#### Architecture Documentation
- **[COMPLETE_HEXAGONAL_UML.svg](COMPLETE_HEXAGONAL_UML.svg)**: Complete system architecture
- **Doxygen Documentation**: Auto-generated API documentation
- **Service Configuration**: Systemd service details

### 🎯 Use Cases

#### Enterprise Environments
- **Government Systems**: Air-gapped classified networks
- **Financial Institutions**: Isolated trading systems
- **Industrial Control**: SCADA and manufacturing networks
- **Research Facilities**: Secure research environments

#### Benefits
- **Security**: No internet exposure reduces attack surface
- **Reliability**: Self-contained deployment reduces dependencies
- **Compliance**: Meets air-gap security requirements
- **Performance**: Optimized compilation for target hardware

### 📊 Installation Monitoring

#### Real-time Progress
- **Colorized Output**: Visual progress indicators
- **Time Estimates**: Dynamic completion time updates
- **Error Handling**: Automatic rollback on failures
- **Log Generation**: Detailed installation logs

#### Post-installation Verification
- **Health Checks**: Automated service validation
- **Performance Tests**: Built-in benchmarking
- **Configuration Validation**: Settings verification
- **Service Monitoring**: Continuous health monitoring

### 🔄 Service Management

#### Systemd Integration
```bash
# Start services
systemctl start hexagon-extrapolator
systemctl start hexagon-publisher

# Enable auto-start
systemctl enable hexagon-extrapolator
systemctl enable hexagon-publisher

# Monitor services
systemctl status hexagon-*
journalctl -u hexagon-extrapolator -f
```

#### Configuration Management
- **Service Files**: `/etc/systemd/system/hexagon-*.service`
- **Application Config**: `/opt/hexagon/config/`
- **Log Configuration**: `/opt/hexagon/config/logging.conf`
- **Runtime Settings**: Environment variables and paths

### 🛡️ Enterprise Support

#### Maintenance
- **Automated Health Checks**: Service monitoring scripts
- **Log Rotation**: Automated log management
- **Performance Monitoring**: Resource usage tracking
- **Update Procedures**: Safe update mechanisms

#### Uninstallation
```bash
# Complete system cleanup
systemctl stop hexagon-*
systemctl disable hexagon-*
rm -rf /opt/hexagon /opt/gcc-8.5
userdel -r hexagon
```

This offline installation solution ensures reliable deployment in the most restrictive network environments while maintaining enterprise-grade security and monitoring capabilities.