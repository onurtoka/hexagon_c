# External libzmq Integration Guide

This guide shows how to use the external libzmq build with the main b_hexagon project.

## 🚀 Quick Start

### 1. Clone with Submodules

```bash
# When cloning the repository
git clone --recursive https://github.com/emrekaragozz/b_hexagon.git

# Or if already cloned, initialize submodules
git submodule update --init --recursive
```

### 2. Build External libzmq

```bash
cd external_libzmq
./build_libzmq.sh
./verify_libzmq.sh  # Optional verification
```

### 3. Build Main Project with External libzmq

```bash
# Source the environment
source external_libzmq/setup_env.sh

# Build main project
cd b_hexagon
mkdir -p build
cd build
cmake .. -DUSE_EXTERNAL_LIBZMQ=ON
make -j$(nproc)
```

## 🔧 Integration Methods

### Method 1: Environment Variables (Recommended)

```bash
# Set up environment
export PKG_CONFIG_PATH="/path/to/external_libzmq/install/lib/pkgconfig:$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="/path/to/external_libzmq/install/lib:$LD_LIBRARY_PATH"

# Your project can now use pkg-config
pkg-config --cflags --libs libzmq-external
```

### Method 2: CMake Integration

Add to your `CMakeLists.txt`:

```cmake
# Option to use external libzmq
option(USE_EXTERNAL_LIBZMQ "Use external libzmq build" OFF)

if(USE_EXTERNAL_LIBZMQ)
    # Set the path to external libzmq
    set(EXTERNAL_LIBZMQ_DIR "${CMAKE_SOURCE_DIR}/../external_libzmq/install")
    
    # Find the external libzmq
    find_package(PkgConfig REQUIRED)
    set(PKG_CONFIG_PATH "${EXTERNAL_LIBZMQ_DIR}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}")
    
    pkg_check_modules(ZMQ REQUIRED libzmq-external)
    
    # Create imported target
    add_library(external_zmq INTERFACE)
    target_include_directories(external_zmq INTERFACE ${ZMQ_INCLUDE_DIRS})
    target_link_libraries(external_zmq INTERFACE ${ZMQ_LIBRARIES})
    target_link_directories(external_zmq INTERFACE ${ZMQ_LIBRARY_DIRS})
    
    # Set the target to use
    set(ZMQ_TARGET external_zmq)
    
    message(STATUS "Using external libzmq from: ${EXTERNAL_LIBZMQ_DIR}")
else()
    # Use bundled or system libzmq
    find_package(ZMQ REQUIRED)
    set(ZMQ_TARGET zmq)
endif()

# Link your targets
target_link_libraries(your_target ${ZMQ_TARGET})
```

### Method 3: Direct Path Configuration

```cmake
# Set CMAKE_PREFIX_PATH to find external libzmq
set(CMAKE_PREFIX_PATH "/path/to/external_libzmq/install" ${CMAKE_PREFIX_PATH})

# Find ZMQ normally
find_package(ZMQ REQUIRED)
target_link_libraries(your_target zmq)
```

## 🔍 Verification

Test that your application can link with external libzmq:

```cpp
#define ZMQ_BUILD_DRAFT_API 1
#include <zmq.h>
#include <iostream>

int main() {
    void *context = zmq_ctx_new();
    void *radio = zmq_socket(context, ZMQ_RADIO);  // DRAFT API
    void *dish = zmq_socket(context, ZMQ_DISH);    // DRAFT API
    
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    std::cout << "ZMQ Version: " << major << "." << minor << "." << patch << std::endl;
    
    zmq_close(dish);
    zmq_close(radio);
    zmq_ctx_destroy(context);
    return 0;
}
```

Compile and run:

```bash
# Using pkg-config
g++ -std=c++17 $(pkg-config --cflags libzmq-external) test.cpp $(pkg-config --libs libzmq-external) -o test
./test
```

## 🚚 Deployment

### Development Environment

```bash
# Set up development environment
source external_libzmq/setup_env.sh
```

### Production Deployment

1. **Copy built libraries**:
   ```bash
   cp -r external_libzmq/install/lib/* /usr/local/lib/
   cp -r external_libzmq/install/include/* /usr/local/include/
   ldconfig  # Update library cache
   ```

2. **Use static linking** (recommended for containerized deployments):
   ```bash
   g++ your_app.cpp -static -L/path/to/external_libzmq/install/lib -lzmq -lpthread -lrt
   ```

## 📋 Maintenance

### Update libzmq

```bash
# Update the submodule
cd b_hexagon/third_party/libzmq
git pull origin master
cd ../../../external_libzmq

# Rebuild
./build_libzmq.sh clean
./build_libzmq.sh
./verify_libzmq.sh
```

### Clean Builds

```bash
# Clean external libzmq
cd external_libzmq
./build_libzmq.sh clean

# Clean main project
cd ../b_hexagon
rm -rf build/
```

## 🆔 Compatibility

- **libzmq Version**: 4.3.6+ (with DRAFT APIs)
- **CMake**: 3.10+
- **Compiler**: GCC 7+, Clang 5+
- **Platform**: Linux (primary), macOS, Windows (with adjustments)

## 📝 Notes

- External libzmq build is optimized for the b_hexagon project requirements
- DRAFT APIs are enabled by default (ZMQ_RADIO/ZMQ_DISH pattern)
- Both shared and static libraries are built
- pkg-config integration simplifies linking
- Compatible with CI/CD systems and containerized deployments
