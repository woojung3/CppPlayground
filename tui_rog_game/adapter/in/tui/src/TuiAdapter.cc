#include "TuiAdapter.h"
#include "PlayerMovedEvent.h"
#include "Map.h"
#include "Player.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <string>

namespace TuiRogGame {
namespace Adapter {
namespace In {
namespace Tui {

// Helper to convert domain Tile to ftxui Element
ftxui::Element TileToElement(Domain::Model::Tile tile) {
    switch (tile) {
        case Domain::Model::Tile::WALL:  return ftxui::text("#") | ftxui::color(ftxui::Color::GrayDark);
        case Domain::Model::Tile::FLOOR: return ftxui::text(".") | ftxui::color(ftxui::Color::GrayLight);
        case Domain::Model::Tile::EXIT:  return ftxui::text(">") | ftxui::color(ftxui::Color::Yellow);
        case Domain::Model::Tile::ENEMY: return ftxui::text("E") | ftxui::color(ftxui::Color::Red);
        case Domain::Model::Tile::ITEM:  return ftxui::text("I") | ftxui::color(ftxui::Color::Green);
        default: return ftxui::text("?");
    }
}

TuiAdapter::TuiAdapter(Port::In::IGetPlayerActionUseCase& game_engine, ftxui::ScreenInteractive& screen)
    : game_engine_(game_engine), screen_(screen), game_state_ptr_(std::make_shared<std::optional<Port::Out::GameStateDTO>>(std::nullopt)) {
    spdlog::info("TuiAdapter initialized.");
}

void TuiAdapter::run() {
    spdlog::info("TuiAdapter::run() started.");
    using namespace ftxui;

    auto renderer = Renderer([=] {
        if (!game_state_ptr_->has_value()) {
            return text("Initializing...");
        }

        const auto& state = game_state_ptr_->value();
        const auto& map = state.map;
        const auto& player = state.player;

        std::vector<Element> rows_elements;
        for (int y = 0; y < map.getHeight(); ++y) {
            Elements row_chars;
            for (int x = 0; x < map.getWidth(); ++x) {
                if (player.getPosition().x == x && player.getPosition().y == y) {
                    row_chars.push_back(text("@") | color(Color::Blue));
                } else {
                    row_chars.push_back(TileToElement(map.getTile(x, y)));
                }
            }
            rows_elements.push_back(hbox(row_chars));
        }
        auto map_view = vbox(rows_elements);

        auto stats_view = vbox({
            text("Player Stats"),
            text("HP: " + std::to_string(player.getHp())),
            text("Level: " + std::to_string(player.getLevel())),
            text("XP: " + std::to_string(player.getXp())),
        });

        return hbox({map_view, separator(), stats_view});
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event.is_character()) {
            Port::In::PlayerActionCommand command(Port::In::PlayerActionCommand::UNKNOWN);
            char input = event.character()[0];
            switch (input) {
                case 'w': command = Port::In::PlayerActionCommand(Port::In::PlayerActionCommand::MOVE_UP); break;
                case 's': command = Port::In::PlayerActionCommand(Port::In::PlayerActionCommand::MOVE_DOWN); break;
                case 'a': command = Port::In::PlayerActionCommand(Port::In::PlayerActionCommand::MOVE_LEFT); break;
                case 'd': command = Port::In::PlayerActionCommand(Port::In::PlayerActionCommand::MOVE_RIGHT); break;
                case 'q': screen_.Exit(); return true;
            }
            game_engine_.handlePlayerAction(command);
            return true;
        }
        return false;
    });

    // Send initialize command to load initial game state
    game_engine_.handlePlayerAction(Port::In::PlayerActionCommand(Port::In::PlayerActionCommand::INITIALIZE));

    screen_.Loop(component);
    spdlog::info("TuiAdapter::run() exited.");
}

void TuiAdapter::render(const Port::Out::GameStateDTO& game_state, const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>& events) {
    // This method is called from the GameEngine thread.
    // To update the UI, we use emplace to construct a new GameStateDTO in the optional
    // and then post an event to the UI thread.
    game_state_ptr_->emplace(game_state);
    screen_.PostEvent(ftxui::Event::Custom);
}

} // namespace Tui
} // namespace In
} // namespace Adapter
} // namespace TuiRogGame
