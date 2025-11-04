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

                    // Saves a copy of the player's state to an in-memory store.
                    void savePlayer(const TuiRogGame::Domain::Model::Player& player) override;

                    // Loads the player's state from the in-memory store.
                    // Returns a unique_ptr to a new Player object, or nullptr if no player was saved.
                    std::unique_ptr<TuiRogGame::Domain::Model::Player> loadPlayer() override;

                private:
                    // Stores the last saved player state. Using std::optional to represent
                    // whether a player has been saved or not. When a player is saved,
                    // a copy is made and stored here.
                    std::optional<TuiRogGame::Domain::Model::Player> stored_player_;
                };

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_INMEMORYADAPTER_H
