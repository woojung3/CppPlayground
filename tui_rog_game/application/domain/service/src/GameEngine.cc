#include "GameEngine.h"
#include "CombatStartedEvent.h"
#include "DescriptionGeneratedEvent.h"
#include "EnemyAttackedEvent.h"
#include "EnemyDefeatedEvent.h"
#include "GameLoadedEvent.h"
#include "ItemFoundEvent.h"
#include "ItemUsedEvent.h"
#include "MapChangedEvent.h"
#include "PlayerAttackedEvent.h"
#include "PlayerDiedEvent.h"
#include "PlayerLeveledUpEvent.h"
#include "PlayerMovedEvent.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Domain {
namespace Service {

GameEngine::GameEngine(
    std::shared_ptr<TuiRogGame::Port::Out::ISaveGameStatePort> save_port,
    std::shared_ptr<TuiRogGame::Port::Out::ILoadGameStatePort> load_port,
    std::unique_ptr<TuiRogGame::Port::Out::IGenerateDescriptionPort>
        primary_description_port,
    std::unique_ptr<TuiRogGame::Port::Out::IGenerateDescriptionPort>
        alternative_description_port)
    : save_port_(std::move(save_port)), load_port_(std::move(load_port)),
      primary_description_port_(std::move(primary_description_port)),
      alternative_description_port_(std::move(alternative_description_port)) {
  spdlog::info("GameEngine initialized.");
}

void GameEngine::setRenderPort(
    TuiRogGame::Port::Out::IRenderPort *render_port) {
  render_port_ = render_port;
}

std::vector<std::unique_ptr<TuiRogGame::Domain::Event::DomainEvent>>
GameEngine::initializeGame() {
  std::vector<std::unique_ptr<Domain::Event::DomainEvent>> events;

  if (load_port_) {
    auto loaded_game_state = load_port_->loadGameState();
    if (loaded_game_state) {

      map_ = std::make_unique<Model::Map>(loaded_game_state->map);
      player_ = std::make_unique<Model::Player>(loaded_game_state->player);
      events.push_back(std::make_unique<Domain::Event::GameLoadedEvent>());
      if (auto desc_event = createDescriptionEvent(
              Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
        events.push_back(std::move(desc_event));
      }

      spdlog::info("Game loaded. Player: {} at ({}, {})", player_->getName(),
                   player_->getPosition().x, player_->getPosition().y);
      return events;
    }
  }

  map_ = std::make_unique<Model::Map>(20, 10);
  map_->generate();

  player_ = std::make_unique<Model::Player>("player1", Model::Stats{},
                                            map_->getStartPlayerPosition());
  spdlog::info("New game initialized. Player: {} at ({}, {})",
               player_->getName(), player_->getPosition().x,
               player_->getPosition().y);

  events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(
      player_->getPosition()));
  if (auto desc_event = createDescriptionEvent(
          Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
    events.push_back(std::move(desc_event));
  }
  return events;
}

void GameEngine::handlePlayerAction(
    const TuiRogGame::Port::In::PlayerActionCommand &command) {
  spdlog::info("GameEngine: Handling player action type: {}",
               static_cast<int>(command.type));
  std::vector<std::unique_ptr<Domain::Event::DomainEvent>> events;

  switch (command.type) {
  case TuiRogGame::Port::In::PlayerActionCommand::INITIALIZE:
    events = initializeGame();
    break;
  case TuiRogGame::Port::In::PlayerActionCommand::MOVE_UP:
    events = processPlayerMove(0, -1);
    break;
  case TuiRogGame::Port::In::PlayerActionCommand::MOVE_DOWN:
    events = processPlayerMove(0, 1);
    break;
  case TuiRogGame::Port::In::PlayerActionCommand::MOVE_LEFT:
    events = processPlayerMove(-1, 0);
    break;
  case TuiRogGame::Port::In::PlayerActionCommand::MOVE_RIGHT:
    events = processPlayerMove(1, 0);
    break;
  case TuiRogGame::Port::In::PlayerActionCommand::ATTACK: {
    spdlog::debug("GameEngine: Player initiated attack command.");
    if (!current_enemy_) { // If not already in combat
      spdlog::debug(
          "GameEngine: Not in combat, checking for adjacent enemies.");

      Model::Position player_pos = player_->getPosition();
      std::vector<Model::Position> adjacent_positions = {
          {player_pos.x, player_pos.y - 1}, // Up
          {player_pos.x, player_pos.y + 1}, // Down
          {player_pos.x - 1, player_pos.y}, // Left
          {player_pos.x + 1, player_pos.y}  // Right
      };

      bool enemy_found = false;
      for (const auto &adj_pos : adjacent_positions) {
        spdlog::debug("GameEngine: Checking adjacent position ({}, {}).",
                      adj_pos.x, adj_pos.y);
        if (map_->isValidPosition(adj_pos.x, adj_pos.y)) {
          if (auto enemy_opt = map_->getEnemyAt(adj_pos)) {
            current_enemy_ = std::ref(enemy_opt->get());
            spdlog::debug("GameEngine: Enemy found at ({}, {}): {}. Setting "
                          "current_enemy_.",
                          adj_pos.x, adj_pos.y,
                          current_enemy_->get().getName());
            events.push_back(
                std::make_unique<Domain::Event::CombatStartedEvent>(
                    current_enemy_->get().getTypeName(),
                    current_enemy_->get().getName(),
                    current_enemy_->get().getHealth(),
                    current_enemy_->get().getStats().strength,
                    current_enemy_->get().getStats().vitality));
            if (auto desc_event = createDescriptionEvent(
                    Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
              events.push_back(std::move(desc_event));
            }
            spdlog::info("Combat started with adjacent enemy {} at ({}, {}).",
                         current_enemy_->get().getName(), adj_pos.x, adj_pos.y);
            enemy_found = true;
            break; // Found an enemy, start combat
          }
        }
      }

      if (!enemy_found) {
        spdlog::warn("Player tried to attack but no adjacent enemy found.");

        break; // Exit ATTACK case
      }
    } else {
      spdlog::debug(
          "GameEngine: Already in combat with {}. Proceeding with attack.",
          current_enemy_->get().getName());
    }

    int player_damage = player_->getAttackPower();
    spdlog::debug("GameEngine: Player attacking {} for {} damage.",
                  current_enemy_->get().getName(), player_damage);
    current_enemy_->get().takeDamage(player_damage);
    events.push_back(std::make_unique<Domain::Event::PlayerAttackedEvent>(
        player_damage, current_enemy_->get().getName(),
        current_enemy_->get().getHealth()));
    if (auto desc_event = createDescriptionEvent(
            Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
      events.push_back(std::move(desc_event));
    }
    spdlog::info("Player attacked {} for {} damage. {}'s health: {}",
                 current_enemy_->get().getName(), player_damage,
                 current_enemy_->get().getName(),
                 current_enemy_->get().getHealth());

    if (current_enemy_->get().getHealth() <= 0) {
      spdlog::debug("GameEngine: Enemy {} defeated.",
                    current_enemy_->get().getName());

      int xp_gained = 50; // Example XP
      bool leveled_up = player_->gainXp(xp_gained);
      events.push_back(std::make_unique<Domain::Event::EnemyDefeatedEvent>(
          current_enemy_->get().getName(), xp_gained));
      if (auto desc_event = createDescriptionEvent(
              Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
        events.push_back(std::move(desc_event));
      }
      spdlog::info("{} defeated! Player gained {} XP.",
                   current_enemy_->get().getName(), xp_gained);

      if (leveled_up) {
        events.push_back(std::make_unique<Domain::Event::PlayerLeveledUpEvent>(
            player_->getLevel(), player_->getStats()));
        if (auto desc_event = createDescriptionEvent(
                Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
          events.push_back(std::move(desc_event));
        }
        spdlog::info("Player leveled up to level {}!", player_->getLevel());
      }

      map_->removeEnemyAt(current_enemy_->get().getPosition());
      current_enemy_.reset(); // Clear current enemy
    } else {
      spdlog::debug("GameEngine: Enemy {} still alive. Health: {}. Enemy "
                    "attacking player.",
                    current_enemy_->get().getName(),
                    current_enemy_->get().getHealth());

      int enemy_damage = current_enemy_->get().getAttackPower();
      player_->takeDamage(enemy_damage);
      events.push_back(std::make_unique<Domain::Event::EnemyAttackedEvent>(
          current_enemy_->get().getName(), enemy_damage, player_->getHp()));
      if (auto desc_event = createDescriptionEvent(
              Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
        events.push_back(std::move(desc_event));
      }
      spdlog::info("{} attacked player for {} damage. Player's health: {}",
                   current_enemy_->get().getName(), enemy_damage,
                   player_->getHp());

      if (player_->getHp() <= 0) {
        spdlog::debug("GameEngine: Player defeated, but will be resurrected.");

        events.push_back(std::make_unique<Domain::Event::PlayerDiedEvent>());
        player_->setHp(player_->getMaxHp());

        if (auto desc_event = createDescriptionEvent(
                Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
          events.push_back(std::move(desc_event));
        }

        spdlog::info("Player defeated, but resurrected!");
      }
    }
    break;
  }
  case TuiRogGame::Port::In::PlayerActionCommand::INTERACT:

    spdlog::info("Player interacted.");
    break;
  case TuiRogGame::Port::In::PlayerActionCommand::USE_ITEM: {
    spdlog::info("Player initiated use item.");
    if (std::holds_alternative<std::string>(command.payload)) {
      const std::string &item_name = std::get<std::string>(command.payload);
      if (player_->useItem(item_name)) {
        events.push_back(
            std::make_unique<Domain::Event::ItemUsedEvent>(item_name));
        if (auto desc_event = createDescriptionEvent(
                Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
          events.push_back(std::move(desc_event));
        }
        spdlog::info("Player used item: {}", item_name);
      } else {
        spdlog::warn("Player tried to use item '{}' but it was not found or "
                     "could not be used.",
                     item_name);
      }
    } else {
      spdlog::error("USE_ITEM command received without a string payload.");
    }
    break;
  }
  case TuiRogGame::Port::In::PlayerActionCommand::QUIT:
    is_running_ = false;
    spdlog::info("Game quit by player.");
    break;
  case TuiRogGame::Port::In::PlayerActionCommand::UNKNOWN:
  default:
    spdlog::warn("Unknown player action received.");
    break;
  }

  processEvents(events);

  if (save_port_) {
    Port::Out::GameStateDTO game_state_to_save(*map_, *player_);
    save_port_->saveGameState(game_state_to_save);
    spdlog::info("Game auto-saved.");
  }
}

std::vector<std::unique_ptr<TuiRogGame::Domain::Event::DomainEvent>>
GameEngine::processPlayerMove(int dx, int dy) {
  spdlog::info("GameEngine: Entering processPlayerMove(dx={}, dy={}).", dx, dy);
  Model::Position current_pos = player_->getPosition();
  Model::Position new_pos = {current_pos.x + dx, current_pos.y + dy};

  if (!map_->isWalkable(new_pos.x, new_pos.y)) {
    spdlog::info("GameEngine: Player move blocked to ({}, {}). Wall detected.",
                 new_pos.x, new_pos.y);
    return {}; // Return no events, player does not move
  }

  spdlog::info(
      "GameEngine: Player current position ({}, {}), new position ({}, {}).",
      current_pos.x, current_pos.y, new_pos.x, new_pos.y);
  player_->moveTo(new_pos);
  spdlog::info("GameEngine: Player moved to new position ({}, {}).", new_pos.x,
               new_pos.y);

  std::vector<std::unique_ptr<Domain::Event::DomainEvent>> events;
  events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(new_pos));
  spdlog::info("GameEngine: PlayerMovedEvent created.");

  if (auto desc_event = createDescriptionEvent(
          Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
    events.push_back(std::move(desc_event));
  }

  if (auto enemy_opt = map_->getEnemyAt(new_pos)) {
    current_enemy_ = std::ref(enemy_opt->get()); // Store the enemy for combat
    spdlog::debug("GameEngine: Enemy encountered at ({}, {}): {}. Setting "
                  "current_enemy_.",
                  new_pos.x, new_pos.y, current_enemy_->get().getName());
    events.push_back(std::make_unique<Domain::Event::CombatStartedEvent>(
        current_enemy_->get().getTypeName(), current_enemy_->get().getName(),
        current_enemy_->get().getHealth(),
        current_enemy_->get().getStats().strength,
        current_enemy_->get().getStats().vitality));
    if (auto desc_event = createDescriptionEvent(
            Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
      events.push_back(std::move(desc_event));
    }
    spdlog::info(
        "GameEngine: CombatStartedEvent created with enemy {} at ({}, {}).",
        current_enemy_->get().getName(), new_pos.x, new_pos.y);
  } else if (auto item_opt = map_->getItemAt(new_pos)) {

    auto item_unique_ptr = map_->takeItemAt(new_pos);
    if (item_unique_ptr) {
      events.push_back(std::make_unique<Domain::Event::ItemFoundEvent>(
          static_cast<Domain::Event::ItemFoundEvent::ItemType>(
              item_unique_ptr->getType()),
          item_unique_ptr->getName(), ""));
      if (auto desc_event = createDescriptionEvent(
              Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
        events.push_back(std::move(desc_event));
      }
      player_->addItem(
          std::move(item_unique_ptr)); // Add item to player inventory
      spdlog::info("GameEngine: ItemFoundEvent created with item at ({}, {}).",
                   new_pos.x, new_pos.y);
    }
  } else if (map_->getTile(new_pos.x, new_pos.y) == Model::Tile::EXIT) {

    map_->generate();

    player_->moveTo(map_->getStartPlayerPosition());

    events.push_back(std::make_unique<Domain::Event::MapChangedEvent>());
    if (auto desc_event = createDescriptionEvent(
            Port::Out::GameStateDTO(*map_, *player_), *events.back())) {
      events.push_back(std::move(desc_event));
    }
    events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(
        player_->getPosition()));
    spdlog::info("GameEngine: Player reached exit. New map generated.");
  }

  return events;
}

void GameEngine::processEvents(
    const std::vector<std::unique_ptr<Domain::Event::DomainEvent>> &events) {
  spdlog::info("GameEngine: Entering processEvents. Event count: {}",
               events.size());

  if (render_port_) {

    Port::Out::GameStateDTO game_state_dto{*map_, *player_};
    render_port_->render(game_state_dto, events);
  }

  if (events.empty()) {
    spdlog::info("GameEngine: No new events to process.");
  } else {
    for (const auto &event : events) {
      spdlog::info("GameEngine: Processing event: {}", event->toString());
    }
  }

  spdlog::info("GameEngine: Exiting processEvents.");
}

void GameEngine::toggleDescriptionPort() {
  use_alternative_description_port_ = !use_alternative_description_port_;
  spdlog::info("Description port toggled. Using {} description port.",
               use_alternative_description_port_ ? "alternative" : "primary");
}

std::unique_ptr<TuiRogGame::Domain::Event::DescriptionGeneratedEvent>
GameEngine::createDescriptionEvent(const Port::Out::GameStateDTO &game_state,
                                   const Domain::Event::DomainEvent &event) {
  Port::Out::IGenerateDescriptionPort *active_description_port = nullptr;
  if (use_alternative_description_port_ && alternative_description_port_) {
    active_description_port = alternative_description_port_.get();
  } else if (primary_description_port_) {
    active_description_port = primary_description_port_.get();
  }

  if (active_description_port) {
    std::string generated_description =
        active_description_port->generateDescription(game_state, event);
    if (!generated_description.empty()) {
      return std::make_unique<Domain::Event::DescriptionGeneratedEvent>(
          generated_description);
    }
  }
  return nullptr;
}
} // namespace Service
} // namespace Domain
} // namespace TuiRogGame
