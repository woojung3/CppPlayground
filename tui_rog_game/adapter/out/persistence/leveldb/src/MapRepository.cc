#include "MapRepository.h"
#include "LevelDbProvider.h"
#include <algorithm>
#include <cctype>
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

MapRepository::MapRepository(EnemyRepository &enemy_repo,
                             ItemRepository &item_repo)
    : enemy_repo_(enemy_repo), item_repo_(item_repo) {}

std::string MapRepository::toLower(std::string s) const {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

nlohmann::json
MapRepository::serializeMapNonStandard(const Domain::Model::Map &map) const {
  nlohmann::json j;

  nlohmann::json tiles_json = nlohmann::json::array();
  for (int y = 0; y < map.getHeight(); ++y) {
    nlohmann::json row_json = nlohmann::json::array();
    for (int x = 0; x < map.getWidth(); ++x) {
      row_json.push_back(static_cast<int>(map.getTile(x, y)));
    }
    tiles_json.push_back(row_json);
  }
  j["tiles"] = tiles_json;

  nlohmann::json enemies_json = nlohmann::json::array();
  for (const auto &pair : map.getEnemies()) {
    enemies_json.push_back(
        {{"position", {{"x", pair.first.x}, {"y", pair.first.y}}},
         {"id", pair.second->getName()}}); // Using name as ID for now
  }
  j["enemies"] = enemies_json;

  nlohmann::json items_json = nlohmann::json::array();
  for (const auto &pair : map.getItems()) {
    items_json.push_back(
        {{"position", {{"x", pair.first.x}, {"y", pair.first.y}}},
         {"id", pair.second->getName()}}); // Using name as ID for now
  }
  j["items"] = items_json;

  return j;
}

std::optional<std::vector<std::vector<Domain::Model::Tile>>>
MapRepository::deserializeMapTiles(const nlohmann::json &j) const {
  if (j.contains("tiles") && j["tiles"].is_array()) {
    std::vector<std::vector<Domain::Model::Tile>> tiles;
    for (const auto &row_json : j["tiles"]) {
      std::vector<Domain::Model::Tile> row;
      for (const auto &tile_json : row_json) {
        row.push_back(static_cast<Domain::Model::Tile>(tile_json.get<int>()));
      }
      tiles.push_back(row);
    }
    return tiles;
  }
  return std::nullopt;
}

std::vector<std::pair<Domain::Model::Position, std::string>>
MapRepository::deserializeMapEnemyIds(const nlohmann::json &j) const {
  std::vector<std::pair<Domain::Model::Position, std::string>> enemy_ids;
  if (j.contains("enemies") && j["enemies"].is_array()) {
    for (const auto &enemy_json : j["enemies"]) {
      if (enemy_json.contains("position") && enemy_json.contains("id")) {
        Domain::Model::Position pos = {enemy_json["position"]["x"].get<int>(),
                                       enemy_json["position"]["y"].get<int>()};
        enemy_ids.push_back({pos, enemy_json["id"].get<std::string>()});
      }
    }
  }
  return enemy_ids;
}

std::vector<std::pair<Domain::Model::Position, std::string>>
MapRepository::deserializeMapItemIds(const nlohmann::json &j) const {
  std::vector<std::pair<Domain::Model::Position, std::string>> item_ids;
  if (j.contains("items") && j["items"].is_array()) {
    for (const auto &item_json : j["items"]) {
      if (item_json.contains("position") && item_json.contains("id")) {
        Domain::Model::Position pos = {item_json["position"]["x"].get<int>(),
                                       item_json["position"]["y"].get<int>()};
        item_ids.push_back({pos, item_json["id"].get<std::string>()});
      }
    }
  }
  return item_ids;
}

void MapRepository::saveForBatch(const std::string &key,
                                 const Domain::Model::Map &map) {
  std::string base_key = toLower(key);

  Domain::Model::MapDimensions dimensions = {map.getWidth(), map.getHeight()};
  map_dimensions_crud_.saveForBatch(base_key + ":dimensions", dimensions);
  map_start_position_crud_.saveForBatch(base_key + ":start_position",
                                        map.getStartPlayerPosition());

  nlohmann::json j = serializeMapNonStandard(map);
  LevelDbProvider::getInstance().addToBatch(base_key + ":non_standard",
                                            j.dump());
  spdlog::debug(
      "MapRepository: Added non-standard parts for key '{}' to batch.",
      base_key);

  for (const auto &pair : map.getEnemies()) {
    enemy_repo_.saveForBatch(base_key + ":enemies:" + pair.second->getName(),
                             *pair.second); // Using name as ID
  }
  for (const auto &pair : map.getItems()) {
    item_repo_.saveForBatch(base_key + ":items:" + pair.second->getName(),
                            *pair.second); // Using name as ID
  }
  spdlog::debug("MapRepository: Added map '{}' and its entities to batch.",
                base_key);
}

std::optional<Domain::Model::Map>
MapRepository::findById(const std::string &key) {
  std::string base_key = toLower(key);

  auto dimensions_opt = map_dimensions_crud_.findById(base_key + ":dimensions");
  auto start_pos_opt =
      map_start_position_crud_.findById(base_key + ":start_position");

  if (!dimensions_opt || !start_pos_opt) {
    spdlog::debug("MapRepository: Missing standard layout parts for key '{}'.",
                  base_key);
    return std::nullopt;
  }

  auto &provider = LevelDbProvider::getInstance();
  auto non_standard_json_str_opt = provider.Get(base_key + ":non_standard");
  if (!non_standard_json_str_opt) {
    spdlog::debug("MapRepository: Missing non-standard parts for key '{}'.",
                  base_key);
    return std::nullopt;
  }

  try {
    nlohmann::json j = nlohmann::json::parse(*non_standard_json_str_opt);
    auto tiles_opt = deserializeMapTiles(j);
    std::vector<std::pair<Domain::Model::Position, std::string>>
        enemy_ids_and_pos = deserializeMapEnemyIds(j);
    std::vector<std::pair<Domain::Model::Position, std::string>>
        item_ids_and_pos = deserializeMapItemIds(j);

    if (!tiles_opt) {
      spdlog::error(
          "MapRepository: Failed to deserialize map tiles for key '{}'.",
          base_key);
      return std::nullopt;
    }

    std::map<Domain::Model::Position, std::unique_ptr<Domain::Model::Enemy>>
        loaded_enemies;
    for (const auto &pair : enemy_ids_and_pos) {
      auto enemy_opt = enemy_repo_.findById(
          base_key + ":enemies:" + pair.second); // Use full enemy ID
      if (enemy_opt) {
        loaded_enemies[pair.first] = std::move(enemy_opt);
      } else {
        spdlog::warn("MapRepository: Enemy '{}' not found for map '{}'.",
                     pair.second, base_key);
      }
    }

    std::map<Domain::Model::Position, std::unique_ptr<Domain::Model::Item>>
        loaded_items;
    for (const auto &pair : item_ids_and_pos) {
      auto item_opt = item_repo_.findById(
          base_key + ":items:" + pair.second); // Use full item ID
      if (item_opt) {
        loaded_items[pair.first] =
            std::make_unique<Domain::Model::Item>(item_opt.value());
      } else {
        spdlog::warn("MapRepository: Item '{}' not found for map '{}'.",
                     pair.second, base_key);
      }
    }

    Domain::Model::Map map(dimensions_opt->width, dimensions_opt->height,
                           start_pos_opt.value(), tiles_opt.value(),
                           std::move(loaded_enemies), std::move(loaded_items));

    spdlog::debug("MapRepository: Loaded map '{}' and its entities.", base_key);
    return map;

  } catch (const nlohmann::json::exception &e) {
    spdlog::error("MapRepository: Failed to parse JSON for non-standard parts "
                  "for key '{}': {}",
                  base_key, e.what());
    return std::nullopt;
  }
}

void MapRepository::deleteById(const std::string &key) {
  auto &provider = LevelDbProvider::getInstance();
  std::string base_key = toLower(key);

  map_dimensions_crud_.deleteById(base_key + ":dimensions");
  map_start_position_crud_.deleteById(base_key + ":start_position");

  auto non_standard_json_str_opt = provider.Get(base_key + ":non_standard");
  if (non_standard_json_str_opt) {
    try {
      nlohmann::json j = nlohmann::json::parse(*non_standard_json_str_opt);
      std::vector<std::pair<Domain::Model::Position, std::string>>
          enemy_ids_and_pos = deserializeMapEnemyIds(j);
      std::vector<std::pair<Domain::Model::Position, std::string>>
          item_ids_and_pos = deserializeMapItemIds(j);

      for (const auto &pair : enemy_ids_and_pos) {
        enemy_repo_.deleteById(base_key + ":enemies:" + pair.second);
      }

      for (const auto &pair : item_ids_and_pos) {
        item_repo_.deleteById(base_key + ":items:" + pair.second);
      }
    } catch (const nlohmann::json::exception &e) {
      spdlog::error("MapRepository: Failed to parse JSON for non-standard "
                    "parts during deletion for key '{}': {}",
                    base_key, e.what());
    }
  }

  provider.Delete(base_key + ":non_standard");
  spdlog::debug("MapRepository: Deleted map '{}' and its entities.", base_key);
}

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
