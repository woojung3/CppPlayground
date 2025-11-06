#pragma once

#include "DomainEvent.h"
#include <memory>
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

// Concrete event for item found
class ItemFoundEvent : public DomainEvent {
public:
  // Redefine ItemType for the event to avoid direct dependency on model/Item.h
  enum class ItemType { HealthPotion, StrengthScroll };

  explicit ItemFoundEvent(ItemType item_type, const std::string &item_name,
                          const std::string &item_description);

  std::string toString() const override;

  ItemType getItemType() const { return item_type_; }
  const std::string &getItemName() const { return item_name_; }
  const std::string &getItemDescription() const { return item_description_; }

private:
  ItemType item_type_;
  std::string item_name_;
  std::string item_description_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
