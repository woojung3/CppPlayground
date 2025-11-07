#include "Position.h"
#include "Map.h"
#include "Orc.h"
#include "gtest/gtest.h"
#include <map>

using namespace TuiRogGame::Domain::Model;

TEST(PositionTest, EqualityOperator) {
  Position p1 = {1, 2};
  Position p2 = {1, 2};
  Position p3 = {2, 1};

  ASSERT_TRUE(p1 == p2);
  ASSERT_FALSE(p1 == p3);
}

TEST(PositionTest, LessThanOperator) {
  Position p1 = {1, 2};
  Position p2 = {1, 3};
  Position p3 = {2, 1};
  Position p4 = {1, 2};

  ASSERT_TRUE(p1 < p3);
  ASSERT_FALSE(p3 < p1);

  ASSERT_TRUE(p1 < p2);
  ASSERT_FALSE(p2 < p1);

  ASSERT_FALSE(p1 < p4);
}

TEST(PositionTest, MapKeyBehavior) {
  std::map<Position, std::string> testMap;

  Position p1 = {1, 2};
  Position p2 = {1, 2};
  Position p3 = {2, 1};

  testMap[p1] = "Player";
  testMap[p3] = "Enemy";

  auto it = testMap.find(p2);
  ASSERT_NE(it, testMap.end());
  ASSERT_EQ(it->second, "Player");

  it = testMap.find(p1);
  ASSERT_NE(it, testMap.end());
  ASSERT_EQ(it->second, "Player");

  it = testMap.find(p3);
  ASSERT_NE(it, testMap.end());
  ASSERT_EQ(it->second, "Enemy");

  Position p_non_existent = {99, 99};
  it = testMap.find(p_non_existent);
  ASSERT_EQ(it, testMap.end());
}

TEST(MapTest, EnemyManagement) {
  Map map(10, 10);
  map.generate();

  Position newEnemyPos = {1, 1};

  ASSERT_EQ(map.getTile(newEnemyPos.x, newEnemyPos.y), Tile::FLOOR);

  auto enemy = std::make_unique<Orc>(newEnemyPos);
  std::string enemyName = enemy->getName();

  map.addEnemy(newEnemyPos, std::move(enemy));

  auto retrievedEnemy = map.getEnemyAt(newEnemyPos);
  ASSERT_TRUE(retrievedEnemy.has_value());
  ASSERT_EQ(retrievedEnemy->get().getName(), enemyName);

  ASSERT_EQ(map.getTile(newEnemyPos.x, newEnemyPos.y), Tile::ENEMY);

  Position otherPos = {9, 9};
  auto nonExistentEnemy = map.getEnemyAt(otherPos);
  ASSERT_FALSE(nonExistentEnemy.has_value());
}
