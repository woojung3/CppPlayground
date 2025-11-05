#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

class ItemUsedEvent : public DomainEvent {
public:
    ItemUsedEvent(const std::string& item_name);


    std::string toString() const override;

    const std::string& getItemName() const { return item_name_; }

private:
    std::string item_name_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
