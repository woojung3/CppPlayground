#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_PLAYERREPOSITORY_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_PLAYERREPOSITORY_H

#include <memory>            // For std::shared_ptr
#include <nlohmann/json.hpp> // For JSON serialization
#include <optional>
#include <string>

#include "ItemRepository.h"               // For Player's inventory items
#include "Player.h"                       // Domain Model Player
#include "PlayerCoreStats.h"              // For Player's core stats
#include "Position.h"                     // For Player's Position
#include "StandardLayoutCrudRepository.h" // For standard layout parts
#include "Stats.h"                        // For Player's Stats

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
  ItemRepository &item_repo_; // Dependency on ItemRepository

  StandardLayoutCrudRepository<Domain::Model::PlayerCoreStats>
      player_core_stats_crud_;
  StandardLayoutCrudRepository<Domain::Model::Stats> player_stats_crud_;
  StandardLayoutCrudRepository<Domain::Model::Position> player_position_crud_;

  // Helper to convert string to lowercase
  std::string toLower(std::string s) const;

  // Serialization/Deserialization helpers for Player's non-standard layout
  // parts
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

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_PLAYERREPOSITORY_H