#include "Goblin.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

Goblin::Goblin(Position position)
    : Enemy("Goblin", "Goblin", Stats{8, 15, 10, 5}, position) {}

std::string Goblin::getTypeName() const { return "Goblin"; }

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
