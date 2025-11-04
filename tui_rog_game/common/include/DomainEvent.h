#pragma once

#include <string>
#include <memory>

namespace TuiRogGame {
namespace Common {

// Base class for all domain events
class DomainEvent {
public:
    // Enum to identify the type of event
    enum class Type {
        PlayerMoved,
        ItemFound,
        CombatStarted,
        PlayerLeveledUp,
        PlayerDied,
        DescriptionGenerated,
        GameSaved,
        GameLoaded,
        // Add other event types as needed
        Unknown // Default or error type
    };

    virtual ~DomainEvent() = default;
    virtual Type getType() const = 0; // Pure virtual function to get the event type
    virtual std::string toString() const = 0; // Pure virtual function for string representation (e.g., for logging/display)
};

} // namespace Common
} // namespace TuiRogGame
