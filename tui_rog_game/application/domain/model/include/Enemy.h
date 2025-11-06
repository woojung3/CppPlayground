#pragma once

#include "Position.h"
#include "Stats.h"
#include <memory>
#include <string>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {
class EnemyRepository;
} // namespace Persistence
} // namespace Out
} // namespace Adapter

namespace Domain {
namespace Model {

class Enemy {
  friend class Adapter::Out::Persistence::EnemyRepository;
public:
  Enemy(std::string name, std::string type_name, Stats stats,
        Position position);
  virtual ~Enemy() = default;

  virtual std::unique_ptr<Enemy> clone() const = 0;

  const std::string &getName() const { return name_; }
  virtual std::string getTypeName() const;
  const Position &getPosition() const { return position_; }
  int getHealth() const;
  const Stats &getStats() const { return stats_; }
  int getAttackPower() const;

  void setPosition(const Position &pos) { position_ = pos; }
  void takeDamage(int damage);

protected:
  std::string name_;
  std::string type_name_;
  Position position_;
  Stats stats_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
