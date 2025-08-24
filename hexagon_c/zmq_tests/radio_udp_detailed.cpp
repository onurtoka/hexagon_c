/*
 * Bu dosya, ZeroMQ kütüphanesinin temel C API'sini kullanarak bir RADIO soketi oluşturur
 * ve UDP multicast üzerinden anlık olarak basit metin mesajları yayınlar.
 * Amaç, RADIO/DISH deseninin UDP taşıyıcısı ile çalışıp çalışmadığını test etmektir.
 */

// Derleyiciye DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH, zmq_join, zmq_msg_set_group vb.)
// aktive etmesini söyleyen bir "preprocessor" (ön işlemci) direktifidir.
//#define ZMQ_BUILD_DRAFT_API

// ZeroMQ'nun temel C API'sini içeren ana başlık dosyası.
// C++ sarmalayıcısı olan <zmq.hpp> yerine bunu kullanıyoruz
#include <zmq.h>

// Standart C++ kütüphanelerini dahil ediyoruz.
#include <iostream>  // Konsola yazı yazdırmak için (std::cout, std::cerr)
#include <string>    // Modern C++ metin işlemleri için (std::string)
#include <thread>    // Programı belirli bir süre bekletmek için (std::this_thread)
#include <chrono>    // Zaman işlemleri için (std::chrono)
#include <sstream>   // Metinleri verimli bir şekilde birleştirmek için (std::stringstream)
#include <cassert>   // Programın kritik noktalarında varsayımları kontrol etmek için (assert)
#include <cstring>   // C-stili metin ve bellek işlemleri için (memcpy, strlen)


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
        // zmq_errno(), son oluşan hatanın sayısal kodunu verir.
        // zmq_strerror(), bu sayısal kodu okunabilir bir metne çevirir.
        std::cerr << context_msg << " - ZMQ Error: " << zmq_strerror(zmq_errno()) << std::endl;
        // Programın daha fazla devam etmesi anlamsız olduğu için 1 hata koduyla sonlandır.
        exit(1);
    }
}

// Her C++ programının başlangıç noktası olan ana fonksiyon.
int main() {
    /*
     * const char* endpoint = "udp://239.255.0.1:7779";
     *
     * Neden const char*?
     * Bu değişken, ZeroMQ soketinin bağlanacağı adresi tutar. Bunu modern C++ tipi olan
     * `std::string` yerine C'nin temel metin tipi olan `const char*` (sabit karakter pointer'ı)
     * olarak tanımladık. Bunun temel sebebi, kullandığımız `zmq_connect` gibi C API
     * fonksiyonlarının parametre olarak `const char*` beklemesidir. `std::string` kullansaydık,
     * fonksiyonu her çağırdığımızda `.c_str()` metodu ile dönüşüm yapmamız gerekirdi.
     * `const` anahtar kelimesi, bu metnin program boyunca değiştirilmeyeceğini belirtir,
     * bu da derleyiciye optimizasyon yapma imkanı tanır ve yanlışlıkla değiştirilmesini önler.
     */
    const char* endpoint = "udp://239.255.0.1:7779";
    const char* group = "TRACK_DATA_UDP";
    
    // --- 1. Context Oluşturma ---
    // ZeroMQ'da tüm soketler bir "context" (bağlam) içinde yaşar. Context, soketlerin
    // paylaştığı I/O thread havuzunu ve diğer kaynakları yönetir. Bir programda genellikle
    // sadece bir tane context oluşturulur.
    void* context = zmq_ctx_new();
    
    /*
     * assert(context);
     *
     * Neden assert?
     * `assert`, bir programlama varsayımını test eden bir makrodur. Eğer parantez içindeki
     * ifade 'false' (yanlış) ise, program bir hata mesajı ile anında çöker.
     * `zmq_ctx_new()` fonksiyonu, eğer başarılı olamazsa (örn: bellek yetersizliği),
     * NULL (boş bir pointer) döndürür. Bizim programımızın bir ZeroMQ context'i olmadan
     * çalışması imkansızdır. Bu yüzden, `context`'in NULL olmaması gerektiğini `assert` ile
     * varsayıyoruz. Bu, bir "fail-fast" (hızlıca başarısız ol) prensibidir. Hata varsa,
     * programın en başında ve kaynağında çökmesini sağlayarak hata ayıklamayı kolaylaştırır.
     * `assert`'ler genellikle sadece Debug modunda çalışır, Release modunda derleyici
     * tarafından yok sayılırlar.
     */
    assert(context);

    // --- 2. RADIO Soketi Oluşturma ---
    // Belirtilen context içinde yeni bir soket oluşturuyoruz.
    // İkinci parametre olan `ZMQ_RADIO`, oluşturulacak soketin tipini belirtir.
    // Bu, DRAFT API'leri aktive ettiğimiz için kullanılabilir olan bir sabittir.
    void* publisher = zmq_socket(context, ZMQ_RADIO);
    assert(publisher); // Soket oluşturmanın da başarısız olmaması gerekir.

    std::cout << "UDP RADIO Publisher (C API): Connecting to " << endpoint << "..." << std::endl;
    
    // --- 3. Soketi Bağlama (Adrese) ---
    // `zmq_connect`, bir soketin belirtilen adrese bağlanmasını sağlar. RADIO soketleri,
    // bir multicast grubuna yayın yapmak için genellikle 'connect' olur.
    // Fonksiyon, başarı durumunda 0, hata durumunda -1 döndürür.
    int rc = zmq_connect(publisher, endpoint);
    // Manuel hata kontrolü. `check_rc` fonksiyonu yerine doğrudan `if` de kullanılabilir.
    if (rc != 0) {
        std::cerr << "UDP RADIO Publisher Error: " << zmq_strerror(zmq_errno()) << std::endl;
        return 1; // Hata durumunda programdan çık.
    }
    std::cout << "UDP RADIO Publisher: Successfully connected. Starting to send messages..." << std::endl;

    long trackId = 800;
    // Sonsuz döngü: Program `Ctrl+C` ile kapatılana kadar mesaj göndermeye devam eder.
    while (true) {
        // `std::stringstream`, C++'ta metinleri ve sayıları birleştirmek için verimli
        // bir yoldur. '+' operatörü ile string birleştirmekten daha performanslıdır.
        std::stringstream ss;
        
        /*
         * auto now = std::chrono::system_clock::now();
         *
         * Neden auto?
         * `auto` anahtar kelimesi, C++11 ile gelen bir özelliktir ve derleyicinin bir
         * değişkenin tipini, atandığı değerden otomatik olarak çıkarmasını sağlar.
         * `std::chrono::system_clock::now()` fonksiyonunun geri dönüş tipi
         * `std::chrono::time_point<std::chrono::system_clock>` gibi çok uzun ve karmaşık
         * bir tiptir. Bu uzun tipi elle yazmak yerine `auto` kullanarak hem kodumuzu
         * kısaltmış oluruz hem de olası yazım hatalarını önleriz. Derleyici bizim yerimize
         * doğru tipi bulur.
         */
        auto now = std::chrono::system_clock::now();
        // `now` zaman noktasını, "epoch" (1 Ocak 1970) başlangıcından bu yana geçen
        // saniye sayısına dönüştürüyoruz.
        auto epoch_time = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        // Oluşturduğumuz `stringstream` nesnesine verileri `<<` operatörü ile ekliyoruz.
        ss << "TrackID: " << trackId << ", Time: " << epoch_time;
        // `stringstream`'in içindeki birleştirilmiş metni `std::string`'e çeviriyoruz.
        std::string payload_str = ss.str();

        // --- 4. Gönderilecek Mesajı Oluşturma ---
        // `zmq_msg_t`, ZeroMQ'da bir mesajı temsil eden temel yapıdır.
        zmq_msg_t message;
        // `zmq_msg_init_size`, belirtilen boyutta bir mesaj için bellek ayırır.
        rc = zmq_msg_init_size(&message, payload_str.size());
        check_rc(rc, "Message Init"); // Yardımcı fonksiyonumuzla hata kontrolü
        // `memcpy`, C'nin standart bellek kopyalama fonksiyonudur. `payload_str`'deki metni,
        // `zmq_msg_data(&message)` ile adresini aldığımız ZMQ mesaj belleğine kopyalarız.
        memcpy(zmq_msg_data(&message), payload_str.c_str(), payload_str.size());

        // --- 5. Mesajın Grubunu Ayarlama ---
        // `zmq_msg_set_group`, gönderilecek olan bu spesifik mesaja bir "grup" etiketi atar.
        // Alıcı `DISH` soketleri, sadece katıldıkları gruptaki mesajları alırlar.
        rc = zmq_msg_set_group(&message, group);
        check_rc(rc, "Set Group");

        // --- 6. Mesajı Gönderme ---
        // `zmq_msg_send`, hazırlanan mesajı belirtilen soket üzerinden gönderir.
        // Üçüncü parametre olan `0`, özel bir bayrak (flag) olmadığını belirtir.
        rc = zmq_msg_send(&message, publisher, 0);
        // `zmq_msg_send` başarı durumunda gönderilen byte sayısını döndürür.
        // Bu sayının, bizim gönderdiğimiz metnin boyutuyla aynı olduğunu `assert` ile kontrol ediyoruz.
        assert(rc == (int)payload_str.size());

        std::cout << "Sent: '" << payload_str << "' to group '" << group << "'" << std::endl;
        trackId++;

        // --- 7. Mesajı Kapatma (Belleği Serbest Bırakma) ---
        // `zmq_msg_init_size` ile ayrılan belleği `zmq_msg_close` ile serbest bırakmamız gerekir.
        // Bu, C API'sinde manuel bellek yönetiminin bir parçasıdır. Bunu yapmazsak
        // bellek sızıntısı (memory leak) olur.
        zmq_msg_close(&message);
        
        // Bir sonraki mesajı göndermeden önce 1 saniye bekle.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // --- 8. Kaynakları Temizleme ---
    // Program sonlandığında, oluşturulan soketi ve context'i manuel olarak kapatmamız gerekir.
    zmq_close(publisher);
    zmq_ctx_term(context);

    // Programın başarıyla sonlandığını belirtir.
    return 0;
}
