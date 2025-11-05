#pragma once

#include <string>
#include <memory>

namespace TuiRogGame {
namespace Domain {
namespace Event {

// Base class for all domain events
class DomainEvent {
public:
    // Enum to identify the type of event
    enum class Type {
        PlayerMoved,
        ItemFound,
        CombatStarted,
        PlayerAttacked, // New event
        EnemyAttacked,  // New event
        EnemyDefeated,  // New event
        ItemUsed,       // New event
        PlayerLeveledUp,
        PlayerDied,
        DescriptionGenerated,
        GameSaved,
        GameLoaded,
        MapChanged,     // New event
        // Add other event types as needed
        Unknown // Default or error type
    };

    virtual ~DomainEvent() = default;
    Type getType() const { return type_; }
    virtual std::string toString() const = 0; // Pure virtual function for string representation (e.g., for logging/display)

protected:
    DomainEvent(Type type) : type_(type) {}

private:
    Type type_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame