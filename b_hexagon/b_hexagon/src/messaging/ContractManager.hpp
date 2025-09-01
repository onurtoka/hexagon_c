/**
 * @file ContractManager.hpp
 * @brief Manages JSON schema contracts for message validation in hexagonal architecture
 * 
 * The ContractManager ensures data integrity by:
 * - Loading JSON schema contracts from files
 * - Validating incoming messages against schemas
 * - Providing contract-based messaging guarantees
 * - Supporting multiple contract types for different message formats
 */

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <optional>
#include <sstream>
#include <fstream>

/**
 * @class ContractManager
 * @brief Manages and validates JSON message contracts for secure communication
 * 
 * Key responsibilities:
 * - Load schema contracts from configuration files
 * - Validate messages against predefined contracts
 * - Ensure message format compliance across system boundaries
 * - Support multiple contract types for different data formats
 */
class ContractManager final {
public:
    ContractManager() noexcept = default;
    ~ContractManager() = default;

    // Disable copy operations (manage unique resources)
    ContractManager(const ContractManager&) = delete;
    ContractManager& operator=(const ContractManager&) = delete;

    // Enable move operations
    ContractManager(ContractManager&&) noexcept = default;
    ContractManager& operator=(ContractManager&&) noexcept = default;

    /**
     * @brief Load JSON schema contracts from directory
     * @param directory_path Path to directory containing schema files
     * @return true if contracts loaded successfully
     */
    bool load_contracts_from_directory(const std::string& directory_path);

    /**
     * @brief Validate message against loaded contracts
     * @param message JSON message string to validate
     * @return true if message is valid according to schema
     */
    bool validate_message(const std::string& message) const;

    /**
     * @brief Check if any contracts are loaded
     * @return true if contracts are available for validation
     */
    bool has_contracts() const noexcept;

    /**
     * @brief Get number of loaded contracts
     * @return Count of available schema contracts
     */
    std::size_t get_contract_count() const noexcept;

    /**
     * @brief Clear all loaded contracts
     */
    void clear_contracts() noexcept;

private:
    // Hash map storing contract name -> JSON schema content
    std::unordered_map<std::string, std::string> contracts_;

    /**
     * @brief Loads a single JSON schema file from disk
     * @param file_path Full path to the schema file
     * @return Schema content string or empty if failed to load
     */
    std::optional<std::string> load_contract_file(const std::string& file_path) const;

    /**
     * @brief Determines message type from JSON content for contract selection
     * @param json_content Raw JSON message content
     * @return Message type identifier string (empty if not found)
     */
    std::string extract_message_type(const std::string& json_content) const;

    /**
     * @brief Basic JSON syntax validation (brackets, quotes, etc.)
     * @param json_content JSON string to check
     * @return true if basic JSON structure is valid
     */
    bool is_valid_json_format(const std::string& json_content) const noexcept;

    /**
     * @brief Basic JSON schema validation
     * @param schema JSON schema string
     * @param message JSON message to validate
     * @return true if message conforms to schema
     */
    bool validate_against_schema(const std::string& schema, const std::string& message) const;
};
