/*
 * Bu dosya, ZeroMQ kütüphanesinin temel C API'sini kullanarak bir DISH soketi oluşturur
 * ve UDP multicast üzerinden yayınlanan anlık metin mesajlarını dinler.
 * Amacı, radio_udp.cpp yayıncısı ile birlikte RADIO/DISH deseninin UDP taşıyıcısı
 * üzerinde çalışıp çalışmadığını test etmektir.
 */

// Derleyiciye DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH, zmq_join vb.) aktive etmesini
// söyleyen bir "preprocessor" (ön işlemci) direktifidir. Bu, CMakeLists.txt dosyasından
// da ayarlanabilir, ancak burada olması bu dosyanın kendi kendine yeterli olmasını sağlar.
//#define ZMQ_BUILD_DRAFT_API

// ZeroMQ'nun temel C API'sini içeren ana başlık dosyası.
#include <zmq.h>

// Standart C++ kütüphanelerini dahil ediyoruz.
#include <cstring>   // C-stili metin işlemleri için (strcmp, strlen)
#include <iostream>  // Konsola yazı yazdırmak için (std::cout, std::cerr)
#include <string>    // Modern C++ metin işlemleri için (std::string)
#include <vector>    // Bu örnekte doğrudan kullanılmasa da genel amaçlı bir kütüphanedir.
#include <cassert>   // Programın kritik noktalarında varsayımları kontrol etmek için (assert)

/*
 * Hata kontrolünü basitleştirmek için yazılmış bir yardımcı fonksiyon.
 * ZeroMQ'nun C API'sindeki birçok fonksiyon, başarı durumunda 0, hata durumunda -1 döndürür.
 * Bu fonksiyon, her ZMQ çağrısından sonra hata olup olmadığını kontrol etmeyi kolaylaştırır.
 */
void check_rc(int rc, const std::string& context_msg) {
    // Eğer rc (return code - geri dönüş kodu) 0 değilse (yani bir hata varsa)
    if (rc != 0) {
        // Ekrana, hatanın hangi işlem sırasında oluştuğunu (context_msg) ve
        // ZeroMQ'nun kendi hata açıklamasını (zmq_strerror) yazdır.
        std::cerr << context_msg << " - ZMQ Error: " << zmq_strerror(zmq_errno()) << std::endl;
        // Programın daha fazla devam etmesi anlamsız olduğu için 1 hata koduyla sonlandır.
        exit(1);
    }
}

// Her C++ programının başlangıç noktası olan ana fonksiyon.
int main() {
    // Dinlenecek endpoint ve grup adını tanımlıyoruz.
    // Bu bilgilerin, yayıncı olan radio_udp.cpp dosyasındaki bilgilerle
    // birebir aynı olması kritik öneme sahiptir.
    const char* endpoint_udp = "udp://239.255.0.1:7779";
    const char* group_udp = "TRACK_DATA_UDP";

    // --- 1. Context Oluşturma ---
    // Tıpkı yayıncıda olduğu gibi, dinleyici de kendi ZeroMQ context'ini oluşturur.
    void* context = zmq_ctx_new();
    assert(context); // Context oluşturmanın başarılı olduğunu varsayıyoruz.

    // --- 2. DISH Soketi Oluşturma ve Ayarlama ---
    // Soket tipi olarak ZMQ_DISH kullanıyoruz. Bu, RADIO soketlerinden gelen
    // grup yayınlarını almak için tasarlanmış özel bir soket tipidir.
    void* sub_udp = zmq_socket(context, ZMQ_DISH);
    assert(sub_udp);

    std::cout << "DISH UDP Subscriber: Binding to " << endpoint_udp << "..." << std::endl;
    
    /*
     * Neden bind?
     * Normalde SUB soketleri 'connect' olurken, DISH soketleri multicast dinlemek
     * için genellikle 'bind' olur. Bu, soketin belirtilen multicast adresini ve portunu
     * "dinlemeye başlamasını" sağlar. Ağdaki switch'ler, bu porta bind olan bir uygulama
     * olduğunu gördüğünde, ilgili multicast grubuna ait paketleri bu makineye yönlendirmeye başlar.
     */
    check_rc(zmq_bind(sub_udp, endpoint_udp), "UDP Bind");
    
    /*
     * zmq_join(sub_udp, group_udp);
     *
     * Bu, DISH soketinin en önemli fonksiyonudur. Bu komut, sokete "Ben 'TRACK_DATA_UDP'
     * adındaki grupla ilgileniyorum. Bu gruba ait olan mesajları bana ilet." der.
     * Bir DISH soketi, en az bir gruba katılmadığı sürece sağırdır ve hiçbir mesaj almaz.
     */
    check_rc(zmq_join(sub_udp, group_udp), "UDP Join Group");
    std::cout << "  - Joined group '" << group_udp << "'" << std::endl;


    std::cout << "\nWaiting for messages from RADIO UDP publisher..." << std::endl;

    // Sonsuz döngü: Sürekli olarak mesaj bekler.
    while (true) {
        // `zmq_msg_t`, bir mesajı temsil eden temel yapıdır.
        zmq_msg_t msg;
        // `zmq_msg_init`, mesaj yapısını bir sonraki kullanım için başlatır/hazırlar.
        // Bu, mesajı almadan önce yapılması gereken bir adımdır.
        zmq_msg_init(&msg);
        
        // --- Mesajı Alma ---
        // `zmq_msg_recv`, belirtilen soketten bir mesaj bekler.
        // Üçüncü parametre olan `0`, özel bir bayrak olmadığını belirtir. Bu fonksiyon
        // bir mesaj gelene kadar programı burada "bloke eder" (bekletir).
        int bytes = zmq_msg_recv(&msg, sub_udp, 0);
        
        // Eğer `zmq_msg_recv` -1 döndürmezse, bu, mesajın başarıyla alındığı anlamına gelir.
        if (bytes != -1) {
            // --- Mesajı İşleme ---
            
            // `zmq_msg_group`, alınan son mesajın hangi gruba ait olduğunu döndürür.
            // Bu, aynı soketin birden fazla gruba katıldığı durumlarda kullanışlıdır.
            const char* group_str = zmq_msg_group(&msg);
            
            /*
             * std::string data_str(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
             *
             * Bu satır, ham ZMQ mesajını modern bir C++ `std::string` nesnesine çevirir.
             * `zmq_msg_data(&msg)`: Mesajın içeriğinin başlangıç adresini `void*` olarak verir.
             * `static_cast<char*>`: Bu `void*` pointer'ını, C++ string'inin anlayacağı
             * `char*` tipine güvenli bir şekilde dönüştürürüz.
             * `zmq_msg_size(&msg)`: Mesajın bayt cinsinden boyutunu verir.
             * `std::string(...)`: Bu `std::string` kurucu metodu, bir başlangıç adresi ve
             * bir boyut alarak o bellek bölgesinden yeni bir string nesnesi oluşturur.
             */
            std::string data_str(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
            
            std::cout << "\n---[ MESSAGE RECEIVED on RADIO/DISH UDP ]---" << std::endl;
            // Eğer grup bilgisi mevcutsa (NULL değilse), ekrana yazdır.
            if (group_str) {
                std::cout << "  Group: " << group_str << std::endl;
            }
            std::cout << "  Message: " << data_str << std::endl;
        }

        // `zmq_msg_init` ile başlatılan veya `zmq_msg_recv` ile doldurulan her mesajın
        // işi bittiğinde `zmq_msg_close` ile serbest bırakılması gerekir.
        zmq_msg_close(&msg);
    }

    // Program sonlandığında, oluşturulan soketi ve context'i manuel olarak kapatmamız gerekir.
    zmq_close(sub_udp);
    zmq_ctx_term(context);

    return 0;
}
