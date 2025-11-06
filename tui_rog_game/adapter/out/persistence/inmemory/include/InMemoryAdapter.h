#pragma once

#include "ILoadGameStatePort.h"
#include "ISaveGameStatePort.h"
#include <memory>
#include <optional>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

class InMemoryAdapter : public Port::Out::ISaveGameStatePort,
                        public Port::Out::ILoadGameStatePort {
public:
  InMemoryAdapter() = default;
  ~InMemoryAdapter() override = default;

  void
  saveGameState(const TuiRogGame::Port::Out::GameStateDTO &gameState) override;
  std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO> loadGameState() override;

private:
  std::optional<TuiRogGame::Port::Out::GameStateDTO> stored_game_state_;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame

