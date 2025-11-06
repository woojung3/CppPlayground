#pragma once

#include <stdexcept>
#include <string>
#include <variant>

#include "Position.h"

namespace TuiRogGame {
namespace Port {
namespace In {




struct PlayerActionCommand {
  enum ActionType {
    INITIALIZE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    ATTACK,
    INTERACT,
    USE_ITEM,
    QUIT,
    UNKNOWN
  };

  ActionType type;





  std::variant<std::monostate, int, TuiRogGame::Domain::Model::Position,
               std::string>
      payload;

  explicit PlayerActionCommand(ActionType type)
      : type(type), payload(std::monostate{}) {
    validate();
  }

  PlayerActionCommand(ActionType type, int intPayload)
      : type(type), payload(intPayload) {
    validate();
  }

  PlayerActionCommand(ActionType type,
                      const TuiRogGame::Domain::Model::Position &posPayload)
      : type(type), payload(posPayload) {
    validate();
  }

  PlayerActionCommand(ActionType type, const std::string &stringPayload)
      : type(type), payload(stringPayload) {
    validate();
  }


private:
  void validate() const {
    switch (type) {
    case ATTACK:
      if (!std::holds_alternative<int>(payload)) {
        throw std::invalid_argument(
            "Attack action requires an integer payload (e.g., damage).");
      }
      break;
    case MOVE_UP:
    case MOVE_DOWN:
    case MOVE_LEFT:
    case MOVE_RIGHT:



      break;
    case INTERACT:
      if (!std::holds_alternative<std::string>(payload)) {



      }
      break;
    case USE_ITEM:
      if (!std::holds_alternative<std::string>(payload)) {
        throw std::invalid_argument(
            "UseItem action requires a string payload (item ID/name).");
      }
      break;
    case QUIT:
    case UNKNOWN:

      break;
    }
  }
};

} // namespace In
} // namespace Port
} // namespace TuiRogGame

