#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class PlayerDiedEvent : public DomainEvent {
public:
  PlayerDiedEvent() : DomainEvent(Type::PlayerDied) {}
  std::string toString() const override { return "You died! Game Over."; }
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame