#include "InMemoryAdapter.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                void InMemoryAdapter::savePlayer(const TuiRogGame::Domain::Model::Player& player) {
                    stored_player_ = player; // Store a copy of the player object
                    spdlog::info("[InMemoryAdapter] Player saved: {}", player.getName());
                }

                std::unique_ptr<TuiRogGame::Domain::Model::Player> InMemoryAdapter::loadPlayer() {
                    if (stored_player_) {
                        // Create a new Player object from the stored optional and return its unique_ptr
                        spdlog::info("[InMemoryAdapter] Player loaded: {}", stored_player_->getName());
                        return std::make_unique<TuiRogGame::Domain::Model::Player>(*stored_player_);
                    } else {
                        spdlog::info("[InMemoryAdapter] No player found to load.");
                        return nullptr;
                    }
                }

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame
