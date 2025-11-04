#include "Player.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

namespace {
    // Helper function to calculate max HP from stats
    int calculateMaxHp(const Stats& stats) {
        return stats.vitality * 10;
    }
} // namespace

Player::Player(PlayerId id, int level, int xp, Stats stats, Position position)
    : id_(std::move(id)),
      level_(level),
      xp_(xp),
      stats_(stats),
      position_(position) 
{
    max_hp_ = calculateMaxHp(stats_);
    hp_ = max_hp_; // Start with full health
}

void Player::moveTo(Position new_position) {
    position_ = new_position;
}

bool Player::gainXp(int amount) {
    xp_ += amount;
    
    bool leveled_up = false;
    // Level up condition
    while (xp_ >= 100 * level_) {
        xp_ -= 100 * level_;
        level_++;
        
        // Increase stats
        stats_.strength++;
        stats_.dexterity++;
        stats_.intelligence++;
        stats_.vitality++;

        // Recalculate max_hp and heal to full
        max_hp_ = calculateMaxHp(stats_);
        hp_ = max_hp_;

        leveled_up = true;
    }
    return leveled_up;
}

void Player::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) {
        hp_ = 0;
    }
}

void Player::addItem(const Item& item) {
    inventory_.push_back(item);
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
