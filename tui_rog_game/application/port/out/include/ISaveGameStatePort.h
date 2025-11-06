#pragma once

#include "GameStateDTO.h"
#include <memory>

namespace TuiRogGame {
namespace Port {
namespace Out {

// ISaveGameStatePort defines the interface for saving the game state.
class ISaveGameStatePort {
public:
  virtual ~ISaveGameStatePort() = default;

  // Pure virtual function to save the full game state.
  virtual void
  saveGameState(const TuiRogGame::Port::Out::GameStateDTO &gameState) = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame

