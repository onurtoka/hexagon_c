# External libzmq Compilation Guide

This document provides instructions for compiling libzmq separately and configuring the b_hexagon project to use the external installation.

## Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler (GCC 8+ recommended)
- Git
- pkg-config (optional but recommended)

## Step 1: Download and Compile libzmq

### Option A: Using the vendored source (recommended)

```bash
# Navigate to the vendored libzmq directory
cd /workspaces/b_hexagon/b_hexagon/third_party/libzmq

# Create build directory
mkdir build && cd build

# Configure with draft API support (required for RADIO/DISH)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_DRAFTS=ON \
    -DBUILD_TESTS=OFF \
    -DWITH_PERF_TOOL=OFF \
    -DCMAKE_INSTALL_PREFIX=/opt/zmq

# Compile
make -j$(nproc)

# Install (may require sudo)
sudo make install
```

### Option B: Download latest from GitHub

```bash
# Clone the official libzmq repository
git clone https://github.com/zeromq/libzmq.git
cd libzmq

# Create build directory
mkdir build && cd build

# Configure with draft API support
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_DRAFTS=ON \
    -DBUILD_TESTS=OFF \
    -DWITH_PERF_TOOL=OFF \
    -DCMAKE_INSTALL_PREFIX=/opt/zmq

# Compile
make -j$(nproc)

# Install (may require sudo)
sudo make install
```

### Option C: Using package manager (if available)

```bash
# Ubuntu/Debian (may not have draft API enabled)
sudo apt-get install libzmq3-dev

# Note: Most package manager versions don't enable draft APIs
# For RADIO/DISH support, manual compilation is recommended
```

## Step 2: Configure b_hexagon Project

The b_hexagon project is now configured to use external libzmq by default. The build system will automatically detect your libzmq installation.

### Build with external libzmq (default)

```bash
cd /workspaces/b_hexagon/b_hexagon
mkdir build && cd build
cmake ..
make
```

### Verify external libzmq detection

Check the CMake output for messages like:
```
-- Using external libzmq
-- Found libzmq via pkg-config: 4.3.4
```

### Fallback to vendored libzmq (if needed)

If external libzmq is not found or doesn't have draft API support:

```bash
cmake .. -DUSE_VENDORED_ZMQ=ON
make
```

## Step 3: Verify Installation

Test that the application builds and runs correctly:

```bash
./b_hexagon_app
```

You should see output indicating successful initialization of ZeroMQ RADIO/DISH components.

## Troubleshooting

### External libzmq not found

**Error**: `External libzmq not found! Please install libzmq with draft API support`

**Solutions**:
1. Install libzmq using the instructions above
2. Set the installation path: `cmake .. -DCMAKE_PREFIX_PATH=/opt/zmq`
3. Use vendored version: `cmake .. -DUSE_VENDORED_ZMQ=ON`

### Draft API not available

**Error**: Compilation errors related to `ZMQ_RADIO`, `ZMQ_DISH`, or group operations

**Solution**: Ensure libzmq was compiled with `-DENABLE_DRAFTS=ON`

### pkg-config issues

**Warning**: `pkg-config not found, using manual libzmq search...`

**Solutions**:
1. Install pkg-config: `sudo apt-get install pkg-config`
2. Manually specify paths: 
   ```bash
   cmake .. \
       -DZMQ_LIB=/opt/zmq/lib/libzmq.so \
       -DZMQ_INCLUDE_DIR=/opt/zmq/include
   ```

## Environment Variables

You can also set these environment variables to help CMake find libzmq:

```bash
export PKG_CONFIG_PATH="/opt/zmq/lib/pkgconfig:$PKG_CONFIG_PATH"
export CMAKE_PREFIX_PATH="/opt/zmq:$CMAKE_PREFIX_PATH"
```

## Validation Commands

Verify your libzmq installation:

```bash
# Check if libzmq is installed
pkg-config --exists libzmq && echo "libzmq found" || echo "libzmq not found"

# Check libzmq version
pkg-config --modversion libzmq

# Check if draft API is available (should show symbols like zmq_radio, zmq_dish)
nm -D /opt/zmq/lib/libzmq.so | grep -E "(radio|dish)" || echo "Draft API may not be enabled"
```

## Performance Considerations

- **Static vs Dynamic**: The build system uses dynamic linking for external libzmq
- **Release builds**: Use `-DCMAKE_BUILD_TYPE=Release` for production
- **Optimization**: Consider `-DCMAKE_CXX_FLAGS="-O3 -march=native"` for architecture-specific optimizations

## Notes

- The b_hexagon project requires libzmq with draft API support for RADIO/DISH functionality
- The vendored libzmq in `third_party/libzmq` is already configured with the correct options
- External libzmq installations from package managers may not have draft APIs enabled
- Manual compilation is recommended for production deployments requiring RADIO/DISH patterns
