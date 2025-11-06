#ifndef TUI_ROG_GAME_APPLICATION_DOMAIN_EVENT_INCLUDE_GAMELOADEDEVENT_H
#define TUI_ROG_GAME_APPLICATION_DOMAIN_EVENT_INCLUDE_GAMELOADEDEVENT_H

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class GameLoadedEvent : public DomainEvent {
public:
  GameLoadedEvent() : DomainEvent(Type::GameLoaded) {}
  std::string toString() const override { return "Game loaded successfully!"; }
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_DOMAIN_EVENT_INCLUDE_GAMELOADEDEVENT_H