cmake_minimum_required(VERSION 3.16)
project(HaT_CPP VERSION 1.0.0 LANGUAGES CXX)

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
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

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
    message(FATAL_ERROR "ZeroMQ library not found. Please install libzmq-dev")
endif()

# cppzmq (ZeroMQ C++ bindings) - header-only
find_path(CPPZMQ_INCLUDE_DIR zmq.hpp
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES zmq
)

if(NOT CPPZMQ_INCLUDE_DIR)
    message(FATAL_ERROR "cppzmq headers not found. Please install libzmq3-dev or cppzmq")
endif()

# Thread support
find_package(Threads REQUIRED)

# Source files - Domain Models
set(DOMAIN_MODEL_SOURCES
    src/domain/model/DelayCalcTrackData.hpp
    src/domain/model/FinalCalcDelayData.hpp
)

# Source files - Domain Ports
set(DOMAIN_PORTS_SOURCES
    src/domain/ports/incoming/TrackDataSubmission.hpp
    src/domain/ports/outgoing/TrackDataPublisher.hpp
    src/domain/ports/outgoing/TrackDataRepository.hpp
)

# Source files - Domain Logic
set(DOMAIN_LOGIC_SOURCES
    src/domain/logic/TrackDataProcessor.hpp
)

# Source files - Adapters
set(ADAPTER_SOURCES
    src/adapters/incoming/zeromq/ZeroMQTrackDataSubscriber.hpp
    src/adapters/outgoing/zeromq/ZeroMQTrackDataPublisher.hpp
)

# Ana library - Domain + Adapters
add_library(hat_core INTERFACE)
target_include_directories(hat_core INTERFACE
    ${CMAKE_SOURCE_DIR}/src
    ${ZMQ_INCLUDE_DIRS}
    ${CPPZMQ_INCLUDE_DIR}
)

target_link_libraries(hat_core INTERFACE
    ${ZMQ_LIBRARIES}
    Threads::Threads
)

# Ana uygulama
add_executable(hat_app
    src/application/main.cpp
)

target_link_libraries(hat_app PRIVATE
    hat_core
    ${ZMQ_LIBRARIES}
    Threads::Threads
)

# Test executable (opsiyonel)
option(BUILD_TESTS "Build test executable" ON)

if(BUILD_TESTS)
    add_executable(hat_tests
        test/domain/model/DelayCalcTrackDataTest.cpp
        test/domain/model/FinalCalcDelayDataTest.cpp
        test/domain/logic/TrackDataProcessorTest.cpp
    )
    
    target_link_libraries(hat_tests PRIVATE
        hat_core
    )
endif()

# Install rules
install(TARGETS hat_app
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/hat
    FILES_MATCHING PATTERN "*.hpp"
)

# Package configuration
set(CPACK_PACKAGE_NAME "HaT-CPP")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Hexagonal Architecture Tracking System - C++ Implementation")
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
add_library(hat_domain INTERFACE)
target_sources(hat_domain INTERFACE
    ${DOMAIN_MODEL_SOURCES}
    ${DOMAIN_PORTS_SOURCES}
    ${DOMAIN_LOGIC_SOURCES}
)
target_include_directories(hat_domain INTERFACE ${CMAKE_SOURCE_DIR}/src)

add_library(hat_adapters INTERFACE)
target_sources(hat_adapters INTERFACE
    ${ADAPTER_SOURCES}
)
target_include_directories(hat_adapters INTERFACE 
    ${CMAKE_SOURCE_DIR}/src
    ${ZMQ_INCLUDE_DIRS}
    ${CPPZMQ_INCLUDE_DIR}
)
target_link_libraries(hat_adapters INTERFACE
    hat_domain
    ${ZMQ_LIBRARIES}
    Threads::Threads
)