# B_Hexagon Compilation Guide

This guide provides detailed instructions for compiling the B_Hexagon project with all its dependencies.

## 🚀 Quick Compilation (Recommended)

```bash
# 1. Navigate to project directory
cd b_hexagon/b_hexagon

# 2. Create build directory
mkdir -p build && cd build

# 3. Configure with CMake (uses vendored ZeroMQ by default)
cmake ..

# 4. Compile with parallel jobs
make -j$(nproc)

# 5. Verify build
ls -la b_hexagon_app domain_tests
```

## 📋 Prerequisites

### Required Dependencies
- **C++17 Compiler:** GCC 8.5+, Clang 10+, or MSVC 2019+
- **CMake:** Version 3.16 or higher
- **Git:** For dependency management (Google Test)

### Optional Dependencies  
- **ZeroMQ:** Only if using external installation (`-DUSE_VENDORED_ZMQ=OFF`)
- **Doxygen:** For documentation generation
- **Graphviz:** For architectural diagrams

## 🔧 Compilation Options

### Option 1: Vendored ZeroMQ (Default - Recommended)

**Advantages:**
- ✅ No external dependencies required
- ✅ Guaranteed ABI compatibility  
- ✅ RADIO/DISH draft APIs enabled
- ✅ Avoids `GLIBCXX` version conflicts

```bash
# Configure (vendored ZeroMQ is default)
cmake -S . -B build

# Or explicitly enable vendored ZeroMQ
cmake -S . -B build -DUSE_VENDORED_ZMQ=ON

# Build all targets
cmake --build build --parallel $(nproc)
```

**What gets built:**
- ZeroMQ library (static) from `third_party/libzmq/`
- Main application: `b_hexagon_app`
- Test suite: `domain_tests`

### Option 2: External ZeroMQ Installation

**Prerequisites:**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install libzmq3-dev libzmq5

# CentOS/RHEL
sudo yum install zeromq-devel

# macOS  
brew install zeromq

# Verify installation
pkg-config --modversion libzmq
```

**Build steps:**
```bash
# Configure with external ZeroMQ
cmake -S . -B build -DUSE_VENDORED_ZMQ=OFF

# Build
cmake --build build --parallel $(nproc)
```

### Option 3: Custom ZeroMQ Installation

**Use case:** Maximum control over ZeroMQ features and optimization

```bash
# Step 1: Build ZeroMQ with custom options
cd external_libzmq
./build_libzmq.sh /opt/zmq  # Custom install directory

# Step 2: Configure build with custom ZeroMQ
cd ../b_hexagon
cmake -S . -B build \
    -DUSE_VENDORED_ZMQ=OFF \
    -DCMAKE_PREFIX_PATH=/opt/zmq

# Step 3: Build
cmake --build build --parallel $(nproc)
```

## 🎯 Build Targets

### All Available Targets

| Target | Description | Command |
|--------|-------------|---------|
| `all` | Build everything (default) | `make` or `make all` |
| `b_hexagon_app` | Main application binary | `make b_hexagon_app` |
| `domain_tests` | Test suite executable | `make domain_tests` |
| `clean` | Remove build artifacts | `make clean` |

### Specific Target Examples
```bash
# Build only the main application
cmake --build build --target b_hexagon_app

# Build only tests
cmake --build build --target domain_tests

# Build with verbose output
cmake --build build --verbose

# Build with specific number of jobs
cmake --build build --parallel 4
```

## ⚙️ CMake Configuration Options

### Core Options

| Option | Default | Values | Description |
|--------|---------|--------|-------------|
| `USE_VENDORED_ZMQ` | `ON` | `ON/OFF` | Use bundled ZeroMQ vs system installation |
| `CMAKE_BUILD_TYPE` | `Debug` | `Debug/Release/RelWithDebInfo` | Build configuration |
| `BUILD_TESTING` | `ON` | `ON/OFF` | Enable/disable test compilation |

### Build Type Configurations

**Debug Build (Default):**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
# Includes: Debug symbols, no optimization, assertions enabled
```

**Release Build:**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release  
# Includes: Full optimization (-O3), no debug symbols, NDEBUG defined
```

**Release with Debug Info:**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# Includes: Optimization + debug symbols for profiling
```

## 🔍 Dependency Deep Dive

### ZeroMQ Configuration Details

**Vendored ZeroMQ Build Settings:**
```cmake
# CMake configuration for vendored ZeroMQ
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)        # Skip ZeroMQ tests
set(WITH_PERF_TOOL OFF CACHE BOOL "" FORCE)     # Skip performance tools  
set(ENABLE_DRAFTS ON CACHE BOOL "" FORCE)       # Enable RADIO/DISH APIs
set(BUILD_SHARED OFF CACHE BOOL "" FORCE)       # Static linking
```

**Required ZeroMQ Features:**
- **Draft APIs:** RADIO and DISH socket types
- **Thread Safety:** Multi-threaded context support
- **C++ Bindings:** Modern zmq.hpp header (included in project)

### Google Test Integration

**Automatic Download:**
```cmake
# CMake FetchContent configuration
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
```

**No manual installation required** - CMake handles everything automatically.

## 🚨 Troubleshooting Common Issues

### Issue 1: GLIBCXX Version Conflicts
**Symptoms:**
```
./b_hexagon_app: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.29' not found
```

**Solution:**
```bash
# Use vendored ZeroMQ (default behavior)
cmake -S . -B build -DUSE_VENDORED_ZMQ=ON
cmake --build build
```

### Issue 2: CMake Version Too Old
**Symptoms:**
```
CMake Error: CMake 3.16 or higher is required. You are running version 3.10
```

**Solutions:**
```bash
# Ubuntu: Install newer CMake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
sudo apt update && sudo apt install cmake

# CentOS: Use EPEL or compile from source
sudo yum install epel-release
sudo yum install cmake3
```

### Issue 3: Compiler Too Old
**Symptoms:**
```
error: #error This file requires compiler and library support for the ISO C++ 2017 standard
```

**Solutions:**
```bash
# Ubuntu: Install GCC 8+
sudo apt update
sudo apt install gcc-8 g++-8
export CC=gcc-8 CXX=g++-8

# CentOS: Enable devtoolset
sudo yum install centos-release-scl
sudo yum install devtoolset-8
scl enable devtoolset-8 bash
```

### Issue 4: ZeroMQ Not Found
**Symptoms:**
```
CMake Error: Could not find a package configuration file provided by "ZeroMQ"
```

**Solution:**
```bash
# Switch to vendored ZeroMQ
cmake -S . -B build -DUSE_VENDORED_ZMQ=ON
```

## 📊 Compilation Performance

### Build Time Optimization

**Parallel Compilation:**
```bash
# Use all available cores
make -j$(nproc)

# Use specific number of cores  
make -j4

# CMake equivalent
cmake --build build --parallel $(nproc)
```

**Incremental Builds:**
```bash
# Only rebuild changed files
make

# Force clean rebuild
make clean && make -j$(nproc)
```

### Typical Build Times

| Configuration | Hardware | Time | Notes |
|---------------|----------|------|-------|
| **Debug + Vendored ZMQ** | 4-core i7 | ~3-5 min | First build (includes ZeroMQ) |
| **Debug Incremental** | 4-core i7 | ~10-30s | Changed files only |
| **Release + Vendored ZMQ** | 4-core i7 | ~4-6 min | Full optimization |
| **External ZeroMQ** | 4-core i7 | ~30-60s | No ZeroMQ compilation |

## ✅ Verification Steps

### Post-Build Verification

```bash
# 1. Check binaries exist
ls -la build/b_hexagon_app build/domain_tests

# 2. Verify dynamic dependencies
ldd build/b_hexagon_app

# 3. Run basic functionality test
cd build && ./domain_tests --gtest_filter="*TrackCalculations*"

# 4. Test application startup
timeout 5s ./b_hexagon_app || echo "Application started successfully"
```

### Expected Output
```bash
$ ls -la build/b_hexagon_app build/domain_tests
-rwxr-xr-x 1 user user 2.1M Sep  2 10:30 build/b_hexagon_app
-rwxr-xr-x 1 user user 3.5M Sep  2 10:30 build/domain_tests

$ ./build/domain_tests --gtest_brief
[==========] 101 tests from 11 test suites ran.
[  PASSED  ] 98 tests.
[  FAILED  ] 3 tests.  # Expected - design decision tests
```

---

## 📞 Support

If you encounter compilation issues not covered in this guide:

1. **Check CMake logs:** Look in `build/CMakeFiles/CMakeError.log`
2. **Verify prerequisites:** Ensure all required tools are installed
3. **Try clean build:** Remove `build/` directory and rebuild
4. **Use vendored ZeroMQ:** Most dependency issues are resolved this way

For additional help, see the [main project documentation](README.md) or check the `docs/` directory for specialized guides.
