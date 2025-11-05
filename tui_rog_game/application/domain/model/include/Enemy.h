#pragma once

#include "Stats.h"
#include "Position.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Enemy {
public:
    enum class EnemyType {
        Orc,
        Goblin
    };

    Enemy(EnemyType type, std::string name, Stats stats, Position position);

    // Getters
    EnemyType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    int getHealth() const { return health_; }
    int getMaxHealth() const; // Calculated from stats_.vitality
    int getAttackPower() const; // Calculated from stats_.strength
    const Stats& getStats() const { return stats_; }
    const Position& getPosition() const { return position_; }

    void takeDamage(int amount);

private:
    EnemyType type_;
    std::string name_;
    int health_; // Current health
    Stats stats_;
    Position position_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
