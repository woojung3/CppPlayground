#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include "EnemyRepository.h"
#include "ItemRepository.h"
#include "Map.h"
#include "MapDimensions.h"
#include "Position.h"
#include "StandardLayoutCrudRepository.h"

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
  EnemyRepository &enemy_repo_;
  ItemRepository &item_repo_;

  StandardLayoutCrudRepository<Domain::Model::MapDimensions>
      map_dimensions_crud_;
  StandardLayoutCrudRepository<Domain::Model::Position>
      map_start_position_crud_;

  std::string toLower(std::string s) const;

  nlohmann::json serializeMapNonStandard(const Domain::Model::Map &map) const;
  std::optional<std::vector<std::vector<Domain::Model::Tile>>>
  deserializeMapTiles(const nlohmann::json &j) const;
  std::vector<std::pair<Domain::Model::Position, std::string>>
  deserializeMapEnemyIds(const nlohmann::json &j) const;
  std::vector<std::pair<Domain::Model::Position, std::string>>
  deserializeMapItemIds(const nlohmann::json &j) const;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
