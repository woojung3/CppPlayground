#pragma once

#include "DomainEvent.h"
#include "Position.h"

namespace TuiRogGame {
namespace Domain {
namespace Event {

// Concrete event for player movement
class PlayerMovedEvent : public DomainEvent {
public:
    explicit PlayerMovedEvent(const TuiRogGame::Domain::Model::Position& new_position);

    Type getType() const override;
    std::string toString() const override;

    TuiRogGame::Domain::Model::Position getNewPosition() const;

private:
    TuiRogGame::Domain::Model::Position new_position_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
