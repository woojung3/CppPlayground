#include "Map.h"
#include "Orc.h" // Include for Orc class
#include "Goblin.h" // Include for Goblin class
#include <stdexcept>
#include <random>     // For std::random_device, std::mt19937
#include <algorithm>  // For std::shuffle
#include <spdlog/spdlog.h> // New include

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

// New constructor for persistence
Map::Map(int width, int height, Position start_player_position,
    std::vector<std::vector<Tile>> tiles,
    std::map<Position, std::unique_ptr<Enemy>> enemies,
    std::map<Position, std::unique_ptr<Item>> items)
    : width_(width),
      height_(height),
      tiles_(std::move(tiles)),
      enemies_(std::move(enemies)),
      items_(std::move(items)),
      start_player_position_(start_player_position)
{
    if (width_ <= 0 || height_ <= 0) {
        throw std::invalid_argument("Map dimensions must be positive.");
    }
    // No need to generate here, as tiles, enemies, items are provided
}

Map::Map(const Map& other)
    : width_(other.width_),
      height_(other.height_),
      tiles_(other.tiles_),
      start_player_position_(other.start_player_position_)
{
    for (const auto& pair : other.enemies_) {
        enemies_.emplace(pair.first, pair.second->clone());
    }
    for (const auto& pair : other.items_) {
        items_.emplace(pair.first, std::make_unique<Item>(*pair.second));
    }
}

// A simple procedural generation algorithm
void Map::generate() {
    // Clear existing entities
    enemies_.clear();
    items_.clear();

    // Initialize all tiles to WALL
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            tiles_[y][x] = Tile::WALL;
        }
    }

    // Create a simple room (all inner tiles are FLOOR)
    for (int y = 1; y < height_ - 1; ++y) {
        for (int x = 1; x < width_ - 1; ++x) {
            tiles_[y][x] = Tile::FLOOR;
        }
    }

    // Collect all available floor positions
    std::vector<Position> floor_positions;
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (tiles_[y][x] == Tile::FLOOR) {
                floor_positions.push_back({x, y});
            }
        }
    }

    // Ensure there are enough floor tiles for all elements
    if (floor_positions.size() < 5) { // Player, Orc, Goblin, Potion, Exit
        spdlog::error("Map is too small for all elements. Minimum 5 floor tiles required.");
        // Fallback to a minimal playable state or throw an error
        return;
    }

    // Shuffle the floor positions to pick random unique spots
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(floor_positions.begin(), floor_positions.end(), g);

    // Place Player
    start_player_position_ = floor_positions.back();
    floor_positions.pop_back();

    // Place Orc
    Position orc_pos = floor_positions.back();
    floor_positions.pop_back();
    auto orc = std::make_unique<Orc>(orc_pos);
    spdlog::debug("Map::generate: Placing Orc at ({}, {}).", orc_pos.x, orc_pos.y);
    addEnemy(orc_pos, std::move(orc));

    // Place Goblin
    Position goblin_pos = floor_positions.back();
    floor_positions.pop_back();
    auto goblin = std::make_unique<Goblin>(goblin_pos);
    spdlog::debug("Map::generate: Placing Goblin at ({}, {}).", goblin_pos.x, goblin_pos.y);
    addEnemy(goblin_pos, std::move(goblin));

    // Place Health Potion
    Position potion_pos = floor_positions.back();
    floor_positions.pop_back();
    auto potion = std::make_unique<Item>(Item::ItemType::HealthPotion, "Health Potion");
    addItem(potion_pos, std::move(potion));

    // Place Exit
    Position exit_pos = floor_positions.back();
    floor_positions.pop_back();
    tiles_[exit_pos.y][exit_pos.x] = Tile::EXIT;
}

Tile Map::getTile(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return Tile::WALL; // Treat out-of-bounds as walls
    }
    return tiles_[y][x];
}

bool Map::isWalkable(int x, int y) const {
    Tile tile = getTile(x, y);
    return tile == Tile::FLOOR || tile == Tile::EXIT || tile == Tile::ITEM;
}

bool Map::isValidPosition(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

// --- Add/Get/Remove Entities ---

void Map::addEnemy(Position position, std::unique_ptr<Enemy> enemy) {
    spdlog::debug("Map::addEnemy: Attempting to add enemy at ({}, {}).", position.x, position.y);
    Tile tile_at_pos = getTile(position.x, position.y);
    spdlog::debug("Map::addEnemy: Tile at ({}, {}) is {}.", position.x, position.y, static_cast<int>(tile_at_pos));
    if (tile_at_pos == Tile::FLOOR) {
        enemies_[position] = std::move(enemy);
        tiles_[position.y][position.x] = Tile::ENEMY;
        spdlog::debug("Map::addEnemy: Successfully added enemy at ({}, {}). Tile set to ENEMY.", position.x, position.y);
    } else {
        spdlog::warn("Map::addEnemy: Failed to add enemy at ({}, {}). Tile is not FLOOR.", position.x, position.y);
    }
}

void Map::addItem(Position position, std::unique_ptr<Item> item) {
    if (getTile(position.x, position.y) == Tile::FLOOR) {
        items_[position] = std::move(item);
        tiles_[position.y][position.x] = Tile::ITEM;
    }
}

std::optional<std::reference_wrapper<Enemy>> Map::getEnemyAt(const Position& position) {
    spdlog::debug("Map::getEnemyAt: Searching for enemy at ({}, {}).", position.x, position.y);
    auto it = enemies_.find(position);
    if (it != enemies_.end()) {
        spdlog::debug("Map::getEnemyAt: Enemy found at ({}, {}): {}.", position.x, position.y, it->second->getName());
        return std::ref(*it->second);
    }
    spdlog::debug("Map::getEnemyAt: No enemy found at ({}, {}).", position.x, position.y);
    return std::nullopt;
}

const std::optional<std::reference_wrapper<const Enemy>> Map::getEnemyAt(const Position& position) const {
    spdlog::debug("Map::getEnemyAt (const): Searching for enemy at ({}, {}).", position.x, position.y);
    auto it = enemies_.find(position);
    if (it != enemies_.end()) {
        spdlog::debug("Map::getEnemyAt (const): Enemy found at ({}, {}): {}.", position.x, position.y, it->second->getName());
        return std::cref(*it->second);
    }
    spdlog::debug("Map::getEnemyAt (const): No enemy found at ({}, {}).", position.x, position.y);
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
