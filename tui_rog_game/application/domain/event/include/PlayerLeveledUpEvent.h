#pragma once

#include "DomainEvent.h"
#include "Stats.h"
#include <format>
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class PlayerLeveledUpEvent : public DomainEvent {
public:
  PlayerLeveledUpEvent(int new_level, const Model::Stats &new_stats);

  std::string toString() const override;

  int getNewLevel() const { return new_level_; }
  const Model::Stats &getNewStats() const { return new_stats_; }

private:
  int new_level_;
  Model::Stats new_stats_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
