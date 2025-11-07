#pragma once

#include "DomainEvent.h"
#include "GameStateDTO.h"
#include <string>

namespace TuiRogGame {
namespace Port {
namespace Out {

class IGenerateDescriptionPort {
public:
  virtual ~IGenerateDescriptionPort() = default;
  virtual std::string
  generateDescription(const GameStateDTO &game_state,
                      const Domain::Event::DomainEvent &event) = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame
