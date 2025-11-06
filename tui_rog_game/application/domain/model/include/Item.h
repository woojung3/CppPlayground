#pragma once

#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Item {
public:
  enum class ItemType { HealthPotion, StrengthScroll };

  Item(ItemType type, std::string name);

  ItemType getType() const { return type_; }
  const std::string &getName() const { return name_; }

private:
  ItemType type_;
  std::string name_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
