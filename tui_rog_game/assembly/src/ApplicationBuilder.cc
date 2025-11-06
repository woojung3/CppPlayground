#include "ApplicationBuilder.h"

#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "GameEngine.h"
#include "HardcodedDescAdapter.h"
#include "ISaveGameStatePort.h"
#include "ILoadGameStatePort.h"
#include "LevelDbAdapter.h"
#include "LlmAdapter.h"
#include "TuiAdapter.h"
#include "ftxui/component/screen_interactive.hpp"

namespace TuiRogGame {
namespace Assembly {

std::unique_ptr<TuiRogGame::Adapter::In::Tui::TuiAdapter> TuiRogGame::Assembly::ApplicationBuilder::build(ftxui::ScreenInteractive& screen) {
    auto persistence_adapter = std::make_shared<Adapter::Out::Persistence::LevelDbAdapter>("./game_data.db");
    auto hardcoded_desc_adapter = std::make_unique<Adapter::Out::Description::HardcodedDescAdapter>();
    auto chatgpt_desc_adapter = std::make_unique<Adapter::Out::Description::LlmAdapter>();

    auto game_engine = std::make_shared<Domain::Service::GameEngine>(
        std::static_pointer_cast<Port::Out::ISaveGameStatePort>(persistence_adapter),
        std::static_pointer_cast<Port::Out::ILoadGameStatePort>(persistence_adapter),
        std::move(hardcoded_desc_adapter),
        std::move(chatgpt_desc_adapter)
    );

    auto tui_adapter = std::make_unique<Adapter::In::Tui::TuiAdapter>(game_engine, screen);

    game_engine->setRenderPort(tui_adapter.get());

    return tui_adapter;
}

} // namespace Assembly
} // namespace TuiRogGame
