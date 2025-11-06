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

  // Saves the full game state to an in-memory store.
  void
  saveGameState(const TuiRogGame::Port::Out::GameStateDTO &gameState) override;

  // Loads the full game state from the in-memory store.
  // Returns a unique_ptr to a new GameStateDTO object, or nullptr if no game
  // was saved.
  std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO> loadGameState() override;

private:
  // Stores the last saved game state. Using std::optional to represent
  // whether a game has been saved or not.
  std::optional<TuiRogGame::Port::Out::GameStateDTO> stored_game_state_;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame

