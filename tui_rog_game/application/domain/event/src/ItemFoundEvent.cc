#include "ItemFoundEvent.h"
#include "Item.h"
#include <type_traits>

namespace TuiRogGame {
namespace Domain {
namespace Event {

static_assert(
    std::is_same_v<
        std::underlying_type_t<ItemFoundEvent::ItemType>,
        std::underlying_type_t<TuiRogGame::Domain::Model::Item::ItemType>>,
    "Underlying types of ItemFoundEvent::ItemType and "
    "Domain::Model::Item::ItemType mismatch");

static_assert(static_cast<int>(ItemFoundEvent::ItemType::HealthPotion) ==
                  static_cast<int>(
                      TuiRogGame::Domain::Model::Item::ItemType::HealthPotion),
              "ItemFoundEvent::ItemType::HealthPotion value mismatch");
static_assert(
    static_cast<int>(ItemFoundEvent::ItemType::StrengthScroll) ==
        static_cast<int>(
            TuiRogGame::Domain::Model::Item::ItemType::StrengthScroll),
    "ItemFoundEvent::ItemType::StrengthScroll value mismatch");

ItemFoundEvent::ItemFoundEvent(ItemType item_type, const std::string &item_name,
                               const std::string &item_description)
    : DomainEvent(Type::ItemFound), item_type_(item_type),
      item_name_(item_name), item_description_(item_description) {}

std::string ItemFoundEvent::toString() const {
  return "ItemFoundEvent: Found " + item_name_ + ". " + item_description_;
}

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
