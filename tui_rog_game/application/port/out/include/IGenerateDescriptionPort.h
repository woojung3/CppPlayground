#pragma once

#include "GameStateDTO.h" // GameStateDTO를 포함
#include <string>

namespace TuiRogGame {
namespace Port {
namespace Out {

class IGenerateDescriptionPort {
public:
  virtual ~IGenerateDescriptionPort() = default;
  virtual std::string generateDescription(const GameStateDTO &game_state) = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame
