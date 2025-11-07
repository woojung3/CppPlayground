#pragma once

#include <type_traits>

namespace TuiRogGame {
namespace Domain {
namespace Model {

struct Position {
  int x = 0;
  int y = 0;

  bool operator==(const Position &other) const {
    return x == other.x && y == other.y;
  }

  bool operator<(const Position &other) const {
    if (x != other.x) {
      return x < other.x;
    }
    return y < other.y;
  }
};

static_assert(std::is_standard_layout<Position>::value,
              "Position must be a standard layout type for serialization.");

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
