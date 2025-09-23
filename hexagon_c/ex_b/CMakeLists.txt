cmake_minimum_required(VERSION 3.16)
project(HaT_B_CPP VERSION 1.0.0 LANGUAGES CXX)

# C++ standart ayarları
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type ayarları
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# ZeroMQ dependency
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(ZMQ libzmq QUIET)
endif()

# Alternatif ZeroMQ bulma yöntemi
if(NOT ZMQ_FOUND)
    find_path(ZMQ_INCLUDE_DIR zmq.h)
    find_library(ZMQ_LIBRARY NAMES zmq libzmq)
    
    if(ZMQ_INCLUDE_DIR AND ZMQ_LIBRARY)
        set(ZMQ_FOUND TRUE)
        set(ZMQ_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})
        set(ZMQ_LIBRARIES ${ZMQ_LIBRARY})
    endif()
endif()

if(NOT ZMQ_FOUND)
    # Fall back to standard library name
    find_library(ZMQ_LIBRARY NAMES zmq)
    if(ZMQ_LIBRARY)
        set(ZMQ_FOUND TRUE)
        set(ZMQ_LIBRARIES ${ZMQ_LIBRARY})
    else()
        message(FATAL_ERROR "ZeroMQ library not found (tried pkg-config and find_library). Install libzmq3-dev.")
    endif()
endif()

# cppzmq (ZeroMQ C++ bindings) - header-only
find_path(CPPZMQ_INCLUDE_DIR zmq.hpp
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES zmq
)

if(NOT CPPZMQ_INCLUDE_DIR)
    # Use local bundled headers under include/zmq
    if(EXISTS ${CMAKE_SOURCE_DIR}/include/zmq/zmq.hpp)
        set(CPPZMQ_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/zmq)
    else()
        message(FATAL_ERROR "cppzmq headers not found. Provide include/zmq or install libzmq3-dev.")
    endif()
endif()

# Thread support
find_package(Threads REQUIRED)

# Source files - Domain Models (will be populated with new models)
set(DOMAIN_MODEL_SOURCES
    # New models will be added here
)

# Source files - Domain Ports
set(DOMAIN_PORTS_SOURCES
    src/domain/ports/incoming/DataSubmission.hpp
    src/domain/ports/outgoing/DataPublisher.hpp
    src/domain/ports/outgoing/DataRepository.hpp
)

# Source files - Domain Logic
set(DOMAIN_LOGIC_SOURCES
    src/domain/logic/DataProcessor.hpp
)

# Source files - Adapters
set(ADAPTER_SOURCES
    src/adapters/incoming/zeromq/ZeroMQDataSubscriber.hpp
    src/adapters/outgoing/zeromq/ZeroMQRadioPublisher.hpp
)

# Ana library - Domain + Adapters
add_library(hat_b_core INTERFACE)
target_include_directories(hat_b_core INTERFACE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${ZMQ_INCLUDE_DIRS}
    ${CPPZMQ_INCLUDE_DIR}
)

target_link_libraries(hat_b_core INTERFACE
    ${ZMQ_LIBRARIES}
    Threads::Threads
)

# Ana uygulama
add_executable(hat_b_app
    src/application/main.cpp
)

target_link_libraries(hat_b_app PRIVATE
    hat_b_core
    ${ZMQ_LIBRARIES}
    Threads::Threads
)

# Test executable (opsiyonel)
option(BUILD_TESTS "Build test executable" ON)

if(BUILD_TESTS)
    set(HAT_B_TEST_SOURCES)
    foreach(f IN ITEMS test/domain/model/ModelTest.cpp test/domain/logic/DataProcessorTest.cpp)
        if(EXISTS ${CMAKE_SOURCE_DIR}/${f})
            list(APPEND HAT_B_TEST_SOURCES ${f})
        endif()
    endforeach()
    if(HAT_B_TEST_SOURCES)
        add_executable(hat_b_tests ${HAT_B_TEST_SOURCES})
        target_link_libraries(hat_b_tests PRIVATE hat_b_core)
    else()
        message(STATUS "No test sources found; skipping hat_b_tests target")
    endif()
endif()

# Install rules
install(TARGETS hat_b_app
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/hat_b
    FILES_MATCHING PATTERN "*.hpp"
)

# Package configuration
set(CPACK_PACKAGE_NAME "HaT-B-CPP")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Hexagonal Architecture System B - C++ Implementation")
set(CPACK_PACKAGE_CONTACT "your-email@example.com")

include(CPack)

# Debug bilgileri
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "ZeroMQ found: ${ZMQ_FOUND}")
message(STATUS "ZeroMQ include dirs: ${ZMQ_INCLUDE_DIRS}")
message(STATUS "ZeroMQ libraries: ${ZMQ_LIBRARIES}")
message(STATUS "cppzmq include dir: ${CPPZMQ_INCLUDE_DIR}")

# Hexagonal Architecture katmanları için ayrı target'lar (opsiyonel)
add_library(hat_b_domain INTERFACE)
target_sources(hat_b_domain INTERFACE
    ${DOMAIN_MODEL_SOURCES}
    ${DOMAIN_PORTS_SOURCES}
    ${DOMAIN_LOGIC_SOURCES}
)
target_include_directories(hat_b_domain INTERFACE ${CMAKE_SOURCE_DIR}/src)

add_library(hat_b_adapters INTERFACE)
target_sources(hat_b_adapters INTERFACE
    ${ADAPTER_SOURCES}
)
target_include_directories(hat_b_adapters INTERFACE 
    ${CMAKE_SOURCE_DIR}/src
    ${ZMQ_INCLUDE_DIRS}
    ${CPPZMQ_INCLUDE_DIR}
)
target_link_libraries(hat_b_adapters INTERFACE
    hat_b_domain
    ${ZMQ_LIBRARIES}
    Threads::Threads
)