cd /workspaces/hexagon_c/hexagon_c/libzmq
cmake -B build -DENABLE_DRAFTS=ON -DBUILD_TESTS=OFF -DZMQ_BUILD_TESTS=OFF
cd /workspaces/hexagon_c/hexagon_c/libzmq/build
make -j$(nproc)
cd /workspaces/hexagon_c/hexagon_c/hexagon_bx
g++ -std=c++17 -I../libzmq/include -I../include -DZMQ_BUILD_DRAFT_API -pthread -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG src/application/main.cpp src/adapters/outgoing/zeromq/ZeroMQRadioPublisher.cpp ../libzmq/build/lib/libzmq.a -lsodium -lrt -lgnutls -o hat_b_app_radio_optimized
cd /workspaces/hexagon_c/hexagon_c/hexagon_c
rm -rf build
cmake -B build .
cmake --build build
./hexagon_c_app &
cd /workspaces/hexagon_c/hexagon_c/hexagon_bx
./hat_b_app_radio_optimized
