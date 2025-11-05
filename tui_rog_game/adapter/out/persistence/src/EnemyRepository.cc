#include "EnemyRepository.h"
#include <spdlog/spdlog.h>
#include <algorithm> // For std::transform
#include <cctype> // For std::tolower

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                EnemyRepository::EnemyRepository(std::shared_ptr<leveldb::DB> db)
                    : db_(db) {
                    if (!db_) {
                        spdlog::error("EnemyRepository: Initialized with null LevelDB pointer.");
                    }
                }

                std::string EnemyRepository::toLower(std::string s) const {
                    std::transform(s.begin(), s.end(), s.begin(),
                                   [](unsigned char c){ return std::tolower(c); });
                    return s;
                }

                nlohmann::json EnemyRepository::serializeEnemy(const Domain::Model::Enemy& enemy) const {
                    nlohmann::json j;
                    j["type"] = static_cast<int>(enemy.getType()); // Store enum as int
                    j["name"] = enemy.getName();
                    j["health"] = enemy.getHealth();
                    j["stats"]["strength"] = enemy.getStats().strength;
                    j["stats"]["dexterity"] = enemy.getStats().dexterity;
                    j["stats"]["intelligence"] = enemy.getStats().intelligence;
                    j["stats"]["vitality"] = enemy.getStats().vitality;
                    j["position"]["x"] = enemy.getPosition().x;
                    j["position"]["y"] = enemy.getPosition().y;
                    return j;
                }

                std::optional<Domain::Model::Enemy> EnemyRepository::deserializeEnemy(const nlohmann::json& j) const {
                    try {
                        if (j.contains("type") && j.contains("name") && j.contains("health") && j.contains("stats") && j.contains("position")) {
                            Domain::Model::Enemy::EnemyType type = static_cast<Domain::Model::Enemy::EnemyType>(j["type"].get<int>());
                            std::string name = j["name"].get<std::string>();
                            int health = j["health"].get<int>(); // Read health, but Enemy constructor might override
                            
                            Domain::Model::Stats stats;
                            stats.strength = j["stats"]["strength"].get<int>();
                            stats.dexterity = j["stats"]["dexterity"].get<int>();
                            stats.intelligence = j["stats"]["intelligence"].get<int>();
                            stats.vitality = j["stats"]["vitality"].get<int>();

                            Domain::Model::Position position;
                            position.x = j["position"]["x"].get<int>();
                            position.y = j["position"]["y"].get<int>();

                            // Construct Enemy. Note: Enemy constructor sets health based on vitality.
                            // If current health needs to be persisted independently, Enemy class needs modification.
                            Domain::Model::Enemy enemy(type, name, stats, position);
                            // If Enemy had a setHealth method, we would call enemy.setHealth(health) here.
                            // For now, the health from JSON is effectively ignored if constructor sets it.
                            return enemy;
                        }
                        spdlog::error("EnemyRepository: JSON missing required fields.");
                        return std::nullopt;
                    } catch (const nlohmann::json::exception& e) {
                        spdlog::error("EnemyRepository: Failed to deserialize Enemy JSON: {}", e.what());
                        return std::nullopt;
                    }
                }

                void EnemyRepository::save(const std::string& key, const Domain::Model::Enemy& enemy, leveldb::WriteBatch& batch) {
                    std::string lower_key = toLower(key);
                    nlohmann::json j = serializeEnemy(enemy);
                    batch.Put(lower_key, j.dump());
                    spdlog::debug("EnemyRepository: Added Put for key '{}' to WriteBatch.", lower_key);
                }

                std::optional<Domain::Model::Enemy> EnemyRepository::findById(const std::string& key) {
                    if (!db_) {
                        spdlog::error("EnemyRepository: Cannot find, LevelDB is not open.");
                        return std::nullopt;
                    }

                    std::string lower_key = toLower(key);
                    std::string value_str;
                    leveldb::Status status = db_->Get(leveldb::ReadOptions(), lower_key, &value_str);

                    if (status.IsNotFound()) {
                        spdlog::debug("EnemyRepository: Key '{}' not found.", lower_key);
                        return std::nullopt;
                    } else if (!status.ok()) {
                        spdlog::error("EnemyRepository: Failed to find key '{}': {}", lower_key, status.ToString());
                        return std::nullopt;
                    }
                    else {
                        try {
                            nlohmann::json j = nlohmann::json::parse(value_str);
                            return deserializeEnemy(j);
                        } catch (const nlohmann::json::exception& e) {
                            spdlog::error("EnemyRepository: Failed to parse JSON for key '{}': {}", lower_key, e.what());
                            return std::nullopt;
                        }
                    }
                }

                void EnemyRepository::deleteById(const std::string& key) {
                    if (!db_) {
                        spdlog::error("EnemyRepository: Cannot delete, LevelDB is not open.");
                        return;
                    }

                    std::string lower_key = toLower(key);
                    leveldb::Status status = db_->Delete(leveldb::WriteOptions(), lower_key);

                    if (!status.ok()) {
                        spdlog::error("EnemyRepository: Failed to delete key '{}': {}", lower_key, status.ToString());
                    } else {
                        spdlog::debug("EnemyRepository: Deleted key '{}'.", lower_key);
                    }
                }

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame