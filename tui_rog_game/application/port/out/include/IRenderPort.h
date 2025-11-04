#ifndef TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IRENDERPORT_H
#define TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IRENDERPORT_H

#include <vector>
#include <memory> // For std::unique_ptr
#include "DomainEvent.h" // Include the DomainEvent definition

namespace TuiRogGame {
    namespace Port {
        namespace Out {

            // IRenderPort is an outbound port interface that defines operations
            // for rendering the game state to a user interface.
            // Adapters like TuiAdapter will implement this interface.
            class IRenderPort {
            public:
                virtual ~IRenderPort() = default;

                // Pure virtual function to render the current game state based on a list of domain events.
                virtual void render(const std::vector<std::unique_ptr<Common::DomainEvent>>& events) = 0;
            };

        } // namespace Out
    } // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_OUT_INCLUDE_IRENDERPORT_H
