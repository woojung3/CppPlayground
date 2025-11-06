#pragma once

#include "GameStateDTO.h"
#include <memory>

namespace TuiRogGame {
namespace Port {
namespace Out {

// ILoadGameStatePort defines the interface for loading the game state.
class ILoadGameStatePort {
public:
  virtual ~ILoadGameStatePort() = default;

  // Pure virtual function to load the full game state.
  virtual std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO>
  loadGameState() = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame

