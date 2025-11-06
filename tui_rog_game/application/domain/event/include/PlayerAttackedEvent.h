#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class PlayerAttackedEvent : public DomainEvent {
public:
  PlayerAttackedEvent(int damage_dealt, const std::string &enemy_name,
                      int enemy_current_health);

  std::string toString() const override;

  int getDamageDealt() const { return damage_dealt_; }
  const std::string &getEnemyName() const { return enemy_name_; }
  int getEnemyCurrentHealth() const { return enemy_current_health_; }

private:
  int damage_dealt_;
  std::string enemy_name_;
  int enemy_current_health_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
