#pragma once

#include <functional>
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

  Map(int width, int height, Position start_player_position,
      std::vector<std::vector<Tile>> tiles,
      std::map<Position, std::unique_ptr<Enemy>> enemies,
      std::map<Position, std::unique_ptr<Item>> items);

  Map(const Map &other);

  void generate();
  Position getStartPlayerPosition() const { return start_player_position_; }

  int getWidth() const { return width_; }
  int getHeight() const { return height_; }
  Tile getTile(int x, int y) const;
  bool isWalkable(int x, int y) const;
  bool isValidPosition(int x, int y) const;

  const std::vector<std::vector<Tile>> &getTiles() const { return tiles_; }
  const std::map<Position, std::unique_ptr<Enemy>> &getEnemies() const {
    return enemies_;
  }
  const std::map<Position, std::unique_ptr<Item>> &getItems() const {
    return items_;
  }

  void setTiles(std::vector<std::vector<Tile>> tiles) {
    tiles_ = std::move(tiles);
  }
  void addEnemy(Position position, std::unique_ptr<Enemy> enemy);
  void addItem(Position position, std::unique_ptr<Item> item);
  void setStartPlayerPosition(Position pos) { start_player_position_ = pos; }
  void setTile(int x, int y, Tile tile);

  std::optional<std::reference_wrapper<Enemy>>
  getEnemyAt(const Position &position);
  const std::optional<std::reference_wrapper<const Enemy>>
  getEnemyAt(const Position &position) const;

  void removeEnemyAt(const Position &position);

  std::optional<std::reference_wrapper<Item>>
  getItemAt(const Position &position);
  const std::optional<std::reference_wrapper<const Item>>
  getItemAt(const Position &position) const;

  std::unique_ptr<Item> takeItemAt(const Position &position);

private:
  int width_;
  int height_;
  std::vector<std::vector<Tile>> tiles_;
  std::map<Position, std::unique_ptr<Enemy>> enemies_;
  std::map<Position, std::unique_ptr<Item>> items_;
  Position start_player_position_;
};

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame
