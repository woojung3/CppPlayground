#pragma once

#include <memory>
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class DomainEvent {
public:

  enum class Type {
    PlayerMoved,
    ItemFound,
    CombatStarted,
    PlayerAttacked,
    EnemyAttacked,
    EnemyDefeated,
    ItemUsed,
    PlayerLeveledUp,
    PlayerDied,
    DescriptionGenerated,
    GameSaved,
    GameLoaded,
    MapChanged,
    Unknown
  };

  virtual ~DomainEvent() = default;
  Type getType() const { return type_; }
  virtual std::string
  toString() const = 0;


protected:
  DomainEvent(Type type) : type_(type) {}

private:
  Type type_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
