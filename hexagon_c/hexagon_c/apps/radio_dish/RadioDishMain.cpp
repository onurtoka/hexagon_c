#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include <cstring>
#include <zmq.h>

#ifdef ZMQ_BUILD_DRAFT_API
#define USE_RADIO_DISH 1
#else
#define USE_RADIO_DISH 0
#endif

static std::atomic<bool> running(true);

void signalHandler(int) { std::cout << "\n[RadioDish] Signal received, shutting down..." << std::endl; running.store(false); }

static void check_rc(int rc, const char* where) { if(rc != 0){ std::cerr << where << " failed: (" << zmq_errno() << ") " << zmq_strerror(zmq_errno()) << std::endl; throw std::runtime_error(std::string("ZMQ error at ")+where);} }

int main(int, char**) {
    signal(SIGINT, signalHandler); signal(SIGTERM, signalHandler);
    std::cout << "=== RadioDish Processor ===" << std::endl;
    std::cout << "Draft API (RADIO/DISH) Enabled (compile): " << (USE_RADIO_DISH?"YES":"NO") << std::endl;

#if USE_RADIO_DISH
    std::cout << "Warning: Library may still lack draft symbols; will fallback if link errors would occur." << std::endl;
#endif

#if USE_RADIO_DISH
    // Derleme esnasında draft aktif; ancak link zamanı sembolleri yoksa fallback kullanacağız.
#endif

    // Konfigürasyon
    const char* in_endpoint  = "udp://239.1.1.1:9001";
    const char* in_group     = "SOURCE_DATA";
    const char* out_endpoint = "udp://239.1.1.1:9002";
    const char* out_group    = "PROCESSED_DATA";

#if USE_RADIO_DISH
    bool draft_runtime_enabled = true; // Varsayılan dene
#else
    bool draft_runtime_enabled = false;
#endif

    void* ctx = zmq_ctx_new(); if(!ctx){ std::cerr << "Failed to create ZMQ context" << std::endl; return 1; }
    void* in_sock = nullptr; void* out_sock = nullptr;

    if(draft_runtime_enabled){
#if USE_RADIO_DISH
        in_sock = zmq_socket(ctx, ZMQ_DISH);
        if(!in_sock){ draft_runtime_enabled=false; }
        else {
            if(zmq_bind(in_sock, in_endpoint)!=0 || zmq_join(in_sock, in_group)!=0){
                std::cerr << "[Draft] DISH setup failed -> fallback PUB/SUB." << std::endl; zmq_close(in_sock); in_sock=nullptr; draft_runtime_enabled=false; }
        }
#else
        draft_runtime_enabled=false;
#endif
    }

    if(!draft_runtime_enabled){
        in_sock = zmq_socket(ctx, ZMQ_SUB);
        if(!in_sock){ std::cerr << "Failed to create SUB socket" << std::endl; zmq_ctx_term(ctx); return 1; }
        check_rc(zmq_connect(in_sock, in_endpoint), "zmq_connect(SUB)");
        check_rc(zmq_setsockopt(in_sock, ZMQ_SUBSCRIBE, "", 0), "zmq_setsockopt(SUBSCRIBE)");
    }

    if(draft_runtime_enabled){
#if USE_RADIO_DISH
        out_sock = zmq_socket(ctx, ZMQ_RADIO);
        if(!out_sock || zmq_connect(out_sock, out_endpoint)!=0){
            if(out_sock) zmq_close(out_sock); out_sock=nullptr; draft_runtime_enabled=false; std::cerr << "[Draft] RADIO setup failed -> fallback PUB/SUB." << std::endl; }
#endif
    }
    if(!draft_runtime_enabled){
        if(out_sock) { zmq_close(out_sock); out_sock=nullptr; }
        out_sock = zmq_socket(ctx, ZMQ_PUB);
        if(!out_sock){ std::cerr << "Failed to create PUB socket" << std::endl; zmq_close(in_sock); zmq_ctx_term(ctx); return 1; }
        check_rc(zmq_bind(out_sock, out_endpoint), "zmq_bind(PUB)");
    }

    std::cout << "Mode: " << (draft_runtime_enabled?"RADIO/DISH":"PUB/SUB Fallback") << std::endl;
    std::cout << "Input:  " << in_endpoint  << (draft_runtime_enabled?" group=":" topic=") << (draft_runtime_enabled?in_group:"<any>") << std::endl;
    std::cout << "Output: " << out_endpoint << (draft_runtime_enabled?" group=":" topic=") << (draft_runtime_enabled?out_group:"broadcast") << std::endl;

    int rcv_timeout = 500; zmq_setsockopt(in_sock, ZMQ_RCVTIMEO, &rcv_timeout, sizeof(rcv_timeout));

    while(running.load()){
        zmq_msg_t msg; zmq_msg_init(&msg); int bytes = zmq_msg_recv(&msg, in_sock, 0);
        if(bytes == -1){ zmq_msg_close(&msg); if(zmq_errno()==EAGAIN) continue; break; }
        auto now = std::chrono::high_resolution_clock::now(); long long now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        std::string payload(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg)); zmq_msg_close(&msg);
        size_t sep = payload.find_last_of('|'); if(sep==std::string::npos){ std::cerr << "Malformed message" << std::endl; continue; }
        long long sent_ns = 0; try{ sent_ns = std::stoll(payload.substr(sep+1)); }catch(...){ }
        double latency_us = (now_ns - sent_ns)/1000.0;
        std::cout << "[Proc] IN latency(us): " << latency_us << " data=" << payload.substr(0, sep) << std::endl;
        std::stringstream ss; ss << payload.substr(0, sep) << "|" << now_ns; std::string out_data = ss.str();
        zmq_msg_t outm; check_rc(zmq_msg_init_size(&outm, out_data.size()), "zmq_msg_init_size(out)");
        memcpy(zmq_msg_data(&outm), out_data.data(), out_data.size());
#if USE_RADIO_DISH
        if(draft_runtime_enabled) {
            if(zmq_msg_set_group(&outm, out_group)!=0){ std::cerr << "set_group failed" << std::endl; }
        }
#endif
        int s = zmq_msg_send(&outm, out_sock, 0); if(s != (int)out_data.size()) std::cerr << "Partial send" << std::endl; zmq_msg_close(&outm);
    }

    zmq_close(in_sock); zmq_close(out_sock); zmq_ctx_term(ctx);
    std::cout << "[RadioDish] Shutdown complete." << std::endl; return 0; }
