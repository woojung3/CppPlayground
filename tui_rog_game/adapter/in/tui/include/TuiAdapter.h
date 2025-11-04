#pragma once

#include "IRenderPort.h"
#include "IGetPlayerActionUseCase.h"

namespace TuiRogGame {
namespace Adapter {
namespace In {
namespace Tui {

class TuiAdapter : public Port::Out::IRenderPort {
public:
    // Constructor takes a reference to the game engine (inbound port).
    explicit TuiAdapter(Port::In::IGetPlayerActionUseCase& game_engine);

    // Entry point to start the TUI event loop
    void run();

    // Implementation of the outbound render port
    void render(const std::vector<std::unique_ptr<Common::DomainEvent>>& events) override;

private:
    // Non-owning reference to the game engine. Its lifetime is managed externally.
    Port::In::IGetPlayerActionUseCase& game_engine_;
};

} // namespace Tui
} // namespace In
} // namespace Adapter
} // namespace TuiRogGame
