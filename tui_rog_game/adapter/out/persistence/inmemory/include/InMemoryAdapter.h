#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_INMEMORYADAPTER_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_INMEMORYADAPTER_H

#include "IPersistencePort.h"
#include <optional>
#include <memory>

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                // InMemoryAdapter is a concrete implementation of IPersistencePort
                // that stores game data in memory. This is useful for testing
                // and for initial development without a real database.
                class InMemoryAdapter : public Port::Out::IPersistencePort {
                public:
                    InMemoryAdapter() = default;
                    ~InMemoryAdapter() override = default;

                    // Saves the full game state to an in-memory store.
                    void saveGame(const TuiRogGame::Port::Out::GameStateDTO& gameState) override;

                    // Loads the full game state from the in-memory store.
                    // Returns a unique_ptr to a new GameStateDTO object, or nullptr if no game was saved.
                    std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO> loadGame() override;

                private:
                    // Stores the last saved game state. Using std::optional to represent
                    // whether a game has been saved or not.
                    std::optional<TuiRogGame::Port::Out::GameStateDTO> stored_game_state_;
                };

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_INMEMORYADAPTER_H
