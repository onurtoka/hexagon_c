# B_Hexagon Çalışan Script'ler

## 1. Hızlı Build ve Çalıştırma (Debug Mode)
```bash
cd /workspaces/hexagon_c/b_hexagon/b_hexagon
./quick_build.sh
```

## 2. Optimize Build ve Çalıştırma (Production Mode)  
```bash
cd /workspaces/hexagon_c/b_hexagon/b_hexagon
./build_and_run.sh
```

## 3. Manuel G++ Komutu (Debug)
```bash
cd /workspaces/hexagon_c/b_hexagon/b_hexagon

g++ -std=c++17 \
    -Isrc \
    -I/workspaces/hexagon_c/hexagon_c/libzmq/include \
    -DZMQ_BUILD_DRAFT_API \
    -pthread \
    -g -O0 \
    src/application/main.cpp \
    src/domain/logic/CalculatorService.cpp \
    src/domain/model/DelayCalculatedTrackData.cpp \
    src/domain/model/TrackData.cpp \
    src/adapters/outgoing/ZeroMQDataSender.cpp \
    src/adapters/incoming/ZeroMQDataHandler.cpp \
    src/messaging/ContractManager.cpp \
    src/messaging/messaging.cpp \
    src/common/JsonConfigReader.cpp \
    /workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a \
    -lsodium -lrt -lgnutls \
    -o b_hexagon_app_debug

./b_hexagon_app_debug
```

## 4. Manuel G++ Komutu (Optimized)
```bash
cd /workspaces/hexagon_c/b_hexagon/b_hexagon

g++ -std=c++17 \
    -Isrc \
    -I/workspaces/hexagon_c/hexagon_c/libzmq/include \
    -DZMQ_BUILD_DRAFT_API \
    -pthread \
    -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG \
    src/application/main.cpp \
    src/domain/logic/CalculatorService.cpp \
    src/domain/model/DelayCalculatedTrackData.cpp \
    src/domain/model/TrackData.cpp \
    src/adapters/outgoing/ZeroMQDataSender.cpp \
    src/adapters/incoming/ZeroMQDataHandler.cpp \
    src/messaging/ContractManager.cpp \
    src/messaging/messaging.cpp \
    src/common/JsonConfigReader.cpp \
    /workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a \
    -lsodium -lrt -lgnutls \
    -o b_hexagon_app

./b_hexagon_app
```

## 5. Hexagon_c Komut Formatı Adaptasyonu (Ultra Optimized)
```bash
cd /workspaces/hexagon_c/b_hexagon/b_hexagon

g++ -std=c++17 \
    -Isrc \
    -I/workspaces/hexagon_c/hexagon_c/libzmq/include \
    -I/workspaces/hexagon_c/hexagon_c/include \
    -DZMQ_BUILD_DRAFT_API \
    -pthread \
    -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG \
    src/application/main.cpp \
    src/common/JsonConfigReader.cpp \
    src/messaging/messaging.cpp \
    src/messaging/ContractManager.cpp \
    src/adapters/incoming/ZeroMQDataHandler.cpp \
    src/adapters/outgoing/ZeroMQDataSender.cpp \
    src/domain/model/TrackData.cpp \
    src/domain/model/DelayCalculatedTrackData.cpp \
    src/domain/logic/CalculatorService.cpp \
    /workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a \
    -lsodium -lrt -lgnutls \
    -o b_hexagon_app_optimized

./b_hexagon_app_optimized
```

## LibZMQ İlk Kurulum (Sadece bir kez gerekli)
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq
cmake -B build -DENABLE_DRAFTS=ON -DBUILD_TESTS=OFF -DZMQ_BUILD_TESTS=OFF
make -C build -j$(nproc)
```

## Notlar
- LibZMQ sadece bir kez derlenmeli, sonra bu script'ler kullanılabilir
- `quick_build.sh` = Hızlı debug derleme
- `build_and_run.sh` = Optimized production derleme  
- Her iki script de libzmq'yu yeniden derlemez, mevcut olanı kullanır
