#pragma once

#include "DomainEvent.h"
#include <string>

namespace TuiRogGame {
namespace Domain {
namespace Event {

// Concrete event for combat started
class CombatStartedEvent : public DomainEvent {
public:
    // Redefine EnemyType for the event to avoid direct dependency on model/Enemy.h
    enum class EnemyType {
        Orc,
        Goblin
    };

    explicit CombatStartedEvent(
        EnemyType enemy_type,
        const std::string& enemy_name,
        int enemy_hp,
        int enemy_attack,
        int enemy_defense
    );

    Type getType() const override;
    std::string toString() const override;

    EnemyType getEnemyType() const { return enemy_type_; }
    const std::string& getEnemyName() const { return enemy_name_; }
    int getEnemyHp() const { return enemy_hp_; }
    int getEnemyAttack() const { return enemy_attack_; }
    int getEnemyDefense() const { return enemy_defense_; }

private:
    EnemyType enemy_type_;
    std::string enemy_name_;
    int enemy_hp_;
    int enemy_attack_;
    int enemy_defense_;
};

} // namespace Event
} // namespace Domain
} // namespace TuiRogGame
