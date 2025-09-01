#include "JsonConfigParser.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace utilities {

std::map<std::string, std::string> JsonConfigParser::parseServiceMetadata(const std::string& filename) {
    try {
        std::string jsonContent = readJsonFile(filename);
        std::string metadata = extractServiceMetadataSection(jsonContent);
        
        std::map<std::string, std::string> config;
        
        // Her bir key-value çiftini çıkar
        config["protocol"] = extractJsonValue(metadata, "protocol");
        config["multicast_address"] = extractJsonValue(metadata, "multicast_address");
        config["port"] = extractJsonValue(metadata, "port");
        
        return config;
        
    } catch (const std::exception& e) {
        std::cerr << "JSON konfigürasyon parse hatası: " << e.what() << std::endl;
        throw;
    }
}

std::string JsonConfigParser::readJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("JSON dosyası açılamadı: " + filename);
    }
    
    std::string jsonContent((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    file.close();
    
    if (jsonContent.empty()) {
        throw std::runtime_error("JSON dosyası boş: " + filename);
    }
    
    return jsonContent;
}

std::string JsonConfigParser::extractServiceMetadataSection(const std::string& jsonContent) {
    // x-service-metadata bölümünü bul
    size_t metadataStart = jsonContent.find("\"x-service-metadata\"");
    if (metadataStart == std::string::npos) {
        throw std::runtime_error("x-service-metadata bölümü bulunamadı");
    }
    
    // metadata bölümünün başlangıcını bul (açılış süslü parantezi)
    size_t braceStart = jsonContent.find("{", metadataStart);
    if (braceStart == std::string::npos) {
        throw std::runtime_error("x-service-metadata bölümü formatı hatalı");
    }
    
    // Nested braces'i say ve kapatma parantezini bul
    int braceCount = 1;
    size_t currentPos = braceStart + 1;
    size_t braceEnd = braceStart;
    
    while (braceCount > 0 && currentPos < jsonContent.length()) {
        if (jsonContent[currentPos] == '{') {
            braceCount++;
        } else if (jsonContent[currentPos] == '}') {
            braceCount--;
            if (braceCount == 0) {
                braceEnd = currentPos;
            }
        }
        currentPos++;
    }
    
    if (braceCount > 0) {
        throw std::runtime_error("x-service-metadata bölümü tamamlanmamış");
    }
    
    // metadata içeriğini çıkar (süslü parantezler hariç)
    return jsonContent.substr(braceStart + 1, braceEnd - braceStart - 1);
}

std::string JsonConfigParser::extractJsonValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    
    if (keyPos == std::string::npos) {
        return "";
    }
    
    // ":" karakterini bul
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) {
        return "";
    }
    
    // Değerin başlangıcını bul (whitespace'leri atla)
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && 
           (json[valueStart] == ' ' || json[valueStart] == '\t' || 
            json[valueStart] == '\n' || json[valueStart] == '\r')) {
        valueStart++;
    }
    
    if (valueStart >= json.length()) {
        return "";
    }
    
    std::string value;
    
    if (json[valueStart] == '"') {
        // String değer
        valueStart++; // Açılış tırnak işaretini atla
        size_t valueEnd = json.find("\"", valueStart);
        if (valueEnd != std::string::npos) {
            value = json.substr(valueStart, valueEnd - valueStart);
        }
    } else {
        // Numeric değer
        size_t valueEnd = valueStart;
        while (valueEnd < json.length() && 
               json[valueEnd] != ',' && json[valueEnd] != '}' && 
               json[valueEnd] != '\n' && json[valueEnd] != '\r') {
            valueEnd++;
        }
        value = json.substr(valueStart, valueEnd - valueStart);
        
        // Trailing whitespace'leri temizle
        while (!value.empty() && 
               (value.back() == ' ' || value.back() == '\t')) {
            value.pop_back();
        }
    }
    
    return value;
}

} // namespace utilities
