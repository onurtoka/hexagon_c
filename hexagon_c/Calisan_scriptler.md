# Hexagon Projesi Derleme Komutları

## LibZMQ İlk Kurulum (Sadece bir kez)
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq
cmake -B build -DENABLE_DRAFTS=ON -DBUILD_TESTS=OFF -DZMQ_BUILD_TESTS=OFF
cd /workspaces/hexagon_c/hexagon_c/libzmq/build
make -j$(nproc)
```

## Hexagon_bx (Original CMake Build)
```bash
cd /workspaces/hexagon_c/hexagon_c/hexagon_bx
g++ -std=c++17 -I../libzmq/include -I../include -DZMQ_BUILD_DRAFT_API -pthread -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG src/application/main.cpp src/adapters/outgoing/zeromq/ZeroMQRadioPublisher.cpp ../libzmq/build/lib/libzmq.a -lsodium -lrt -lgnutls -o hat_b_app_radio_optimized
```

## Hexagon_c CMake Build
```bash
cd /workspaces/hexagon_c/hexagon_c/hexagon_c
rm -rf build
cmake -B build .
cmake --build build
./hexagon_c_app &
```

## Hexagon_c Optimized Build (G++ Manual)
```bash
cd /workspaces/hexagon_c/hexagon_c/hexagon_c
g++ -std=c++17 -I../libzmq/include -I../include -DZMQ_BUILD_DRAFT_API -pthread -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG src/application/main.cpp src/adapters/incoming/zeromq/ZeroMQDishTrackDataSubscriber.cpp src/adapters/incoming/zeromq/ZeroMQTrackDataSubscriber.cpp src/adapters/outgoing/zeromq/ZeroMQTrackDataPublisher.cpp src/adapters/outgoing/zeromq/ZeroMQRadioTrackDataPublisher.cpp src/domain/model/FinalCalcDelayData.cpp src/domain/model/DelayCalcTrackData.cpp src/domain/logic/TrackDataProcessor.cpp ../libzmq/build/lib/libzmq.a -lsodium -lrt -lgnutls -o hexagon_c_app_optimized
```

## Hexagon_c Binary Serialization Build (Recommended)
```bash
cd /workspaces/hexagon_c/hexagon_c/hexagon_c
g++ -std=c++17 -I../libzmq/include -I../include -DZMQ_BUILD_DRAFT_API -pthread -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG src/application/main.cpp src/domain/model/DelayCalcTrackData.cpp src/domain/model/FinalCalcTrackData.cpp src/domain/logic/TrackDataProcessor.cpp ../libzmq/build/lib/libzmq.a -lsodium -lrt -lgnutls -o hexagon_c_app_binary_optimized
```

## B_hexagon Optimized Build (G++ Manual)
```bash
cd /workspaces/hexagon_c/b_hexagon/b_hexagon
g++ -std=c++17 -Isrc -I/workspaces/hexagon_c/hexagon_c/libzmq/include -I/workspaces/hexagon_c/hexagon_c/include -DZMQ_BUILD_DRAFT_API -pthread -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG src/application/main.cpp src/common/JsonConfigReader.cpp src/messaging/messaging.cpp src/messaging/ContractManager.cpp src/adapters/incoming/ZeroMQDataHandler.cpp src/adapters/outgoing/ZeroMQDataSender.cpp src/domain/model/TrackData.cpp src/domain/model/DelayCalculatedTrackData.cpp src/domain/logic/CalculatorService.cpp /workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a -lsodium -lrt -lgnutls -o b_hexagon_app_optimized
```

## A_hexagon-main Optimized Build (G++ Manual)
```bash
cd /workspaces/hexagon_c/a_hexagon/a_hexagon-main
g++ -std=c++17 -Isrc -I/workspaces/hexagon_c/hexagon_c/libzmq/include -I/workspaces/hexagon_c/hexagon_c/include -DZMQ_BUILD_DRAFT_API -pthread -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG src/application/main.cpp src/adapters/outgoing/ZeroMQExtrapTrackDataAdapter.cpp src/utilities/JsonConfigParser.cpp src/domain/ports/incoming/TrackDataIncomingPort.cpp src/domain/model/ExtrapTrackData.cpp src/domain/model/TrackData.cpp src/domain/logic/TrackDataExtrapolator.cpp src/domain/ports/outgoing/TrackDataOutgoingPort.cpp /workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a -lsodium -lrt -lgnutls -o a_hexagon_app_optimized
```

## A_hexagon-main 100Hz Build (G++ Manual)
```bash
cd /workspaces/hexagon_c/a_hexagon/a_hexagon-main
g++ -std=c++17 -Isrc -I/workspaces/hexagon_c/hexagon_c/libzmq/include -I/workspaces/hexagon_c/hexagon_c/include -DZMQ_BUILD_DRAFT_API -pthread -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG src/application/main.cpp src/adapters/outgoing/ZeroMQExtrapTrackDataAdapter.cpp src/utilities/JsonConfigParser.cpp src/domain/ports/incoming/TrackDataIncomingPort.cpp src/domain/model/ExtrapTrackData.cpp src/domain/model/TrackData.cpp src/domain/logic/TrackDataExtrapolator.cpp src/domain/ports/outgoing/TrackDataOutgoingPort.cpp /workspaces/hexagon_c/hexagon_c/libzmq/build/lib/libzmq.a -lsodium -lrt -lgnutls -o a_hexagon_app_100hz
```

## Notlar
- Tüm projeler aynı libzmq'yu paylaşıyor (/workspaces/hexagon_c/hexagon_c/libzmq)
- LibZMQ sadece bir kez derlenmeli
- Her proje ultra-optimized (-O3, -march=native, -flto) parametrelerle derleniyor
- ZeroMQ DRAFT API'leri etkinleştirilmiş (RADIO/DISH pattern için)

## Port Ayırımı
- **Port 9001:** A_hexagon → B_hexagon (ExtrapTrackData)
- **Port 9002:** B_hexagon → Hexagon_c (DelayCalcTrackData)
- **Group:** Tüm projeler "SOURCE_DATA" grubunu kullanır
- **Multicast Address:** 239.1.1.1 (Class D multicast)