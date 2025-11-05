#include "DescriptionGeneratedEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

DescriptionGeneratedEvent::DescriptionGeneratedEvent(const std::string& description)
    : DomainEvent(Type::DescriptionGenerated), description_(description) {}

std::string DescriptionGeneratedEvent::toString() const {
    return description_;
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
