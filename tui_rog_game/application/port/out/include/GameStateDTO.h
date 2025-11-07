#pragma once

#include "Map.h"
#include "Player.h"

namespace TuiRogGame {
namespace Port {
namespace Out {

struct GameStateDTO {
  Domain::Model::Map map;
  Domain::Model::Player player;

  GameStateDTO(Domain::Model::Map map_val, Domain::Model::Player player_val)
      : map(std::move(map_val)), player(std::move(player_val)) {}
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame
