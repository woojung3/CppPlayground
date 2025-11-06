#include "CombatStartedEvent.h"

namespace TuiRogGame {
namespace Domain {
namespace Event {

CombatStartedEvent::CombatStartedEvent(const std::string &enemy_type_name,
                                       const std::string &enemy_name,
                                       int enemy_hp, int enemy_attack,
                                       int enemy_defense)
    : DomainEvent(Type::CombatStarted), enemy_type_name_(enemy_type_name),
      enemy_name_(enemy_name), enemy_hp_(enemy_hp), enemy_attack_(enemy_attack),
      enemy_defense_(enemy_defense) {}

std::string CombatStartedEvent::toString() const {
  return "CombatStartedEvent: Combat started with " + enemy_type_name_ + " " +
         enemy_name_ + ". HP: " + std::to_string(enemy_hp_);
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
