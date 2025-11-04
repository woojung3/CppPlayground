#include <iostream>
#include <memory>

#include "InMemoryAdapter.h"
#include "HardcodedDescAdapter.h"
#include "GameEngine.h"
#include "TuiAdapter.h"

int main() {
    // 1. Create concrete implementations of outbound adapters (excluding renderer)
    auto persistence_adapter = std::make_unique<TuiRogGame::Adapter::Out::Persistence::InMemoryAdapter>();
    auto description_adapter = std::make_unique<TuiRogGame::Adapter::Out::Description::HardcodedDescAdapter>();

    // 2. Create the GameEngine. It doesn't know about the renderer yet.
    auto game_engine = std::make_unique<TuiRogGame::Domain::Service::GameEngine>(
        std::move(persistence_adapter),
        std::move(description_adapter)
    );

    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    // 3. Create the TuiAdapter, passing it a non-owning reference to the GameEngine and the screen instance.
    TuiRogGame::Adapter::In::Tui::TuiAdapter tui_adapter(*game_engine, screen);

    // 4. Now, connect the GameEngine to the TuiAdapter (setter injection).
    game_engine->setRenderPort(&tui_adapter);

    // 5. Start the TuiAdapter's game loop, which is the main application loop.
    // The adapter will send an INITIALIZE command to the engine to start the game.
    tui_adapter.run();

    return 0;
}
