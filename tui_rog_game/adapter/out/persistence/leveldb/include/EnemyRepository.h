#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include "Enemy.h"
#include "Goblin.h"
#include "Orc.h"
#include "Position.h"
#include "Stats.h"

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

  std::string toLower(std::string s) const;

  nlohmann::json serializeEnemy(const Domain::Model::Enemy &enemy) const;
  std::unique_ptr<Domain::Model::Enemy>
  deserializeEnemy(const nlohmann::json &j) const;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
