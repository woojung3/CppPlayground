#pragma once

#include "PlayerCoreStats.h"
#include "Position.h"
#include "Stats.h"
#include <memory>
#include <string>
#include <vector>

#include "Item.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Player {
public:
  using PlayerId = std::string;

  Player(PlayerId id, Stats stats, Position position);

  Player(PlayerId id, PlayerCoreStats core_stats, Stats stats,
         Position position, std::vector<std::unique_ptr<Item>> inventory);

  const PlayerId &getId() const { return id_; }
  int getLevel() const { return level_; }
  std::string getName() const { return "Waldo"; }
  int getXp() const { return xp_; }
  int getHp() const { return hp_; }
  int getMaxHp() const;
  int getAttackPower() const;
  const Stats &getStats() const { return stats_; }
  const Position &getPosition() const { return position_; }
  const std::vector<std::unique_ptr<Item>> &getInventory() const {
    return inventory_;
  }

  void setLevel(int level) { level_ = level; }
  void setXp(int xp) { xp_ = xp; }
  void setHp(int hp) { hp_ = hp; }
  void setPosition(Position pos) { position_ = pos; }

  void moveTo(Position new_position);
  bool gainXp(int amount);
  void takeDamage(int amount);
  void addItem(std::unique_ptr<Item> item);
  bool useItem(const std::string &item_name);

  Player(const Player &other);

private:
  PlayerId id_;
  int level_ = 1;
  int xp_ = 0;
  int hp_;
  Stats stats_;
  Position position_;
  std::vector<std::unique_ptr<Item>> inventory_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
