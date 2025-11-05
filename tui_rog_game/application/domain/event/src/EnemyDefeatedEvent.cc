#include "EnemyDefeatedEvent.h"
#include <string> // Use string for concatenation
// #include <format> // Removed

namespace TuiRogGame {
namespace Domain {
namespace Event {

EnemyDefeatedEvent::EnemyDefeatedEvent(const std::string& enemy_name, int xp_gained)
    : DomainEvent(Type::EnemyDefeated),
      enemy_name_(enemy_name),
      xp_gained_(xp_gained) {}

std::string EnemyDefeatedEvent::toString() const {
    return enemy_name_ + " defeated! Player gained " + std::to_string(xp_gained_) + " XP.";
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
