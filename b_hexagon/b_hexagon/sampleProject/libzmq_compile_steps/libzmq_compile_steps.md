# 1. libzmq kaynak kodunun olduğu klasöre gidelim.
cd third_party/libzmq

# 2. İçinde yeni bir build klasörü oluşturalım.
mkdir build
cd build

# 3. CMake'i, doğru PGM paket adını belirterek çalıştıralım.
cmake .. -DENABLE_DRAFTS=ON -DWITH_OPENPGM=ON -DOPENPGM_PKGCONFIG_NAME=openpgm-5.3

-DENABLE_DRAFTS=ON = ZeroMQ'nun taslak aşamasında olan özelliklerini aktif etmek için.
-DWITH_OPENPGM=ON = Multicast yayın yapabilmek için 
-DOPENPGM_PKGCONFIG_NAME=openpgm-5.3 = Sistemde kurulu olan openpgm versiyonu yazılmalı. Dolayısıyla libzmq derlemeden önce kurulmalı. 
(sudo apt update
sudo apt install libopenpgm-dev)

cmake .. yaptıktan sonra build klasöründe oluşan CMakeCache.txt dosyasında
//Build and install draft classes and methods
ENABLE_DRAFTS:BOOL=ON  olduğu görülmelidir. (Default OFF olarak gelir)

# 4. Kütüphaneyi derleyelim.
make -j$(nproc)

# 5. Derlediğimiz bu yeni kütüphaneyi sistemimize kuralım.
sudo make install

# 6. Kütüphane yollarını sistemin tanıması için güncelleyelim.
sudo ldconfig





#####    
libzmq'yu kendimiz derleyip sisteme kurduktan sonra, CMakeList.txt dosyamızda libzmq'yu aşağıdaki gibi kurulu olduğu yerden çağırmamız gerekiyor.

# --- Üçüncü Parti Kütüphaneler ---
# Sistemde kurulu olan kütüphaneleri bulmasını istiyoruz.
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)


#####
