#ifndef JSON_CONFIG_PARSER_H
#define JSON_CONFIG_PARSER_H

#include <string>
#include <map>

namespace utilities {

class JsonConfigParser {
public:
    /**
     * JSON dosyasından x-service-metadata bölümünü parse eder
     * @param filename JSON dosyasının yolu
     * @return key-value çiftlerini içeren map
     * @throws std::runtime_error JSON dosyası okunamazsa veya parse edilemezse
     */
    static std::map<std::string, std::string> parseServiceMetadata(const std::string& filename);

private:
    /**
     * JSON string'inden belirtilen key'in değerini çıkarır
     * @param json JSON içeriği
     * @param key Aranacak key
     * @return key'in değeri, bulunamazsa boş string
     */
    static std::string extractJsonValue(const std::string& json, const std::string& key);
    
    /**
     * JSON dosyasını okur ve string olarak döndürür
     * @param filename Dosya yolu
     * @return Dosya içeriği
     * @throws std::runtime_error Dosya açılamazsa
     */
    static std::string readJsonFile(const std::string& filename);
    
    /**
     * JSON string'inden x-service-metadata bölümünü çıkarır
     * @param jsonContent Tam JSON içeriği
     * @return x-service-metadata bölümünün içeriği
     * @throws std::runtime_error Bölüm bulunamazsa veya hatalıysa
     */
    static std::string extractServiceMetadataSection(const std::string& jsonContent);
};

} // namespace utilities

#endif // JSON_CONFIG_PARSER_H
