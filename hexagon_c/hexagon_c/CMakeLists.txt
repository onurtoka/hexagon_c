cmake_minimum_required(VERSION 3.16)
project(hexagon_c LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# RADIO/DISH draft API'leri gerekli değil; standart PUB/SUB kullanıyoruz

# Seçenek: ZeroMQ draft API (RADIO/DISH) etkinleştirme
option(ENABLE_ZMQ_DRAFT "Enable ZeroMQ draft (RADIO/DISH) API" OFF)

# Kaynak dosyaları (uygulama + domain + adapter katmanları)
file(GLOB_RECURSE HEXAGON_C_ALL_SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Ortak kütüphane kaynaklarından ana uygulama main.cpp dosyasını ayır
set(HEXAGON_C_SOURCES ${HEXAGON_C_ALL_SOURCES})
list(FILTER HEXAGON_C_SOURCES EXCLUDE REGEX ".*/application/main.cpp$")

add_executable(hexagon_c_app ${HEXAGON_C_SOURCES} src/application/main.cpp)

# Radio/Dish benzeri işlemci için ayrı bir executable (apps/radio_dish/RadioDishMain.cpp)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/apps/radio_dish/RadioDishMain.cpp)
    add_executable(hexagon_c_radio_dish_app ${HEXAGON_C_SOURCES} apps/radio_dish/RadioDishMain.cpp)
endif()

# Başlık dizinleri
target_include_directories(hexagon_c_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include  # üst seviyedeki ortak include (zmq/zmq.hpp burada)
)

if(TARGET hexagon_c_radio_dish_app)
    target_include_directories(hexagon_c_radio_dish_app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
endif()

# ZeroMQ bulunması ve linklenmesi
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_ZMQ libzmq QUIET)
endif()

foreach(tgt IN ITEMS hexagon_c_app hexagon_c_radio_dish_app)
    if(TARGET ${tgt})
        if(PC_ZMQ_FOUND)
            target_include_directories(${tgt} PRIVATE ${PC_ZMQ_INCLUDE_DIRS})
            target_link_directories(${tgt} PRIVATE ${PC_ZMQ_LIBRARY_DIRS})
            target_link_libraries(${tgt} PRIVATE ${PC_ZMQ_LIBRARIES})
        else()
            target_link_libraries(${tgt} PRIVATE zmq)
        endif()
    endif()
endforeach()

find_package(Threads REQUIRED)
foreach(tgt IN ITEMS hexagon_c_app hexagon_c_radio_dish_app)
    if(TARGET ${tgt})
        target_link_libraries(${tgt} PRIVATE Threads::Threads)
        if(ENABLE_ZMQ_DRAFT)
            target_compile_definitions(${tgt} PRIVATE ZMQ_BUILD_DRAFT_API)
        endif()
    endif()
endforeach()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Sources (common): ${HEXAGON_C_SOURCES}")
message(STATUS "ENABLE_ZMQ_DRAFT: ${ENABLE_ZMQ_DRAFT}")
