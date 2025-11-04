#include "GameEngine.h"
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

void GameEngine::run() {
    spdlog::info("GameEngine::run() called for initialization.");
    initializeGame();
    is_running_ = true; // Set to true for game state, but no internal loop.
    spdlog::info("GameEngine::run() finished initialization.");
}

void GameEngine::initializeGame() {
    // Create player, map, enemies etc.
    player_ = std::make_unique<Model::Player>("player1", 1, 0, Model::Stats{}, Model::Position{5, 5});
    spdlog::info("Game Initialized. Player: {}", player_->getName());
}

void GameEngine::handlePlayerAction(const TuiRogGame::Port::In::PlayerActionCommand& command) {
    spdlog::info("GameEngine: Handling player action type: {}", static_cast<int>(command.type));
    std::vector<std::unique_ptr<Common::DomainEvent>> events;

    switch (command.type) {
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

std::vector<std::unique_ptr<TuiRogGame::Common::DomainEvent>> GameEngine::processPlayerMove(int dx, int dy) {
    spdlog::info("GameEngine: Entering processPlayerMove(dx={}, dy={}).", dx, dy);
    Model::Position current_pos = player_->getPosition();
    Model::Position new_pos = {current_pos.x + dx, current_pos.y + dy};
    spdlog::info("GameEngine: Player current position ({}, {}), new position ({}, {}).", current_pos.x, current_pos.y, new_pos.x, new_pos.y);
    player_->moveTo(new_pos);
    spdlog::info("GameEngine: Player moved to new position ({}, {}).", new_pos.x, new_pos.y);

    // In the future, create and return PlayerMovedEvent
    spdlog::info("GameEngine: Exiting processPlayerMove.");
    return {};
}

void GameEngine::processEvents(const std::vector<std::unique_ptr<Common::DomainEvent>>& events) {
    spdlog::info("GameEngine: Entering processEvents. Event count: {}", events.size());
    if (events.empty()) {
        spdlog::info("GameEngine: No events to process.");
        return;
    }
    for (const auto& event : events) {
        spdlog::info("GameEngine: Processing event: {}", event->toString());
    }
    spdlog::info("GameEngine: Exiting processEvents.");
}

} // namespace Service
} // namespace Domain
} // namespace TuiRogGame
