#pragma once

#include "Player.h"
#include "DomainEvent.h"
#include "IGetPlayerActionUseCase.h"
#include "IRenderPort.h"
#include "ISaveGameStatePort.h"
#include "ILoadGameStatePort.h"
#include "IGenerateDescriptionPort.h"

#include <vector>
#include <memory>
#include <optional> // For std::optional
#include <functional> // For std::reference_wrapper
#include "Map.h"

namespace TuiRogGame {
namespace Domain {
namespace Service {

class GameEngine : public Port::In::IGetPlayerActionUseCase {
public:
    GameEngine(
        std::shared_ptr<Port::Out::ISaveGameStatePort> save_port,
        std::shared_ptr<Port::Out::ILoadGameStatePort> load_port,
        std::unique_ptr<Port::Out::IGenerateDescriptionPort> primary_description_port,
        std::unique_ptr<Port::Out::IGenerateDescriptionPort> alternative_description_port
    );

    void setRenderPort(Port::Out::IRenderPort* render_port);
    void handlePlayerAction(const Port::In::PlayerActionCommand& command) override;

private:
    // Initializes the game state (player, map, etc.) and returns initial events.
    std::vector<std::unique_ptr<Domain::Event::DomainEvent>> initializeGame();
    std::vector<std::unique_ptr<Domain::Event::DomainEvent>> processPlayerMove(int dx, int dy);
    void processEvents(const std::vector<std::unique_ptr<Domain::Event::DomainEvent>>& events);

    bool is_running_ = false;
    std::shared_ptr<Port::Out::ISaveGameStatePort> save_port_;
    std::shared_ptr<Port::Out::ILoadGameStatePort> load_port_;
    std::unique_ptr<Port::Out::IGenerateDescriptionPort> primary_description_port_; // Renamed
    std::unique_ptr<Port::Out::IGenerateDescriptionPort> alternative_description_port_; // New
    bool use_alternative_description_port_ = false; // New flag
    Port::Out::IRenderPort* render_port_ = nullptr;

    std::unique_ptr<Model::Player> player_;
    std::unique_ptr<Model::Map> map_;
    std::optional<std::reference_wrapper<Model::Enemy>> current_enemy_; // To track the enemy in combat

public: // Added public method for toggling
    void toggleDescriptionPort();
};

} // namespace Service
} // namespace Domain
} // namespace TuiRogGame
