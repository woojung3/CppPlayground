#pragma once

#include "GameStateDTO.h"
#include "IGenerateDescriptionPort.h"
#include <memory>

namespace httplib {
class Client;
}

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Description {

class LlmAdapter : public Port::Out::IGenerateDescriptionPort {
public:
  LlmAdapter();
  ~LlmAdapter() override;
  std::string
  generateDescription(const Port::Out::GameStateDTO &game_state) override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace Description
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
