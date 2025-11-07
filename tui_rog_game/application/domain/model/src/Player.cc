#include "Player.h"
#include <memory>

namespace TuiRogGame {
namespace Domain {
namespace Model {

namespace {

int calculateMaxHp(const Stats &stats) { return stats.vitality * 10; }
} // namespace

Player::Player(PlayerId id, Stats stats, Position position)
    : id_(std::move(id)), stats_(stats), position_(position) {
  hp_ = getMaxHp();
}

Player::Player(PlayerId id, PlayerCoreStats core_stats, Stats stats,
               Position position, std::vector<std::unique_ptr<Item>> inventory)
    : id_(std::move(id)), level_(core_stats.level), xp_(core_stats.xp),
      hp_(core_stats.hp), stats_(stats), position_(position),
      inventory_(std::move(inventory)) {}

Player::Player(const Player &other)
    : id_(other.id_), level_(other.level_), xp_(other.xp_), hp_(other.hp_),
      stats_(other.stats_), position_(other.position_) {
  for (const auto &item_ptr : other.inventory_) {
    inventory_.push_back(std::make_unique<Item>(*item_ptr));
  }
}

int Player::getMaxHp() const { return calculateMaxHp(stats_); }

int Player::getAttackPower() const { return 5 + (stats_.strength * 2); }

void Player::moveTo(Position new_position) { position_ = new_position; }

bool Player::gainXp(int amount) {
  xp_ += amount;

  bool leveled_up = false;

  while (xp_ >= 100 * level_) {
    xp_ -= 100 * level_;
    level_++;

    stats_.strength++;
    stats_.dexterity++;
    stats_.intelligence++;
    stats_.vitality++;

    hp_ = getMaxHp();

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

void Player::addItem(std::unique_ptr<Item> item) {
  inventory_.push_back(std::move(item));
}

bool Player::useItem(const std::string &item_name) {
  for (auto it = inventory_.begin(); it != inventory_.end(); ++it) {
    if ((*it)->getName() == item_name) {

      if ((*it)->getType() == Item::ItemType::HealthPotion) {
        hp_ = std::min(hp_ + 20, getMaxHp());
      }

      inventory_.erase(it);
      return true;
    }
  }
  return false;
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
