#pragma once

#include "DomainEvent.h"
#include "GameStateDTO.h"
#include <memory> // For std::unique_ptr
#include <vector>

namespace TuiRogGame {
namespace Port {
namespace Out {

// IRenderPort is an outbound port interface that defines operations
// for rendering the game state to a user interface.
// Adapters like TuiAdapter will implement this interface.
class IRenderPort {
public:
  virtual ~IRenderPort() = default;

  // Renders the current game state. It receives the full game state DTO
  // and a list of events that led to this state change.
  virtual void
  render(const GameStateDTO &game_state,
         const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>
             &events) = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame

