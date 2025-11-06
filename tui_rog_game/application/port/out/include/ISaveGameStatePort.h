#ifndef TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_ISAVEGAMESTATEPORT_H
#define TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_ISAVEGAMESTATEPORT_H

#include <memory>
#include "GameStateDTO.h"

namespace TuiRogGame {
    namespace Port {
        namespace Out {

            // ISaveGameStatePort defines the interface for saving the game state.
            class ISaveGameStatePort {
            public:
                virtual ~ISaveGameStatePort() = default;

                // Pure virtual function to save the full game state.
                virtual void saveGameState(const TuiRogGame::Port::Out::GameStateDTO& gameState) = 0;
            };

        } // namespace Out
    } // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_ISAVEGAMESTATEPORT_H
