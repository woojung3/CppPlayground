#pragma once

#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <functional> // For std::reference_wrapper

#include "Tile.h"
#include "Position.h"
#include "Enemy.h"
#include "Item.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Map {
public:
    Map(int width, int height);

    void generate();

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    Tile getTile(int x, int y) const;
    bool isWalkable(int x, int y) const;

    // --- Entity Management ---

    // Gets an optional, non-owning reference to the enemy at a position.
    std::optional<std::reference_wrapper<Enemy>> getEnemyAt(const Position& position);
    const std::optional<std::reference_wrapper<const Enemy>> getEnemyAt(const Position& position) const;

    // Removes and destroys the enemy at a position.
    void removeEnemyAt(const Position& position);

    // Gets an optional, non-owning reference to the item at a position.
    std::optional<std::reference_wrapper<Item>> getItemAt(const Position& position);
    const std::optional<std::reference_wrapper<const Item>> getItemAt(const Position& position) const;

    // Removes and transfers ownership of the item at a position.
    std::unique_ptr<Item> takeItemAt(const Position& position);

private:
    void addEnemy(Position position, std::unique_ptr<Enemy> enemy);
    void addItem(Position position, std::unique_ptr<Item> item);

    int width_;
    int height_;
    std::vector<std::vector<Tile>> tiles_;
    std::map<Position, std::unique_ptr<Enemy>> enemies_;
    std::map<Position, std::unique_ptr<Item>> items_;
};

// Position needs a less-than operator to be used as a key in std::map.
inline bool operator<(const Position& lhs, const Position& rhs) {
    if (lhs.y < rhs.y) return true;
    if (lhs.y > rhs.y) return false;
    return lhs.x < rhs.x;
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
