#include "GameEngine.h"
#include "PlayerMovedEvent.h"
#include <iostream> // For temporary output
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Domain {
namespace Service {

GameEngine::GameEngine(
    std::unique_ptr<TuiRogGame::Port::Out::IPersistencePort> persistence_port,
    std::unique_ptr<TuiRogGame::Port::Out::IGenerateDescriptionPort> description_port)
    : persistence_port_(std::move(persistence_port)),
      description_port_(std::move(description_port)) {
    spdlog::info("GameEngine initialized.");
}

void GameEngine::setRenderPort(TuiRogGame::Port::Out::IRenderPort* render_port) {
    render_port_ = render_port;
}

std::vector<std::unique_ptr<TuiRogGame::Domain::Event::DomainEvent>> GameEngine::initializeGame() {
    // Create and generate the map
    map_ = std::make_unique<Model::Map>(20, 10); // Example size
    map_->generate();

    // Create player at a valid starting position
    player_ = std::make_unique<Model::Player>("player1", 1, 0, Model::Stats{}, Model::Position{1, 1});
    spdlog::info("Game Initialized. Player: {} at ({}, {})", player_->getName(), player_->getPosition().x, player_->getPosition().y);

    // Create an initial event to inform the UI about the player's starting position.
    std::vector<std::unique_ptr<Domain::Event::DomainEvent>> events;
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
        case TuiRogGame::Port::In::PlayerActionCommand::ATTACK:
            // TODO: Implement attack logic using command.payload
            spdlog::info("Player initiated attack.");
            break;
        case TuiRogGame::Port::In::PlayerActionCommand::INTERACT:
            // TODO: Implement interact logic using command.payload
            spdlog::info("Player interacted.");
            break;
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
}

std::vector<std::unique_ptr<TuiRogGame::Domain::Event::DomainEvent>> GameEngine::processPlayerMove(int dx, int dy) {
    spdlog::info("GameEngine: Entering processPlayerMove(dx={}, dy={}).", dx, dy);
    Model::Position current_pos = player_->getPosition();
    Model::Position new_pos = {current_pos.x + dx, current_pos.y + dy};
    spdlog::info("GameEngine: Player current position ({}, {}), new position ({}, {}).", current_pos.x, current_pos.y, new_pos.x, new_pos.y);
    player_->moveTo(new_pos);
    spdlog::info("GameEngine: Player moved to new position ({}, {}).", new_pos.x, new_pos.y);

    // Create and return PlayerMovedEvent
    std::vector<std::unique_ptr<Domain::Event::DomainEvent>> events;
    events.push_back(std::make_unique<Domain::Event::PlayerMovedEvent>(new_pos));
    spdlog::info("GameEngine: PlayerMovedEvent created.");
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

} // namespace Service
} // namespace Domain
} // namespace TuiRogGame
