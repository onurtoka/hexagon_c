# Socket Konfigürasyon Eşitleme Raporu

## ✅ Başarıyla Eşitlendi

### 📡 **Ortak Socket Konfigürasyonu:**

- **Endpoint:** `udp://239.1.1.1:9001`
- **Group:** `SOURCE_DATA`
- **Protocol:** UDP Multicast
- **Pattern:** ZeroMQ RADIO/DISH

### 🔄 **Değişiklik Detayları:**

#### B_Hexagon Projesinde Yapılan Değişiklikler:

1. **ExtrapTrackData.json**
   - `multicast_address`: `239.1.1.5` → `239.1.1.1`
   - `port`: `9596` → `9001`

2. **DelayCalcTrackData.json**
   - `multicast_address`: `239.1.1.5` → `239.1.1.1`  
   - `port`: `9595` → `9001`

3. **ZeroMQDataHandler.cpp**
   - `group_`: `"TRACK_DATA_UDP"` → `"SOURCE_DATA"`

4. **ZeroMQDataSender.cpp**
   - `group_`: `"TRACK_DATA_UDP"` → `"SOURCE_DATA"`

5. **main.cpp**
   - Endpoint log: `udp://239.255.0.1:7779` → `udp://239.1.1.1:9001`
   - Group log: `TRACK_DATA_UDP` → `SOURCE_DATA`

### 🧪 **Test Sonuçları:**

✅ **B_Hexagon:** Başarıyla çalışıyor
```
Endpoint: udp://239.1.1.1:9001, Group: SOURCE_DATA
```

✅ **Hexagon_C:** Başarıyla çalışıyor  
```
📡 Endpoint: udp://239.1.1.1:9001
👥 Group: SOURCE_DATA
```

### 📋 **Artık Her İki Proje de Aynı Konfigürasyonu Kullanıyor:**

- Aynı UDP multicast adresinde (`239.1.1.1:9001`) çalışırlar
- Aynı grup adını (`SOURCE_DATA`) kullanırlar
- Birbirleri ile ZeroMQ RADIO/DISH pattern ile haberleşebilirler

### 🚀 **Çalıştırma:**

```bash
# B_Hexagon
cd /workspaces/hexagon_c/b_hexagon/b_hexagon
./quick_build.sh

# Hexagon_C  
cd /workspaces/hexagon_c/hexagon_c/hexagon_c
cmake --build build
./hexagon_c_app
```

Her iki uygulama da artık aynı socket konfigürasyonunu kullandığı için birbirleri ile haberleşebilir!
