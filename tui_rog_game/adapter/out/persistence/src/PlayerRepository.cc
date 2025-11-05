#include "PlayerRepository.h"
#include <spdlog/spdlog.h>
#include <algorithm> // For std::transform
#include <cctype> // For std::tolower

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                PlayerRepository::PlayerRepository(std::shared_ptr<leveldb::DB> db, ItemRepository& item_repo)
                    : db_(db),
                      item_repo_(item_repo),
                      player_core_stats_crud_(db),
                      player_stats_crud_(db),
                      player_position_crud_(db) {
                    if (!db_) {
                        spdlog::error("PlayerRepository: Initialized with null LevelDB pointer.");
                    }
                }

                std::string PlayerRepository::toLower(std::string s) const {
                    std::transform(s.begin(), s.end(), s.begin(),
                                   [](unsigned char c){ return std::tolower(c); });
                    return s;
                }

                nlohmann::json PlayerRepository::serializePlayerNonStandard(const Domain::Model::Player& player) const {
                    nlohmann::json j;
                    j["id"] = player.getId();
                    
                    // Serialize inventory: store only item IDs
                    nlohmann::json inventory_ids_json = nlohmann::json::array();
                    for (const auto& item_ptr : player.getInventory()) {
                        // Assuming Item has a way to get a unique ID for persistence
                        // For now, let's use item name as a placeholder ID. This needs refinement.
                        inventory_ids_json.push_back(item_ptr->getName()); // Placeholder for item ID
                    }
                    j["inventory_item_ids"] = inventory_ids_json;
                    return j;
                }

                std::optional<std::string> PlayerRepository::deserializePlayerId(const nlohmann::json& j) const {
                    if (j.contains("id")) {
                        return j["id"].get<std::string>();
                    }
                    return std::nullopt;
                }

                std::vector<std::string> PlayerRepository::deserializePlayerInventoryItemIds(const nlohmann::json& j) const {
                    std::vector<std::string> item_ids;
                    if (j.contains("inventory_item_ids") && j["inventory_item_ids"].is_array()) {
                        for (const auto& id_json : j["inventory_item_ids"]) {
                            item_ids.push_back(id_json.get<std::string>());
                        }
                    }
                    return item_ids;
                }

                void PlayerRepository::save(const std::string& key, const Domain::Model::Player& player, leveldb::WriteBatch& batch) {
                    // Key convention: player:id
                    std::string base_key = toLower(key); // e.g., "player:main_player"

                    // Save standard layout parts using CrudRepository
                    Domain::Model::PlayerCoreStats core_stats = {player.getLevel(), player.getXp(), player.getHp()};
                    player_core_stats_crud_.save(base_key + ":core_stats", core_stats, batch);
                    player_stats_crud_.save(base_key + ":stats", player.getStats(), batch);
                    player_position_crud_.save(base_key + ":position", player.getPosition(), batch);

                    // Save non-standard layout parts (id and inventory item IDs) as JSON
                    nlohmann::json j = serializePlayerNonStandard(player);
                    batch.Put(base_key + ":non_standard", j.dump());
                    spdlog::debug("PlayerRepository: Added Put for non-standard parts for key '{}' to WriteBatch.", base_key);

                    // Save each item in the inventory using ItemRepository
                    int item_index = 0;
                    for (const auto& item_ptr : player.getInventory()) {
                        // Key convention: player:id:inventory:item_index
                        item_repo_.save(base_key + ":inventory:" + std::to_string(item_index), *item_ptr, batch);
                        item_index++;
                    }
                    spdlog::debug("PlayerRepository: Added player '{}' and its inventory to WriteBatch.", base_key);
                }

                std::optional<Domain::Model::Player> PlayerRepository::findById(const std::string& key) {
                    std::string base_key = toLower(key);

                    // Load standard layout parts
                    auto core_stats_opt = player_core_stats_crud_.findById(base_key + ":core_stats");
                    auto stats_opt = player_stats_crud_.findById(base_key + ":stats");
                    auto position_opt = player_position_crud_.findById(base_key + ":position");

                    if (!core_stats_opt || !stats_opt || !position_opt) {
                        spdlog::debug("PlayerRepository: Missing standard layout parts for key '{}'.", base_key);
                        return std::nullopt;
                    }

                    // Load non-standard layout parts (id and inventory item IDs) from JSON
                    std::string non_standard_json_str;
                    leveldb::Status status = db_->Get(leveldb::ReadOptions(), base_key + ":non_standard", &non_standard_json_str);
                    if (!status.ok()) {
                        spdlog::debug("PlayerRepository: Missing non-standard parts for key '{}': {}", base_key, status.ToString());
                        return std::nullopt;
                    }

                    try {
                        nlohmann::json j = nlohmann::json::parse(non_standard_json_str);
                        auto id_opt = deserializePlayerId(j);
                        std::vector<std::string> inventory_item_ids = deserializePlayerInventoryItemIds(j);

                        if (!id_opt) {
                            spdlog::error("PlayerRepository: Failed to deserialize player ID for key '{}'.", base_key);
                            return std::nullopt;
                        }

                        // Load inventory items first
                        std::vector<std::unique_ptr<Domain::Model::Item>> loaded_inventory_items;
                        for (const std::string& item_id : inventory_item_ids) {
                            auto item_opt = item_repo_.findById(base_key + ":inventory:" + item_id); // Use full item ID
                            if (item_opt) {
                                loaded_inventory_items.push_back(std::make_unique<Domain::Model::Item>(item_opt.value()));
                            } else {
                                spdlog::warn("PlayerRepository: Item '{}' not found for player '{}'.", item_id, base_key);
                            }
                        }

                        // Reconstruct Player object using the new constructor
                        Domain::Model::Player player(id_opt.value(), core_stats_opt.value(), stats_opt.value(), position_opt.value(), std::move(loaded_inventory_items));
                        
                        spdlog::debug("PlayerRepository: Loaded player '{}' and its inventory.", base_key);
                        return player;

                    } catch (const nlohmann::json::exception& e) {
                        spdlog::error("PlayerRepository: Failed to parse JSON for non-standard parts for key '{}': {}", base_key, e.what());
                        return std::nullopt;
                    }
                }

                void PlayerRepository::deleteById(const std::string& key) {
                    std::string base_key = toLower(key);

                    // Delete standard layout parts
                    player_core_stats_crud_.deleteById(base_key + ":core_stats");
                    player_stats_crud_.deleteById(base_key + ":stats");
                    player_position_crud_.deleteById(base_key + ":position");

                    // Delete non-standard layout parts
                    leveldb::Status status = db_->Delete(leveldb::WriteOptions(), base_key + ":non_standard");
                    if (!status.ok()) {
                        spdlog::error("PlayerRepository: Failed to delete non-standard parts for key '{}': {}", base_key, status.ToString());
                    }

                    // Delete inventory items (this requires iterating through keys or knowing item IDs)
                    // For now, this is a placeholder. A more robust solution would involve
                    // storing a list of item keys with the player's non-standard data.
                    // For simplicity, we'll assume we only delete the player's main data.
                    spdlog::warn("PlayerRepository: Inventory items for player '{}' are not deleted during player deletion. Manual cleanup needed.", base_key);
                }

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame