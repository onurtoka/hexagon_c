cmake_minimum_required(VERSION 3.16)
project(hexagon_c LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# RADIO/DISH draft API'leri gerekli değil; standart PUB/SUB kullanıyoruz

# Kaynak dosyaları (uygulama + domain + adapter katmanları)
file(GLOB_RECURSE HEXAGON_C_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(hexagon_c_app ${HEXAGON_C_SOURCES})

# Başlık dizinleri
target_include_directories(hexagon_c_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include  # üst seviyedeki ortak include (zmq/zmq.hpp burada)
)

# ZeroMQ bulunması ve linklenmesi
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_ZMQ libzmq QUIET)
endif()

if(PC_ZMQ_FOUND)
    target_include_directories(hexagon_c_app PRIVATE ${PC_ZMQ_INCLUDE_DIRS})
    target_link_directories(hexagon_c_app PRIVATE ${PC_ZMQ_LIBRARY_DIRS})
    target_link_libraries(hexagon_c_app PRIVATE ${PC_ZMQ_LIBRARIES})
else()
    # pkg-config başarısız olursa doğrudan 'zmq' kütüphanesini linklemeyi dene
    target_link_libraries(hexagon_c_app PRIVATE zmq)
endif()

find_package(Threads REQUIRED)
target_link_libraries(hexagon_c_app PRIVATE Threads::Threads)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Sources: ${HEXAGON_C_SOURCES}")
