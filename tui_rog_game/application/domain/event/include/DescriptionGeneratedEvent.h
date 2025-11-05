#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class DescriptionGeneratedEvent : public DomainEvent {
public:
    DescriptionGeneratedEvent(const std::string& description);


    std::string toString() const override;

    const std::string& getDescription() const { return description_; }

private:
    std::string description_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
