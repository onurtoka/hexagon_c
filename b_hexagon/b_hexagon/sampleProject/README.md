# ZeroMQ Mesajlaşma Kütüphanesi

Bu proje, ZeroMQ tabanlı, yüksek performanslı ve esnek bir mesajlaşma altyapısı sunan, yeniden kullanılabilir bir C++ kütüphanesidir. Sistem, esnek kontrat yönetimi için **JSON Schema**, verimli ve hızlı ikili (binary) serileştirme için **MessagePack** ve asenkron taşıma katmanı için **ZeroMQ**'yu bir araya getiren hibrit bir mimari kullanır.

## Özellikler

- **Esnek Kontrat Yönetimi:** Mesaj formatları, derleme gerektirmeyen JSON Schema dosyaları ile tanımlanır ve çalışma anında doğrulanır.
- **Yüksek Performanslı Serileştirme:** Ağ üzerinden gönderilen veri, verimli bir ikili format olan MessagePack kullanılarak paketlenir.
- **Asenkron İletişim:** Taşıma katmanı olarak endüstri standardı olan ZeroMQ kütüphanesi kullanılır.
- **Merkezi Kütüphane:** Tüm doğrulama, serileştirme ve deserileştirme mantığı, tüm uygulamaların ortak kullanabileceği tek bir kütüphanede toplanmıştır.

## Proje Yapısı

```text
.
|-- applications/
|   |-- publisher_app.cpp          # Örnek gönderici uygulama
|   `-- subscriber_app.cpp         # Örnek alıcı/köprü uygulama
|
|-- messaging/
|   |-- include/
|   |   `-- messaging/
|   |       `-- messaging.h        # Kütüphanenin genel arayüzü
|   `-- src/
|       `-- messaging.cpp          # Kütüphanenin implementasyonu
|
|-- third_party/
|   |-- cppzmq/                    # Git submodule
|   |-- json/                      # Git submodule
|   |-- json-schema-validator/     # Git submodule
|   `-- msgpack-c/                 # Git submodule
|
|-- zmq_messages/                  # Tüm JSON Schema kontratlarının bulunduğu klasör
|   |-- DelayCalcTrackData.json
|   |-- ExtrapTrackData.json
|   |-- FinalCalcTrackData.json
|   |-- ProcessedTrackData.json
|   `-- TrackStatics.json
|
|-- .gitmodules                    # Submodule tanımlamaları
`-- CMakeLists.txt                 # Projenin derleme betiği
```

## Kurulum

### Ön Gereksinimler

- **C++17** destekli derleyici (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake** (3.16 veya üstü)
- **Git**
- **ZeroMQ (libzmq)**

### Bağımlılıklar

- [ZeroMQ (libzmq)](https://zeromq.org/)
- [cppzmq](https://github.com/zeromq/cppzmq)
- [msgpack-c](https://github.com/msgpack/msgpack-c)
- [nlohmann/json](https://github.com/nlohmann/json)
- [json-schema-validator](https://github.com/pboettch/json-schema-validator)

> **Not:** cppzmq, msgpack-c, nlohmann/json ve json-schema-validator kütüphaneleri git submodule olarak projeye dahil edilmiştir.

### ZeroMQ Kurulumu (Ubuntu/Debian)

```sh
sudo apt-get update && sudo apt-get install build-essential cmake libzmq3-dev
```

### Projeyi Klonlama ve Bağımlılıkları Hazırlama

```sh
git clone --recurse-submodules <repo-url>
cd zmq_messages
```

Eğer submodule'ler eksikse:

```sh
git submodule update --init --recursive
```

### Derleme

```sh
mkdir build
cd build
cmake ..
make
```

## Kullanım

### Kontrat Yükleme

Tüm JSON kontratları `zmq_messages/