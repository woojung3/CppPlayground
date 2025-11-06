#pragma once

#include "Position.h"
#include "Stats.h"
#include <memory> // Required for std::unique_ptr
#include <string>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {
class EnemyRepository; // Forward declaration
} // namespace Persistence
} // namespace Out
} // namespace Adapter

namespace Domain {
namespace Model {

class Enemy {
  friend class Adapter::Out::Persistence::
      EnemyRepository; // Grant EnemyRepository access to protected members
public:
  Enemy(std::string name, std::string type_name, Stats stats,
        Position position);
  virtual ~Enemy() =
      default; // Virtual destructor for proper cleanup of derived classes

  virtual std::unique_ptr<Enemy> clone() const = 0;

  // Getters
  const std::string &getName() const { return name_; }
  virtual std::string getTypeName() const; // Made virtual
  const Position &getPosition() const { return position_; }
  int getHealth() const; // Declared, defined in .cc
  const Stats &getStats() const { return stats_; }
  int getAttackPower() const; // Declared, defined in .cc

  // Setters
  void setPosition(const Position &pos) { position_ = pos; }
  void takeDamage(int damage); // Declared, defined in .cc

protected:
  std::string name_;
  std::string type_name_;
  Position position_;
  Stats stats_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
