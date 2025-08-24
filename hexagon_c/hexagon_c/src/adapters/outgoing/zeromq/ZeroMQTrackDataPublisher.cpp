// Header dosyasını dahil et - Bu class'ın declaration'ını içerir
#include "ZeroMQTrackDataPublisher.hpp"
// Console output için iostream (std::cout kullanılabilir)
#include <iostream>
// C string işlemleri için (memcpy fonksiyonu kullanılacak)
#include <cstring>

// Hat projesi namespace'i altında, adapters->outgoing->zeromq alt namespace'i
// :: operatörü namespace'leri birbirine bağlar (scope resolution)
namespace hat::adapters::outgoing::zeromq {

// CONSTRUCTOR - Class'ın yeni bir instance'ını oluşturur
// Bu fonksiyon class'ın bir objesi yaratıldığında otomatik çalışır
ZeroMQTrackDataPublisher::ZeroMQTrackDataPublisher(
    // const std::string& = String referansı (const = değiştirilemez, & = kopyalama yapmaz)
    // delay_calc_endpoint = DelayCalcTrackData için ZeroMQ bind adresi (örn: tcp://*:5557)
    const std::string& delay_calc_endpoint,
    // final_calc_endpoint = FinalCalcDelayData için ZeroMQ bind adresi (örn: tcp://*:5558)
    const std::string& final_calc_endpoint,
    // topic_prefix = Mesajlarda kullanılacak topic öneki (örn: "TRACK_DATA")
    const std::string& topic_prefix)
    // MEMBER INITIALIZER LIST başlangıcı - : operatörü ile başlar
    // Bu kısım member variable'ları constructor çalışmadan önce initialize eder
    : context_(1)                                   // ZeroMQ context'i 1 thread ile oluştur
    , running_(false)                               // Başlangıçta sistem durmuş durumda
    , delay_calc_endpoint_(delay_calc_endpoint)     // DelayCalc endpoint adresini kaydet
    , final_calc_endpoint_(final_calc_endpoint)     // FinalCalc endpoint adresini kaydet
    , topic_prefix_(topic_prefix) {                // Topic prefix'ini kaydet
    
    // Constructor body - Member'lar initialize edildikten sonra çalışır
    // ZeroMQ publisher'larını kurmak için private fonksiyonu çağır
    initializePublishers();
}

// DESTRUCTOR - Class'ın instance'ı yok edilirken otomatik çalışır
// ~ operatörü destructor olduğunu belirtir
ZeroMQTrackDataPublisher::~ZeroMQTrackDataPublisher() {
    // Sistem durdurmak için stop() fonksiyonunu çağır
    // Bu thread'leri güvenli şekilde kapatır ve kaynakları temizler
    stop();
}

// PUBLIC METHOD - Publisher'ı başlatır
// bool return type = true/false döner (başarı durumu)
bool ZeroMQTrackDataPublisher::start() {
    // running_ atomic variable'ı thread-safe şekilde oku
    // .load() = atomic variable'dan değeri güvenli şekilde al
    if (running_.load()) {
        // return = fonksiyondan çık ve değer döndür
        return false; // Zaten çalışıyor, başlatma başarısız
    }

    // .store() = atomic variable'a thread-safe şekilde değer yaz
    // Sistem çalışır duruma geçir
    running_.store(true);

    // std::thread = Yeni bir thread (işlem parçacığı) oluştur
    // Lambda expression = [this]() { ... } anonim fonksiyon
    // [this] = capture list, bu class'ın instance'ına erişim ver
    // () = parametre listesi (boş)
    // {} = lambda fonksiyonun gövdesi
    publisher_thread_ = std::thread([this]() {
        // Lambda içinde bu class'ın publisherWorker() metodunu çağır
        publisherWorker();
    });

    return true; // Başlatma başarılı
}

// PUBLIC METHOD - Publisher'ı durdur
// void = hiçbir değer döndürme
void ZeroMQTrackDataPublisher::stop() {
    // Atomic variable'a false yaz = sistem durduruluyor sinyali
    running_.store(false);
    
    // .notify_all() = condition_variable'ı bekleyen TÜM thread'leri uyandır
    // Worker thread'in döngüden çıkması için gerekli
    queue_cv_.notify_all();

    // .joinable() = thread hala çalışıyor mu kontrol et
    // Thread'in güvenli şekilde kapanmasını bekle
    if (publisher_thread_.joinable()) {
        // .join() = ana thread'i beklet, bu thread bitene kadar dur
        // Bu olmadan program crash olabilir
        publisher_thread_.join();
    }
}

// PUBLIC METHOD - DelayCalcTrackData yayınlar (asynchronous)
// const domain::model::DelayCalcTrackData& = Domain model referansı (const = değiştirilemez)
bool ZeroMQTrackDataPublisher::publishDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) {
    // try-catch = exception handling (hata yakalama)
    try {
        // auto = type inference: std::vector<uint8_t>
        // Domain model'i binary data'ya çevir
        auto serialized = serializeDelayCalcTrackData(data);
        
        // .empty() = vector boş mu kontrol et
        if (serialized.empty()) {
            return false; // Serialization başarısız
        }

        // PublishTask = struct, yayınlanacak görev bilgilerini tutar
        // Local variable oluştur (stack memory'de)
        PublishTask task;
        
        // = assignment operator, değer atama
        // PublishTask::DELAY_CALC = enum değeri, görev tipini belirtir
        task.type = PublishTask::DELAY_CALC;
        
        // std::move() = move semantics, kopyalama yerine taşıma yapar (performans)
        // Büyük data'ları kopyalamak yerine ownership'i transfer eder
        task.serialized_data = std::move(serialized);
        
        // Domain model'den track ID'yi al
        task.track_id = data.getTrackId();
        
        // std::chrono::steady_clock::now() = monotonic zaman damgası al
        // Performans ölçümü için başlangıç zamanını kaydet
        task.timestamp = std::chrono::steady_clock::now();

        // CRITICAL SECTION - Thread-safe queue operations
        {   // {} = scope block, lock'un yaşam süresini belirler
            // std::lock_guard<std::mutex> = RAII lock, scope'dan çıkınca otomatik unlock
            // queue_mutex_ = shared resource'u koruyan mutex
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            // .push() = queue'nun sonuna eleman ekle
            // std::move() ile task'ı queue'ya taşı (copy yapmaz)
            publish_queue_.push(std::move(task));
        }   // lock burada otomatik olarak serbest bırakılır
        
        // .notify_one() = condition_variable'ı bekleyen BİR thread'i uyandır
        // Worker thread'e yeni görev geldiğini bildir
        queue_cv_.notify_one();

        return true; // Görev queue'ya başarıyla eklendi

    // catch = exception yakalama
    // const std::exception& = exception referansı (const = değiştirilmez)
    } catch (const std::exception& e) {
        return false; // Hata oluştu
    }
}

// PUBLIC METHOD - FinalCalcDelayData yayınlar (asynchronous)
// Yukarıdaki fonksiyonla aynı mantık, sadece farklı data type
bool ZeroMQTrackDataPublisher::publishFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) {
    try {
        // FinalCalcDelayData için özel serialization fonksiyonu çağır
        auto serialized = serializeFinalCalcDelayData(data);
        
        if (serialized.empty()) {
            return false;
        }

        // Task oluştur, bu sefer FINAL_CALC tipi
        PublishTask task;
        task.type = PublishTask::FINAL_CALC; // Enum değeri farklı
        task.serialized_data = std::move(serialized);
        task.track_id = data.getTrackId();
        task.timestamp = std::chrono::steady_clock::now();

        // Thread-safe queue operation
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            publish_queue_.push(std::move(task));
        }
        queue_cv_.notify_one();

        return true;

    } catch (const std::exception& e) {
        return false;
    }
}

// PUBLIC METHOD - Publisher aktif durumu kontrol et
// const = bu fonksiyon class'ı değiştirmez (read-only)
bool ZeroMQTrackDataPublisher::isPublisherActive() const {
    // Atomic variable'dan güvenli şekilde değer oku
    return running_.load();
}

// getPublisherStats method removed - unnecessary complexity

// PRIVATE METHOD - ZeroMQ publisher'ları kurar
void ZeroMQTrackDataPublisher::initializePublishers() {
    // std::make_unique<T> = unique_ptr oluştur, heap'te T tipinde obje yarat
    // zmq::socket_t = ZeroMQ socket class'ı
    // ZMQ_PUB = publisher socket type (mesaj gönderici)
    // context_ = ZeroMQ context referansı
    delay_calc_publisher_ = std::make_unique<zmq::socket_t>(context_, ZMQ_PUB);
    
    // -> = pointer üzerinden method çağırma
    // .bind() = ZeroMQ endpoint'ini dinlemeye başla
    // .c_str() = std::string'i C-style char* array'e çevir
    delay_calc_publisher_->bind(delay_calc_endpoint_.c_str());

    // İkinci publisher için aynı işlemler
    final_calc_publisher_ = std::make_unique<zmq::socket_t>(context_, ZMQ_PUB);
    final_calc_publisher_->bind(final_calc_endpoint_.c_str());

    // PERFORMANS AYARLARI
    // int = 32-bit integer type
    // High Water Mark = kaç mesajın buffer'da bekleyebileceğini belirler
    int hwm = 10000; // Yüksek değer = daha fazla buffer, memory kullanımı artar
    
    // ZMQ_SNDHWM = send high water mark option
    // &hwm = hwm variable'ının memory adresini ver
    // sizeof(hwm) = hwm variable'ının byte cinsinden boyutu
    delay_calc_publisher_->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
    final_calc_publisher_->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
}

// PRIVATE METHOD - Worker thread'in ana fonksiyonu
// Bu fonksiyon ayrı bir thread'de sürekli çalışır
void ZeroMQTrackDataPublisher::publisherWorker() {
    // while = döngü, condition true olduğu sürece devam et
    // .load() = atomic variable'ı thread-safe oku
    while (running_.load()) {
        // std::unique_lock<std::mutex> = lock_guard'dan daha esnek, manuel unlock yapılabilir
        // queue_mutex_ = shared resource'u koruyan mutex
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        // .wait() = condition_variable ile bekle
        // İkinci parametre = lambda predicate, bu true dönünce beklemeden devam et
        // [this]() = capture this pointer
        // return = lambda'dan değer döndür
        queue_cv_.wait(lock, [this]() {
            // ! = logical NOT operator
            // .empty() = queue boş mu kontrol et
            // || = logical OR operator
            // "Queue'da iş var VEYA sistem durduruluyor" durumunda beklemeden devam et
            return !publish_queue_.empty() || !running_.load();
        });

        // Sistem durduruluyorsa döngüden çık
        if (!running_.load()) {
            // break = while döngüsünden çık
            break;
        }

        // Queue'daki tüm task'ları işle
        // .empty() = queue boş mu kontrol et
        while (!publish_queue_.empty()) {
            // .front() = queue'nun en önündeki elemanı al (FIFO - First In First Out)
            // std::move() = move semantics, kopyalama yerine taşıma
            auto task = std::move(publish_queue_.front());
            
            // .pop() = queue'nun en önündeki elemanı sil
            publish_queue_.pop();
            
            // .unlock() = mutex'i manuel olarak serbest bırak
            // Task'ı işlerken diğer thread'lerin queue'ya erişebilmesi için
            lock.unlock();

            // Actual publishing operation - mutex olmadan çalışır
            publishTask(task);

            // .lock() = mutex'i tekrar kilitle
            // Döngünün bir sonraki iterasyonu için queue'ya erişim gerekli
            lock.lock();
        } // inner while loop sonu
    } // outer while loop sonu
}

// PRIVATE METHOD - Tek bir task'ı ZeroMQ üzerinden yayınlar
// const PublishTask& = PublishTask referansı (const = değiştirilemez, & = kopyalama yapmaz)
void ZeroMQTrackDataPublisher::publishTask(const PublishTask& task) {
    // try-catch = exception handling (hata yakalama)
    try {
        // std::chrono::steady_clock::now() = monotonic zaman damgası
        // Performans ölçümü için başlangıç zamanını kaydet
        auto start_time = std::chrono::steady_clock::now();

        // Topic string'i oluştur
        // + operatörü = string concatenation (birleştirme)
        // std::to_string() = integer'ı string'e çevir
        // Format: "TRACK_DATA.123" (prefix + "." + track_id)
        std::string topic = topic_prefix_ + "." + std::to_string(task.track_id);

        // Raw pointer kullanımı (performans için)
        // zmq::socket_t* = ZeroMQ socket pointer type
        // nullptr = null pointer literal (C++11)
        zmq::socket_t* socket = nullptr;
        
        // == operatörü = equality comparison
        // Enum karşılaştırması: hangi socket'i kullanacağını belirle
        if (task.type == PublishTask::DELAY_CALC) {
            // .get() = unique_ptr'den raw pointer al
            socket = delay_calc_publisher_.get();
        } else {
            socket = final_calc_publisher_.get();
        }

        // ZeroMQ MULTIPART MESSAGE - İki parça: Topic + Data
        
        // PART 1: Topic mesajını oluştur ve gönder
        // zmq::message_t constructor = belirtilen boyutta mesaj oluştur
        // .size() = string'in byte cinsinden boyutu
        zmq::message_t topic_msg(topic.size());
        
        // memcpy() = memory copy, C fonksiyonu
        // .data() = mesajın raw memory pointer'ını al
        // .c_str() = string'i C-style char* array'e çevir
        // topic.size() = kopyalanacak byte sayısı
        memcpy(topic_msg.data(), topic.c_str(), topic.size());
        
        // .send() = ZeroMQ mesajını gönder
        // zmq::send_flags::sndmore = "daha fazla part gelecek" flag'i
        // Multipart message'ın ilk parçası olduğunu belirtir
        socket->send(topic_msg, zmq::send_flags::sndmore);

        // PART 2: Data mesajını oluştur ve gönder
        // task.serialized_data = std::vector<uint8_t> binary data
        zmq::message_t data_msg(task.serialized_data.size());
        
        // .data() = vector'ün raw data pointer'ını al
        memcpy(data_msg.data(), task.serialized_data.data(), task.serialized_data.size());
        
        // zmq::send_flags::dontwait = non-blocking mode, beklemeden gönder
        // Multipart message'ın son parçası (more flag yok)
        socket->send(data_msg, zmq::send_flags::dontwait);

        // Statistics removed - unnecessary complexity

    // catch = exception yakalama
    } catch (const std::exception& e) {
        // Error occurred - no statistics needed
    }
}

// PRIVATE METHOD - DelayCalcTrackData'yı binary format'a çevir
// std::vector<uint8_t> = byte array (8-bit unsigned integer'ların vector'ü)
// const domain::model::DelayCalcTrackData& = Domain model referansı
std::vector<uint8_t> ZeroMQTrackDataPublisher::serializeDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) {
    // try-catch = exception handling
    try {
        // std::ostringstream = output string stream, string oluşturmak için
        // Memory'de string builder gibi çalışır
        std::ostringstream oss;
        
        // .imbue() = locale ayarla
        // std::locale::classic() = "C" locale, sayıların . ile ayrılması için
        // Farklı ülkelerde , kullanılabilir, tutarlılık için . zorla
        oss.imbue(std::locale::classic());
        
        // << operatörü = stream insertion operator, data'yı stream'e yaz
        // '|' = pipe character, field separator olarak kullanılıyor
        // Domain model'den getter'larla değerleri al ve pipe ile ayırarak birleştir
        oss 
        // Y Position ECEF
            << data.getZPositionECEF() << '|'         // Z Position ECEF
            << data.getUpdateTime() << '|'            // Update Time
            << data.getOriginalUpdateTime() << '|'    // Original Update Time
            << data.getFirstHopSentTime() << '|'      // First Hop Sent Time
            << data.getFirstHopDelayTime() << '|'     // First Hop Delay Time
            << data.getSecondHopSentTime();           // Second Hop Sent Time (son field, pipe yok)
        
        // .str() = ostringstream'den final string'i al
        // const = string değiştirilemez
        const std::string s = oss.str();
        
        // std::vector<uint8_t> constructor: (başlangıç_iterator, bitiş_iterator)
        // .begin() = string'in başlangıç iterator'ü
        // .end() = string'in bitiş iterator'ü
        // String'i byte array'e çevir (char -> uint8_t conversion)
        return std::vector<uint8_t>(s.begin(), s.end());
        
    // catch (...) = tüm exception türlerini yakala
    } catch (...) {
        // Herhangi bir hata durumunda boş vector döndür
        // {} = empty initializer list, boş vector oluşturur
        return {};
    }
}

// PRIVATE METHOD - FinalCalcDelayData'yı binary format'a çevir
// Yukarıdaki fonksiyonla aynı mantık, sadak farklı data type ve daha fazla field
std::vector<uint8_t> ZeroMQTrackDataPublisher::serializeFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) {
    try {
        std::ostringstream oss;
        oss.imbue(std::locale::classic());
        
        // FinalCalcDelayData daha fazla field içeriyor (15 field)
        oss 
            << data.getTrackId() << '|'               // Track ID
            << data.getXVelocityECEF() << '|'         // X Velocity ECEF
            << data.getYVelocityECEF() << '|'         // Y Velocity ECEF
            << data.getZVelocityECEF() << '|'         // Z Velocity ECEF
            << data.getXPositionECEF() << '|'         // X Position ECEF
            << data.getYPositionECEF() << '|'         // Y Position ECEF
            << data.getZPositionECEF() << '|'         // Z Position ECEF
            << data.getUpdateTime() << '|'            // Update Time
            << data.getOriginalUpdateTime() << '|'    // Original Update Time
            << data.getFirstHopSentTime() << '|'      // First Hop Sent Time
            << data.getFirstHopDelayTime() << '|'     // First Hop Delay Time
            << data.getSecondHopSentTime() << '|'     // Second Hop Sent Time
            << data.getSecondHopDelayTime() << '|'    // Second Hop Delay Time
            << data.getTotalDelayTime() << '|'        // Total Delay Time
            << data.getThirdHopSentTime();            // Third Hop Sent Time (son field)
            
        const std::string s = oss.str();
        return std::vector<uint8_t>(s.begin(), s.end());
        
    } catch (...) {
        return {}; // Boş vector = serialization başarısız
    }
}

// Namespace kapatma
} // namespace hat::adapters::outgoing::zeromq