#include "MapChangedEvent.h"
#include <string>


namespace TuiRogGame {
namespace Domain {
namespace Event {

MapChangedEvent::MapChangedEvent() : DomainEvent(Type::MapChanged) {}

std::string MapChangedEvent::toString() const {
  return "You entered a new area. The map has changed!";
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
