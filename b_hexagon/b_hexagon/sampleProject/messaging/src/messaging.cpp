#include "messaging/messaging.h"
#include <fstream>
#include <iostream>

bool ContractManager::load_contracts_from_directory(const std::filesystem::path& contracts_directory) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(contracts_directory)) {
            // Sadece .json uzantılı dosyaları kontrol et
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::ifstream contract_file(entry.path());
                json contract_json;
                contract_file >> contract_json;

                if (contract_json.contains("title") && contract_json["title"].is_string()) {
                    std::string title = contract_json["title"];
                    json_validator validator;
                    validator.set_root_schema(contract_json);
                    validators_[title] = std::move(validator);
                    std::cout << "Kontrat yüklendi: " << title << " (" << entry.path().filename() << ")" << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Kontrat dizini okunurken hata: " << e.what() << std::endl;
        return false;
    }
    return true;
}

const json_validator* ContractManager::get_validator(const std::string& message_type) const {
    auto it = validators_.find(message_type);
    if (it == validators_.end()) {
        return nullptr;
    }
    return &it->second;
}


bool messaging::serialize_and_send(zmq::socket_t& socket, const std::string& topic, const json& message, const ContractManager& contract_manager) {
    const json_validator* validator = contract_manager.get_validator(topic);
    if (!validator) {
        std::cerr << "HATA: '" << topic << "' için kontrat bulunamadı. Mesaj gönderilmedi." << std::endl;
        return false;
    }

    try {
        validator->validate(message);
    } catch (const std::exception& e) {
        std::cerr << "HATA: '" << topic << "' mesajı kontrat doğrulamadan geçemedi. Detay: " << e.what() << std::endl;
        return false;
    }

    std::vector<uint8_t> packed_bytes = json::to_msgpack(message);

    try {
        socket.send(zmq::buffer(topic), zmq::send_flags::sndmore);
        socket.send(zmq::buffer(packed_bytes), zmq::send_flags::none);
    } catch (const zmq::error_t& e) {
        std::cerr << "HATA: ZeroMQ gönderme hatası: " << e.what() << std::endl;
        return false;
    }
    
    return true;
}


std::optional<json> messaging::receive_and_deserialize(zmq::socket_t& socket, const ContractManager& contract_manager) {
    std::vector<zmq::message_t> received_parts;
    auto result = zmq::recv_multipart(socket, std::back_inserter(received_parts));

    if (!result.has_value() || received_parts.size() != 2) {
        return std::nullopt; 
    }

    std::string topic = received_parts[0].to_string();
    const zmq::message_t& payload = received_parts[1];

    const json_validator* validator = contract_manager.get_validator(topic);
    if (!validator) {
        std::cerr << "HATA: Alınan '" << topic << "' mesajı için kontrat bulunamadı." << std::endl;
        return std::nullopt;
    }
    
    json deserialized_json;
    try {
        deserialized_json = json::from_msgpack(payload.to_string_view());
    } catch(const std::exception& e) {
        std::cerr << "HATA: MessagePack verisi çözülürken hata: " << e.what() << std::endl;
        return std::nullopt;
    }

    try {
        validator->validate(deserialized_json);
    } catch (const std::exception& e) {
        std::cerr << "HATA: Alınan '" << topic << "' mesajı kontrat doğrulamadan geçemedi. Detay: " << e.what() << std::endl;
        return std::nullopt;
    }

    return deserialized_json;
}