# HaT - Hexagonal Architecture Tracking System

C++ ile ZeroMQ kullanan hexagonal mimari tracking sistemi.

## Mimari Yapısı

```
hexagon_c/
├── src/
│   ├── domain/                 # Domain Katmanı (İş Mantığı)
│   │   ├── model/             # Domain Modelleri
│   │   │   ├── DelayCalcTrackData.hpp
│   │   │   └── FinalCalcDelayData.hpp
│   │   ├── logic/             # İş Mantığı
│   │   │   └── TrackDataProcessor.hpp
│   │   └── ports/             # Port Arayüzleri
│   │       ├── incoming/      # Primary Ports
│   │       │   └── TrackDataSubmission.hpp
│   │       └── outgoing/      # Secondary Ports
│   │           ├── TrackDataPublisher.hpp
│   │           └── TrackDataRepository.hpp
│   ├── adapters/              # Adapter Katmanı
│   │   ├── incoming/          # Primary Adapters
│   │   │   └── zeromq/
│   │   │       └── ZeroMQTrackDataSubscriber.hpp
│   │   └── outgoing/          # Secondary Adapters
│   │       ├── zeromq/
│   │       │   └── ZeroMQTrackDataPublisher.hpp
│   │       ├── persistence/   # Veri Saklama
│   │       └── marketdata/    # Dış Veri Kaynakları
│   └── application/           # Uygulama Katmanı
│       └── main.cpp
├── test/                      # Test Dosyaları
├── build/                     # Build Çıktıları
└── CMakeLists.txt            # Build Sistemi
```

## Domain Modelleri

### DelayCalcTrackData
- **ID**: int
- **Velocity**: double
- **Position**: double  
- **Time Field**: long (milliseconds)
- **Track ID**: int (*)
- **ECEF Coordinates**: XVelocityECEF, YVelocityECEF, ZVelocityECEF
- **ECEF Positions**: XPositionECEF, YPositionECEF, ZPositionECEF
- **Timing Fields**: UpdateTime, OriginalUpdateTime, FirstHopSentTime, FirstHopDelayTime, SecondHopSentTime

### FinalCalcDelayData
- **Track ID**: int (*)
- **ECEF Velocity**: XVelocity, YVelocity, ZVelocity ECEF
- **ECEF Position**: XPositionECEF, YPositionECEF, ZPositionECEF
- **Advanced Timing**: UpdateTime, OriginalUpdateTime, FirstHopSentTime, FirstHopDelayTime, SecondHopSentTime, SecondHopDelayTime, TotalDelayTime, ThirdHopSentTime

## Hexagonal Mimari

### Domain Katmanı (Hexagon'un Kalbi)
- **TrackDataProcessor**: Ana iş mantığı
- **Port Arayüzleri**: Dış dünya ile iletişim kontratları
- **Domain Modelleri**: İş kuralları ve validasyonlar

### Adapter Katmanı (Dış Dünya)
- **ZeroMQ Subscriber**: Veri alma
- **ZeroMQ Publisher**: Veri gönderme  
- **Repository**: Veri saklama
- **Market Data**: Dış veri kaynakları

## Gereksinimler

### Sistem Gereksinimleri
- C++17 veya üzeri
- CMake 3.16+
- ZeroMQ library (libzmq)
- cppzmq (ZeroMQ C++ bindings)

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libzmq3-dev libzmq5 libzmq5-dev
sudo apt-get install libcppzmq-dev
```

### Windows (vcpkg)
```bash
vcpkg install zeromq cppzmq
```

## Build İşlemi

### Debug Build
```bash
cd hexagon_c
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### Release Build
```bash
cd hexagon_c
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Test Build
```bash
cmake .. -DBUILD_TESTS=ON
make -j4
./hat_tests
```

## Çalıştırma

### Ana Uygulama
```bash
./hat_app
```

### Konfigürasyon Seçenekleri
```bash
./hat_app --help
./hat_app --sub-port1 5555 --sub-port2 5556 --pub-port1 5557 --pub-port2 5558
```

### ZeroMQ Endpoint'leri
- **DelayCalc Subscriber**: tcp://localhost:5555 (Veri alır)
- **DelayCalc Publisher**: tcp://*:5557 (Orijinal veriyi yayınlar)
- **FinalCalc Publisher**: tcp://*:5558 (Üretilen final veriyi yayınlar)

**Not**: FinalCalcDelayData dışardan alınmaz, DelayCalcTrackData'dan hesaplanarak üretilir.

## Veri Akışı

```
[DelayCalcTrackData Input] → [Subscriber] → [TrackDataProcessor] → [Dual Publisher]
                                               ↓                         ↓
                                         [Repository]              [DelayCalc Out]
                                               ↓                         ↓
                                    [Generate FinalCalc] → [FinalCalc Out]
```

1. **Veri Alma**: ZeroMQ üzerinden sadece DelayCalcTrackData alınır
2. **İş Mantığı**: TrackDataProcessor verileri işler, validasyon ve analiz yapar
3. **FinalCalc Üretimi**: DelayCalcTrackData'dan FinalCalcDelayData hesaplanır
   - SecondHopDelayTime = ReceivedTime - FirstHopSentTime
   - TotalDelayTime = ReceivedTime - FirstHopSentTime  
   - ThirdHopSentTime = CurrentTime
4. **Veri Gönderme**: Hem orijinal hem de üretilen veriler ZeroMQ üzerinden yayınlanır
5. **Saklama**: Her iki veri tipi de repository'de kalıcı olarak saklanır

## İş Kuralları

### Veri Validasyonu
- Track ID > 0 olmalı
- Update time > 0 olmalı
- ECEF koordinatları geçerli olmalı

### Performans Analizi
- **EXCELLENT**: < 100ms gecikme
- **GOOD**: 100-500ms gecikme
- **ACCEPTABLE**: 500-1000ms gecikme
- **POOR**: 1000-5000ms gecikme
- **CRITICAL**: > 5000ms gecikme

### Yüksek Hız Kontrolü
- Eşik değeri: 100 m/s
- Yüksek hız durumunda öncelikli işlem

## Monitoring

### İstatistikler
- Alınan mesaj sayısı
- İşlenen mesaj sayısı
- Başarısız işlem sayısı
- Ortalama gecikme
- Aktif track sayısı

### Loglar
Sistem 10 saniyede bir istatistikleri konsola yazdırır.

## Geliştirme

### Yeni Adapter Ekleme
1. Port arayüzünü implement et
2. `src/adapters/` altına ekle
3. `main.cpp`'de wire'la

### Yeni İş Kuralı Ekleme
1. `TrackDataProcessor.hpp`'de metod ekle
2. Domain model'lerde gerekli alanları ekle
3. Test yaz

### Serialization
Şu an basit binary serialization kullanıyor. Production'da:
- JSON
- Protocol Buffers  
- MessagePack
- Apache Avro
kullanılabilir.

## Test

```bash
# Unit testler
./hat_tests

# Manuel test - veri gönderme
python test_sender.py

# Manuel test - veri alma  
python test_receiver.py
```

## Performans

### Optimizasyonlar
- Batch processing desteği
- Non-blocking ZeroMQ operations
- Thread-safe repository
- Memory pool kullanımı (gelecekte)

### Benchmarks
- Single message: < 1ms latency
- Batch processing: 10x daha hızlı
- Memory usage: < 100MB (10K messages)

## Lisans

MIT License

## Katkıda Bulunma

1. Fork yapın
2. Feature branch oluşturun
3. Commit'lerinizi yapın
4. Pull request gönderin

## İletişim

- Email: your-email@example.com
- Issues: GitHub Issues kullanın