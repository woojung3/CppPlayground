#pragma once

#include "GameStateDTO.h"
#include "IGenerateDescriptionPort.h"
#include <string>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Description {

class HardcodedDescAdapter : public Port::Out::IGenerateDescriptionPort {
public:
  HardcodedDescAdapter() = default;
  ~HardcodedDescAdapter() override = default;

  std::string
  generateDescription(const Port::Out::GameStateDTO &game_state,
                      const Domain::Event::DomainEvent &event) override;
};

} // namespace Description
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
