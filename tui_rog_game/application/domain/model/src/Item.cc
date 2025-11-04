#include "Item.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

Item::Item(ItemType type, std::string name)
    : type_(type),
      name_(std::move(name)) {}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
