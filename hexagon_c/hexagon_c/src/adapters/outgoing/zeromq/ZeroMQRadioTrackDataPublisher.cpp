// DRAFT API'ler CMake üzerinden tanımlanıyor; burada yeniden tanımlamaya gerek yok
#ifdef ZMQ_BUILD_DRAFT_API
#else
#define ZMQ_BUILD_DRAFT_API
#endif

#include "ZeroMQRadioTrackDataPublisher.hpp"
#include <iostream>
#include <cstring>
#include <cassert>

namespace hat::adapters::outgoing::zeromq {

// Hata kontrolü yardımcı fonksiyonu (app2_processor.cpp'den uyarlandı)
void ZeroMQRadioTrackDataPublisher::check_rc(int rc, const std::string& context_msg) {
    if (rc != 0) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        throw std::runtime_error("ZMQ Error: " + context_msg);
    }
}

ZeroMQRadioTrackDataPublisher::ZeroMQRadioTrackDataPublisher(
    const std::string& multicast_endpoint,
    const std::string& group_name)
    : running_(false)
    , multicast_endpoint_(multicast_endpoint)
    , group_name_(group_name)
    , zmq_context_(nullptr)
    , radio_socket_(nullptr) {
    
    initializeRadioSocket();
}

ZeroMQRadioTrackDataPublisher::~ZeroMQRadioTrackDataPublisher() {
    stop();
    
    // Socket'leri temizle
    if (radio_socket_) {
        zmq_close(radio_socket_);
    }
    if (zmq_context_) {
        zmq_ctx_term(zmq_context_);
    }
}

void ZeroMQRadioTrackDataPublisher::initializeRadioSocket() {
    try {
        // ZeroMQ context oluştur
        zmq_context_ = zmq_ctx_new();
        assert(zmq_context_);

        // RADIO socket oluştur (app2_processor.cpp pattern'i)
    radio_socket_ = zmq_socket(zmq_context_, ZMQ_PUB);
        assert(radio_socket_);

        std::cout << "[RadioPublisher] Radio socket oluşturuldu." << std::endl;
        std::cout << "[RadioPublisher] Multicast endpoint: " << multicast_endpoint_ << std::endl;
        std::cout << "[RadioPublisher] Grup adı: " << group_name_ << std::endl;

        // Multicast endpoint'e connect
        check_rc(zmq_connect(radio_socket_, multicast_endpoint_.c_str()), "RADIO zmq_connect");
        
        std::cout << "[RadioPublisher] Radio socket başarıyla bağlandı." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] Initialize hatası: " << e.what() << std::endl;
        throw;
    }
}

bool ZeroMQRadioTrackDataPublisher::start() {
    if (running_.load()) {
        return false; // Zaten çalışıyor
    }

    running_.store(true);

    // Publisher worker thread'ini başlat
    publisher_thread_ = std::thread([this]() {
        publisherWorker();
    });

    std::cout << "[RadioPublisher] Publisher başlatıldı." << std::endl;
    return true;
}

void ZeroMQRadioTrackDataPublisher::stop() {
    running_.store(false);
    
    // Worker thread'i uyandır
    queue_cv_.notify_all();

    if (publisher_thread_.joinable()) {
        publisher_thread_.join();
    }

    std::cout << "[RadioPublisher] Publisher durduruldu." << std::endl;
}

bool ZeroMQRadioTrackDataPublisher::publishDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) {
    try {
        // Zaman damgası ile serialize et
        auto serialized = serializeDelayCalcTrackDataWithTimestamp(data);
        
        if (serialized.empty()) {
            return false;
        }

        // Task oluştur ve kuyruğa ekle
        RadioPublishTask task;
        task.type = RadioPublishTask::DELAY_CALC;
        task.serialized_data = std::move(serialized);
        task.timestamp = std::chrono::steady_clock::now();

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            publish_queue_.push(std::move(task));
        }

        // Worker thread'i uyandır
        queue_cv_.notify_one();
        
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] DelayCalcTrackData publish hatası: " << e.what() << std::endl;
        return false;
    }
}

bool ZeroMQRadioTrackDataPublisher::publishFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) {
    try {
        // Zaman damgası ile serialize et
        auto serialized = serializeFinalCalcDelayDataWithTimestamp(data);
        
        if (serialized.empty()) {
            return false;
        }

        // Task oluştur ve kuyruğa ekle
        RadioPublishTask task;
        task.type = RadioPublishTask::FINAL_CALC;
        task.serialized_data = std::move(serialized);
        task.timestamp = std::chrono::steady_clock::now();

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            publish_queue_.push(std::move(task));
        }

        // Worker thread'i uyandır
        queue_cv_.notify_one();
        
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] FinalCalcDelayData publish hatası: " << e.what() << std::endl;
        return false;
    }
}

bool ZeroMQRadioTrackDataPublisher::isPublisherActive() const {
    return running_.load();
}

void ZeroMQRadioTrackDataPublisher::publisherWorker() {
    std::cout << "[RadioPublisher] Worker thread başlatıldı." << std::endl;

    while (running_.load()) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        // Kuyrukta task bekle
        queue_cv_.wait(lock, [this] { 
            return !publish_queue_.empty() || !running_.load(); 
        });

        // Sistem durdurulmuşsa çık
        if (!running_.load()) {
            break;
        }

        // Task'ları işle
        while (!publish_queue_.empty()) {
            auto task = std::move(publish_queue_.front());
            publish_queue_.pop();
            
            lock.unlock(); // Publish sırasında mutex'i bırak
            
            publishRadioTask(task);
            
            lock.lock(); // Tekrar kilitle
        }
    }

    std::cout << "[RadioPublisher] Worker thread sonlandırıldı." << std::endl;
}

void ZeroMQRadioTrackDataPublisher::publishRadioTask(const RadioPublishTask& task) {
    try {
        // app2_processor.cpp pattern'ini takip et
        zmq_msg_t msg;
        check_rc(zmq_msg_init_size(&msg, task.serialized_data.size()), "zmq_msg_init_size");
        
        // Veriyi mesaja kopyala
        memcpy(zmq_msg_data(&msg), task.serialized_data.c_str(), task.serialized_data.size());

        // Draft API (zmq_msg_set_group) yerine topic prefiksi ekliyoruz
        // Basitçe mesajın başına grup adını ve ayırıcıyı ekleyelim
        std::string grouped_data = group_name_ + ":" + task.serialized_data;
        if(grouped_data.size() != task.serialized_data.size()) {
            // allocate new frame with grouped prefix
            zmq_msg_close(&msg);
            check_rc(zmq_msg_init_size(&msg, grouped_data.size()), "zmq_msg_init_size (grouped)");
            memcpy(zmq_msg_data(&msg), grouped_data.data(), grouped_data.size());
        }

        // Mesajı gönder
        int sent_bytes = zmq_msg_send(&msg, radio_socket_, 0);
        assert(sent_bytes == (int)task.serialized_data.size());
        
        std::cout << "[RadioPublisher] Mesaj gönderildi - Grup: " << group_name_ 
                  << ", Boyut: " << sent_bytes << " byte" << std::endl;

        zmq_msg_close(&msg);

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] Publish task hatası: " << e.what() << std::endl;
    }
}

std::string ZeroMQRadioTrackDataPublisher::serializeDelayCalcTrackDataWithTimestamp(
    const domain::model::DelayCalcTrackData& data) {
    
    try {
        // app2_processor.cpp pattern'ini takip et - zaman damgası ekle
        auto forward_time = std::chrono::high_resolution_clock::now();
        auto forward_ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
            forward_time.time_since_epoch()).count();

        // Basit JSON-like serialization (gerçek uygulamada JSON library kullanılabilir)
        std::stringstream ss;
    ss << "{" 
       << "\"type\":\"DelayCalcTrackData\"," 
       << "\"track_id\":" << data.getTrackId() << "," 
       // Orijinal modelde x/y alanları yok; örnek amaçlı ECEF X/Y pozisyonları kullanılıyor
       << "\"x_pos_ecef\":" << data.getXPositionECEF() << "," 
       << "\"y_pos_ecef\":" << data.getYPositionECEF() << "," 
       << "\"timestamp\":" << forward_ns_since_epoch 
       << "}|" << forward_ns_since_epoch;  // app2_processor pattern - zaman damgası sonunda

        return ss.str();

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] DelayCalcTrackData serialization hatası: " << e.what() << std::endl;
        return "";
    }
}

std::string ZeroMQRadioTrackDataPublisher::serializeFinalCalcDelayDataWithTimestamp(
    const domain::model::FinalCalcDelayData& data) {
    
    try {
        // app2_processor.cpp pattern'ini takip et - zaman damgası ekle
        auto forward_time = std::chrono::high_resolution_clock::now();
        auto forward_ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
            forward_time.time_since_epoch()).count();

        // Basit JSON-like serialization
        std::stringstream ss;
    ss << "{" 
       << "\"type\":\"FinalCalcDelayData\"," 
       << "\"track_id\":" << data.getTrackId() << "," 
       // Modelde public total/processing latency alanları yok; mevcut getter'ları kullanıyoruz
       << "\"total_delay_time\":" << data.getTotalDelayTime() << "," 
       << "\"first_hop_delay\":" << data.getFirstHopDelayTime() << "," 
       << "\"second_hop_delay\":" << data.getSecondHopDelayTime() << "," 
       << "\"timestamp\":" << forward_ns_since_epoch 
       << "}|" << forward_ns_since_epoch;  // app2_processor pattern

        return ss.str();

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] FinalCalcDelayData serialization hatası: " << e.what() << std::endl;
        return "";
    }
}

} // namespace hat::adapters::outgoing::zeromq
