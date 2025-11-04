#include "Enemy.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

Enemy::Enemy(EnemyType type, int hp, Stats stats, Position position)
    : type_(type),
      hp_(hp),
      stats_(stats),
      position_(position) {}

void Enemy::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) {
        hp_ = 0;
    }
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
