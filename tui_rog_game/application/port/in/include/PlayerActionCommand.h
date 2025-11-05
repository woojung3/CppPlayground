#ifndef TUI_ROG_GAME_APPLICATION_PORT_IN_INCLUDE_PLAYERACTIONCOMMAND_H
#define TUI_ROG_GAME_APPLICATION_PORT_IN_INCLUDE_PLAYERACTIONCOMMAND_H

#include <string>
#include <variant>
#include <stdexcept>

// Include the actual Position header from the domain model
#include "Position.h"

namespace TuiRogGame {
    namespace Port {
        namespace In {

            // PlayerActionCommand is a data transfer object (DTO) that encapsulates
            // the details of a player's action. It acts as the input model for use cases.
            // As per HexBook.md Chapter 5, input validation should ideally happen
            // within the command's constructor or a dedicated validator.
            struct PlayerActionCommand {
                enum ActionType {
                    INITIALIZE, // To initialize the game state
                    MOVE_UP,
                    MOVE_DOWN,
                    MOVE_LEFT,
                    MOVE_RIGHT,
                    ATTACK,
                    INTERACT,
                    USE_ITEM, // New action type
                    QUIT,
                    UNKNOWN // Default or error state
                };

                ActionType type;

                // Using std::variant for a type-safe payload
                // std::monostate for actions with no specific payload
                // int for simple integer values (e.g., attack strength, direction magnitude)
                // TuiRogGame::Domain::Model::Position for specific coordinates
                // std::string for item IDs or other string-based data
                std::variant<std::monostate, int, TuiRogGame::Domain::Model::Position, std::string> payload;

                // Constructor for actions with no specific payload
                explicit PlayerActionCommand(ActionType type) : type(type), payload(std::monostate{}) {
                    validate();
                }

                // Constructor for actions with an integer payload
                PlayerActionCommand(ActionType type, int intPayload) : type(type), payload(intPayload) {
                    validate();
                }

                // Constructor for actions with a Position payload
                PlayerActionCommand(ActionType type, const TuiRogGame::Domain::Model::Position& posPayload) : type(type), payload(posPayload) {
                    validate();
                }

                // Constructor for actions with a string payload
                PlayerActionCommand(ActionType type, const std::string& stringPayload) : type(type), payload(stringPayload) {
                    validate();
                }

                // Basic validation example (as discussed in HexBook.md Chapter 5)
                // This could be expanded with more specific validation logic
                private:
                void validate() const {
                    switch (type) {
                        case ATTACK:
                            if (!std::holds_alternative<int>(payload)) {
                                throw std::invalid_argument("Attack action requires an integer payload (e.g., damage).");
                            }
                            break;
                        case MOVE_UP:
                        case MOVE_DOWN:
                        case MOVE_LEFT:
                        case MOVE_RIGHT:
                            // Movement might use int for steps, or Position for target
                            // For now, let's assume simple moves don't need complex payload validation here
                            break;
                        case INTERACT:
                            if (!std::holds_alternative<std::string>(payload)) {
                                // Example: Interact with an item by its ID
                                // throw std::invalid_argument("Interact action requires a string payload (e.g., item ID).");
                            }
                            break;
                        case USE_ITEM:
                            if (!std::holds_alternative<std::string>(payload)) {
                                throw std::invalid_argument("UseItem action requires a string payload (item ID/name).");
                            }
                            break;
                        case QUIT:
                        case UNKNOWN:
                            // No specific payload validation needed
                            break;
                    }
                }
            };

        } // namespace In
    } // namespace Port
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_APPLICATION_PORT_IN_INCLUDE_PLAYERACTIONCOMMAND_H
