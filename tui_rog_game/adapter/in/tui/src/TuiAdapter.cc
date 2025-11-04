#include "TuiAdapter.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <spdlog/spdlog.h>
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::milliseconds
#include <termios.h> // For tcgetattr, tcsetattr
#include <unistd.h>  // For STDIN_FILENO

namespace TuiRogGame {
namespace Adapter {
namespace In {
namespace Tui {

// Helper function to explicitly restore terminal settings
void restore_terminal_settings() {
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) == 0) {
        term.c_lflag |= (ICANON | ECHO); // Re-enable canonical mode and echoing
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        spdlog::info("Terminal settings explicitly restored.");
    } else {
        spdlog::warn("Failed to get terminal attributes for restoration.");
    }
}

TuiAdapter::TuiAdapter(Port::In::IGetPlayerActionUseCase& game_engine)
    : game_engine_(game_engine) {
    spdlog::info("TuiAdapter initialized.");
}

void TuiAdapter::run() {
    spdlog::info("TuiAdapter::run() started.");
    using namespace ftxui;

    auto screen = ScreenInteractive::TerminalOutput();

    // This will be the main component that renders the game UI.
    auto renderer = Renderer([&] {
        // For now, just a placeholder. Later, this will render the map, player, stats, etc.
        return vbox({
            text("TUI-ROG Game Window"),
            text("Press 'q' to quit."),
        });
    });

    // Capture keyboard events.
    auto component = CatchEvent(renderer, [&](Event event) {
        if (event.is_character()) {
            spdlog::info("TuiAdapter: Caught character event: {}", event.character());
            // Create a PlayerActionCommand from the event
            TuiRogGame::Port::In::PlayerActionCommand command(TuiRogGame::Port::In::PlayerActionCommand::UNKNOWN);
            if (event.character() == "w") {
                command = TuiRogGame::Port::In::PlayerActionCommand(TuiRogGame::Port::In::PlayerActionCommand::MOVE_UP);
            } else if (event.character() == "s") {
                command = TuiRogGame::Port::In::PlayerActionCommand(TuiRogGame::Port::In::PlayerActionCommand::MOVE_DOWN);
            } else if (event.character() == "a") {
                command = TuiRogGame::Port::In::PlayerActionCommand(TuiRogGame::Port::In::PlayerActionCommand::MOVE_LEFT);
            } else if (event.character() == "d") {
                command = TuiRogGame::Port::In::PlayerActionCommand(TuiRogGame::Port::In::PlayerActionCommand::MOVE_RIGHT);
            } else if (event.character() == "q") {
                command = TuiRogGame::Port::In::PlayerActionCommand(TuiRogGame::Port::In::PlayerActionCommand::QUIT);
                screen.Exit(); // Exit the FTXUI loop
            }
            // Pass the command to the game engine
            game_engine_.handlePlayerAction(command);
            return true; // Signal that the event has been handled.
        }
        return false;
    });

    screen.Loop(component);

    // Explicitly restore terminal settings after the FTXUI loop exits.
    restore_terminal_settings();

    spdlog::info("TuiAdapter::run() exited.");
}

void TuiAdapter::render(const std::vector<std::unique_ptr<Common::DomainEvent>>& events) {
    spdlog::info("TuiAdapter::render() called with {} events.", events.size());
    for (const auto& event : events) {
        // For now, just log the event. Later, this will trigger UI updates.
        spdlog::info("  - Event: {}", event->toString());
    }
    // In a real FTXUI app, you would typically call screen.PostEvent(Event::Custom)
    // to trigger a repaint in a thread-safe way.
}

} // namespace Tui
} // namespace In
} // namespace Adapter
} // namespace TuiRogGame
