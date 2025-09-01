# External libzmq Build System

This directory contains scripts and configuration for building libzmq externally from the main b_hexagon project. This approach provides better control over ZeroMQ compilation and enables easier deployment scenarios.

## 📁 Directory Structure

```
external_libzmq/
├── build_libzmq.sh      # Main build script
├── verify_libzmq.sh     # Verification and testing script
├── setup_env.sh         # Environment setup (generated after build)
├── README.md           # This file
├── build/              # Build artifacts (generated)
└── install/            # Installation directory (generated)
    ├── lib/            # Compiled libraries
    ├── include/        # Header files
    └── lib/pkgconfig/  # pkg-config files
```

## 🚀 Quick Start

### 1. Build libzmq Externally

```bash
# From the project root
cd external_libzmq
./build_libzmq.sh
```

### 2. Verify the Build

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
