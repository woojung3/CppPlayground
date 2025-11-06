#pragma once

#include "IGenerateDescriptionPort.h"
#include "GameStateDTO.h" // GameStateDTO를 포함
#include <memory>
#include <httplib.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Description {

class LlmAdapter : public Port::Out::IGenerateDescriptionPort {
public:
    LlmAdapter();
    std::string generateDescription(const Port::Out::GameStateDTO& game_state) override;

private:
    std::unique_ptr<httplib::Client> cli_;
};

} // namespace Description
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
