cmake_minimum_required(VERSION 3.20)
project(hexagon_c_app)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ultra optimization flags for real-time performance
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

# Enable testing
enable_testing()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../libzmq/include)
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR})

# Find required packages
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add ZeroMQ library
add_library(zmq STATIC IMPORTED)
set_target_properties(zmq PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../libzmq/build/lib/libzmq.a"
)

# Source files
set(SOURCES
    src/domain/model/DelayCalcTrackData.cpp
    src/domain/model/FinalCalcTrackData.cpp
    src/domain/logic/TrackDataProcessor.cpp
)

# Test files
set(TEST_SOURCES
    tests/unit/domain/logic/FinalCalculatorServiceTest.cpp
    tests/domain/model/DelayCalcTrackData_test.cpp
    tests/domain/logic/TrackDataProcessor_test.cpp
)

# Library target for shared code
add_library(hexagon_core STATIC ${SOURCES})
target_link_libraries(hexagon_core 
    PUBLIC 
    Threads::Threads
    zmq
    gnutls
)

# Main executable
add_executable(${PROJECT_NAME}
    src/application/main.cpp
)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    hexagon_core
    zmq
    gnutls
)

# Test executable
add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    hexagon_core
    zmq
    gnutls
)

# Register tests
add_test(NAME unit_tests COMMAND run_tests)
