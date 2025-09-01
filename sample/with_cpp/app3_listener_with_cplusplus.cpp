#define ZMQ_BUILD_DRAFT_API

#include <zmq.hpp>         // C++ ZeroMQ binding
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

void check_rc(bool condition, const std::string& context_msg) {
    if (!condition) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        exit(1);
    }
}

// Gelen mesajı ayrıştırıp gecikmeyi hesaplayan ve yazdıran bir yardımcı fonksiyon
void process_message(zmq::message_t& msg, const std::string& source_name) {
    auto receive_time = std::chrono::high_resolution_clock::now();

    std::string payload(static_cast<char*>(msg.data()), msg.size());

    size_t separator_pos = payload.find_last_of('|');
    if (separator_pos == std::string::npos) {
        std::cerr << "[" << source_name << "] Hata: Alınan mesajda zaman damgası ayıracı ('|') bulunamadı." << std::endl;
        return;
    }

    std::string original_data = payload.substr(0, separator_pos);
    long long send_timestamp_ns = 0;
    try {
        send_timestamp_ns = std::stoll(payload.substr(separator_pos + 1));
    } catch (const std::invalid_argument& e) {
        std::cerr << "[" << source_name << "] Hata: Geçersiz zaman damgası formatı." << std::endl;
        return;
    }

    auto receive_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(receive_time.time_since_epoch()).count();
    long long latency_ns = receive_time_ns - send_timestamp_ns;

    std::cout << "\n---[ " << source_name << " MESAJI ALINDI ]---" << std::endl;
    std::cout << "  Orijinal Mesaj: \"" << original_data << "\"" << std::endl;
    std::cout << "  Gecikme: " << latency_ns / 1000.0 << " mikrosaniye" << std::endl;
}

int main() {
    // Dinlenecek endpoint'ler ve gruplar
    const std::string source_endpoint = "udp://239.1.1.1:9001";
    const std::string source_group = "SOURCE_DATA";

    const std::string processed_endpoint = "udp://239.1.1.1:9002";
    const std::string processed_group = "PROCESSED_DATA";

    zmq::context_t context(1);

    // Kaynak veriyi dinlemek için soket (App1'den)
    zmq::socket_t source_dish(context, zmq::socket_type::dish);
    std::cout << "Uygulama 3 (Dinleyici) Başlatıldı." << std::endl;
    std::cout << "Kaynak dinleme adresi: " << source_endpoint << std::endl;
    source_dish.bind(source_endpoint);
    source_dish.join(source_group.c_str());

    // İşlenmiş veriyi dinlemek için soket (App2'den)
    zmq::socket_t processed_dish(context, zmq::socket_type::dish);
    std::cout << "İşlenmiş veri dinleme adresi: " << processed_endpoint << std::endl;
    processed_dish.bind(processed_endpoint);
    processed_dish.join(processed_group.c_str());

    // Poller kurulumu
    zmq::pollitem_t items[] = {
        { source_dish, 0, ZMQ_POLLIN, 0 }, 
        { processed_dish, 0, ZMQ_POLLIN, 0 }
    };

    std::cout << "\nTüm yayıncılardan mesajlar bekleniyor..." << std::endl;

    while (true) {
        // -1, sonsuz süreyle poll işlemi
        int rc = zmq::poll(items, 2, std::chrono::milliseconds(-1));
        if (rc < 0) {
            std::cerr << "Poll işlemi başarısız oldu." << std::endl;
            break;
        }

        // Kaynak soketinde mesaj var mı?
        if (items[0].revents & ZMQ_POLLIN) {
            zmq::message_t msg;
            auto recv_result = source_dish.recv(msg, zmq::recv_flags::none);
            if (!recv_result) {
                std::cerr << "[KAYNAK] Mesaj alımı başarısız oldu." << std::endl;
                continue;
            }
            process_message(msg, "KAYNAK (App1 -> App3)");
        }

        // İşlenmiş veri soketinde mesaj var mı?
        if (items[1].revents & ZMQ_POLLIN) {
            zmq::message_t msg;
            auto recv_result = processed_dish.recv(msg, zmq::recv_flags::none);
            if (!recv_result) {
                std::cerr << "[İŞLENMİŞ] Mesaj alımı başarısız oldu." << std::endl;
                continue;
            }
            process_message(msg, "İŞLENMİŞ (App2 -> App3)");
        }
    }

    // `context.close()` gereksiz, çünkü nesne kapsam dışına çıkınca otomatik olarak kapanır
    // Gömülü kaynaklar ve soketler de burada kapanacak
    return 0;
}
