#include "Enemy.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

Enemy::Enemy(EnemyType type, std::string name, Stats stats, Position position)
    : type_(type),
      name_(name),
      stats_(stats),
      position_(position) {
    health_ = getMaxHealth(); // Initialize current health to max health
}

int Enemy::getMaxHealth() const {
    // Example: 10 base health + 5 health per vitality point
    return 10 + (stats_.vitality * 5);
}

int Enemy::getAttackPower() const {
    // Example: 5 base attack + 2 attack per strength point
    return 5 + (stats_.strength * 2);
}

void Enemy::takeDamage(int amount) {
    health_ -= amount;
    if (health_ < 0) {
        health_ = 0;
    }
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
