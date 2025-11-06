#pragma once

#include <memory>            // For std::unique_ptr
#include <nlohmann/json.hpp> // For JSON serialization
#include <string>

#include "Enemy.h"    // Domain Model Enemy
#include "Goblin.h"   // For Goblin derived class
#include "Orc.h"      // For Orc derived class
#include "Position.h" // For Enemy's Position
#include "Stats.h"    // For Enemy's Stats

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

class EnemyRepository {
public:
  explicit EnemyRepository();

  void saveForBatch(const std::string &key, const Domain::Model::Enemy &enemy);
  std::unique_ptr<Domain::Model::Enemy> findById(const std::string &key);
  void deleteById(const std::string &key);

private:
  // Helper to convert string to lowercase
  std::string toLower(std::string s) const;

  // Serialization/Deserialization helpers
  nlohmann::json serializeEnemy(const Domain::Model::Enemy &enemy) const;
  std::unique_ptr<Domain::Model::Enemy>
  deserializeEnemy(const nlohmann::json &j) const;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame