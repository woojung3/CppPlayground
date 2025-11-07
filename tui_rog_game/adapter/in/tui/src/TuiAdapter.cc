#include "TuiAdapter.h"
#include "CombatStartedEvent.h"
#include "DescriptionGeneratedEvent.h"
#include "EnemyAttackedEvent.h"
#include "EnemyDefeatedEvent.h"
#include "ItemFoundEvent.h"
#include "ItemUsedEvent.h"
#include "Map.h"
#include "Player.h"
#include "PlayerAttackedEvent.h"
#include "PlayerLeveledUpEvent.h"
#include "PlayerMovedEvent.h"
#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace TuiRogGame {
namespace Adapter {
namespace In {
namespace Tui {

ftxui::Element TileToElement(Domain::Model::Tile tile) {
  switch (tile) {
  case Domain::Model::Tile::WALL:
    return ftxui::text("#") | ftxui::color(ftxui::Color::GrayDark);
  case Domain::Model::Tile::FLOOR:
    return ftxui::text(".") | ftxui::color(ftxui::Color::GrayLight);
  case Domain::Model::Tile::EXIT:
    return ftxui::text(">") | ftxui::color(ftxui::Color::Yellow);
  case Domain::Model::Tile::ENEMY:
    return ftxui::text("E") | ftxui::color(ftxui::Color::Red);
  case Domain::Model::Tile::ITEM:
    return ftxui::text("I") | ftxui::color(ftxui::Color::Green);
  default:
    return ftxui::text("?");
  }
}

TuiAdapter::TuiAdapter(
    std::shared_ptr<Port::In::IGetPlayerActionUseCase> game_engine,
    ftxui::ScreenInteractive &screen)
    : game_engine_(std::move(game_engine)), screen_(screen),
      game_state_ptr_(std::make_shared<std::optional<Port::Out::GameStateDTO>>(
          std::nullopt)) {
  spdlog::info("TuiAdapter initialized.");
  message_log_.push_back("Welcome to TUI-ROG!");
}

void TuiAdapter::run() {
  spdlog::info("TuiAdapter::run() started.");
  using namespace ftxui;

  auto renderer = Renderer([&] {
    if (show_start_screen_) {
      auto title =
          ftxui::text("TUI-ROG: Dungeon Master") | ftxui::bold | ftxui::center;
      auto instructions =
          ftxui::vbox(
              {ftxui::text(""), ftxui::text("Controls:"),
               ftxui::text("  W: Move Up"), ftxui::text("  S: Move Down"),
               ftxui::text("  A: Move Left"), ftxui::text("  D: Move Right"),
               ftxui::text("  X: Attack (if enemy present)"),
               ftxui::text("  U: Use Item (requires item name)"),
               ftxui::text("  M: Toggle Description Model "
                           "(Hardcoded/AI)"),
               ftxui::text("  Q: Quit"), ftxui::text(""),
               ftxui::text("Press any key to start...") | ftxui::center}) |
          ftxui::border;
      return ftxui::vbox({title, instructions}) | ftxui::center | ftxui::flex;
    }

    if (!game_state_ptr_->has_value()) {
      return text("Initializing...");
    }

    const auto &state = game_state_ptr_->value();
    const auto &map = state.map;
    const auto &player = state.player;

    auto render_map_elements =
        [&](const Domain::Model::Map &current_map,
            const Domain::Model::Player &current_player) {
          std::vector<Element> rows_elements;
          for (int y = 0; y < current_map.getHeight(); ++y) {
            Elements row_chars;
            for (int x = 0; x < current_map.getWidth(); ++x) {
              if (current_player.getPosition().x == x &&
                  current_player.getPosition().y == y) {
                row_chars.push_back(text("@") | color(Color::Blue) |
                                    ftxui::blink);
              } else {
                row_chars.push_back(TileToElement(current_map.getTile(x, y)));
              }
            }
            rows_elements.push_back(hbox(row_chars));
          }
          return vbox(rows_elements) | ftxui::flex;
        };

    auto render_ascii_art = [&]() -> Element {
      auto px = player.getPosition().x;
      auto py = player.getPosition().y;

      auto current_tile = map.getTile(px, py);

      std::vector<std::pair<int, int>> adjacent = {
          {px, py - 1}, // 위
          {px, py + 1}, // 아래
          {px - 1, py}, // 왼쪽
          {px + 1, py}  // 오른쪽
      };

      Domain::Model::Tile display_tile = current_tile;

      for (const auto &pos : adjacent) {
        if (pos.first >= 0 && pos.first < map.getWidth() && pos.second >= 0 &&
            pos.second < map.getHeight()) {
          auto tile = map.getTile(pos.first, pos.second);
          if (tile == Domain::Model::Tile::ENEMY) {
            display_tile = tile;
            break; // Enemy가 최우선
          } else if (tile == Domain::Model::Tile::ITEM &&
                     display_tile != Domain::Model::Tile::ENEMY) {
            display_tile = tile;
          } else if (tile == Domain::Model::Tile::EXIT &&
                     display_tile != Domain::Model::Tile::ENEMY &&
                     display_tile != Domain::Model::Tile::ITEM) {
            display_tile = tile;
          }
        }
      }

      switch (display_tile) {
      case Domain::Model::Tile::FLOOR:
        return vbox({text(""), 
                     text("    ╔════════════════╗"), 
                     text("    ║  ░░░░░░░░░░░░  ║"),
                     text("    ║  ░░ STONE ░░░  ║"),
                     text("    ║  ░░ FLOOR ░░░  ║"),
                     text("    ║  ░░░░░░░░░░░░  ║"),
                     text("    ╚════════════════╝"),
                     text("")});
      case Domain::Model::Tile::EXIT:
        return vbox({text(""),
                     text("    ╔════════════════╗"),
                     text("    ║    ┌──────┐    ║"),
                     text("    ║    │ ►►►► │    ║"),
                     text("    ║    │ EXIT │    ║"),
                     text("    ║    │ ►►►► │    ║"),
                     text("    ║    └──────┘    ║"),
                     text("    ╚════════════════╝"),
                     text("")}) |
               ftxui::color(ftxui::Color::Yellow);
      case Domain::Model::Tile::WALL:
        return vbox({text(""),
                     text("    ╔════════════════╗"),
                     text("    ║████████████████║"),
                     text("    ║█╬══╬══╬══╬═══██║"),
                     text("    ║█║  ║  ║  ║   ██║"),
                     text("    ║█╬══╬══╬══╬═══██║"),
                     text("    ║████████████████║"),
                     text("    ╚════════════════╝"),
                     text("")}) |
               ftxui::color(ftxui::Color::GrayDark);
      case Domain::Model::Tile::ENEMY:
        return vbox({text(""),
                     text("    ╔════════════════╗"),
                     text("    ║   ▄███████▄    ║"),
                     text("    ║  ███ ◉ ◉ ███   ║"),
                     text("    ║   ▀██▄▄▄██▀    ║"),
                     text("    ║     ▀███▀      ║"),
                     text("    ║   ⚔ ENEMY ⚔    ║"),
                     text("    ╚════════════════╝"),
                     text("")}) |
               ftxui::color(ftxui::Color::Red) | ftxui::bold | ftxui::blink;
      case Domain::Model::Tile::ITEM:
        return vbox({text(""),
                     text("    ╔════════════════╗"),
                     text("    ║       ✦        ║"),
                     text("    ║    ╔═════╗     ║"),
                     text("    ║  ✦ ║ ◈◈◈ ║ ✦   ║"),
                     text("    ║    ╚═════╝     ║"),
                     text("    ║    ⟨ ITEM ⟩    ║"),
                     text("    ╚════════════════╝"),
                     text("")}) |
               ftxui::color(ftxui::Color::Green) | ftxui::bold;
      default:
        return vbox({text(""), 
                     text("    ╔════════════════╗"),
                     text("    ║                ║"),
                     text("    ║   Loading...   ║"),
                     text("    ║                ║"),
                     text("    ╚════════════════╝"),
                     text("")});
      }
    };

    auto ascii_art_view =
        vbox({
            text("Nearby") | ftxui::bold | ftxui::center, separator(),
            render_ascii_art() | ftxui::center // 수평 가운데 정렬
        }) |
        ftxui::vcenter | border | flex; // 수직 가운데 정렬

    auto stats_view = vbox(Elements{
        text("Player Stats") | ftxui::bold, separator(),
        text("HP: " + std::to_string(player.getHp()) + "/" +
             std::to_string(player.getMaxHp())),
        gauge((float)player.getHp() / player.getMaxHp()) |
            color(ftxui::Color::RedLight),
        text("Level: " + std::to_string(player.getLevel())),
        text("XP: " + std::to_string(player.getXp())), separator(),
        text("Attack: " + std::to_string(player.getAttackPower())),
        text("Str: " + std::to_string(player.getStats().strength)),
        text("Dex: " + std::to_string(player.getStats().dexterity)),
        text("Int: " + std::to_string(player.getStats().intelligence)),
        text("Vit: " + std::to_string(player.getStats().vitality)), separator(),
        text("Inventory:") | ftxui::bold, vbox([&] {
          Elements inventory_elements;
          std::transform(
              player.getInventory().begin(), player.getInventory().end(),
              std::back_inserter(inventory_elements),
              [](const std::unique_ptr<Domain::Model::Item> &item_ptr) {
                return ftxui::text("- " + item_ptr->getName());
              });
          return inventory_elements;
        }())});

    auto mini_map_view = render_map_elements(map, player) | border |
                         ftxui::size(ftxui::HEIGHT, ftxui::EQUAL,
                                     map.getHeight() + 2); // +2 for border

    Elements log_lines;
    int start_index =
        std::max(0, (int)message_log_.size() - 5); // Display last 5 messages
    for (size_t i = start_index; i < message_log_.size(); ++i) {
      log_lines.push_back(
          paragraph(message_log_[i])); // Use paragraph for word wrapping
    }
    auto message_log_view = vbox(log_lines) | border | flex;

    auto top_panel = hbox(Elements{
                         ascii_art_view, separator(),
                         stats_view | ftxui::size(ftxui::WIDTH, ftxui::EQUAL,
                                                  30) // Fixed width for stats
                     }) |
                     flex;

    auto bottom_panel =
        hbox(Elements{
            mini_map_view |
                ftxui::size(ftxui::WIDTH, ftxui::EQUAL,
                            map.getWidth() + 2), // Explicitly set minimap width
            separator(),
            message_log_view |
                flex // Message log takes flexible width in bottom panel
        }) |
        ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, map.getHeight() + 2);

    return vbox(Elements{top_panel, separator(), bottom_panel}) | ftxui::flex;
  });

  auto component = CatchEvent(renderer, [&](Event event) {
    if (show_start_screen_) {
      show_start_screen_ = false; // Dismiss start screen on any key press
      game_engine_->handlePlayerAction(Port::In::PlayerActionCommand(
          Port::In::PlayerActionCommand::
              INITIALIZE)); // Initialize game after dismissing start screen
      return true;
    }

    if (event.is_character()) {
      Port::In::PlayerActionCommand command(
          Port::In::PlayerActionCommand::UNKNOWN);
      char input = event.character()[0];
      switch (input) {
      case 'w':
        command = Port::In::PlayerActionCommand(
            Port::In::PlayerActionCommand::MOVE_UP);
        break;
      case 's':
        command = Port::In::PlayerActionCommand(
            Port::In::PlayerActionCommand::MOVE_DOWN);
        break;
      case 'a':
        command = Port::In::PlayerActionCommand(
            Port::In::PlayerActionCommand::MOVE_LEFT);
        break;
      case 'd':
        command = Port::In::PlayerActionCommand(
            Port::In::PlayerActionCommand::MOVE_RIGHT);
        break;
      case 'x':
        if (game_state_ptr_->has_value()) {
          command = Port::In::PlayerActionCommand(
              Port::In::PlayerActionCommand::ATTACK,
              game_state_ptr_->value().player.getAttackPower());
        } else {
          spdlog::warn("Cannot attack: Game state not initialized.");
          return false;
        }
        break; // New control for attack
      case 'u':

        command = Port::In::PlayerActionCommand(
            Port::In::PlayerActionCommand::USE_ITEM, "Health Potion");
        break;
      case 'm': // Toggle description model
        game_engine_->toggleDescriptionPort();
        message_log_.push_back("Description model toggled.");
        return true; // Event handled, no further action needed for game_engine_
      case 'q':
        screen_.Exit();
        return true;
      default:
        spdlog::warn("Unknown input character: {}", input);
        return false; // Event not handled by game logic, return false
      }
      game_engine_->handlePlayerAction(command);
      return true; // Event handled by game logic
    }
    return false; // Event not a character event, return false
  });

  screen_.SetCursor(ftxui::Screen::Cursor{0, 0});

  screen_.Loop(component);
  spdlog::info("TuiAdapter::run() exited.");
}

void TuiAdapter::render(
    const Port::Out::GameStateDTO &game_state,
    const std::vector<std::unique_ptr<Domain::Event::DomainEvent>> &events) {
  bool state_changed = false;

  if (!game_state_ptr_->has_value() ||
      game_state_ptr_->value().player.getPosition().x !=
          game_state.player.getPosition().x ||
      game_state_ptr_->value().player.getPosition().y !=
          game_state.player.getPosition().y ||
      game_state_ptr_->value().player.getHp() != game_state.player.getHp()) {
    state_changed = true;
  }

  game_state_ptr_->emplace(game_state);

  for (const auto &event : events) {
    if (event->getType() == Domain::Event::DomainEvent::Type::CombatStarted) {
      const auto *combat_event =
          dynamic_cast<const Domain::Event::CombatStartedEvent *>(event.get());
      if (combat_event) {
        message_log_.push_back("You encountered a " +
                               combat_event->getEnemyName() + "!");
      }
    } else if (event->getType() ==
               Domain::Event::DomainEvent::Type::ItemFound) {
      const auto *item_event =
          dynamic_cast<const Domain::Event::ItemFoundEvent *>(event.get());
      if (item_event) {
        message_log_.push_back("You found a " + item_event->getItemName() +
                               "!");
      }
    } else if (event->getType() ==
               Domain::Event::DomainEvent::Type::PlayerAttacked) {
      const auto *player_attacked_event =
          dynamic_cast<const Domain::Event::PlayerAttackedEvent *>(event.get());
      if (player_attacked_event) {
        message_log_.push_back(player_attacked_event->toString());
      }
    } else if (event->getType() ==
               Domain::Event::DomainEvent::Type::EnemyAttacked) {
      const auto *enemy_attacked_event =
          dynamic_cast<const Domain::Event::EnemyAttackedEvent *>(event.get());
      if (enemy_attacked_event) {
        message_log_.push_back(enemy_attacked_event->toString());
      }
    } else if (event->getType() ==
               Domain::Event::DomainEvent::Type::EnemyDefeated) {
      const auto *enemy_defeated_event =
          dynamic_cast<const Domain::Event::EnemyDefeatedEvent *>(event.get());
      if (enemy_defeated_event) {
        message_log_.push_back(enemy_defeated_event->toString());
      }
    } else if (event->getType() ==
               Domain::Event::DomainEvent::Type::PlayerLeveledUp) {
      const auto *player_leveled_up_event =
          dynamic_cast<const Domain::Event::PlayerLeveledUpEvent *>(
              event.get());
      if (player_leveled_up_event) {
        message_log_.push_back(player_leveled_up_event->toString());
      }
    } else if (event->getType() == Domain::Event::DomainEvent::Type::ItemUsed) {
      spdlog::info("TuiAdapter: Received ItemUsedEvent. Type: {}",
                   static_cast<int>(event->getType()));
      const auto *item_used_event =
          dynamic_cast<const Domain::Event::ItemUsedEvent *>(event.get());
      if (item_used_event) {
        spdlog::info("TuiAdapter: dynamic_cast to ItemUsedEvent successful. "
                     "Item name: {}",
                     item_used_event->getItemName());
        message_log_.push_back("You used a " + item_used_event->getItemName() +
                               "!");
      } else {
        spdlog::warn("TuiAdapter: dynamic_cast to ItemUsedEvent failed for "
                     "event type {}.",
                     static_cast<int>(event->getType()));
      }
    } else if (event->getType() ==
               Domain::Event::DomainEvent::Type::DescriptionGenerated) {
      const auto *description_generated_event =
          dynamic_cast<const Domain::Event::DescriptionGeneratedEvent *>(
              event.get());
      if (description_generated_event) {
        message_log_.push_back(description_generated_event->getDescription());
      }
    }
    state_changed = true; // 이벤트가 있으면 무조건 업데이트
  }

  if (state_changed) {
    screen_.PostEvent(ftxui::Event::Custom);
  }
}

} // namespace Tui
} // namespace In
} // namespace Adapter
} // namespace TuiRogGame
