#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class EnemyAttackedEvent : public DomainEvent {
public:
  EnemyAttackedEvent(const std::string &enemy_name, int damage_dealt,
                     int player_current_health);

  std::string toString() const override;

  const std::string &getEnemyName() const { return enemy_name_; }
  int getDamageDealt() const { return damage_dealt_; }
  int getPlayerCurrentHealth() const { return player_current_health_; }

private:
  std::string enemy_name_;
  int damage_dealt_;
  int player_current_health_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
