#include "PlayerLeveledUpEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

PlayerLeveledUpEvent::PlayerLeveledUpEvent(int new_level, const Model::Stats& new_stats)
    : DomainEvent(Type::PlayerLeveledUp),
      new_level_(new_level),
      new_stats_(new_stats) {}

std::string PlayerLeveledUpEvent::toString() const {
    return "Player leveled up to level " + std::to_string(new_level_) + "! Stats: Str " + std::to_string(new_stats_.strength) + ", Dex " + std::to_string(new_stats_.dexterity) + ", Int " + std::to_string(new_stats_.intelligence) + ", Vit " + std::to_string(new_stats_.vitality) + ".";
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
