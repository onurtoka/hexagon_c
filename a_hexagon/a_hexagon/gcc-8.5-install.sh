#!/usr/bin/env bash
# Bu betik, Ubuntu 24.04 üzerinde GCC 8.5'i /opt/gcc-8.5 altına derleyip kurar ve update-alternatives'e kaydeder.
# Her satırın üstünde veya sonunda ne yaptığını açıklayan yorum bulunmaktadır.

# Hata olduğunda betiği sonlandır; tanımsız değişken kullanımlarını ve pipe hatalarını yakala.
set -euo pipefail

# Derlemek istediğimiz GCC sürümü.
VERSION="8.5.0"
# Kurulum dizini; sistemden bağımsız, güvenli bir konum.
PREFIX="/opt/gcc-8.5"
# Çalışma klasörü; kaynak kodları buraya indirilecek. Değiştirmek istersen WORKDIR ortam değişkeni ile geçersiz kıl.
WORKDIR="${WORKDIR:-$HOME/src}"
# Paralel derleme için iş sayısı; varsayılan CPU çekirdek sayısı.
JOBS="${JOBS:-$(nproc)}"
# GCC kaynak paketinin indirileceği URL.
TARBALL_URL="https://ftp.gnu.org/gnu/gcc/gcc-${VERSION}/gcc-${VERSION}.tar.xz"
# İndirilecek arşiv dosyanın yerel ismi.
TARBALL_NAME="gcc-${VERSION}.tar.xz"

# Log mesajlarını tek biçimde göstermek için küçük bir yardımcı işlev.
log() { echo -e "\n[+] $*\n"; }

# root olarak çalıştırmayı zorunlu kılmıyoruz; sadece kurulum adımlarında sudo kullanacağız.
# Kullanıcıya özet bilgi ver.
log "GCC ${VERSION} derleme ve kurulum betiği başlıyor. Kurulum hedefi: ${PREFIX}"

# Gerekli paketleri apt ile kurmaya çalış. apt yoksa adımı atla.
# Bu paketler tipik olarak GCC derlemek için gerekli araç ve kütüphanelerdir.
if command -v apt >/dev/null 2>&1; then
  log "Gerekli paketler denetleniyor ve kuruluyor (sudo gerektirir)..."
  sudo apt update
  sudo apt install -y build-essential flex bison libgmp-dev libmpfr-dev libmpc-dev texinfo wget curl ca-certificates
else
  log "apt bulunamadı, paket kurulum adımı atlanıyor. Gerekli bağımlılıkları manuel sağlamalısınız."
fi

# Çalışma klasörünü oluştur.
log "Çalışma klasörü oluşturuluyor: ${WORKDIR}"
mkdir -p "${WORKDIR}"

# Çalışma klasörüne geç.
cd "${WORKDIR}"

# Kaynak arşivi yoksa indir.
if [[ ! -f "${TARBALL_NAME}" ]]; then
  log "GCC ${VERSION} kaynak arşivi indiriliyor..."
  # wget varsa onu, yoksa curl'u kullan.
  if command -v wget >/dev/null 2>&1; then
    wget -O "${TARBALL_NAME}" "${TARBALL_URL}"
  else
    curl -L -o "${TARBALL_NAME}" "${TARBALL_URL}"
  fi
else
  log "Kaynak arşivi zaten mevcut: ${TARBALL_NAME}"
fi

# Varsa önceki kaynak dizinlerini kaldır (temiz derleme için).
log "Eski kaynak klasörleri temizleniyor (varsa)."
rm -rf "gcc-${VERSION}"

# Arşivi aç.
log "Kaynak arşivi açılıyor..."
tar -xf "${TARBALL_NAME}"

# Kaynak dizinine gir.
cd "gcc-${VERSION}"

# GCC'nin gerekli bağımlılıklarını (gmp, mpfr, mpc) doğru sürümlerle içeri almak için resmi yardımcı betik.
log "Gerekli harici kütüphaneler indiriliyor (contrib/download_prerequisites)..."
./contrib/download_prerequisites

# Olası ortam çakışmalarını engellemek için derleme öncesi kritik değişkenleri temizle.
log "Ortam değişkenleri temizleniyor (LD_LIBRARY_PATH, LIBRARY_PATH, CPATH, C_INCLUDE_PATH, CPLUS_INCLUDE_PATH, PKG_CONFIG_PATH)..."
unset LD_LIBRARY_PATH || true
unset LIBRARY_PATH || true
unset CPATH || true
unset C_INCLUDE_PATH || true
unset CPLUS_INCLUDE_PATH || true
unset PKG_CONFIG_PATH || true

# Ayrı bir build klasörü oluştur (out-of-tree build, daha temizdir).
log "Build klasörü oluşturuluyor..."
rm -rf build
mkdir build
cd build

# Yapılandırma adımı: yalnızca C ve C++ dilleri; 64-bit dışı çoklu mimari desteği kapalı; NLS kapalı (msgfmt/ICU sorununun önüne geçer);
# program adlarının sonuna -8.5 ekleyerek sistem gcc ile çakışmayı önle.
log "Yapılandırma çalıştırılıyor..."
../configure   --prefix="${PREFIX}"   --enable-languages=c,c++   --disable-multilib   --disable-nls   --program-suffix="-8.5"

# Kaynakları derle; JOBS paralel iş sayısını kullan.
log "Derleme başlıyor (make -j${JOBS})... Bu adım makinenize bağlı olarak uzun sürebilir."
make -j"${JOBS}"

# Kurulum: /opt/gcc-8.5 altına dosyaları yerleştirir (sudo gerektirir).
log "Kurulum yapılıyor (sudo make install)..."
sudo make install

# update-alternatives ile yeni gcc ve g++ ikililerini sisteme tanıt (varsayılanı otomatik olarak değiştirmez).
log "update-alternatives ile kaydediliyor..."
sudo update-alternatives --install /usr/bin/gcc gcc "${PREFIX}/bin/gcc-8.5" 20
sudo update-alternatives --install /usr/bin/g++ g++ "${PREFIX}/bin/g++-8.5" 20

# Kullanıcıya nasıl geçiş yapacağını göster.
log "Kurulum tamamlandı. Varsayılan derleyiciyi seçmek için aşağıdakini çalıştırabilirsiniz:"
echo "  sudo update-alternatives --config gcc"
echo "  sudo update-alternatives --config g++"

# Yeni derleyicinin sürümünü göster.
log "Yeni derleyici sürümü:"
"${PREFIX}/bin/gcc-8.5" --version || true
