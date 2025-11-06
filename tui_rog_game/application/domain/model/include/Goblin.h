#pragma once

#include "Enemy.h"
#include <memory>

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Goblin : public Enemy {
public:
  Goblin(Position position);
  Goblin(const Goblin &other) : Enemy(other) {}

  std::string getTypeName() const override;
  std::unique_ptr<Enemy> clone() const override {
    return std::make_unique<Goblin>(*this);
  }
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
