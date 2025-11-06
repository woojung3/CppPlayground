#pragma once

#include <memory>            // For std::shared_ptr
#include <nlohmann/json.hpp> // For JSON serialization
#include <optional>
#include <string>

#include "EnemyRepository.h"              // For Map's enemies
#include "ItemRepository.h"               // For Map's items
#include "Map.h"                          // Domain Model Map
#include "MapDimensions.h"                // For Map's dimensions
#include "Position.h"                     // For Map's start player position
#include "StandardLayoutCrudRepository.h" // For standard layout parts

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

class MapRepository {
public:
  explicit MapRepository(EnemyRepository &enemy_repo,
                         ItemRepository &item_repo);

  void saveForBatch(const std::string &key, const Domain::Model::Map &map);
  std::optional<Domain::Model::Map> findById(const std::string &key);
  void deleteById(const std::string &key);

private:
  EnemyRepository &enemy_repo_; // Dependency on EnemyRepository
  ItemRepository &item_repo_;   // Dependency on ItemRepository

  StandardLayoutCrudRepository<Domain::Model::MapDimensions>
      map_dimensions_crud_;
  StandardLayoutCrudRepository<Domain::Model::Position>
      map_start_position_crud_;

  // Helper to convert string to lowercase
  std::string toLower(std::string s) const;

  // Serialization/Deserialization helpers for Map's non-standard layout parts
  nlohmann::json serializeMapNonStandard(const Domain::Model::Map &map) const;
  std::optional<std::vector<std::vector<Domain::Model::Tile>>>
  deserializeMapTiles(const nlohmann::json &j) const;
  std::vector<std::pair<Domain::Model::Position, std::string>>
  deserializeMapEnemyIds(const nlohmann::json &j) const; // Position, Enemy ID
  std::vector<std::pair<Domain::Model::Position, std::string>>
  deserializeMapItemIds(const nlohmann::json &j) const; // Position, Item ID
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame