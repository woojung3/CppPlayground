#pragma once

#include "Stats.h"
#include "Position.h"
#include <vector>
#include <string>

#include "Item.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Player {
public:
    using PlayerId = std::string;

    Player(PlayerId id, int level, int xp, Stats stats, Position position);

    // Getters
    const PlayerId& getId() const { return id_; }
    int getLevel() const { return level_; }
    std::string getName() const { return "Waldo"; }
    int getXp() const { return xp_; }
    int getHp() const { return hp_; }
    int getMaxHp() const { return max_hp_; }
    const Stats& getStats() const { return stats_; }
    const Position& getPosition() const { return position_; }
    const std::vector<Item>& getInventory() const { return inventory_; }

    // Actions
    void moveTo(Position new_position);
    bool gainXp(int amount); // Returns true if leveled up
    void takeDamage(int amount);
    void addItem(const Item& item);

private:
    PlayerId id_;
    int level_ = 1;
    int xp_ = 0;
    int hp_ = 100;
    int max_hp_ = 100;
    Stats stats_;
    Position position_;
    std::vector<Item> inventory_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
