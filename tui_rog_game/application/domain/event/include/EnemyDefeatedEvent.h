#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class EnemyDefeatedEvent : public DomainEvent {
public:
  EnemyDefeatedEvent(const std::string &enemy_name, int xp_gained);

  std::string toString() const override;

  const std::string &getEnemyName() const { return enemy_name_; }
  int getXpGained() const { return xp_gained_; }

private:
  std::string enemy_name_;
  int xp_gained_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
