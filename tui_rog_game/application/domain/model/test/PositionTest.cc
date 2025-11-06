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

  // Test x-coordinate comparison first
  ASSERT_TRUE(p1 < p3);  // {1,2} < {2,1} should be true (1 < 2)
  ASSERT_FALSE(p3 < p1); // {2,1} < {1,2} should be false (2 is not < 1)

  // Test y-coordinate comparison when x is equal
  ASSERT_TRUE(p1 < p2);  // {1,2} < {1,3} should be true (2 < 3)
  ASSERT_FALSE(p2 < p1); // {1,3} < {1,2} should be false (3 is not < 2)

  // Test equality (not less than)
  ASSERT_FALSE(p1 < p4); // {1,2} < {1,2} should be false
}

TEST(PositionTest, MapKeyBehavior) {
  std::map<Position, std::string> testMap;

  Position p1 = {1, 2};
  Position p2 = {1, 2}; // Same logical position as p1
  Position p3 = {2, 1};

  testMap[p1] = "Player";
  testMap[p3] = "Enemy";

  // Should find p1 using p2
  auto it = testMap.find(p2);
  ASSERT_NE(it, testMap.end());
  ASSERT_EQ(it->second, "Player");

  // Should not find p3 using p1
  it = testMap.find(p1);
  ASSERT_NE(it, testMap.end());
  ASSERT_EQ(it->second, "Player");

  // Should find p3
  it = testMap.find(p3);
  ASSERT_NE(it, testMap.end());
  ASSERT_EQ(it->second, "Enemy");

  // Test a non-existent position
  Position p_non_existent = {99, 99};
  it = testMap.find(p_non_existent);
  ASSERT_EQ(it, testMap.end());
}

TEST(MapTest, EnemyManagement) {
  Map map(10, 10); // Create a 10x10 map
  map.generate();  // Generate floor tiles and place initial entities

  // Find a floor position to place a new enemy
  Position newEnemyPos = {1,
                          1}; // Assuming (1,1) is a floor tile after generate()
  // Ensure the tile is actually a floor before adding enemy
  ASSERT_EQ(map.getTile(newEnemyPos.x, newEnemyPos.y), Tile::FLOOR);

  auto enemy = std::make_unique<Orc>(newEnemyPos); // Create an Orc
  std::string enemyName = enemy->getName();

  map.addEnemy(newEnemyPos, std::move(enemy));

  // Try to get the enemy at the position it was added
  auto retrievedEnemy = map.getEnemyAt(newEnemyPos);
  ASSERT_TRUE(retrievedEnemy.has_value());
  ASSERT_EQ(retrievedEnemy->get().getName(), enemyName);

  // Verify the tile type changed to ENEMY
  ASSERT_EQ(map.getTile(newEnemyPos.x, newEnemyPos.y), Tile::ENEMY);

  // Try to get an enemy at a different position where no enemy was added
  Position otherPos = {9, 9};
  auto nonExistentEnemy = map.getEnemyAt(otherPos);
  ASSERT_FALSE(nonExistentEnemy.has_value());
}
