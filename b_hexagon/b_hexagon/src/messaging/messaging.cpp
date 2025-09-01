/**
 * @file messaging.cpp
 * @brief Implementation of high-level ZeroMQ messaging with contract validation
 * 
 * Provides implementations for:
 * - JSON formatting with proper indentation
 * - Message validation and contract checking
 * - ZeroMQ RADIO/DISH group messaging operations
 * - Error handling for messaging operations
 */

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types
#include "messaging/messaging.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

// Formats JSON string with proper indentation for readable output
std::string format_json_output(const std::string& json_str, int indent) noexcept {
    if (json_str.empty() || indent <= 0) {
        return json_str;
    }
    
    std::string formatted;
    std::string indent_str(static_cast<std::size_t>(indent), ' ');
    int current_indent = 0;
    bool in_string = false;
    bool escaped = false;
    
    for (std::size_t i = 0; i < json_str.length(); ++i) {
        char c = json_str[i];
        
        if (escaped) {
            formatted += c;
            escaped = false;
            continue;
        }
        
        if (c == '\\') {
            escaped = true;
            formatted += c;
            continue;
        }
        
        if (c == '"') {
            in_string = !in_string;
            formatted += c;
            continue;
        }
        
        if (!in_string) {
            if (c == '{' || c == '[') {
                formatted += c;
                formatted += '\n';
                current_indent += indent;
                for (int j = 0; j < current_indent; ++j) {
                    formatted += ' ';
                }
            } else if (c == '}' || c == ']') {
                formatted += '\n';
                current_indent -= indent;
                for (int j = 0; j < current_indent; ++j) {
                    formatted += ' ';
                }
                formatted += c;
            } else if (c == ',') {
                formatted += c;
                formatted += '\n';
                for (int j = 0; j < current_indent; ++j) {
                    formatted += ' ';
                }
            } else if (c == ':') {
                formatted += c;
                formatted += ' ';
            } else if (!std::isspace(static_cast<unsigned char>(c))) {
                formatted += c;
            }
        } else {
            formatted += c;
        }
    }
    
    return formatted;
}

// JsonMessage implementation
JsonMessage::JsonMessage(const std::string& json_content)
    : json_content_(json_content), is_valid_(validate_json(json_content)) {
}

std::string JsonMessage::dump(int indent) const noexcept {
    if (indent <= 0) {
        return json_content_;
    }
    return format_json_output(json_content_, indent);
}

const std::string& JsonMessage::raw() const noexcept {
    return json_content_;
}

bool JsonMessage::is_valid() const noexcept {
    return is_valid_;
}

bool JsonMessage::validate_json(const std::string& json) noexcept {
    if (json.empty()) {
        return false;
    }
    
    // Basic JSON validation - check balanced braces
    int brace_count = 0;
    bool in_string = false;
    bool escaped = false;
    
    for (char c : json) {
        if (escaped) {
            escaped = false;
            continue;
        }
        
        if (c == '\\') {
            escaped = true;
            continue;
        }
        
        if (c == '"') {
            in_string = !in_string;
            continue;
        }
        
        if (!in_string) {
            if (c == '{') {
                ++brace_count;
            } else if (c == '}') {
                --brace_count;
                if (brace_count < 0) {
                    return false;
                }
            }
        }
    }
    
    return brace_count == 0;
}

// Messaging namespace implementation
namespace messaging {

std::optional<JsonMessage> receive_contract_message(
    zmq::socket_t& socket,
    const ContractManager& contract_manager
) {
    try {
        zmq::message_t message;
        auto recv_result = socket.recv(message, zmq::recv_flags::dontwait);
        
        if (!recv_result.has_value()) {
            return std::nullopt;
        }
        
        std::string message_str(static_cast<char*>(message.data()), message.size());
        
        // Validate with contract manager if contracts are available
        if (contract_manager.has_contracts()) {
            if (!contract_manager.validate_message(message_str)) {
                return std::nullopt;
            }
        }
        
        JsonMessage json_msg(message_str);
        if (!json_msg.is_valid()) {
            return std::nullopt;
        }
        
        return json_msg;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<JsonMessage> receive_group_message(
    zmq::socket_t& socket,
    const std::string& group,
    const ContractManager& contract_manager
) {
    try {
        // Use C++ API for RADIO/DISH group operations
        zmq::message_t received_msg;
        
        // Non-blocking receive
        auto recv_result = socket.recv(received_msg, zmq::recv_flags::dontwait);
        if (!recv_result) {
            return std::nullopt;
        }
        
        // Check group
        const char* msg_group = received_msg.group();
        if (msg_group == nullptr || group != std::string(msg_group)) {
            return std::nullopt;
        }
        
        // Extract message content
        std::string message_str(static_cast<char*>(received_msg.data()), received_msg.size());
        
        // Validate with contract manager
        if (contract_manager.has_contracts()) {
            if (!contract_manager.validate_message(message_str)) {
                return std::nullopt;
            }
        }
        
        JsonMessage json_msg(message_str);
        if (!json_msg.is_valid()) {
            return std::nullopt;
        }
        
        return json_msg;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

bool send_contract_message(
    zmq::socket_t& socket,
    const JsonMessage& message,
    const ContractManager& contract_manager
) {
    try {
        // Validate with contract manager if contracts are available
        if (contract_manager.has_contracts()) {
            if (!contract_manager.validate_message(message.raw())) {
                return false;
            }
        }
        
        if (!message.is_valid()) {
            return false;
        }
        
        zmq::message_t zmq_msg(message.raw().size());
        std::memcpy(zmq_msg.data(), message.raw().c_str(), message.raw().size());
        
        auto send_result = socket.send(zmq_msg, zmq::send_flags::dontwait);
        return send_result.has_value();
    } catch (const std::exception&) {
        return false;
    }
}

bool send_group_message(
    zmq::socket_t& socket,
    const JsonMessage& message,
    const std::string& group,
    const ContractManager& contract_manager
) {
    try {
        // Validate with contract manager
        if (contract_manager.has_contracts()) {
            if (!contract_manager.validate_message(message.raw())) {
                return false;
            }
        }
        
        if (!message.is_valid()) {
            return false;
        }
        
        // Use C++ API for RADIO group tagging
        zmq::message_t zmq_msg(message.raw().begin(), message.raw().end());
        
        // Set group for RADIO socket
        zmq_msg.set_group(group.c_str());
        
        // Send message (non-blocking)
        auto send_result = socket.send(zmq_msg, zmq::send_flags::dontwait);
        
        return send_result && (*send_result == message.raw().size());
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace messaging
