/**
 * @file messaging.hpp
 * @brief High-level messaging library for ZeroMQ RADIO/DISH communication with contract validation
 * 
 * This file provides:
 * - JSON message formatting and validation
 * - Contract-based message validation
 * - ZeroMQ RADIO/DISH group messaging utilities
 * - Simple wrapper classes for clean messaging API
 */

#pragma once

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types for group messaging
#include "messaging/ContractManager.hpp"
#include <zmq.hpp>
#include <string>
#include <optional>
#include <memory>

/**
 * @brief Format JSON string with proper indentation
 * @param json_str Raw JSON string to format
 * @param indent Number of spaces for indentation  
 * @return Formatted JSON string
 */
std::string format_json_output(const std::string& json_str, int indent) noexcept;

/**
 * @class JsonMessage
 * @brief Simple wrapper for JSON messages with formatting and validation
 * 
 * Provides easy-to-use interface for:
 * - Creating JSON messages from strings
 * - Formatting JSON with proper indentation
 * - Validating JSON structure
 * - Getting raw or formatted JSON content
 */
class JsonMessage {
public:
    /**
     * @brief Constructor from JSON string
     * @param json_content Valid JSON string content
     */
    explicit JsonMessage(const std::string& json_content);

    /**
     * @brief Get formatted JSON string
     * @param indent Indentation level (spaces)
     * @return Formatted JSON string
     */
    std::string dump(int indent = 0) const noexcept;

    /**
     * @brief Get raw JSON content
     * @return Unformatted JSON string
     */
    const std::string& raw() const noexcept;

    /**
     * @brief Check if JSON content is valid
     * @return true if valid JSON format
     */
    bool is_valid() const noexcept;

private:
    std::string json_content_;  // Stores the raw JSON string
    bool is_valid_;             // Tracks if JSON format is valid

    /**
     * @brief Basic JSON format validation
     * @param json JSON string to check
     * @return true if basic JSON structure is valid
     */
    static bool validate_json(const std::string& json) noexcept;
};

/**
 * @brief High-level messaging utilities for ZeroMQ RADIO/DISH pattern
 * 
 * This namespace provides convenient functions for:
 * - Sending/receiving messages with contract validation
 * - Group-based messaging using RADIO/DISH sockets
 * - Automatic message validation and error handling
 */
namespace messaging {

    /**
     * @brief Receive contract-validated message from ZeroMQ socket
     * @param socket ZeroMQ socket to receive from
     * @param contract_manager Contract manager for validation
     * @return JSON message if received and valid, nullopt otherwise
     */
    std::optional<JsonMessage> receive_contract_message(
        zmq::socket_t& socket,
        const ContractManager& contract_manager
    );

    /**
     * @brief Receive message with group filtering (RADIO/DISH pattern)
     * @param socket ZeroMQ DISH socket
     * @param group Group name to filter by
     * @param contract_manager Contract manager for validation
     * @return JSON message if received and valid, nullopt otherwise
     */
    std::optional<JsonMessage> receive_group_message(
        zmq::socket_t& socket,
        const std::string& group,
        const ContractManager& contract_manager
    );

    /**
     * @brief Send contract-validated message to ZeroMQ socket
     * @param socket ZeroMQ socket to send to
     * @param message JSON message to send
     * @param contract_manager Contract manager for validation
     * @return true if message was validated and sent successfully
     */
    bool send_contract_message(
        zmq::socket_t& socket,
        const JsonMessage& message,
        const ContractManager& contract_manager
    );

    /**
     * @brief Send message with group tagging (RADIO/DISH pattern)
     * @param socket ZeroMQ RADIO socket
     * @param message JSON message to send
     * @param group Group name to tag message with
     * @param contract_manager Contract manager for validation
     * @return true if message was validated and sent successfully
     */
    bool send_group_message(
        zmq::socket_t& socket,
        const JsonMessage& message,
        const std::string& group,
        const ContractManager& contract_manager
    );

} // namespace messaging
