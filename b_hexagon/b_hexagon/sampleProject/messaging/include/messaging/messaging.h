#pragma once

#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <string>
#include <optional>
#include <map>
#include <filesystem>

using json = nlohmann::json;
using json_validator = nlohmann::json_schema::json_validator;

/**
 * @class ContractManager
 * @brief JSON formatındaki kontratları (şemaları) bir dizinden yükler ve yönetir.
 * Bu sınıf, başlangıçta tüm kontratları yükleyip 'derler' (doğrulayıcı oluşturur),
 * bu sayede çalışma anında her mesaj için dosya okuma ve şema derleme
 * maliyetinden kurtulunur.
 */
class ContractManager {
public:
    /**
     * @brief Belirtilen dizindeki tüm '.json' kontrat dosyalarını yükler.
     * @param contracts_directory Kontratların bulunduğu klasörün yolu.
     * @return Başarılı olursa true, aksi halde false döner.
     */
    bool load_contracts_from_directory(const std::filesystem::path& contracts_directory);

    /**
     * @brief Verilen mesaj tipine (konu başlığına) uygun doğrulayıcıyı döndürür.
     * @param message_type Kontratın 'title' alanıyla eşleşen mesaj tipi.
     * @return Doğrulayıcı bulunursa pointer'ını, bulunamazsa nullptr döner.
     */
    const json_validator* get_validator(const std::string& message_type) const;

private:
    std::map<std::string, json_validator> validators_;
};


/**
 * @namespace messaging
 * @brief Tüm uygulamalar için ortak mesajlaşma fonksiyonlarını içerir.
 */
namespace messaging {

/**
 * @brief Bir JSON nesnesini doğrular, MessagePack'e serileştirir ve ZeroMQ soketine gönderir.
 * @param socket Mesajın gönderileceği ZeroMQ soketi.
 * @param topic Gönderilecek mesajın tipi (örn: "TrackDataECEF").
 * @param message Gönderilecek nlohmann::json nesnesi.
 * @param contract_manager Kontratları içeren ve doğrulamayı yapacak olan yönetici.
 * @return Başarılı olursa true, doğrulama veya gönderme hatasında false döner.
 */
bool serialize_and_send(
    zmq::socket_t& socket, 
    const std::string& topic, 
    const json& message, 
    const ContractManager& contract_manager
);

/**
 * @brief ZeroMQ soketinden bir mesaj alır, MessagePack'ten çözer ve doğrular.
 * @param socket Mesajın alınacağı ZeroMQ soketi.
 * @param contract_manager Kontratları içeren ve doğrulamayı yapacak olan yönetici.
 * @return Başarılı olursa çözülmüş nlohmann::json nesnesini içeren bir std::optional döner.
 * Hata durumunda std::nullopt döner.
 */
std::optional<json> receive_and_deserialize(
    zmq::socket_t& socket, 
    const ContractManager& contract_manager
);

} // namespace messaging