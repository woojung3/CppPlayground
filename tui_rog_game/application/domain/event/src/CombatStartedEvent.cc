#include "CombatStartedEvent.h"
#include "Enemy.h" // For static_assert against original enum
#include <type_traits>   // For std::underlying_type_t

namespace TuiRogGame {
namespace Domain {
namespace Event {

// Static asserts to ensure consistency with Domain::Model::Enemy::EnemyType
static_assert(std::is_same_v<
                  std::underlying_type_t<CombatStartedEvent::EnemyType>,
                  std::underlying_type_t<TuiRogGame::Domain::Model::Enemy::EnemyType>>,
              "Underlying types of CombatStartedEvent::EnemyType and Domain::Model::Enemy::EnemyType mismatch");

static_assert(static_cast<int>(CombatStartedEvent::EnemyType::Orc) == static_cast<int>(TuiRogGame::Domain::Model::Enemy::EnemyType::Orc),
              "CombatStartedEvent::EnemyType::Orc value mismatch");
static_assert(static_cast<int>(CombatStartedEvent::EnemyType::Goblin) == static_cast<int>(TuiRogGame::Domain::Model::Enemy::EnemyType::Goblin),
              "CombatStartedEvent::EnemyType::Goblin value mismatch");
// Add more static_asserts here if more EnemyType enumerators are added in the future.

CombatStartedEvent::CombatStartedEvent(
    EnemyType enemy_type,
    const std::string& enemy_name,
    int enemy_hp,
    int enemy_attack,
    int enemy_defense)
    : DomainEvent(Type::CombatStarted),
      enemy_type_(enemy_type),
      enemy_name_(enemy_name),
      enemy_hp_(enemy_hp),
      enemy_attack_(enemy_attack),
      enemy_defense_(enemy_defense) {}



std::string CombatStartedEvent::toString() const {
    return "CombatStartedEvent: Combat started with " + enemy_name_ + ". HP: " + std::to_string(enemy_hp_);
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
