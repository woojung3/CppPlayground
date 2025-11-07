#pragma once

#include "GameStateDTO.h"
#include <memory>

namespace TuiRogGame {
namespace Port {
namespace Out {

class ISaveGameStatePort {
public:
  virtual ~ISaveGameStatePort() = default;

  virtual void
  saveGameState(const TuiRogGame::Port::Out::GameStateDTO &gameState) = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame
