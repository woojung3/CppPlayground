#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class MapChangedEvent : public DomainEvent {
public:
  MapChangedEvent();

  std::string toString() const override;

private:
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
