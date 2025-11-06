#include "EnemyRepository.h"
#include "LevelDbProvider.h"
#include <algorithm> // For std::transform
#include <cctype>    // For std::tolower
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

EnemyRepository::EnemyRepository() = default;

std::string EnemyRepository::toLower(std::string s) const {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

nlohmann::json
EnemyRepository::serializeEnemy(const Domain::Model::Enemy &enemy) const {
  nlohmann::json j;
  j["type_name"] = enemy.getTypeName(); // Store type name as string
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

std::unique_ptr<Domain::Model::Enemy>
EnemyRepository::deserializeEnemy(const nlohmann::json &j) const {
  try {
    if (j.contains("type_name") && j.contains("name") && j.contains("health") &&
        j.contains("stats") && j.contains("position")) {
      std::string type_name = j["type_name"].get<std::string>();
      std::string name = j["name"].get<std::string>();
      int health = j["health"].get<int>();

      Domain::Model::Stats stats;
      stats.strength = j["stats"]["strength"].get<int>();
      stats.dexterity = j["stats"]["dexterity"].get<int>();
      stats.intelligence = j["stats"]["intelligence"].get<int>();
      stats.vitality = j["stats"]["vitality"].get<int>();

      Domain::Model::Position position;
      position.x = j["position"]["x"].get<int>();
      position.y = j["position"]["y"].get<int>();

      std::unique_ptr<Domain::Model::Enemy> enemy_ptr;

      if (type_name == "Orc") {
        enemy_ptr = std::make_unique<Domain::Model::Orc>(position);
      } else if (type_name == "Goblin") {
        enemy_ptr = std::make_unique<Domain::Model::Goblin>(position);
      } else {
        spdlog::error("EnemyRepository: Unknown enemy type_name: {}",
                      type_name);
        return nullptr;
      }

      // After construction, update common properties like health and stats
      // Note: Derived class constructors set initial stats and health. We need
      // to override if persisted values differ. For simplicity, we'll assume
      // the name and stats from JSON are the source of truth. This might
      // require adding setters to the Enemy base class or derived classes. For
      // now, we'll directly set the health_ and stats_ if possible, or rely on
      // the constructor. A more robust solution would involve passing stats and
      // name to the derived constructors. For this refactoring, we'll assume
      // the derived constructors set the base stats and name, and we only need
      // to update the current health.
      if (enemy_ptr) {
        enemy_ptr->stats_.health = health; // Directly set health in stats_
      }

      return enemy_ptr;
    }
    spdlog::error(
        "EnemyRepository: JSON missing required fields for deserialization.");
    return nullptr;
  } catch (const nlohmann::json::exception &e) {
    spdlog::error("EnemyRepository: Failed to deserialize Enemy JSON: {}",
                  e.what());
    return nullptr;
  }
}

void EnemyRepository::saveForBatch(const std::string &key,
                                   const Domain::Model::Enemy &enemy) {
  std::string lower_key = toLower(key);
  nlohmann::json j = serializeEnemy(enemy);
  LevelDbProvider::getInstance().addToBatch(lower_key, j.dump());
  spdlog::debug("EnemyRepository: Added Put for key '{}' to batch.", lower_key);
}

std::unique_ptr<Domain::Model::Enemy>
EnemyRepository::findById(const std::string &key) {
  auto &provider = LevelDbProvider::getInstance();

  std::string lower_key = toLower(key);
  auto value_str_opt = provider.Get(lower_key);

  if (!value_str_opt) {
    return nullptr; // Not found or error already logged by provider
  }

  try {
    nlohmann::json j = nlohmann::json::parse(*value_str_opt);
    return deserializeEnemy(j);
  } catch (const nlohmann::json::exception &e) {
    spdlog::error("EnemyRepository: Failed to parse JSON for key '{}': {}",
                  lower_key, e.what());
    return nullptr;
  }
}

void EnemyRepository::deleteById(const std::string &key) {
  auto &provider = LevelDbProvider::getInstance();
  std::string lower_key = toLower(key);
  if (provider.Delete(lower_key)) {
    spdlog::debug("EnemyRepository: Deleted key '{}'.", lower_key);
  }
}

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame