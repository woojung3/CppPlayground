#pragma once

#include <type_traits>

namespace TuiRogGame {
namespace Domain {
namespace Model {

struct Stats {
  int strength = 10;
  int dexterity = 10;
  int intelligence = 10;
  int vitality = 10;
  int health;
  int max_health;

  Stats(int str = 10, int dex = 10, int intel = 10, int vit = 10)
      : strength(str), dexterity(dex), intelligence(intel), vitality(vit) {
    max_health = 100 + (vitality * 10);
    health = max_health;
  }
};

static_assert(std::is_standard_layout<Stats>::value,
              "Stats must be a standard layout type for serialization.");

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
