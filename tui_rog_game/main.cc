#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h> // New include

#include "LevelDbAdapter.h"
#include "HardcodedDescAdapter.h"
#include "ChatGptAdapter.h" // New include
#include "GameEngine.h"
#include "TuiAdapter.h"

int main() {
    // Configure spdlog to write to a file
    try {
        auto file_logger = spdlog::basic_logger_mt("file_logger", "game.log");
        spdlog::set_default_logger(file_logger);
        spdlog::set_level(spdlog::level::info); // Set desired log level
        spdlog::flush_on(spdlog::level::info); // Flush logs immediately for info and above
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    // 1. Create concrete implementations of outbound adapters (excluding renderer)
    auto persistence_adapter = std::make_unique<TuiRogGame::Adapter::Out::Persistence::LevelDbAdapter>("./game_data.db");
    auto hardcoded_desc_adapter = std::make_unique<TuiRogGame::Adapter::Out::Description::HardcodedDescAdapter>();
    auto chatgpt_desc_adapter = std::make_unique<TuiRogGame::Adapter::Out::Description::ChatGptAdapter>(); // New adapter

    // 2. Create the GameEngine. It doesn't know about the renderer yet.
    // Pass both description adapters. The primary will be Hardcoded, alternative will be ChatGPT.
    auto game_engine = std::make_unique<TuiRogGame::Domain::Service::GameEngine>(
        std::move(persistence_adapter),
        std::move(hardcoded_desc_adapter),
        std::move(chatgpt_desc_adapter)
    );

    auto screen = ftxui::ScreenInteractive::Fullscreen();

    // 3. Create the TuiAdapter, passing it a non-owning reference to the GameEngine and the screen instance.
    TuiRogGame::Adapter::In::Tui::TuiAdapter tui_adapter(*game_engine, screen);

    // 4. Now, connect the GameEngine to the TuiAdapter (setter injection).
    game_engine->setRenderPort(&tui_adapter);

    // 5. Start the TuiAdapter's game loop, which is the main application loop.
    // The adapter will send an INITIALIZE command to the engine to start the game.
    tui_adapter.run();

    return 0;
}
