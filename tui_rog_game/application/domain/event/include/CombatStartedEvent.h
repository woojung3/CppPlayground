#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

// Concrete event for combat started
class CombatStartedEvent : public DomainEvent {
public:
    explicit CombatStartedEvent(
        const std::string& enemy_type_name,
        const std::string& enemy_name,
        int enemy_hp,
        int enemy_attack,
        int enemy_defense
    );


    std::string toString() const override;

    const std::string& getEnemyTypeName() const { return enemy_type_name_; }
    const std::string& getEnemyName() const { return enemy_name_; }
    int getEnemyHp() const { return enemy_hp_; }
    int getEnemyAttack() const { return enemy_attack_; }
    int getEnemyDefense() const { return enemy_defense_; }

private:
    std::string enemy_type_name_;
    std::string enemy_name_;
    int enemy_hp_;
    int enemy_attack_;
    int enemy_defense_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
