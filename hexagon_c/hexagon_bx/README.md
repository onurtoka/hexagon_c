# HaT-B - Hexagonal Architecture System B

C++ ile ZeroMQ kullanan hexagonal mimari sistemi - B versiyonu.

## Mimari Yapısı

```
hexagon_b/
├── src/
│   ├── domain/                 # Domain Katmanı (İş Mantığı)
│   │   ├── model/             # Domain Modelleri (Özel modeller)
│   │   ├── logic/             # İş Mantığı
│   │   │   └── DataProcessor.hpp
│   │   └── ports/             # Port Arayüzleri
│   │       ├── incoming/      # Primary Ports
│   │       │   └── DataSubmission.hpp
│   │       └── outgoing/      # Secondary Ports
│   │           ├── DataPublisher.hpp
│   │           └── DataRepository.hpp
│   ├── adapters/              # Adapter Katmanı
│   │   ├── incoming/          # Primary Adapters
│   │   │   └── zeromq/
│   │   │       └── ZeroMQDataSubscriber.hpp
│   │   └── outgoing/          # Secondary Adapters
│   │       ├── zeromq/
│   │       │   └── ZeroMQDataPublisher.hpp
│   │       └── persistence/   # Veri Saklama
│   └── application/           # Uygulama Katmanı
│       └── main.cpp
├── test/                      # Test Dosyaları
├── build/                     # Build Çıktıları
└── CMakeLists.txt            # Build Sistemi
```

## Domain Modelleri

*Modeller henüz tanımlanmadı - kullanıcıdan bekliyor*

## Hexagonal Mimari

### Domain Katmanı (Hexagon'un Kalbi)
- **DataProcessor**: Ana iş mantığı
- **Port Arayüzleri**: Dış dünya ile iletişim kontratları
- **Domain Modelleri**: İş kuralları ve validasyonlar

### Adapter Katmanı (Dış Dünya)
- **ZeroMQ Subscriber**: Veri alma
- **ZeroMQ Publisher**: Veri gönderme  
- **Repository**: Veri saklama

## Gereksinimler

### Sistem Gereksinimleri
- C++17 veya üzeri
- GCC 8.5+ (test edildi)
- ZeroMQ library (libzmq)
- cppzmq (ZeroMQ C++ bindings)

### Windows MinGW-w64
```bash
# GCC 8.5.0 kurulu olmalı
gcc --version

# ZeroMQ library gerekli (opsiyonel)
```

## Build İşlemi

### GCC ile Manuel Derleme
```bash
cd hexagon_b
g++ -std=c++17 -Wall -Wextra -I. test_simple.cpp -o test_simple.exe
```

### CMake ile Build (ZeroMQ gerekli)
```bash
cd hexagon_b
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Çalıştırma

### Test Uygulaması
```bash
./test_simple.exe
```

### Ana Uygulama (ZeroMQ ile)
```bash
./hat_b_app
```

### ZeroMQ Endpoint'leri
*Modeller tanımlandıktan sonra belirlenecek*

## Veri Akışı

```
[ZeroMQ Input] → [Subscriber] → [DataProcessor] → [Publisher] → [ZeroMQ Output]
                                      ↓
                               [Repository] (Storage)
```

## Geliştirme Durumu

### ✅ Tamamlanan
- [x] Proje yapısı oluşturuldu
- [x] CMakeLists.txt hazırlandı
- [x] README.md oluşturuldu
- [x] Dizin yapısı kuruldu

### ⏳ Bekleyen
- [ ] Domain modelleri (kullanıcıdan bekliyor)
- [ ] Port arayüzleri (modellere göre)
- [ ] Domain logic implementasyonu
- [ ] ZeroMQ adapter'ları
- [ ] Test dosyaları

## Katkıda Bulunma

1. Yeni modelleri tanımlayın
2. Port arayüzlerini güncelleyin
3. Domain logic'i implement edin
4. Test yazın

## Lisans

MIT License