#include "EnemyAttackedEvent.h"
#include <string> // Use string for concatenation
// #include <format> // Removed

namespace TuiRogGame {
namespace Domain {
namespace Event {

EnemyAttackedEvent::EnemyAttackedEvent(const std::string &enemy_name,
                                       int damage_dealt,
                                       int player_current_health)
    : DomainEvent(Type::EnemyAttacked), enemy_name_(enemy_name),
      damage_dealt_(damage_dealt),
      player_current_health_(player_current_health) {}

std::string EnemyAttackedEvent::toString() const {
  return enemy_name_ + " attacked player for " + std::to_string(damage_dealt_) +
         " damage. Player's health: " + std::to_string(player_current_health_) +
         ".";
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
