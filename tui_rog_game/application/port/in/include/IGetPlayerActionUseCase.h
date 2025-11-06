#ifndef TUI_ROG_GAME_APPLICATION_PORT_IN_INCLUDE_IGETPLAYERACTIONUSECASE_H
#define TUI_ROG_GAME_APPLICATION_PORT_IN_INCLUDE_IGETPLAYERACTIONUSECASE_H

#include "PlayerActionCommand.h"

namespace TuiRogGame {
namespace Port {
namespace In {

// IGetPlayerActionUseCase is an inbound port interface that defines the use
// case for handling player actions. The GameEngine will implement this
// interface. As per HexBook.md Chapter 3 and 5, inbound ports represent use
// cases and are typically named with a 'UseCase' suffix.
class IGetPlayerActionUseCase {
public:
  virtual ~IGetPlayerActionUseCase() = default;

  // Pure virtual function to handle a player action.
  // The command object encapsulates the action details and is validated on
  // construction.
  virtual void handlePlayerAction(
      const TuiRogGame::Port::In::PlayerActionCommand &command) = 0;

  // Toggles the description port between hardcoded and AI.
  virtual void toggleDescriptionPort() = 0;
};

} // namespace In
} // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_IN_INCLUDE_IGETPLAYERACTIONUSECASE_H
