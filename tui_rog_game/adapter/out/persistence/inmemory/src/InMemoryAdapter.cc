#include "InMemoryAdapter.h"
#include "GameStateDTO.h"
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

void InMemoryAdapter::saveGameState(
    const TuiRogGame::Port::Out::GameStateDTO &gameState) {
  stored_game_state_.emplace(gameState); // GameStateDTO를 값 복사하여 저장
  spdlog::info("[InMemoryAdapter] Game saved.");
}

std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO>
InMemoryAdapter::loadGameState() {
  if (stored_game_state_) {
    // Create a new GameStateDTO object from the stored optional and return its
    // unique_ptr
    spdlog::info("[InMemoryAdapter] Game loaded.");
    return std::make_unique<TuiRogGame::Port::Out::GameStateDTO>(
        *stored_game_state_);
  } else {
    spdlog::info("[InMemoryAdapter] No game found to load.");
    return nullptr;
  }
}

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
