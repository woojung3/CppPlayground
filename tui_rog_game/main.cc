#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "ApplicationBuilder.h"

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

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto tui_adapter = TuiRogGame::Assembly::ApplicationBuilder::build(screen);
    tui_adapter->run();

    return 0;
}
