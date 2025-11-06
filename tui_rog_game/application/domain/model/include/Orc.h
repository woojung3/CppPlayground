#pragma once

#include "Enemy.h"
#include <memory>

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Orc : public Enemy {
public:
  Orc(Position position);
  Orc(const Orc &other) : Enemy(other) {}

  std::string getTypeName() const override;
  std::unique_ptr<Enemy> clone() const override {
    return std::make_unique<Orc>(*this);
  }
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
