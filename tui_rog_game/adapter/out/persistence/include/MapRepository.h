#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_MAPREPOSITORY_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_MAPREPOSITORY_H

#include <string>
#include <optional>
#include <memory> // For std::shared_ptr
#include <leveldb/db.h> // LevelDB header
#include <nlohmann/json.hpp> // For JSON serialization

#include "Map.h" // Domain Model Map
#include "MapDimensions.h" // For Map's dimensions
#include "Position.h" // For Map's start player position
#include "CrudRepository.h" // For standard layout parts
#include "EnemyRepository.h" // For Map's enemies
#include "ItemRepository.h" // For Map's items

#include <leveldb/write_batch.h> // For leveldb::WriteBatch

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                class MapRepository {
                public:
                    explicit MapRepository(std::shared_ptr<leveldb::DB> db, EnemyRepository& enemy_repo, ItemRepository& item_repo);

                    void save(const std::string& key, const Domain::Model::Map& map, leveldb::WriteBatch& batch);
                    std::optional<Domain::Model::Map> findById(const std::string& key);
                    void deleteById(const std::string& key);

                private:
                    std::shared_ptr<leveldb::DB> db_;
                    EnemyRepository& enemy_repo_; // Dependency on EnemyRepository
                    ItemRepository& item_repo_; // Dependency on ItemRepository

                    CrudRepository<Domain::Model::MapDimensions> map_dimensions_crud_;
                    CrudRepository<Domain::Model::Position> map_start_position_crud_;

                    // Helper to convert string to lowercase
                    std::string toLower(std::string s) const;

                    // Serialization/Deserialization helpers for Map's non-standard layout parts
                    nlohmann::json serializeMapNonStandard(const Domain::Model::Map& map) const;
                    std::optional<std::vector<std::vector<Domain::Model::Tile>>> deserializeMapTiles(const nlohmann::json& j) const;
                    std::vector<std::pair<Domain::Model::Position, std::string>> deserializeMapEnemyIds(const nlohmann::json& j) const; // Position, Enemy ID
                    std::vector<std::pair<Domain::Model::Position, std::string>> deserializeMapItemIds(const nlohmann::json& j) const; // Position, Item ID
                };

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_MAPREPOSITORY_H