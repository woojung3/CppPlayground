#ifndef TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_ILOADGAMESTATEPORT_H
#define TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_ILOADGAMESTATEPORT_H

#include <memory>
#include "GameStateDTO.h"

namespace TuiRogGame {
    namespace Port {
        namespace Out {

            // ILoadGameStatePort defines the interface for loading the game state.
            class ILoadGameStatePort {
            public:
                virtual ~ILoadGameStatePort() = default;

                // Pure virtual function to load the full game state.
                virtual std::unique_ptr<TuiRogGame::Port::Out::GameStateDTO> loadGameState() = 0;
            };

        } // namespace Out
    } // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_ILOADGAMESTATEPORT_H
