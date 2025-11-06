#pragma once

#include <type_traits>

namespace TuiRogGame {
namespace Domain {
namespace Model {

struct PlayerCoreStats {
  int level = 1;
  int xp = 0;
  int hp = 0;
};

static_assert(
    std::is_standard_layout<PlayerCoreStats>::value,
    "PlayerCoreStats must be a standard layout type for serialization.");

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
