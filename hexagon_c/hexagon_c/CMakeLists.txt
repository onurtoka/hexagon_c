cmake_minimum_required(VERSION 3.20)
project(hexagon_c_app)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ultra optimization flags for real-time performance
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../libzmq/include)
include_directories(${CMAKE_SOURCE_DIR}/../include)

# Find required packages
find_package(Threads REQUIRED)

# Source files
set(SOURCES
    src/application/main.cpp
    src/domain/model/DelayCalcTrackData.cpp
    src/domain/model/FinalCalcTrackData.cpp
)

# Create executable
add_executable(hexagon_c_app ${SOURCES})

# Link libraries - static linking for optimal performance
target_link_libraries(hexagon_c_app 
    /workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a
    sodium
    rt
    gnutls
    pthread
    dl
)

# Real-time performance flags
target_compile_options(hexagon_c_app PRIVATE 
    -DZMQ_BUILD_DRAFT_API=1
    -pthread
    -O3
    -march=native
    -mtune=native
    -flto
    -ffast-math
    -DNDEBUG
)

# Set output directory
set_target_properties(hexagon_c_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
