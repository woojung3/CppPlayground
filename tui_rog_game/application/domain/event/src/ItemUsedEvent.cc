#include "ItemUsedEvent.h"
#include <string> // Use string for concatenation
// #include <format> // Removed

namespace TuiRogGame {
namespace Domain {
namespace Event {

ItemUsedEvent::ItemUsedEvent(const std::string& item_name)
    : DomainEvent(Type::ItemUsed), item_name_(item_name) {}

std::string ItemUsedEvent::toString() const {
    return "You used a " + item_name_ + ".";
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
