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

    Enemy(EnemyType type, int hp, Stats stats, Position position);

    // Getters
    EnemyType getType() const { return type_; }
    int getHp() const { return hp_; }
    const Stats& getStats() const { return stats_; }
    const Position& getPosition() const { return position_; }

    void takeDamage(int amount);

private:
    EnemyType type_;
    int hp_ = 50;
    Stats stats_;
    Position position_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
