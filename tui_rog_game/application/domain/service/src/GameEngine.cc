#include "GameEngine.h"
#include "PlayerMovedEvent.h"
#include "CombatStartedEvent.h"
#include "ItemFoundEvent.h"
#include "PlayerAttackedEvent.h" // New
#include "EnemyAttackedEvent.h"  // New
#include "EnemyDefeatedEvent.h"  // New
#include "ItemUsedEvent.h"       // New
#include "PlayerLeveledUpEvent.h" // New
#include "DescriptionGeneratedEvent.h" // New
#include "MapChangedEvent.h"     // New
#include "GameLoadedEvent.h"     // New
#include "PlayerDiedEvent.h"     // New
#include <iostream> // For temporary output
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Domain {
namespace Service {

GameEngine::GameEngine(
    std::shared_ptr<TuiRogGame::Port::Out::ISaveGameStatePort> save_port,
    std::shared_ptr<TuiRogGame::Port::Out::ILoadGameStatePort> load_port,
    std::unique_ptr<TuiRogGame::Port::Out::IGenerateDescriptionPort> primary_description_port,
    std::unique_ptr<TuiRogGame::Port::Out::IGenerateDescriptionPort> alternative_description_port)
    : save_port_(std::move(save_port)),
      load_port_(std::move(load_port)),
      primary_description_port_(std::move(primary_description_port)),
      alternative_description_port_(std::move(alternative_description_port)) {
    spdlog::info("GameEngine initialized.");
}

void GameEngine::setRenderPort(TuiRogGame::Port::Out::IRenderPort* render_port) {
    render_port_ = render_port;
}

std::vector<std::unique_ptr<TuiRogGame::Domain::Event::DomainEvent>> GameEngine::initializeGame() {
    std::vector<std::unique_ptr<Domain::Event::DomainEvent>> events;

    if (load_port_) {
        auto loaded_game_state = load_port_->loadGameState();
        if (loaded_game_state) {
            // Game loaded successfully
            map_ = std::make_unique<Model::Map>(loaded_game_state->map);
            player_ = std::make_unique<Model::Player>(loaded_game_state->player);
            events.push_back(std::make_unique<Domain::Event::GameLoadedEvent>()); // Assuming GameLoadedEvent exists
            spdlog::info("Game loaded. Player: {} at ({}, {})", player_->getName(), player_->getPosition().x, player_->getPosition().y);
            events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(player_->getPosition())); // Notify UI of player position
            return events;
        }
    }

    // If no game loaded, create a new one
    map_ = std::make_unique<Model::Map>(20, 10); // Example size
    map_->generate();

    // Create player at a valid starting position
    player_ = std::make_unique<Model::Player>("player1", Model::Stats{}, Model::Position{1, 1});
    spdlog::info("New game initialized. Player: {} at ({}, {})", player_->getName(), player_->getPosition().x, player_->getPosition().y);

    // Create an initial event to inform the UI about the player's starting position.
    events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(player_->getPosition()));
    return events;
}

void GameEngine::handlePlayerAction(const TuiRogGame::Port::In::PlayerActionCommand& command) {
    spdlog::info("GameEngine: Handling player action type: {}", static_cast<int>(command.type));
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
            spdlog::info("Player initiated attack.");
            if (!current_enemy_) { // If not already in combat
                // Check for adjacent enemies
                Model::Position player_pos = player_->getPosition();
                std::vector<Model::Position> adjacent_positions = {
                    {player_pos.x, player_pos.y - 1}, // Up
                    {player_pos.x, player_pos.y + 1}, // Down
                    {player_pos.x - 1, player_pos.y}, // Left
                    {player_pos.x + 1, player_pos.y}  // Right
                };

                bool enemy_found = false;
                for (const auto& adj_pos : adjacent_positions) {
                    if (map_->isValidPosition(adj_pos.x, adj_pos.y)) {
                        if (auto enemy_opt = map_->getEnemyAt(adj_pos)) {
                            current_enemy_ = enemy_opt->get();
                            events.push_back(std::make_unique<Domain::Event::CombatStartedEvent>(
                                static_cast<Domain::Event::CombatStartedEvent::EnemyType>(current_enemy_->getType()),
                                current_enemy_->getName(),
                                current_enemy_->getHealth(),
                                current_enemy_->getStats().strength,
                                current_enemy_->getStats().vitality
                            ));
                            spdlog::info("Combat started with adjacent enemy {} at ({}, {}).", current_enemy_->getName(), adj_pos.x, adj_pos.y);
                            enemy_found = true;
                            break; // Found an enemy, start combat
                        }
                    }
                }

                if (!enemy_found) {
                    spdlog::warn("Player tried to attack but no adjacent enemy found.");
                    // Optionally, generate an event for "no enemy to attack"
                    break; // Exit ATTACK case
                }
            }

            // Player attacks enemy
            int player_damage = player_->getAttackPower();
            current_enemy_->takeDamage(player_damage);
            events.push_back(std::make_unique<Domain::Event::PlayerAttackedEvent>(
                player_damage, current_enemy_->getName(), current_enemy_->getHealth()));
            spdlog::info("Player attacked {} for {} damage. {}'s health: {}",
                         current_enemy_->getName(), player_damage, current_enemy_->getName(), current_enemy_->getHealth());

            if (current_enemy_->getHealth() <= 0) {
                // Enemy defeated
                int xp_gained = 50; // Example XP
                bool leveled_up = player_->gainXp(xp_gained);
                events.push_back(std::make_unique<Domain::Event::EnemyDefeatedEvent>(
                    current_enemy_->getName(), xp_gained));
                spdlog::info("{} defeated! Player gained {} XP.", current_enemy_->getName(), xp_gained);

                if (leveled_up) {
                    events.push_back(std::make_unique<Domain::Event::PlayerLeveledUpEvent>(
                        player_->getLevel(), player_->getStats()));
                    spdlog::info("Player leveled up to level {}!", player_->getLevel());
                }

                // Remove enemy from map
                map_->removeEnemyAt(current_enemy_->getPosition());
                current_enemy_.reset(); // Clear current enemy
            } else {
                // Enemy attacks player
                int enemy_damage = current_enemy_->getAttackPower();
                player_->takeDamage(enemy_damage);
                events.push_back(std::make_unique<Domain::Event::EnemyAttackedEvent>(
                    current_enemy_->getName(), enemy_damage, player_->getHp()));
                spdlog::info("{} attacked player for {} damage. Player's health: {}",
                             current_enemy_->getName(), enemy_damage, player_->getHp());

                if (player_->getHp() <= 0) {
                    // Player defeated
                    events.push_back(std::make_unique<Domain::Event::PlayerDiedEvent>()); // Assuming PlayerDiedEvent exists
                    spdlog::info("Player defeated!");
                    // TODO: Handle game over (e.g., reset player position, health)
                }
            }
            break;
        }
        case TuiRogGame::Port::In::PlayerActionCommand::INTERACT:
            // TODO: Implement interact logic using command.payload
            spdlog::info("Player interacted.");
            break;
        case TuiRogGame::Port::In::PlayerActionCommand::USE_ITEM: {
            spdlog::info("Player initiated use item.");
            if (std::holds_alternative<std::string>(command.payload)) {
                const std::string& item_name = std::get<std::string>(command.payload);
                if (player_->useItem(item_name)) {
                    events.push_back(std::make_unique<Domain::Event::ItemUsedEvent>(item_name));
                    spdlog::info("Player used item: {}", item_name);
                } else {
                    spdlog::warn("Player tried to use item '{}' but it was not found or could not be used.", item_name);
                    // Optionally, generate an event for "item not found/cannot be used"
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

    // Auto-save after every player action
    if (save_port_) {
        Port::Out::GameStateDTO game_state_to_save(*map_, *player_);
        save_port_->saveGameState(game_state_to_save);
        spdlog::info("Game auto-saved.");
    }
}

std::vector<std::unique_ptr<TuiRogGame::Domain::Event::DomainEvent>> GameEngine::processPlayerMove(int dx, int dy) {
    spdlog::info("GameEngine: Entering processPlayerMove(dx={}, dy={}).", dx, dy);
    Model::Position current_pos = player_->getPosition();
    Model::Position new_pos = {current_pos.x + dx, current_pos.y + dy};

    // Wall collision detection
    if (!map_->isWalkable(new_pos.x, new_pos.y)) {
        spdlog::info("GameEngine: Player move blocked to ({}, {}). Wall detected.", new_pos.x, new_pos.y);
        return {}; // Return no events, player does not move
    }

    spdlog::info("GameEngine: Player current position ({}, {}), new position ({}, {}).", current_pos.x, current_pos.y, new_pos.x, new_pos.y);
    player_->moveTo(new_pos);
    spdlog::info("GameEngine: Player moved to new position ({}, {}).", new_pos.x, new_pos.y);

    // Create and return PlayerMovedEvent
    std::vector<std::unique_ptr<Domain::Event::DomainEvent>> events;
    events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(new_pos));
    spdlog::info("GameEngine: PlayerMovedEvent created.");

    // Generate description for the new position
    Port::Out::IGenerateDescriptionPort* active_description_port = nullptr;
    if (use_alternative_description_port_ && alternative_description_port_) {
        active_description_port = alternative_description_port_.get();
    } else if (primary_description_port_) {
        active_description_port = primary_description_port_.get();
    }

    if (active_description_port) {
        Port::Out::GameStateDTO current_game_state(*map_, *player_);
        std::string generated_description = active_description_port->generateDescription(current_game_state);
        events.push_back(std::make_unique<Domain::Event::DescriptionGeneratedEvent>(generated_description));
        spdlog::info("GameEngine: DescriptionGeneratedEvent created.");
    }

    // Check for enemy encounter
    if (auto enemy_opt = map_->getEnemyAt(new_pos)) {
        current_enemy_ = enemy_opt->get(); // Store the enemy for combat
        events.push_back(std::make_unique<Domain::Event::CombatStartedEvent>(
            static_cast<Domain::Event::CombatStartedEvent::EnemyType>(current_enemy_->getType()),
            current_enemy_->getName(),
            current_enemy_->getHealth(),
            current_enemy_->getStats().strength,
            current_enemy_->getStats().vitality
        ));
        spdlog::info("GameEngine: CombatStartedEvent created with enemy {} at ({}, {}).", current_enemy_->getName(), new_pos.x, new_pos.y);
    } else if (auto item_opt = map_->getItemAt(new_pos)) {
        // Check for item found (only if no enemy)
        auto item_unique_ptr = map_->takeItemAt(new_pos);
        if (item_unique_ptr) {
            events.push_back(std::make_unique<Domain::Event::ItemFoundEvent>(
                static_cast<Domain::Event::ItemFoundEvent::ItemType>(item_unique_ptr->getType()),
                item_unique_ptr->getName(),
                "" // Item description is not directly available in Item.h, assuming it's part of type or will be added
            ));
            player_->addItem(std::move(item_unique_ptr)); // Add item to player inventory
            spdlog::info("GameEngine: ItemFoundEvent created with item at ({}, {}).", new_pos.x, new_pos.y);
        }
    } else if (map_->getTile(new_pos.x, new_pos.y) == Model::Tile::EXIT) {
        // Player reached the exit, generate a new map
        map_->generate();
        // Reset player position to a new valid starting point on the new map
        player_->moveTo(map_->getStartPlayerPosition()); // Assuming Map has a getStartPlayerPosition()
        events.push_back(std::make_unique<Domain::Event::MapChangedEvent>());
        events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(player_->getPosition()));
        spdlog::info("GameEngine: Player reached exit. New map generated.");
    }

    return events;
}

void GameEngine::processEvents(const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>& events) {
    spdlog::info("GameEngine: Entering processEvents. Event count: {}", events.size());

    // Always render the current state, even if there are no new events.
    if (render_port_) {
        // Create the DTO with the current state
        Port::Out::GameStateDTO game_state_dto{*map_, *player_};
        render_port_->render(game_state_dto, events);
    }

    if (events.empty()) {
        spdlog::info("GameEngine: No new events to process.");
    } else {
        for (const auto& event : events) {
            spdlog::info("GameEngine: Processing event: {}", event->toString());
            // Here you could have logic that reacts to events, e.g., saving the game.
        }
    }

    spdlog::info("GameEngine: Exiting processEvents.");
}

void GameEngine::toggleDescriptionPort() {
    use_alternative_description_port_ = !use_alternative_description_port_;
    spdlog::info("Description port toggled. Using {} description port.",
                 use_alternative_description_port_ ? "alternative" : "primary");
}

} // namespace Service
} // namespace Domain
} // namespace TuiRogGame
