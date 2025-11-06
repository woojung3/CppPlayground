#include "PlayerAttackedEvent.h"
#include <string> // Use string for concatenation
// #include <format> // Removed

namespace TuiRogGame {
namespace Domain {
namespace Event {

PlayerAttackedEvent::PlayerAttackedEvent(int damage_dealt,
                                         const std::string &enemy_name,
                                         int enemy_current_health)
    : DomainEvent(Type::PlayerAttacked), damage_dealt_(damage_dealt),
      enemy_name_(enemy_name), enemy_current_health_(enemy_current_health) {}

std::string PlayerAttackedEvent::toString() const {
  return "Player attacked " + enemy_name_ + " for " +
         std::to_string(damage_dealt_) + " damage. " + enemy_name_ +
         "'s health: " + std::to_string(enemy_current_health_) + ".";
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
