#ifndef TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IPERSISTENCEPORT_H
#define TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IPERSISTENCEPORT_H

#include <memory>
#include "Player.h"

namespace TuiRogGame {
    namespace Port {
        namespace Out {

            // IPersistencePort is an outbound port interface that defines operations
            // for persisting and loading game entities. The PersistenceAdapter will
            // implement this interface.
            class IPersistencePort {
            public:
                virtual ~IPersistencePort() = default;

                // Pure virtual function to save the player's state
                virtual void savePlayer(const TuiRogGame::Domain::Model::Player& player) = 0;

                // Pure virtual function to load the player's state
                // Returns a unique_ptr to the loaded player, or nullptr if not found
                virtual std::unique_ptr<TuiRogGame::Domain::Model::Player> loadPlayer() = 0;
            };

        } // namespace Out
    } // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IPERSISTENCEPORT_H
