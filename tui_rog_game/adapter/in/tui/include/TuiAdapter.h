#pragma once

#include "DomainEvent.h"
#include "GameStateDTO.h"
#include "IGetPlayerActionUseCase.h"
#include "IRenderPort.h"
#include <ftxui/component/screen_interactive.hpp>
#include <memory>
#include <optional>

namespace TuiRogGame {
namespace Adapter {
namespace In {
namespace Tui {

class TuiAdapter : public Port::Out::IRenderPort {
public:
  explicit TuiAdapter(
      std::shared_ptr<Port::In::IGetPlayerActionUseCase> game_engine,
      ftxui::ScreenInteractive &screen);

  void run();
  void render(const Port::Out::GameStateDTO &game_state,
              const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>
                  &events) override;

private:
  std::shared_ptr<Port::In::IGetPlayerActionUseCase> game_engine_;
  ftxui::ScreenInteractive &screen_;
  std::shared_ptr<std::optional<Port::Out::GameStateDTO>> game_state_ptr_;
  std::vector<std::string> message_log_;
  bool show_start_screen_ = true;
};

} // namespace Tui
} // namespace In
} // namespace Adapter
} // namespace TuiRogGame
