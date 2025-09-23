#!/bin/bash

# Akıllı C++ Model Sınıfları Oluşturucu Script
# modelist.xml dosyasından yapılandırma okuyup model sınıfları oluşturur
# Radio (outgoing) ve Dish (incoming) yönüne göre optimize eder

set -euo pipefail

# Renkli çıktı
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}Akıllı C++ Model Sınıfları Oluşturucu${NC}"
echo "========================================="

# Çalıştırılan dizin
WORK_DIR="$(pwd)"
echo -e "${YELLOW}Çalıştırıldığı dizin: $WORK_DIR${NC}"

# Dizin kontrolü
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZMQ_MESSAGES_DIR="$SCRIPT_DIR/zmq_messages"

# modelist.xml dosyasını bul
MODELIST_FILE=""
if [ -f "$WORK_DIR/modelist.xml" ]; then
    MODELIST_FILE="$WORK_DIR/modelist.xml"
    echo -e "${GREEN}✅ modelist.xml bulundu: $MODELIST_FILE${NC}"
else
    echo -e "${RED}❌ Hata: modelist.xml dosyası bulunamadı!${NC}"
    echo -e "${YELLOW}Dosya şu konumda olmalı: $WORK_DIR/modelist.xml${NC}"
    exit 1
fi

# Model dizinini bul - ZMQDEF/zmq_messages/zmq_models altında olmalı
MODEL_DIR=""
# Önce çalıştığı dizinde ZMQDEF/zmq_messages/zmq_models klasörü ara
if [ -d "$WORK_DIR/ZMQDEF/zmq_messages/zmq_models" ]; then
    MODEL_DIR="$WORK_DIR/ZMQDEF/zmq_messages/zmq_models"
elif [ -d "$WORK_DIR/zmq_models" ]; then
    MODEL_DIR="$WORK_DIR/zmq_models"
elif [ -d "$WORK_DIR/../ZMQDEF/zmq_messages/zmq_models" ]; then
    MODEL_DIR="$WORK_DIR/../ZMQDEF/zmq_messages/zmq_models"
else
    # ZMQDEF/zmq_messages/zmq_models dizinini oluştur
    MODEL_DIR="$WORK_DIR/ZMQDEF/zmq_messages/zmq_models"
    mkdir -p "$MODEL_DIR"
    echo -e "${YELLOW}⚠️  ZMQDEF/zmq_messages/zmq_models dizini oluşturuldu: $MODEL_DIR${NC}"
fi

echo -e "${GREEN}📁 Model dizini: $MODEL_DIR${NC}"

# XML'den model yapılandırmasını oku
read_model_config() {
    echo -e "${BLUE}modelist.xml okunuyor...${NC}"
    
    # XML parsing için xmlstarlet kullan (eğer yoksa basit grep/sed)
    if command -v xmlstarlet &> /dev/null; then
        echo -e "${GREEN}✅ xmlstarlet bulundu, XML parsing yapılıyor${NC}"
        xmlstarlet sel -t -m "//model" -v "@name" -o "=" -v "@type" -n "$MODELIST_FILE"
    else
        echo -e "${YELLOW}⚠️  xmlstarlet bulunamadı, basit parsing kullanılıyor${NC}"
        # Basit XML parsing - grep ve sed ile
        grep -E '<model[^>]*name="[^"]*"[^>]*type="[^"]*"' "$MODELIST_FILE" | \
        sed -E 's/.*<model[^>]*name="([^"]*)"[^>]*type="([^"]*)"[^>]*.*/\1=\2/'
    fi
}

# Model tipine göre sınıf yönünü belirle
get_class_direction() {
    local model_type="$1"
    case "$model_type" in
        "radio")
            echo "outgoing"
            ;;
        "dish")
            echo "incoming"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

# JSON'dan C++ tipini belirle (Gelişmiş sürüm)
get_cpp_type() {
    local json_type="$1"
    local minimum="$2"
    local maximum="$3"
    local format="$4"
    
    # Eğer format belirtilmişse, doğrudan kullan
    if [ "$format" != "null" ] && [ -n "$format" ]; then
        case "$format" in
            "int8"|"byte")
                echo "int8_t"
                ;;
            "uint8"|"ubyte")
                echo "uint8_t"
                ;;
            "int16"|"short")
                echo "int16_t"
                ;;
            "uint16"|"ushort")
                echo "uint16_t"
                ;;
            "int32"|"int")
                echo "int32_t"
                ;;
            "uint32"|"uint")
                echo "uint32_t"
                ;;
            "int64"|"long")
                echo "int64_t"
                ;;
            "uint64"|"ulong")
                echo "uint64_t"
                ;;
            "float"|"float32")
                echo "float"
                ;;
            "double"|"float64")
                echo "double"
                ;;
            *)
                echo "std::string"
                ;;
        esac
        return
    fi
    
    # Format yoksa, tip ve range'e göre akıllı seçim yap
    case "$json_type" in
        "integer")
            if [ "$minimum" != "null" ] && [ "$maximum" != "null" ]; then
                # Önce unsigned olup olmadığını kontrol et
                if [ "$minimum" -ge 0 ] 2>/dev/null; then
                    # Unsigned integer ranges
                    if [ "$maximum" -le 255 ] 2>/dev/null; then
                        echo "uint8_t"     # 0 to 255
                    elif [ "$maximum" -le 65535 ] 2>/dev/null; then
                        echo "uint16_t"   # 0 to 65,535
                    elif [ "$maximum" -le 4294967295 ] 2>/dev/null; then
                        echo "uint32_t"   # 0 to 4,294,967,295
                    else
                        echo "uint64_t"   # Büyük unsigned değerler
                    fi
                else
                    # Signed integer ranges
                    if [ "$minimum" -ge -128 ] && [ "$maximum" -le 127 ] 2>/dev/null; then
                        echo "int8_t"     # -128 to 127
                    elif [ "$minimum" -ge -32768 ] && [ "$maximum" -le 32767 ] 2>/dev/null; then
                        echo "int16_t"    # -32,768 to 32,767
                    elif [ "$minimum" -ge -2147483648 ] && [ "$maximum" -le 2147483647 ] 2>/dev/null; then
                        echo "int32_t"    # -2,147,483,648 to 2,147,483,647
                    else
                        echo "int64_t"    # Büyük signed değerler
                    fi
                fi
            else
                # Range belirtilmemişse varsayılan
                echo "int64_t"
            fi
            ;;
        "number")
            if [ "$minimum" != "null" ] && [ "$maximum" != "null" ]; then
                # Float precision kontrolü
                local abs_min=$(echo "$minimum" | tr -d '-')
                local abs_max=$(echo "$maximum" | tr -d '-')
                
                # Float range: yaklaşık ±3.4e38, precision: 7 decimal digit
                # Double range: yaklaşık ±1.7e308, precision: 15 decimal digit
                
                # Basit heuristic: değer aralığı ve precision'a göre karar ver
                # Küçük değerler (1e6'dan az) için float yeterli olabilir
                if ([ "$abs_max" -lt 1000000 ] 2>/dev/null || echo "$abs_max" | grep -E '^[0-9]+(\.[0-9]{1,6})?$' >/dev/null); then
                    echo "float"
                else
                    echo "double"
                fi
            else
                # Range belirtilmemişse varsayılan double
                echo "double"
            fi
            ;;
        "string")
            echo "std::string"
            ;;
        *)
            echo "std::string"
            ;;
    esac
}

# Validation fonksiyonu oluştur
create_validation_function() {
    local cpp_type="$1"
    local field_name="$2"
    local minimum="$3"
    local maximum="$4"
    local var_name="$5"
    
    # Tip kategorisine göre validation oluştur
    if [[ "$cpp_type" =~ ^(int8_t|int16_t|int32_t|int64_t)$ ]]; then
        # Signed integer types - uygun suffix seç
        local min_suffix="LL"
        local max_suffix="LL"
        
        # int8_t ve int16_t için suffix gereksiz
        if [[ "$cpp_type" =~ ^(int8_t|int16_t)$ ]]; then
            min_suffix=""
            max_suffix=""
        fi
        
        echo "    void validate${field_name}($cpp_type value) const {"
        echo "        if (value < ${minimum}${min_suffix} || value > ${maximum}${max_suffix}) {"
        echo "            throw std::out_of_range(\"${field_name} value is out of valid range: \" + std::to_string(value));"
        echo "        }"
        echo "    }"
        echo ""
    elif [[ "$cpp_type" =~ ^(uint8_t|uint16_t|uint32_t|uint64_t)$ ]]; then
        # Unsigned integer types - uygun suffix seç
        local max_suffix="ULL"
        
        # uint8_t ve uint16_t için suffix gereksiz
        if [[ "$cpp_type" =~ ^(uint8_t|uint16_t)$ ]]; then
            max_suffix=""
        elif [[ "$cpp_type" == "uint32_t" ]]; then
            max_suffix="U"
        fi
        
        echo "    void validate${field_name}($cpp_type value) const {"
        echo "        if (value > ${maximum}${max_suffix}) {"
        echo "            throw std::out_of_range(\"${field_name} value is out of valid range: \" + std::to_string(value));"
        echo "        }"
        echo "    }"
        echo ""
    elif [[ "$cpp_type" =~ ^(float|double)$ ]]; then
        # Floating point types
        echo "    void validate${field_name}($cpp_type value) const {"
        echo "        if (std::isnan(value) || value < $minimum || value > $maximum) {"
        echo "            throw std::out_of_range(\"${field_name} value is out of valid range: \" + std::to_string(value));"
        echo "        }"
        echo "    }"
        echo ""
    fi
}

# Tek JSON dosyasını işle (Gelişmiş sürüm - direction aware)
process_json_file() {
    local json_file="$1"
    local model_direction="$2"  # "outgoing" or "incoming"
    local filename=$(basename "$json_file" .json)
    
    echo -e "${YELLOW}${filename} sınıfı işleniyor (${model_direction})...${NC}"
    
    # JSON'dan title ve properties'i çıkar
    local title=$(jq -r '.title // "UnknownClass"' "$json_file")
    local description=$(jq -r '.description // "Açıklama yok"' "$json_file")
    
    # Header ve source dosyalarını oluştur
    local header_file="$MODEL_DIR/${title}.hpp"
    local source_file="$MODEL_DIR/${title}.cpp"
    
    # x-service-metadata bilgilerini çıkar
    local multicast_address=$(jq -r '."x-service-metadata".multicast_address // "null"' "$json_file")
    local port=$(jq -r '."x-service-metadata".port // "null"' "$json_file")
    
    cat > "$header_file" << EOF
#pragma once

// MISRA C++ 2023 compliant includes
#include <string>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <cstring>

/**
 * @brief $description
 * Auto-generated from ${filename}.json
 * MISRA C++ 2023 compliant implementation
 * Direction: $model_direction
 */
class $title final {
public:
EOF

    # Network configuration constants ekle
    if [ "$multicast_address" != "null" ] && [ -n "$multicast_address" ]; then
        echo "    // Network configuration constants" >> "$header_file"
        echo "    static constexpr const char* MULTICAST_ADDRESS = \"$multicast_address\";" >> "$header_file"
    fi
    
    if [ "$port" != "null" ] && [ -n "$port" ]; then
        if [ "$multicast_address" == "null" ] || [ -z "$multicast_address" ]; then
            echo "    // Network configuration constants" >> "$header_file"
        fi
        echo "    static constexpr int PORT = $port;" >> "$header_file"
    fi
    
    # Direction specific constants
    if [ "$model_direction" = "outgoing" ]; then
        cat >> "$header_file" << EOF
    
    // ZeroMQ RADIO socket configuration (outgoing)
    static constexpr const char* ZMQ_SOCKET_TYPE = "RADIO";
    static constexpr bool IS_PUBLISHER = true;
EOF
    elif [ "$model_direction" = "incoming" ]; then
        cat >> "$header_file" << EOF
    
    // ZeroMQ DISH socket configuration (incoming)
    static constexpr const char* ZMQ_SOCKET_TYPE = "DISH";
    static constexpr bool IS_SUBSCRIBER = true;
EOF
    fi
    
    if [ "$multicast_address" != "null" ] || [ "$port" != "null" ] || [ "$model_direction" != "unknown" ]; then
        echo "" >> "$header_file"
    fi

    cat >> "$header_file" << EOF
    // MISRA C++ 2023 compliant constructors
    explicit $title() noexcept;
    
    // Copy constructor
    $title(const $title& other) = default;
    
    // Move constructor
    $title($title&& other) noexcept = default;
    
    // Copy assignment operator
    $title& operator=(const $title& other) = default;
    
    // Move assignment operator
    $title& operator=($title&& other) noexcept = default;
    
    // Destructor
    ~$title() = default;
    
    // Getters and Setters
EOF

    # Getter/Setter declarations
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.minimum // "null") \(.value.maximum // "null") \(.value.format // "null")"' "$json_file" | while read -r field_name json_type minimum maximum format; do
        if [ "$minimum" = "null" ]; then minimum="0"; fi
        if [ "$maximum" = "null" ]; then maximum="1000000"; fi
        
        cpp_type=$(get_cpp_type "$json_type" "$minimum" "$maximum" "$format")
        field_name_cap="$(tr '[:lower:]' '[:upper:]' <<< ${field_name:0:1})${field_name:1}"
        
        # Getter declaration - MISRA compliant
        echo "    $cpp_type get${field_name_cap}() const noexcept;" >> "$header_file"
        
        # Setter declaration - MISRA compliant
        echo "    void set${field_name_cap}(const $cpp_type& value);" >> "$header_file"
        echo "" >> "$header_file"
    done
    
    # isValid method declaration
    cat >> "$header_file" << EOF
    // Validation - MISRA compliant
    [[nodiscard]] bool isValid() const noexcept;

    // Binary Serialization - MISRA compliant
    [[nodiscard]] std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data) noexcept;
    [[nodiscard]] std::size_t getSerializedSize() const noexcept;

private:
EOF

    # Private member variables
    echo "    // Member variables" >> "$header_file"
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.minimum // "null") \(.value.maximum // "null") \(.value.format // "null") \(.value.description)"' "$json_file" | while read -r field_name json_type minimum maximum format description; do
        if [ "$minimum" = "null" ]; then minimum="0"; fi
        if [ "$maximum" = "null" ]; then maximum="1000000"; fi
        
        cpp_type=$(get_cpp_type "$json_type" "$minimum" "$maximum" "$format")
        echo "    /// $description" >> "$header_file"
        echo "    $cpp_type ${field_name}_;" >> "$header_file"
    done
    
    echo "" >> "$header_file"
    echo "    // Validation functions - MISRA compliant" >> "$header_file"
    
    # Validation function declarations
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.minimum // "null") \(.value.maximum // "null") \(.value.format // "null")"' "$json_file" | while read -r field_name json_type minimum maximum format; do
        if [ "$minimum" = "null" ]; then minimum="0"; fi
        if [ "$maximum" = "null" ]; then maximum="1000000"; fi
        
        cpp_type=$(get_cpp_type "$json_type" "$minimum" "$maximum" "$format")
        field_name_cap="$(tr '[:lower:]' '[:upper:]' <<< ${field_name:0:1})${field_name:1}"
        
        if [ "$cpp_type" != "std::string" ]; then
            echo "    void validate${field_name_cap}($cpp_type value) const;" >> "$header_file"
        fi
    done
    
    cat >> "$header_file" << EOF
};
EOF

    # Source dosyası oluştur (.cpp)
    cat > "$source_file" << EOF
#include "${title}.hpp"

// MISRA C++ 2023 compliant constructor implementation
$title::$title() noexcept {
EOF

    # Constructor initialization - MISRA compliant
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.format // "null")"' "$json_file" | while read -r field_name json_type format; do
        cpp_type=$(get_cpp_type "$json_type" "0" "1000000" "$format")
        if [[ "$cpp_type" =~ int.*_t|float|double ]]; then
            echo "    ${field_name}_ = static_cast<$cpp_type>(0);" >> "$source_file"
        else
            echo "    ${field_name}_.clear();" >> "$source_file"
        fi
    done
    
    cat >> "$source_file" << EOF
}

EOF

    # Validation functions implementation
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.minimum // "null") \(.value.maximum // "null") \(.value.format // "null")"' "$json_file" | while read -r field_name json_type minimum maximum format; do
        if [ "$minimum" = "null" ]; then minimum="0"; fi
        if [ "$maximum" = "null" ]; then maximum="1000000"; fi
        
        cpp_type=$(get_cpp_type "$json_type" "$minimum" "$maximum" "$format")
        field_name_cap="$(tr '[:lower:]' '[:upper:]' <<< ${field_name:0:1})${field_name:1}"
        
        if [ "$cpp_type" != "std::string" ]; then
            create_validation_function "$cpp_type" "$field_name_cap" "$minimum" "$maximum" "$field_name" | sed "s/void validate/void $title::validate/" >> "$source_file"
        fi
    done

    # Getter/Setter implementations
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.minimum // "null") \(.value.maximum // "null") \(.value.format // "null")"' "$json_file" | while read -r field_name json_type minimum maximum format; do
        if [ "$minimum" = "null" ]; then minimum="0"; fi
        if [ "$maximum" = "null" ]; then maximum="1000000"; fi
        
        cpp_type=$(get_cpp_type "$json_type" "$minimum" "$maximum" "$format")
        field_name_cap="$(tr '[:lower:]' '[:upper:]' <<< ${field_name:0:1})${field_name:1}"
        
        # Getter implementation - MISRA compliant
        echo "$cpp_type $title::get${field_name_cap}() const noexcept {" >> "$source_file"
        echo "    return ${field_name}_;" >> "$source_file"
        echo "}" >> "$source_file"
        echo "" >> "$source_file"
        
        # Setter implementation - MISRA compliant
        echo "void $title::set${field_name_cap}(const $cpp_type& value) {" >> "$source_file"
        if [ "$cpp_type" != "std::string" ]; then
            echo "    validate${field_name_cap}(value);" >> "$source_file"
        fi
        echo "    ${field_name}_ = value;" >> "$source_file"
        echo "}" >> "$source_file"
        echo "" >> "$source_file"
    done
    
    # isValid method implementation
    cat >> "$source_file" << EOF
bool $title::isValid() const noexcept {
    try {
EOF

    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.format // "null")"' "$json_file" | while read -r field_name json_type format; do
        cpp_type=$(get_cpp_type "$json_type" "0" "1000000" "$format")
        field_name_cap="$(tr '[:lower:]' '[:upper:]' <<< ${field_name:0:1})${field_name:1}"
        
        if [ "$cpp_type" != "std::string" ]; then
            echo "        validate${field_name_cap}(${field_name}_);" >> "$source_file"
        fi
    done
    
    cat >> "$source_file" << EOF
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// MISRA C++ 2023 compliant Binary Serialization Implementation
std::vector<uint8_t> $title::serialize() const {
    std::vector<uint8_t> buffer;
    buffer.reserve(getSerializedSize());
    
EOF

    # Her field için serialize kodu oluştur
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.format // "null")"' "$json_file" | while read -r field_name json_type format; do
        cpp_type=$(get_cpp_type "$json_type" "0" "1000000" "$format")
        
        if [[ "$cpp_type" =~ int.*_t|float|double ]]; then
            cat >> "$source_file" << EOF
    // Serialize ${field_name}_
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&${field_name}_);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(${field_name}_));
    }
    
EOF
        elif [ "$cpp_type" = "std::string" ]; then
            cat >> "$source_file" << EOF
    // Serialize ${field_name}_ (string) - MISRA compliant
    {
        const std::uint32_t length = static_cast<std::uint32_t>(${field_name}_.length());
        const std::uint8_t* length_ptr = reinterpret_cast<const std::uint8_t*>(&length);
        buffer.insert(buffer.end(), length_ptr, length_ptr + sizeof(length));
        buffer.insert(buffer.end(), ${field_name}_.cbegin(), ${field_name}_.cend());
    }
    
EOF
        fi
    done
    
    cat >> "$source_file" << EOF
    return buffer;
}

bool $title::deserialize(const std::vector<uint8_t>& data) noexcept {
    if (data.size() < getSerializedSize()) {
        return false;
    }
    
    std::size_t offset = 0U;
    
EOF

    # Her field için deserialize kodu oluştur
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.format // "null")"' "$json_file" | while read -r field_name json_type format; do
        cpp_type=$(get_cpp_type "$json_type" "0" "1000000" "$format")
        
        if [[ "$cpp_type" =~ int.*_t|float|double ]]; then
            cat >> "$source_file" << EOF
    // Deserialize ${field_name}_
    if (offset + sizeof(${field_name}_) <= data.size()) {
        std::memcpy(&${field_name}_, &data[offset], sizeof(${field_name}_));
        offset += sizeof(${field_name}_);
    } else {
        return false;
    }
    
EOF
        elif [ "$cpp_type" = "std::string" ]; then
            cat >> "$source_file" << EOF
    // Deserialize ${field_name}_ (string) - MISRA compliant
    if (offset + sizeof(std::uint32_t) <= data.size()) {
        std::uint32_t length{0U};
        std::memcpy(&length, &data[offset], sizeof(length));
        offset += sizeof(std::uint32_t);
        
        if (offset + length <= data.size()) {
            ${field_name}_.assign(reinterpret_cast<const char*>(&data[offset]), length);
            offset += length;
        } else {
            return false;
        }
    } else {
        return false;
    }
    
EOF
        fi
    done
    
    cat >> "$source_file" << EOF
    return true;
}

std::size_t $title::getSerializedSize() const noexcept {
    std::size_t size = 0U;
    
EOF

    # Her field için size hesaplama
    jq -r '.properties | to_entries[] | "\(.key) \(.value.type) \(.value.format // "null")"' "$json_file" | while read -r field_name json_type format; do
        cpp_type=$(get_cpp_type "$json_type" "0" "1000000" "$format")
        
        if [[ "$cpp_type" =~ int.*_t|float|double ]]; then
            cat >> "$source_file" << EOF
    size += sizeof(${field_name}_);  // ${cpp_type}
EOF
        elif [ "$cpp_type" = "std::string" ]; then
            cat >> "$source_file" << EOF
    size += sizeof(std::uint32_t) + ${field_name}_.length();  // string length + data
EOF
        fi
    done
    
    cat >> "$source_file" << EOF
    
    return size;
}
EOF

    echo -e "${GREEN}✅ ${title}.hpp ve ${title}.cpp oluşturuldu${NC}"
}

# Ana fonksiyon
main() {
    echo -e "${BLUE}modelist.xml yapılandırması okunuyor...${NC}"
    
    # ZMQ Messages dizini kontrolü
    if [ ! -d "$ZMQ_MESSAGES_DIR" ]; then
        echo -e "${RED}Hata: zmq_messages dizini bulunamadı: $ZMQ_MESSAGES_DIR${NC}"
        exit 1
    fi
    
    # XML'den model yapılandırmasını oku ve array'e dönüştür
    mapfile -t model_configs < <(read_model_config)
    
    if [ ${#model_configs[@]} -eq 0 ]; then
        echo -e "${RED}Hata: modelist.xml'de model bulunamadı!${NC}"
        exit 1
    fi
    
    echo -e "${YELLOW}Bulunan model yapılandırmaları:${NC}"
    for config in "${model_configs[@]}"; do
        if [[ "$config" =~ ^[a-zA-Z] ]]; then  # Sadece geçerli model entries
            IFS='=' read -r model_name model_type <<< "$config"
            direction=$(get_class_direction "$model_type")
            echo "  - $model_name ($model_type → $direction)"
        fi
    done
    echo ""
    
    # Her model için işlem yap
    for config in "${model_configs[@]}"; do
        if [[ ! "$config" =~ ^[a-zA-Z] ]]; then
            continue  # Skip invalid entries
        fi
        
        IFS='=' read -r model_name model_type <<< "$config"
        direction=$(get_class_direction "$model_type")
        
        # Corresponding JSON file'ı bul
        json_file=""
        if [ -f "$ZMQ_MESSAGES_DIR/${model_name}.json" ]; then
            json_file="$ZMQ_MESSAGES_DIR/${model_name}.json"
        else
            echo -e "${RED}⚠️  Uyarı: ${model_name}.json dosyası bulunamadı!${NC}"
            continue
        fi
        
        # jq'nin kurulu olup olmadığını kontrol et
        if ! command -v jq &> /dev/null; then
            echo -e "${RED}Hata: jq komutu bulunamadı. Lütfen jq'yu kurun: sudo apt-get install jq${NC}"
            exit 1
        fi
        
        process_json_file "$json_file" "$direction"
        echo ""
    done
    
    echo -e "${GREEN}🎉 Tüm C++ Model sınıfları başarıyla oluşturuldu!${NC}"
    echo -e "${GREEN}📁 Model dizini: $MODEL_DIR${NC}"
    echo ""
    echo -e "${YELLOW}Oluşturulan dosyalar:${NC}"
    ls -la "$MODEL_DIR"/*.hpp "$MODEL_DIR"/*.cpp 2>/dev/null || echo "Henüz dosya oluşturulmadı"
}

# Script'i çalıştır
main
