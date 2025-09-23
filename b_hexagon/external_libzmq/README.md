# External ZeroMQ Build System for B_Hexagon

This directory provides **optional external ZeroMQ compilation** for the B_Hexagon project. While the main project uses vendored ZeroMQ by default (recommended), this build system offers **maximum control** over ZeroMQ features and optimization for specialized deployment scenarios.

## 🎯 When to Use External ZeroMQ

✅ **Use External ZeroMQ when you need:**
- Custom ZeroMQ configuration options
- System-wide ZeroMQ installation
- Integration with existing ZeroMQ infrastructure  
- Shared library deployment
- Custom performance optimizations

❌ **Stick with Vendored ZeroMQ (default) for:**
- Simple development and testing
- Avoiding dependency conflicts
- Guaranteed ABI compatibility
- Quick project setup

## 📁 Directory Structure

```
external_libzmq/
├── build_libzmq.sh      # Complete build automation script
├── verify_libzmq.sh     # Post-build verification and testing  
├── setup_env.sh         # Environment configuration (auto-generated)
├── README.md           # This documentation
├── build/              # Build artifacts and intermediate files
└── install/            # Final installation with all components
    ├── lib/            # Shared/static libraries (libzmq.so, libzmq.a)
    ├── include/        # C/C++ headers (zmq.h, zmq.hpp)  
    ├── bin/            # Utilities and tools
    └── lib/pkgconfig/  # pkg-config files for easy linking
```

## 🚀 Quick Start Guide

### 1. Build ZeroMQ with Default Configuration
```bash
# Navigate to external build directory
cd external_libzmq

# Run automated build (installs to ./install/ by default)
./build_libzmq.sh

# Verify the build completed successfully
./verify_libzmq.sh
```

### 2. Build with Custom Installation Directory
```bash
# Install to system directory (requires sudo)
./build_libzmq.sh /usr/local

# Install to custom location
./build_libzmq.sh /opt/zeromq

# Verify custom installation
./verify_libzmq.sh /opt/zeromq
```

```bash
./verify_libzmq.sh
```

### 3. Set Up Environment (Optional)

```bash
# Source environment variables
source setup_env.sh
```

## 🔧 Build Configuration

The external libzmq build is configured with:

- **Release build** for optimal performance
- **Shared and static libraries** for flexibility
- **DRAFT APIs enabled** (includes RADIO/DISH pattern used in b_hexagon)
- **CURVE security** enabled
- **Tests disabled** for faster build
- **Documentation disabled** for faster build

## 📦 Integration with Main Project

### Method 1: Using pkg-config (Recommended)

```bash
# Set environment
export PKG_CONFIG_PATH="/path/to/external_libzmq/install/lib/pkgconfig:$PKG_CONFIG_PATH"

# Compile with pkg-config
g++ $(pkg-config --cflags libzmq-external) your_code.cpp $(pkg-config --libs libzmq-external)
```

### Method 2: Using CMake

```cmake
# In your CMakeLists.txt
set(CMAKE_PREFIX_PATH "/path/to/external_libzmq/install" ${CMAKE_PREFIX_PATH})
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq-external)

target_link_libraries(your_target ${ZMQ_LIBRARIES})
target_include_directories(your_target PRIVATE ${ZMQ_INCLUDE_DIRS})
```

### Method 3: Direct Linking

```bash
# Direct compilation
g++ -I/path/to/external_libzmq/install/include \
    your_code.cpp \
    -L/path/to/external_libzmq/install/lib \
    -lzmq
```

## 🧪 Testing and Verification

The verification script (`verify_libzmq.sh`) performs:

1. ✅ Check installation directory structure
2. ✅ Verify library files exist (shared/static)
3. ✅ Confirm header files are present
4. ✅ Test pkg-config configuration
5. ✅ Compile and run a test program
6. ✅ Verify DRAFT APIs (RADIO/DISH pattern)

## 🛠 Maintenance Commands

### Clean Build

```bash
./build_libzmq.sh clean
```

### Rebuild Everything

```bash
./build_libzmq.sh clean
./build_libzmq.sh
```

### Update libzmq Source

```bash
# From project root
cd b_hexagon/third_party/libzmq
git pull origin master
cd ../../../external_libzmq
./build_libzmq.sh clean
./build_libzmq.sh
```

## 🔍 Troubleshooting

### Common Issues

1. **Build fails with missing dependencies**
   ```bash
   # Install required packages (Ubuntu/Debian)
   sudo apt-get update
   sudo apt-get install build-essential cmake pkg-config
   ```

2. **Runtime library not found**
   ```bash
   # Add to LD_LIBRARY_PATH
   export LD_LIBRARY_PATH="/path/to/external_libzmq/install/lib:$LD_LIBRARY_PATH"
   ```

3. **Headers not found during compilation**
   ```bash
   # Check PKG_CONFIG_PATH
   export PKG_CONFIG_PATH="/path/to/external_libzmq/install/lib/pkgconfig:$PKG_CONFIG_PATH"
   pkg-config --cflags libzmq-external
   ```

### Debug Information

```bash
# Check build logs
ls -la build/
cat build/CMakeError.log

# Check installation
ls -la install/
ldd install/lib/libzmq.so

# Test pkg-config
pkg-config --exists libzmq-external && echo "Found" || echo "Not found"
pkg-config --modversion libzmq-external
```

## 📋 Features Enabled

- ✅ **ZMQ_RADIO/ZMQ_DISH** - Draft pattern used in b_hexagon messaging
- ✅ **CURVE Security** - Encryption and authentication
- ✅ **Thread Safety** - Multi-threaded applications support
- ✅ **Shared Libraries** - For dynamic linking
- ✅ **Static Libraries** - For static linking
- ✅ **pkg-config Support** - Easy integration

## 🔗 Integration with b_hexagon

This external libzmq build is specifically configured to work with the b_hexagon project:

1. **DRAFT APIs** are enabled for RADIO/DISH messaging pattern
2. **Thread safety** for hexagonal architecture components
3. **Performance optimizations** for high-throughput scenarios
4. **Compatible versions** tested with the project codebase

## 📝 Notes

- The external build is independent of the main project's CMake configuration
- Build artifacts are in `build/` and should not be committed to Git
- Installation directory `install/` contains the final compiled library
- Environment setup script is regenerated on each build
- Compatible with both development and production deployments

## 🆔 Version Information

- **libzmq source**: Git submodule tracking official ZeroMQ repository
- **Build system**: CMake 3.10+
- **Compiler**: GCC 7+ or Clang 5+
- **Platform**: Linux (primary), macOS and Windows (community tested)
