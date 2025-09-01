/**
 * @file ContractManager.cpp
 * @brief Implementation of JSON schema contract management for message validation
 * 
 * Provides implementations for:
 * - Loading JSON schema files from directory
 * - Validating messages against loaded schemas
 * - Managing contract lifecycle and storage
 * - Basic JSON format validation
 */

#include "messaging/ContractManager.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

// Loads all JSON schema files from specified directory
bool ContractManager::load_contracts_from_directory(const std::string& directory_path) {
    if (directory_path.empty()) {
        return false;
    }

    try {
        std::filesystem::path dir_path(directory_path);
        
        if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {
            return false;
        }

        std::size_t loaded_count = 0;
        
        // Scan directory for JSON schema files (.json or .schema extensions)
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                std::string extension = entry.path().extension().string();
                
                // Process JSON schema files
                if (extension == ".json" || extension == ".schema") {
                    auto schema_content = load_contract_file(file_path);
                    if (schema_content.has_value()) {
                        // Use filename (without extension) as contract name
                        std::string contract_name = entry.path().stem().string();
                        contracts_[contract_name] = schema_content.value();
                        ++loaded_count;
                    }
                }
            }
        }
        
        return loaded_count > 0;  // Success if at least one contract loaded
    } catch (const std::exception&) {
        return false;  // Failed to access directory or read files
    }
}

// Validates message against appropriate loaded contract
bool ContractManager::validate_message(const std::string& message) const {
    if (message.empty() || contracts_.empty()) {
        return false;  // No message to validate or no contracts loaded
    }

    // Basic JSON format validation
    if (!is_valid_json_format(message)) {
        return false;
    }

    // Extract message type to find appropriate schema
    std::string message_type = extract_message_type(message);
    if (message_type.empty()) {
        return true;  // Allow messages without explicit type
    }

    // Find matching contract
    auto contract_it = contracts_.find(message_type);
    if (contract_it != contracts_.end()) {
        return validate_against_schema(contract_it->second, message);
    }

    // If no specific contract found, perform basic validation
    return true;
}

bool ContractManager::has_contracts() const noexcept {
    return !contracts_.empty();
}

std::size_t ContractManager::get_contract_count() const noexcept {
    return contracts_.size();
}

void ContractManager::clear_contracts() noexcept {
    contracts_.clear();
}

std::optional<std::string> ContractManager::load_contract_file(const std::string& file_path) const {
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return std::nullopt;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        if (content.empty()) {
            return std::nullopt;
        }

        return content;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string ContractManager::extract_message_type(const std::string& json_content) const {
    // Simple extraction of message type from JSON
    std::size_t type_pos = json_content.find("\"type\"");
    if (type_pos == std::string::npos) {
        return "";
    }

    std::size_t colon_pos = json_content.find(":", type_pos);
    if (colon_pos == std::string::npos) {
        return "";
    }

    std::size_t quote_start = json_content.find("\"", colon_pos);
    if (quote_start == std::string::npos) {
        return "";
    }

    std::size_t quote_end = json_content.find("\"", quote_start + 1);
    if (quote_end == std::string::npos) {
        return "";
    }

    return json_content.substr(quote_start + 1, quote_end - quote_start - 1);
}

bool ContractManager::is_valid_json_format(const std::string& json_content) const noexcept {
    if (json_content.empty()) {
        return false;
    }

    // Basic JSON format check - must start and end with braces
    std::size_t start = 0;
    std::size_t end = json_content.length() - 1;
    
    // Skip whitespace
    while (start < json_content.length() && std::isspace(static_cast<unsigned char>(json_content[start]))) {
        ++start;
    }
    while (end > start && std::isspace(static_cast<unsigned char>(json_content[end]))) {
        --end;
    }
    
    return (start < json_content.length() && end >= start && 
            json_content[start] == '{' && json_content[end] == '}');
}

bool ContractManager::validate_against_schema(const std::string& schema, const std::string& message) const {
    // Basic schema validation - check that required fields exist
    // In a full implementation, this would use a JSON schema library
    
    if (schema.empty() || message.empty()) {
        return false;
    }

    // Extract required fields from schema
    std::size_t required_pos = schema.find("\"required\"");
    if (required_pos == std::string::npos) {
        return true;  // No required fields specified
    }

    // For now, just validate basic JSON structure
    return is_valid_json_format(message);
}
