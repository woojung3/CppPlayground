#pragma once

#include <type_traits> // For std::is_standard_layout

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
    max_health = 100 + (vitality * 10); // Example calculation
    health = max_health;
  }
};

// Verify at compile time that this struct has a standard layout, which is
// crucial for serialization.
static_assert(std::is_standard_layout<Stats>::value,
              "Stats must be a standard layout type for serialization.");

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame