#include "Enemy.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

Enemy::Enemy(std::string name, std::string type_name, Stats stats,
             Position position)
    : name_(std::move(name)), type_name_(std::move(type_name)),
      stats_(std::move(stats)), position_(std::move(position)) {}

std::string Enemy::getTypeName() const { return type_name_; }

int Enemy::getHealth() const { return stats_.health; }

int Enemy::getAttackPower() const {
  return stats_.strength;
}

void Enemy::takeDamage(int damage) {
  stats_.health -= damage;
  if (stats_.health < 0) {
    stats_.health = 0;
  }
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
