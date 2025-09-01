// DRAFT API'leri etkinleştirmek için
#define ZMQ_BUILD_DRAFT_API

#include "ZeroMQDishTrackDataSubscriber.hpp"
#include <iostream>
#include <zmq.hpp> // C++ wrapper için

namespace hat::adapters::incoming::zeromq {

// Default constructor with standard configuration
ZeroMQDishTrackDataSubscriber::ZeroMQDishTrackDataSubscriber(
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission)
    : track_data_submission_(track_data_submission)
    , running_(false)
    , multicast_endpoint_("udp://239.1.1.1:9002")  // Port 9002 for DelayCalcTrackData from B_hexagon
    , group_name_("SOURCE_DATA")                    // Default group name
    , zmq_context_(1)  // 1 I/O thread
    , dish_socket_(nullptr) {
    
    initializeDishSocket();
}

// Custom configuration constructor
ZeroMQDishTrackDataSubscriber::ZeroMQDishTrackDataSubscriber(
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
    const std::string& multicast_endpoint,
    const std::string& group_name)
    : track_data_submission_(track_data_submission)
    , running_(false)
    , multicast_endpoint_(multicast_endpoint)
    , group_name_(group_name)
    , zmq_context_(1)  // 1 I/O thread
    , dish_socket_(nullptr) {
    
    initializeDishSocket();
}

ZeroMQDishTrackDataSubscriber::~ZeroMQDishTrackDataSubscriber() {
    stop();
    // C++ wrapper RAII ile otomatik cleanup yapar
}

void ZeroMQDishTrackDataSubscriber::initializeDishSocket() {
    try {
        std::cout << "🔧 ZeroMQ DISH Configuration:" << std::endl;
        std::cout << "   📡 Endpoint: " << multicast_endpoint_ << std::endl;
        std::cout << "   👥 Group: " << group_name_ << std::endl;
        
        // DISH socket oluştur (C++ wrapper ile) - Draft API gerekli
        dish_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::dish);

        // Performance optimizations
        dish_socket_->set(zmq::sockopt::rcvhwm, 0);       // Unlimited receive buffer
        dish_socket_->set(zmq::sockopt::rcvtimeo, 100);   // 100ms timeout for graceful shutdown
        dish_socket_->set(zmq::sockopt::linger, 0);       // No linger on close
        dish_socket_->set(zmq::sockopt::immediate, 1);    // Process messages immediately
        
        // UDP multicast için DISH socket bind yapar
        dish_socket_->bind(multicast_endpoint_);
        
        // Gruba join ol (DISH için)
        dish_socket_->join(group_name_.c_str());

    } catch (const zmq::error_t& e) {
        std::cerr << "[DishSubscriber] ZMQ Initialize hatası: " << e.what() << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] Initialize hatası: " << e.what() << std::endl;
        throw;
    }
}

bool ZeroMQDishTrackDataSubscriber::start() {
    if (running_.load()) {
        return false; // Zaten çalışıyor
    }

    running_.store(true);

    // Subscriber worker thread'ini başlat
    subscriber_thread_ = std::thread([this]() {
        // Real-time thread priority ayarla
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = 95; // Max priority
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        
        // CPU affinity - dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(1, &cpuset); // Core 1'e bind et
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        subscriberWorker();
    });

    return true;
}

void ZeroMQDishTrackDataSubscriber::stop() {
    running_.store(false);

    if (subscriber_thread_.joinable()) {
        subscriber_thread_.join();
    }
}

bool ZeroMQDishTrackDataSubscriber::isRunning() const {
    return running_.load();
}

void ZeroMQDishTrackDataSubscriber::subscriberWorker() {
    while (running_.load()) {
        try {
            // C++ wrapper ile mesaj alma
            zmq::message_t received_msg;
            
            // Blocking receive with timeout (daha verimli)
            auto result = dish_socket_->recv(received_msg, zmq::recv_flags::none);
            
            if (!result) {
                continue;
            }

            // Mesajı string'e çevir (C++ wrapper)
            std::string received_payload = received_msg.to_string();

            // app2_processor.cpp pattern'i - gecikme hesaplama
            auto latency_info = processReceivedMessage(received_payload);

            // Veriyi deserialize et
            auto track_data = deserializeDelayCalcTrackData(latency_info.original_data, latency_info);

            if (track_data.has_value() && track_data_submission_) {
                // Domain katmanına gönder
                track_data_submission_->submitDelayCalcTrackData(track_data.value());
                
                // Doğru toplam gecikme hesapla: şu anki zaman - ilk gönderim zamanı
                auto receive_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                    latency_info.receive_time.time_since_epoch()).count();
                long total_latency_us = receive_time_us - track_data->getFirstHopSentTime();
                
                std::cout << "📡 Track " << track_data->getTrackId() 
                          << " alındı - Toplam Gecikme: " << total_latency_us << " μs" << std::endl;
            }

        } catch (const zmq::error_t& e) {
            if (e.num() != EAGAIN) {  // EAGAIN = mesaj yok, normal durum
                std::cerr << "[DishSubscriber] ZMQ Worker thread hatası: " << e.what() << std::endl;
            }
            // Hata durumunda çok kısa bekle
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        } catch (const std::exception& e) {
            std::cerr << "[DishSubscriber] Worker thread hatası: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

ZeroMQDishTrackDataSubscriber::LatencyMeasurement 
ZeroMQDishTrackDataSubscriber::processReceivedMessage(const std::string& received_payload) {
    
    LatencyMeasurement result;
    
    // app2_processor.cpp pattern'ini takip et
    // 1. Mesajı alır almaz mevcut zamanı kaydet (steady_clock daha kararlı)
    result.receive_time = std::chrono::steady_clock::now();

    // 2. Mesajı '|' karakterinden ayırarak orijinal metni ve gönderim zamanını bul
    size_t separator_pos = received_payload.find_last_of('|');
    if (separator_pos == std::string::npos) {
        std::cerr << "[DishSubscriber] Hata: Alınan mesajda zaman damgası ayıracı ('|') bulunamadı." << std::endl;
        result.original_data = received_payload;
        result.send_timestamp_us = 0;
        result.latency_us = 0;
        return result;
    }

    result.original_data = received_payload.substr(0, separator_pos);
    try {
        result.send_timestamp_us = std::stoll(received_payload.substr(separator_pos + 1));
    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] Zaman damgası parse hatası: " << e.what() << std::endl;
        result.send_timestamp_us = 0;
        result.latency_us = 0;
        return result;
    }

    // 3. Gecikmeyi hesapla
    auto receive_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
        result.receive_time.time_since_epoch()).count();
    result.latency_us = receive_time_us - result.send_timestamp_us;

    return result;
}

std::optional<domain::model::DelayCalcTrackData> 
ZeroMQDishTrackDataSubscriber::deserializeDelayCalcTrackData(
    const std::string& original_data, 
    const LatencyMeasurement& latency_info) {
    
    try {
        // A'dan B'ye nasıl başarılı data akışı var ise, B'den C'ye de aynı yöntem kullanılacak
        // B_hexagon'un JSON parsing yöntemi - basit ve güvenli approach
        domain::model::DelayCalcTrackData data; // default constructed
        
        // B_hexagon'un başarılı parsing metodunu kullan
        parseJsonFieldsLikeBHexagon(original_data, data);
        
        return data;

    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] B_hexagon-style deserialization hatası: " << e.what() << std::endl;
        std::cerr << "[DishSubscriber] Problematic data: " << original_data << std::endl;
        return std::nullopt;
    }
}

// B_Hexagon'un JSON parsing yöntemini kopyala - basit ve güvenli
void ZeroMQDishTrackDataSubscriber::parseJsonFieldsLikeBHexagon(
    const std::string& json, domain::model::DelayCalcTrackData& data) {
    
    std::size_t pos = 0;
    auto skipWhitespace = [&]() {
        while (pos < json.length() && std::isspace(static_cast<unsigned char>(json[pos]))) {
            ++pos;
        }
    };
    
    skipWhitespace();
    if (pos >= json.length() || json[pos] != '{') {
        return; // Invalid JSON object
    }
    
    ++pos; // Skip opening brace
    
    while (pos < json.length()) {
        skipWhitespace();
        
        if (pos >= json.length()) break;
        if (json[pos] == '}') break; // End of object
        
        // Extract field name
        if (json[pos] != '"') {
            ++pos;
            continue;
        }
        
        try {
            // Extract field name like B_Hexagon does
            ++pos; // Skip opening quote
            std::size_t start = pos;
            
            // Find closing quote
            while (pos < json.length() && json[pos] != '"') {
                if (json[pos] == '\\') {
                    ++pos; // Skip escaped character
                }
                ++pos;
            }
            
            if (pos >= json.length()) break;
            
            std::string fieldName = json.substr(start, pos - start);
            ++pos; // Skip closing quote
            
            skipWhitespace();
            if (pos >= json.length() || json[pos] != ':') {
                ++pos;
                continue;
            }
            ++pos; // Skip colon
            
            skipWhitespace();
            
            // Parse field values exactly like B_Hexagon does
            if (fieldName == "trackId") {
                data.setTrackId(std::stoul(json.substr(pos)));
            } else if (fieldName == "xVelocityECEF") {
                data.setVelocityECEF(std::stod(json.substr(pos)), data.getYVelocityECEF(), data.getZVelocityECEF());
            } else if (fieldName == "yVelocityECEF") {
                data.setVelocityECEF(data.getXVelocityECEF(), std::stod(json.substr(pos)), data.getZVelocityECEF());
            } else if (fieldName == "zVelocityECEF") {
                data.setVelocityECEF(data.getXVelocityECEF(), data.getYVelocityECEF(), std::stod(json.substr(pos)));
            } else if (fieldName == "xPositionECEF") {
                data.setPositionECEF(std::stod(json.substr(pos)), data.getYPositionECEF(), data.getZPositionECEF());
            } else if (fieldName == "yPositionECEF") {
                data.setPositionECEF(data.getXPositionECEF(), std::stod(json.substr(pos)), data.getZPositionECEF());
            } else if (fieldName == "zPositionECEF") {
                data.setPositionECEF(data.getXPositionECEF(), data.getYPositionECEF(), std::stod(json.substr(pos)));
            } else if (fieldName == "updateTime") {
                data.setUpdateTime(std::stoull(json.substr(pos)));
            } else if (fieldName == "originalUpdateTime") {
                data.setOriginalUpdateTime(std::stoull(json.substr(pos)));
            } else if (fieldName == "firstHopSentTime") {
                data.setFirstHopSentTime(std::stoull(json.substr(pos)));
            } else if (fieldName == "firstHopDelayTime") {
                data.setFirstHopDelayTime(std::stoull(json.substr(pos)));
            } else if (fieldName == "secondHopSentTime") {
                data.setSecondHopSentTime(std::stoull(json.substr(pos)));
            }
            
            // Skip to next field like B_Hexagon does
            while (pos < json.length() && json[pos] != ',' && json[pos] != '}') {
                ++pos;
            }
            if (pos < json.length() && json[pos] == ',') {
                ++pos;
            }
            
        } catch (const std::exception&) {
            // Skip invalid field exactly like B_Hexagon does
            ++pos;
        }
    }
}

} // namespace hat::adapters::incoming::zeromq
