#include "Map.h"
#include "Goblin.h"
#include "Orc.h"
#include <algorithm>
#include <random>
#include <set>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace TuiRogGame {
namespace Domain {
namespace Model {

Map::Map(int width, int height) : width_(width), height_(height) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument("Map dimensions must be positive.");
  }
  tiles_.resize(height, std::vector<Tile>(width, Tile::WALL));
}

Map::Map(int width, int height, Position start_player_position,
         std::vector<std::vector<Tile>> tiles,
         std::map<Position, std::unique_ptr<Enemy>> enemies,
         std::map<Position, std::unique_ptr<Item>> items)
    : width_(width), height_(height), tiles_(std::move(tiles)),
      enemies_(std::move(enemies)), items_(std::move(items)),
      start_player_position_(start_player_position) {
  if (width_ <= 0 || height_ <= 0) {
    throw std::invalid_argument("Map dimensions must be positive.");
  }
}

Map::Map(const Map &other)
    : width_(other.width_), height_(other.height_), tiles_(other.tiles_),
      start_player_position_(other.start_player_position_) {
  for (const auto &pair : other.enemies_) {
    enemies_.emplace(pair.first, pair.second->clone());
  }
  for (const auto &pair : other.items_) {
    items_.emplace(pair.first, std::make_unique<Item>(*pair.second));
  }
}

void Map::generate() {
  enemies_.clear();
  items_.clear();

  // Initialize all tiles to WALL
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      tiles_[y][x] = Tile::WALL;
    }
  }

  // Random walk parameters
  int max_walk_length =
      width_ * height_ * 2; // Walk for a length proportional to map size
  int current_x = width_ / 2;
  int current_y = height_ / 2;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(
      0, 3); // 0: up, 1: down, 2: left, 3: right

  for (int i = 0; i < max_walk_length; ++i) {
    if (isValidPosition(current_x, current_y)) {
      tiles_[current_y][current_x] = Tile::FLOOR;
    }

    int direction = distrib(gen);
    switch (direction) {
    case 0: // Up
      current_y--;
      break;
    case 1: // Down
      current_y++;
      break;
    case 2: // Left
      current_x--;
      break;
    case 3: // Right
      current_x++;
      break;
    }

    // Keep walker within bounds
    current_x = std::max(1, std::min(current_x, width_ - 2));
    current_y = std::max(1, std::min(current_y, height_ - 2));
  }

  // Collect all floor positions after random walk
  std::vector<Position> floor_positions;
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      if (tiles_[y][x] == Tile::FLOOR) {
        floor_positions.push_back({x, y});
      }
    }
  }

  if (!floor_positions.empty()) {
    std::vector<Position> all_floor_tiles = floor_positions;
    std::set<Position> visited;
    std::vector<Position> largest_component;

    for (const auto &start_pos : all_floor_tiles) {
      if (visited.find(start_pos) == visited.end()) {
        std::vector<Position> q;
        q.push_back(start_pos);
        visited.insert(start_pos);

        int head = 0;
        while (head < q.size()) {
          Position pos = q[head++];

          Position neighbors[] = {{pos.x + 1, pos.y},
                                  {pos.x - 1, pos.y},
                                  {pos.x, pos.y + 1},
                                  {pos.x, pos.y - 1}};
          for (const auto &neighbor : neighbors) {
            if (isValidPosition(neighbor.x, neighbor.y) &&
                tiles_[neighbor.y][neighbor.x] == Tile::FLOOR &&
                visited.find(neighbor) == visited.end()) {
              visited.insert(neighbor);
              q.push_back(neighbor);
            }
          }
        }

        if (q.size() > largest_component.size()) {
          largest_component = q;
        }
      }
    }

    std::set<Position> largest_set(largest_component.begin(),
                                   largest_component.end());
    for (const auto &pos : all_floor_tiles) {
      if (largest_set.find(pos) == largest_set.end()) {
        tiles_[pos.y][pos.x] = Tile::WALL;
      }
    }
    floor_positions = largest_component;
  }

  if (floor_positions.empty()) {
    // Fallback if no floor tiles were generated (e.g., very small map or
    // unlucky walk) Create a 3x3 room in the center
    int center_x = width_ / 2;
    int center_y = height_ / 2;
    for (int y = center_y - 1; y <= center_y + 1; ++y) {
      for (int x = center_x - 1; x <= center_x + 1; ++x) {
        if (isValidPosition(x, y)) {
          tiles_[y][x] = Tile::FLOOR;
          floor_positions.push_back({x, y});
        }
      }
    }

    if (floor_positions.empty()) {
      spdlog::error("Map is too small to generate any floor tiles.");
      return;
    }
  }

  // Shuffle floor positions for random placement of entities
  std::shuffle(floor_positions.begin(), floor_positions.end(), gen);

  // Place player, enemies, items, and exit
  if (floor_positions.size() < 2) {
    spdlog::warn("Not enough floor tiles for both player and exit. They might "
                 "share a position.");
  }

  // Ensure player start position is set
  start_player_position_ = floor_positions.back();
  floor_positions.pop_back();

  // Ensure exit position is set
  if (!floor_positions.empty()) {
    Position exit_pos = floor_positions.back();
    floor_positions.pop_back();
    tiles_[exit_pos.y][exit_pos.x] = Tile::EXIT;
  } else {
    // If only one floor tile, player and exit share it
    tiles_[start_player_position_.y][start_player_position_.x] = Tile::EXIT;
    spdlog::warn("Only one floor tile generated. Player and Exit share the "
                 "same position.");
  }

  // Place enemies and items if there are remaining floor positions
  if (!floor_positions.empty()) {
    Position orc_pos = floor_positions.back();
    floor_positions.pop_back();
    auto orc = std::make_unique<Orc>(orc_pos);
    spdlog::debug("Map::generate: Placing Orc at ({}, {}).", orc_pos.x,
                  orc_pos.y);
    addEnemy(orc_pos, std::move(orc));
  }

  if (!floor_positions.empty()) {
    Position goblin_pos = floor_positions.back();
    floor_positions.pop_back();
    auto goblin = std::make_unique<Goblin>(goblin_pos);
    spdlog::debug("Map::generate: Placing Goblin at ({}, {}).", goblin_pos.x,
                  goblin_pos.y);
    addEnemy(goblin_pos, std::move(goblin));
  }

  if (!floor_positions.empty()) {
    Position potion_pos = floor_positions.back();
    floor_positions.pop_back();
    auto potion =
        std::make_unique<Item>(Item::ItemType::HealthPotion, "Health Potion");
    addItem(potion_pos, std::move(potion));
  }
}
Tile Map::getTile(int x, int y) const {
  if (x < 0 || x >= width_ || y < 0 || y >= height_) {
    return Tile::WALL;
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

void Map::addEnemy(Position position, std::unique_ptr<Enemy> enemy) {
  spdlog::debug("Map::addEnemy: Attempting to add enemy at ({}, {}).",
                position.x, position.y);
  Tile tile_at_pos = getTile(position.x, position.y);
  spdlog::debug("Map::addEnemy: Tile at ({}, {}) is {}.", position.x,
                position.y, static_cast<int>(tile_at_pos));
  if (tile_at_pos == Tile::FLOOR) {
    enemies_[position] = std::move(enemy);
    tiles_[position.y][position.x] = Tile::ENEMY;
    spdlog::debug("Map::addEnemy: Successfully added enemy at ({}, {}). Tile "
                  "set to ENEMY.",
                  position.x, position.y);
  } else {
    spdlog::warn(
        "Map::addEnemy: Failed to add enemy at ({}, {}). Tile is not FLOOR.",
        position.x, position.y);
  }
}

void Map::addItem(Position position, std::unique_ptr<Item> item) {
  if (getTile(position.x, position.y) == Tile::FLOOR) {
    items_[position] = std::move(item);
    tiles_[position.y][position.x] = Tile::ITEM;
  }
}

std::optional<std::reference_wrapper<Enemy>>
Map::getEnemyAt(const Position &position) {
  spdlog::debug("Map::getEnemyAt: Searching for enemy at ({}, {}).", position.x,
                position.y);
  auto it = enemies_.find(position);
  if (it != enemies_.end()) {
    spdlog::debug("Map::getEnemyAt: Enemy found at ({}, {}): {}.", position.x,
                  position.y, it->second->getName());
    return std::ref(*it->second);
  }
  spdlog::debug("Map::getEnemyAt: No enemy found at ({}, {}).", position.x,
                position.y);
  return std::nullopt;
}

const std::optional<std::reference_wrapper<const Enemy>>
Map::getEnemyAt(const Position &position) const {
  spdlog::debug("Map::getEnemyAt (const): Searching for enemy at ({}, {}).",
                position.x, position.y);
  auto it = enemies_.find(position);
  if (it != enemies_.end()) {
    spdlog::debug("Map::getEnemyAt (const): Enemy found at ({}, {}): {}.",
                  position.x, position.y, it->second->getName());
    return std::cref(*it->second);
  }
  spdlog::debug("Map::getEnemyAt (const): No enemy found at ({}, {}).",
                position.x, position.y);
  return std::nullopt;
}

void Map::removeEnemyAt(const Position &position) {
  if (enemies_.erase(position) > 0) {
    tiles_[position.y][position.x] = Tile::FLOOR;
  }
}

std::optional<std::reference_wrapper<Item>>
Map::getItemAt(const Position &position) {
  auto it = items_.find(position);
  if (it != items_.end()) {
    return std::ref(*it->second);
  }
  return std::nullopt;
}

const std::optional<std::reference_wrapper<const Item>>
Map::getItemAt(const Position &position) const {
  auto it = items_.find(position);
  if (it != items_.end()) {
    return std::cref(*it->second);
  }
  return std::nullopt;
}

std::unique_ptr<Item> Map::takeItemAt(const Position &position) {
  auto it = items_.find(position);
  if (it != items_.end()) {
    std::unique_ptr<Item> item = std::move(it->second);
    items_.erase(it);
    tiles_[position.y][position.x] = Tile::FLOOR;
    return item;
  }
  return nullptr;
}

void Map::setTile(int x, int y, Tile tile) {
  if (isValidPosition(x, y)) {
    tiles_[y][x] = tile;
  }
}

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
