#pragma once

#include "Stats.h"
#include "Position.h"
#include "PlayerCoreStats.h" // New include for PlayerCoreStats
#include <vector>
#include <string>
#include <memory> // std::unique_ptr를 위해 추가

#include "Item.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Player {
public:
    using PlayerId = std::string;

    // Existing constructor
    Player(PlayerId id, Stats stats, Position position);

    // New constructor for persistence
    Player(PlayerId id, PlayerCoreStats core_stats, Stats stats, Position position, std::vector<std::unique_ptr<Item>> inventory);

    // Getters
    const PlayerId& getId() const { return id_; }
    int getLevel() const { return level_; }
    std::string getName() const { return "Waldo"; } // Placeholder for now
    int getXp() const { return xp_; }
    int getHp() const { return hp_; }
    int getMaxHp() const; // Calculated from stats_.vitality
    int getAttackPower() const; // Calculated from stats_.strength
    const Stats& getStats() const { return stats_; }
    const Position& getPosition() const { return position_; }
    const std::vector<std::unique_ptr<Item>>& getInventory() const { return inventory_; }

    // Setters for persistence (to be used by PlayerRepository)
    void setLevel(int level) { level_ = level; }
    void setXp(int xp) { xp_ = xp; }
    void setHp(int hp) { hp_ = hp; }
    void setPosition(Position pos) { position_ = pos; } // Also useful for loading

    // Actions
    void moveTo(Position new_position);
    bool gainXp(int amount); // Returns true if leveled up
    void takeDamage(int amount);
    void addItem(std::unique_ptr<Item> item);
    bool useItem(const std::string& item_name); // New method

    Player(const Player& other); // 복사 생성자 선언

private:
    PlayerId id_;
    int level_ = 1;
    int xp_ = 0;
    int hp_; // Current health
    Stats stats_;
    Position position_;
    std::vector<std::unique_ptr<Item>> inventory_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
