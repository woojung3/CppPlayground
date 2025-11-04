#ifndef TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IGENERATEDESCRIPTIONPORT_H
#define TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IGENERATEDESCRIPTIONPORT_H

#include <string>
#include "Position.h"
// Potentially other domain models for context

namespace TuiRogGame {
    namespace Port {
        namespace Out {

            // IGenerateDescriptionPort is an outbound port interface that defines operations
            // for generating textual descriptions based on game state.
            // Adapters like ChatGptAdapter or HardcodedDescAdapter will implement this.
            class IGenerateDescriptionPort {
            public:
                virtual ~IGenerateDescriptionPort() = default;

                // Pure virtual function to generate a description for a given context.
                // The input could be more complex (e.g., a GameState object).
                virtual std::string generateDescription(const TuiRogGame::Domain::Model::Position& player_position) = 0;
            };

        } // namespace Out
    } // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IGENERATEDESCRIPTIONPORT_H
