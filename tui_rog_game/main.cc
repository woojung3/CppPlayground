#include <iostream>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "ApplicationBuilder.h"

int main() {

  try {
    auto file_logger = spdlog::basic_logger_mt("file_logger", "game.log");
    spdlog::set_default_logger(file_logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::info);
  } catch (const spdlog::spdlog_ex &ex) {
    std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    return 1;
  }

  auto screen = ftxui::ScreenInteractive::Fullscreen();
  auto tui_adapter = TuiRogGame::Assembly::ApplicationBuilder::build(screen);
  tui_adapter->run();

  return 0;
}
