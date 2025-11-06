#pragma once

#include "GameStateDTO.h" // GameStateDTO를 포함
#include "IGenerateDescriptionPort.h"
#include <memory>

namespace httplib {
class Client; // Forward declaration
}

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Description {

class LlmAdapter : public Port::Out::IGenerateDescriptionPort {
public:
  LlmAdapter();
  ~LlmAdapter() override; // Add destructor
  std::string
  generateDescription(const Port::Out::GameStateDTO &game_state) override;

private:
  struct Impl; // Forward declaration of the implementation struct
  std::unique_ptr<Impl> impl_; // Pointer to implementation
};

} // namespace Description
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
