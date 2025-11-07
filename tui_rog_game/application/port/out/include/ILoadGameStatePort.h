#pragma once

#include "GameStateDTO.h"
#include <memory>

namespace TuiRogGame {
namespace Port {
namespace Out {

class ILoadGameStatePort {
public:
  virtual ~ILoadGameStatePort() = default;

  virtual std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO>
  loadGameState() = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame
