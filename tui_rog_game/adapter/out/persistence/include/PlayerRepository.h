#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_PLAYERREPOSITORY_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_PLAYERREPOSITORY_H

#include <string>
#include <optional>
#include <memory> // For std::shared_ptr
#include <leveldb/db.h> // LevelDB header
#include <nlohmann/json.hpp> // For JSON serialization

#include "Player.h" // Domain Model Player
#include "PlayerCoreStats.h" // For Player's core stats
#include "Stats.h" // For Player's Stats
#include "Position.h" // For Player's Position
#include "CrudRepository.h" // For standard layout parts
#include "ItemRepository.h" // For Player's inventory items

#include <leveldb/write_batch.h> // For leveldb::WriteBatch

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                class PlayerRepository {
                public:
                    explicit PlayerRepository(std::shared_ptr<leveldb::DB> db, ItemRepository& item_repo);

                    void save(const std::string& key, const Domain::Model::Player& player, leveldb::WriteBatch& batch);
                    std::optional<Domain::Model::Player> findById(const std::string& key);
                    void deleteById(const std::string& key);

                private:
                    std::shared_ptr<leveldb::DB> db_;
                    ItemRepository& item_repo_; // Dependency on ItemRepository

                    CrudRepository<Domain::Model::PlayerCoreStats> player_core_stats_crud_;
                    CrudRepository<Domain::Model::Stats> player_stats_crud_;
                    CrudRepository<Domain::Model::Position> player_position_crud_;

                    // Helper to convert string to lowercase
                    std::string toLower(std::string s) const;

                    // Serialization/Deserialization helpers for Player's non-standard layout parts
                    nlohmann::json serializePlayerNonStandard(const Domain::Model::Player& player) const;
                    // This will return a partially constructed Player, or just the non-standard parts
                    // The full Player object will be assembled in findById
                    std::optional<std::string> deserializePlayerId(const nlohmann::json& j) const;
                    std::vector<std::string> deserializePlayerInventoryItemIds(const nlohmann::json& j) const;
                };

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_PLAYERREPOSITORY_H