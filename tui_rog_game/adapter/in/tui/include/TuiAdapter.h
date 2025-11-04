#pragma once

#include "IRenderPort.h"
#include "IGetPlayerActionUseCase.h"
#include "DomainEvent.h"
#include <ftxui/component/screen_interactive.hpp>

namespace TuiRogGame {
namespace Adapter {
namespace In {
namespace Tui {

class TuiAdapter : public Port::Out::IRenderPort {
public:
    // Constructor takes a reference to the game engine (inbound port) and the screen interactive instance.
    explicit TuiAdapter(Port::In::IGetPlayerActionUseCase& game_engine, ftxui::ScreenInteractive& screen);

    // Entry point to start the TUI event loop
    void run();

    // Implementation of the outbound render port
    void render(const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>& events) override;

private:
    // Non-owning reference to the game engine. Its lifetime is managed externally.
    Port::In::IGetPlayerActionUseCase& game_engine_;
    ftxui::ScreenInteractive& screen_;
    TuiRogGame::Domain::Model::Position player_position_;
};

} // namespace Tui
} // namespace In
} // namespace Adapter
} // namespace TuiRogGame
