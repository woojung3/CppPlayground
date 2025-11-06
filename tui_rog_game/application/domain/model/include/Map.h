#pragma once

#include <functional> // For std::reference_wrapper
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "Enemy.h"
#include "Item.h"
#include "Position.h"
#include "Tile.h"

namespace TuiRogGame {
namespace Domain {
namespace Model {

class Map {
public:
  Map(int width, int height);
  // New constructor for persistence
  Map(int width, int height, Position start_player_position,
      std::vector<std::vector<Tile>> tiles,
      std::map<Position, std::unique_ptr<Enemy>> enemies,
      std::map<Position, std::unique_ptr<Item>> items);

  Map(const Map &other); // 복사 생성자 선언

  void generate();
  Position getStartPlayerPosition() const {
    return start_player_position_;
  } // New method

  int getWidth() const { return width_; }
  int getHeight() const { return height_; }
  Tile getTile(int x, int y) const;
  bool isWalkable(int x, int y) const;
  bool isValidPosition(int x, int y) const;

  // --- Entity Management ---

  // Getters for persistence
  const std::vector<std::vector<Tile>> &getTiles() const { return tiles_; }
  const std::map<Position, std::unique_ptr<Enemy>> &getEnemies() const {
    return enemies_;
  }
  const std::map<Position, std::unique_ptr<Item>> &getItems() const {
    return items_;
  }

  // Setters for persistence
  void setTiles(std::vector<std::vector<Tile>> tiles) {
    tiles_ = std::move(tiles);
  }
  void addEnemy(
      Position position,
      std::unique_ptr<Enemy> enemy); // Existing, but now public for persistence
  void addItem(
      Position position,
      std::unique_ptr<Item> item); // Existing, but now public for persistence
  void setStartPlayerPosition(Position pos) { start_player_position_ = pos; }
  void setTile(int x, int y, Tile tile);

  // Gets an optional, non-owning reference to the enemy at a position.
  std::optional<std::reference_wrapper<Enemy>>
  getEnemyAt(const Position &position);
  const std::optional<std::reference_wrapper<const Enemy>>
  getEnemyAt(const Position &position) const;

  // Removes and destroys the enemy at a position.
  void removeEnemyAt(const Position &position);

  // Gets an optional, non-owning reference to the item at a position.
  std::optional<std::reference_wrapper<Item>>
  getItemAt(const Position &position);
  const std::optional<std::reference_wrapper<const Item>>
  getItemAt(const Position &position) const;

  // Removes and transfers ownership of the item at a position.
  std::unique_ptr<Item> takeItemAt(const Position &position);

private:
  // void addEnemy(Position position, std::unique_ptr<Enemy> enemy); // Moved to
  // public void addItem(Position position, std::unique_ptr<Item> item); //
  // Moved to public

  int width_;
  int height_;
  std::vector<std::vector<Tile>> tiles_;
  std::map<Position, std::unique_ptr<Enemy>> enemies_;
  std::map<Position, std::unique_ptr<Item>> items_;
  Position start_player_position_; // New member
};

// Position needs a less-than operator to be used as a key in std::map.
} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
