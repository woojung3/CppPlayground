#pragma once

#include "Player.h"
#include "DomainEvent.h"
#include "IGetPlayerActionUseCase.h"
#include "IRenderPort.h"
#include "IPersistencePort.h"
#include "IGenerateDescriptionPort.h"

#include <vector>
#include <memory>

namespace TuiRogGame {
namespace Domain {
namespace Service {

class GameEngine : public TuiRogGame::Port::In::IGetPlayerActionUseCase {
public:
    // Constructor with dependency injection for the outbound ports
    GameEngine(
        std::unique_ptr<TuiRogGame::Port::Out::IPersistencePort> persistence_port,
        std::unique_ptr<TuiRogGame::Port::Out::IGenerateDescriptionPort> description_port
    );

    // Setter for the render port to break circular dependency
    void setRenderPort(TuiRogGame::Port::Out::IRenderPort* render_port);

    // Main game loop entry point
    void run();

    // Implementation of the inbound port
    void handlePlayerAction(const TuiRogGame::Port::In::PlayerActionCommand& command) override;

private:
    void initializeGame();
    std::vector<std::unique_ptr<TuiRogGame::Common::DomainEvent>> processPlayerMove(int dx, int dy);
    void processEvents(const std::vector<std::unique_ptr<TuiRogGame::Common::DomainEvent>>& events);

    TuiRogGame::Port::Out::IRenderPort* render_port_ = nullptr;
    std::unique_ptr<TuiRogGame::Port::Out::IPersistencePort> persistence_port_;
    std::unique_ptr<TuiRogGame::Port::Out::IGenerateDescriptionPort> description_port_;

    std::unique_ptr<TuiRogGame::Domain::Model::Player> player_;
    // std::vector<Enemy> enemies_;
    // Map map_;
    bool is_running_ = false;
};

} // namespace Service
} // namespace Domain
} // namespace TuiRogGame
