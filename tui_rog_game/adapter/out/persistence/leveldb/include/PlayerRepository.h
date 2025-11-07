#pragma once

#include "ItemRepository.h"
#include "Player.h"
#include "PlayerCoreStats.h"
#include "Position.h"
#include "StandardLayoutCrudRepository.h"
#include "Stats.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

class PlayerRepository {
public:
  explicit PlayerRepository(ItemRepository &item_repo);

  void saveForBatch(const std::string &key,
                    const Domain::Model::Player &player);
  std::optional<Domain::Model::Player> findById(const std::string &key);
  void deleteById(const std::string &key);

private:
  ItemRepository &item_repo_;

  StandardLayoutCrudRepository<Domain::Model::PlayerCoreStats>
      player_core_stats_crud_;
  StandardLayoutCrudRepository<Domain::Model::Stats> player_stats_crud_;
  StandardLayoutCrudRepository<Domain::Model::Position> player_position_crud_;

  std::string toLower(std::string s) const;

  nlohmann::json
  serializePlayerNonStandard(const Domain::Model::Player &player) const;
  std::optional<std::string> deserializePlayerId(const nlohmann::json &j) const;
  std::vector<std::string>
  deserializePlayerInventoryItemIds(const nlohmann::json &j) const;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
