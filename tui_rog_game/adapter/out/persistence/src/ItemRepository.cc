#include "ItemRepository.h"
#include <spdlog/spdlog.h>
#include <algorithm> // For std::transform
#include <cctype> // For std::tolower

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                ItemRepository::ItemRepository(std::shared_ptr<leveldb::DB> db)
                    : db_(db) {
                    if (!db_) {
                        spdlog::error("ItemRepository: Initialized with null LevelDB pointer.");
                    }
                }

                std::string ItemRepository::toLower(std::string s) const {
                    std::transform(s.begin(), s.end(), s.begin(),
                                   [](unsigned char c){ return std::tolower(c); });
                    return s;
                }

                nlohmann::json ItemRepository::serializeItem(const Domain::Model::Item& item) const {
                    nlohmann::json j;
                    j["type"] = static_cast<int>(item.getType()); // Store enum as int
                    j["name"] = item.getName();
                    return j;
                }

                std::optional<Domain::Model::Item> ItemRepository::deserializeItem(const nlohmann::json& j) const {
                    try {
                        if (j.contains("type") && j.contains("name")) {
                            Domain::Model::Item::ItemType type = static_cast<Domain::Model::Item::ItemType>(j["type"].get<int>());
                            std::string name = j["name"].get<std::string>();
                            return Domain::Model::Item(type, name);
                        }
                        spdlog::error("ItemRepository: JSON missing 'type' or 'name' fields.");
                        return std::nullopt;
                    } catch (const nlohmann::json::exception& e) {
                        spdlog::error("ItemRepository: Failed to deserialize Item JSON: {}", e.what());
                        return std::nullopt;
                    }
                }

                void ItemRepository::save(const std::string& key, const Domain::Model::Item& item, leveldb::WriteBatch& batch) {
                    std::string lower_key = toLower(key);
                    nlohmann::json j = serializeItem(item);
                    batch.Put(lower_key, j.dump());
                    spdlog::debug("ItemRepository: Added Put for key '{}' to WriteBatch.", lower_key);
                }

                std::optional<Domain::Model::Item> ItemRepository::findById(const std::string& key) {
                    if (!db_) {
                        spdlog::error("ItemRepository: Cannot find, LevelDB is not open.");
                        return std::nullopt;
                    }

                    std::string lower_key = toLower(key);
                    std::string value_str;
                    leveldb::Status status = db_->Get(leveldb::ReadOptions(), lower_key, &value_str);

                    if (status.IsNotFound()) {
                        spdlog::debug("ItemRepository: Key '{}' not found.", lower_key);
                        return std::nullopt;
                    } else if (!status.ok()) {
                        spdlog::error("ItemRepository: Failed to find key '{}': {}", lower_key, status.ToString());
                        return std::nullopt;
                    }
                    else {
                        try {
                            nlohmann::json j = nlohmann::json::parse(value_str);
                            return deserializeItem(j);
                        } catch (const nlohmann::json::exception& e) {
                            spdlog::error("ItemRepository: Failed to parse JSON for key '{}': {}", lower_key, e.what());
                            return std::nullopt;
                        }
                    }
                }

                void ItemRepository::deleteById(const std::string& key) {
                    if (!db_) {
                        spdlog::error("ItemRepository: Cannot delete, LevelDB is not open.");
                        return;
                    }

                    std::string lower_key = toLower(key);
                    leveldb::Status status = db_->Delete(leveldb::WriteOptions(), lower_key);

                    if (!status.ok()) {
                        spdlog::error("ItemRepository: Failed to delete key '{}': {}", lower_key, status.ToString());
                    } else {
                        spdlog::debug("ItemRepository: Deleted key '{}'.", lower_key);
                    }
                }

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame