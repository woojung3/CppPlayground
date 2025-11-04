#include "Map.h"
#include <stdexcept>

namespace TuiRogGame {
namespace Domain {
namespace Model {

Map::Map(int width, int height)
    : width_(width), height_(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Map dimensions must be positive.");
    }
    tiles_.resize(height, std::vector<Tile>(width, Tile::WALL));
}

// A simple procedural generation algorithm
void Map::generate() {
    // For now, let's just create a simple room
    for (int y = 1; y < height_ - 1; ++y) {
        for (int x = 1; x < width_ - 1; ++x) {
            tiles_[y][x] = Tile::FLOOR;
        }
    }

    // Place some enemies and items for testing
    if (width_ > 5 && height_ > 5) {
        Position orc_pos = {3, 3};
        auto orc = std::make_unique<Enemy>(Enemy::EnemyType::Orc, 50, Stats{10, 2, 1}, orc_pos);
        addEnemy(orc_pos, std::move(orc));

        Position potion_pos = {5, 5};
        auto potion = std::make_unique<Item>(Item::ItemType::HealthPotion, "Health Potion");
        addItem(potion_pos, std::move(potion));
    }
}

Tile Map::getTile(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return Tile::WALL; // Treat out-of-bounds as walls
    }
    return tiles_[y][x];
}

bool Map::isWalkable(int x, int y) const {
    Tile tile = getTile(x, y);
    return tile == Tile::FLOOR || tile == Tile::EXIT;
}

// --- Add/Get/Remove Entities ---

void Map::addEnemy(Position position, std::unique_ptr<Enemy> enemy) {
    if (getTile(position.x, position.y) == Tile::FLOOR) {
        enemies_[position] = std::move(enemy);
        tiles_[position.y][position.x] = Tile::ENEMY;
    }
}

void Map::addItem(Position position, std::unique_ptr<Item> item) {
    if (getTile(position.x, position.y) == Tile::FLOOR) {
        items_[position] = std::move(item);
        tiles_[position.y][position.x] = Tile::ITEM;
    }
}

std::optional<std::reference_wrapper<Enemy>> Map::getEnemyAt(const Position& position) {
    auto it = enemies_.find(position);
    if (it != enemies_.end()) {
        return std::ref(*it->second);
    }
    return std::nullopt;
}

const std::optional<std::reference_wrapper<const Enemy>> Map::getEnemyAt(const Position& position) const {
    auto it = enemies_.find(position);
    if (it != enemies_.end()) {
        return std::cref(*it->second);
    }
    return std::nullopt;
}

void Map::removeEnemyAt(const Position& position) {
    if (enemies_.erase(position) > 0) {
        tiles_[position.y][position.x] = Tile::FLOOR;
    }
}

std::optional<std::reference_wrapper<Item>> Map::getItemAt(const Position& position) {
    auto it = items_.find(position);
    if (it != items_.end()) {
        return std::ref(*it->second);
    }
    return std::nullopt;
}

const std::optional<std::reference_wrapper<const Item>> Map::getItemAt(const Position& position) const {
    auto it = items_.find(position);
    if (it != items_.end()) {
        return std::cref(*it->second);
    }
    return std::nullopt;
}

std::unique_ptr<Item> Map::takeItemAt(const Position& position) {
    auto it = items_.find(position);
    if (it != items_.end()) {
        std::unique_ptr<Item> item = std::move(it->second);
        items_.erase(it);
        tiles_[position.y][position.x] = Tile::FLOOR;
        return item;
    }
    return nullptr;
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
