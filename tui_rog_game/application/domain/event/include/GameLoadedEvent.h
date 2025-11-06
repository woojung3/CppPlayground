#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class GameLoadedEvent : public DomainEvent {
public:
  GameLoadedEvent() : DomainEvent(Type::GameLoaded) {}
  std::string toString() const override { return "GameLoadedEvent"; }
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame