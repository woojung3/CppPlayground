#pragma once

#include "IRenderPort.h"
#include "IGetPlayerActionUseCase.h"
#include "DomainEvent.h"
#include "GameStateDTO.h"
#include <ftxui/component/screen_interactive.hpp>
#include <memory>   // For std::shared_ptr
#include <optional> // For std::optional

namespace TuiRogGame {
namespace Adapter {
namespace In {
namespace Tui {

class TuiAdapter : public Port::Out::IRenderPort {
public:
    explicit TuiAdapter(Port::In::IGetPlayerActionUseCase& game_engine, ftxui::ScreenInteractive& screen);

    void run();
    void render(const Port::Out::GameStateDTO& game_state, const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>& events) override;

private:
    Port::In::IGetPlayerActionUseCase& game_engine_;
    ftxui::ScreenInteractive& screen_;
    std::shared_ptr<std::optional<Port::Out::GameStateDTO>> game_state_ptr_;
    std::vector<std::string> message_log_;
    bool show_start_screen_ = true; // New member to control start screen visibility
};

} // namespace Tui
} // namespace In
} // namespace Adapter
} // namespace TuiRogGame
