#ifndef TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IPERSISTENCEPORT_H
#define TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IPERSISTENCEPORT_H

#include <memory>
#include "GameStateDTO.h"

namespace TuiRogGame {
    namespace Port {
        namespace Out {

            // IPersistencePort is an outbound port interface that defines operations
            // for persisting and loading game entities. The PersistenceAdapter will
            // implement this interface.
            class IPersistencePort {
            public:
                virtual ~IPersistencePort() = default;

                // Pure virtual function to save the full game state.
                virtual void saveGame(const TuiRogGame::Port::Out::GameStateDTO& gameState) = 0;

                // Pure virtual function to load the full game state.
                virtual std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO> loadGame() = 0;
            };

        } // namespace Out
    } // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IPERSISTENCEPORT_H
