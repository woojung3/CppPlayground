#include "Orc.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

Orc::Orc(Position position)
    : Enemy("Orc", "Orc", Stats{15, 8, 5, 10}, position) {}

std::string Orc::getTypeName() const { return "Orc"; }

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
