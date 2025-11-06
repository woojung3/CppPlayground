#include "Goblin.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

// Goblin의 기본 스탯과 이름을 생성자에서 설정
Goblin::Goblin(Position position)
    : Enemy("Goblin", "Goblin", Stats{8, 15, 10, 5}, position) {}

std::string Goblin::getTypeName() const { return "Goblin"; }

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
