set(CMAKE_BUILD_TYPE Debug)

cmake_minimum_required(VERSION 3.10)
project(zmq_all_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Gerekli Kütüphaneleri Bulma ---
# Sistemde manuel olarak kurduğumuz libzmq'yu bulmasını istiyoruz
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

# --- Derleyici Ayarları ---
# Tüm hedefler için DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH dahil) aktive ediyoruz
add_compile_definitions(ZMQ_BUILD_DRAFT_API)

# --- Hedefleri Tanımlama ---
# App1, App2, App3 uygulamalarını derliyoruz.
add_executable(app1 app1_source.cpp)
add_executable(app2 app2_processor.cpp)
add_executable(app3 app3_listener.cpp)

# Diğer test uygulamaları
add_executable(pub_epgm pub_epgm.cpp)
add_executable(pub_udp pub_udp.cpp)
add_executable(radio_udp radio_udp.cpp)
add_executable(subscriber_radio_udp subscriber_radio_udp.cpp)

# --- Kütüphaneleri ve Yolları Tüm Hedeflere Bağlama ---
foreach(TARGET app1 app2 app3 pub_epgm pub_udp radio_udp subscriber_radio_udp)
    # Gerekli başlık dosyalarının (header) yollarını belirtiyoruz.
    target_include_directories(${TARGET} PRIVATE
        # cppzmq ve zmq.h yollarını belirtiyoruz.
        "${CMAKE_SOURCE_DIR}/../third_party/cppzmq"
        ${ZMQ_INCLUDE_DIRS}
    )
    # Gerekli kütüphaneleri bağlıyoruz (linkliyoruz).
    target_link_libraries(${TARGET} PRIVATE
        ${ZMQ_LIBRARIES}
    )
endforeach()