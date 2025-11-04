#pragma once

// Forward declarations to avoid including full headers in this DTO header
namespace TuiRogGame {
    namespace Domain {
        namespace Model {
            class Map;
            class Player;
        }
    }
}

namespace TuiRogGame {
namespace Port {
namespace Out {

// A Data Transfer Object (DTO) to carry game state to the rendering adapter.
struct GameStateDTO {
    const Domain::Model::Map& map;
    const Domain::Model::Player& player;
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame
