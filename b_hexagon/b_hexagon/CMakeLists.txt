cmake_minimum_required(VERSION 3.16)
project(b_hexagon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_VENDORED_ZMQ "Build and link against bundled libzmq" ON)

# Core sources (explicit for clarity)
set(APP_SOURCES
    src/domain/logic/CalculatorService.cpp
    src/domain/model/ExtrapTrackData.cpp
    src/domain/model/DelayCalcTrackData.cpp
    src/adapters/outgoing/ZeroMQDataWriter.cpp
    src/adapters/incoming/ZeroMQDataHandler.cpp
    src/common/BinarySerializer.cpp
)

add_executable(b_hexagon_app
    src/application/main.cpp
    ${APP_SOURCES}
)

# Include directories
target_include_directories(b_hexagon_app PRIVATE src)

# MISRA C++ 2023 compliance flags
target_compile_options(b_hexagon_app PRIVATE
    -Wall -Wextra -Wpedantic          # General warnings
    -Wconversion -Wsign-conversion     # Type conversion warnings
    -Wcast-qual -Wcast-align          # Cast-related warnings
    -Wredundant-decls                 # Redundant declarations
    -Wunreachable-code                # Unreachable code detection
    -Wnon-virtual-dtor                # Virtual destructor warnings
    -Wold-style-cast                  # C-style cast warnings
    -Woverloaded-virtual              # Virtual function hiding warnings
    -Wshadow                          # Variable shadowing warnings
    -Wfloat-equal                     # Floating-point equality warnings
    -Wundef                           # Undefined macro warnings
)

if(USE_VENDORED_ZMQ)
    message(STATUS "Using vendored libzmq")
    # Configure minimal libzmq build (static to avoid runtime libstdc++ symbol issues)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(WITH_PERF_TOOL OFF CACHE BOOL "" FORCE)
    set(ENABLE_DRAFTS ON CACHE BOOL "" FORCE)  # Enable RADIO/DISH and draft APIs
    add_subdirectory(third_party/libzmq EXCLUDE_FROM_ALL)
    target_link_libraries(b_hexagon_app PRIVATE libzmq)
    target_link_libraries(b_hexagon_app PRIVATE stdc++fs)  # Filesystem library for GCC 8
else()
    message(STATUS "Attempting to use system libzmq")
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(PC_ZMQ libzmq QUIET)
        if(PC_ZMQ_FOUND)
            target_include_directories(b_hexagon_app PRIVATE ${PC_ZMQ_INCLUDE_DIRS})
            target_link_libraries(b_hexagon_app PRIVATE ${PC_ZMQ_LIBRARIES})
        else()
            find_library(ZMQ_LIB zmq)
            if(ZMQ_LIB)
                target_link_libraries(b_hexagon_app PRIVATE ${ZMQ_LIB})
            endif()
        endif()
    endif()
endif()

# Enable static analysis (optional, requires clang-tidy)
# Uncomment the following lines if clang-tidy is available:
# find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
# if(CLANG_TIDY_EXECUTABLE)
#     set(CMAKE_CXX_CLANG_TIDY 
#         "${CLANG_TIDY_EXECUTABLE}"
#         "-checks=-*,readability-*,modernize-*,performance-*,cert-*,misc-*"
#         "-header-filter=.*"
#     )
#     set_target_properties(b_hexagon_app PROPERTIES CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY}")
# endif()

# ===== TESTING CONFIGURATION =====
# Enable testing
enable_testing()

# Download and include Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test sources
set(TEST_SOURCES
    tests/test_main.cpp
    tests/TrackCalculationsTest.cpp
    tests/CalculatorServiceTest.cpp
    tests/ExtrapTrackDataTest.cpp
    tests/DelayCalcTrackDataTest.cpp
    tests/common/BinarySerializerTest.cpp
    tests/test_zmq_config.cpp
    tests/test_adapter_config.cpp
    tests/adapters/incoming/ZeroMQDataHandlerTest.cpp
    tests/adapters/outgoing/ZeroMQDataWriterTest.cpp
    tests/integration/EndToEndIntegrationTest.cpp
    tests/performance/PerformanceTest.cpp
)

# Domain test executable
add_executable(domain_tests
    ${TEST_SOURCES}
    ${APP_SOURCES}  # Include source files being tested
)

# Test include directories
target_include_directories(domain_tests PRIVATE 
    src
    tests
)

# Link Google Test and other dependencies
target_link_libraries(domain_tests PRIVATE
    gtest_main
    gmock_main
)

if(USE_VENDORED_ZMQ)
    target_link_libraries(domain_tests PRIVATE libzmq)
endif()

# Link filesystem library for GCC 8.5 compatibility
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(domain_tests PRIVATE stdc++fs)
endif()

# Test compile options (same as main app)
target_compile_options(domain_tests PRIVATE
    -Wall -Wextra -Wpedantic
    -Wconversion -Wsign-conversion
    -Wcast-qual -Wcast-align
    -Wredundant-decls
    -Wunreachable-code
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wshadow
    -Wfloat-equal
    -Wundef
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(domain_tests)
