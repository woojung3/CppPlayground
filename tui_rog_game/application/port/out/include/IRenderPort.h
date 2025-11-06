#pragma once

#include "DomainEvent.h"
#include "GameStateDTO.h"
#include <memory>
#include <vector>

namespace TuiRogGame {
namespace Port {
namespace Out {



class IRenderPort {
public:
  virtual ~IRenderPort() = default;


  virtual void
  render(const GameStateDTO &game_state,
         const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>
             &events) = 0;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame

