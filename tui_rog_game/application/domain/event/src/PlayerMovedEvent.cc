#include "PlayerMovedEvent.h"

namespace TuiRogGame {
namespace Domain {
namespace Event {

PlayerMovedEvent::PlayerMovedEvent(const TuiRogGame::Domain::Model::Position& new_position)
    : new_position_(new_position) {}

DomainEvent::Type PlayerMovedEvent::getType() const {
    return Type::PlayerMoved;
}

std::string PlayerMovedEvent::toString() const {
    return "PlayerMovedEvent: Player moved to (" + std::to_string(new_position_.x) + ", " + std::to_string(new_position_.y) + ")";
}

TuiRogGame::Domain::Model::Position PlayerMovedEvent::getNewPosition() const {
    return new_position_;
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
