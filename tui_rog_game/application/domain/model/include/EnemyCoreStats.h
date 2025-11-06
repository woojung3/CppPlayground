#pragma once

#include <type_traits>

namespace TuiRogGame {
namespace Domain {
namespace Model {

struct EnemyCoreStats {
  int health = 0;
};

static_assert(
    std::is_standard_layout<EnemyCoreStats>::value,
    "EnemyCoreStats must be a standard layout type for serialization.");

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
