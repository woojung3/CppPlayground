#include "GameEngine.h"
#include "DomainEvent.h"
#include "GameStateDTO.h"
#include "Goblin.h"
#include "IGenerateDescriptionPort.h"
#include "ILoadGameStatePort.h"
#include "IRenderPort.h"
#include "ISaveGameStatePort.h"
#include "Item.h"
#include "Map.h"
#include "Orc.h"
#include "Player.h"
#include "PlayerActionCommand.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace TuiRogGame::Domain::Service;
using namespace TuiRogGame::Port::Out;
using namespace TuiRogGame::Port::In;
using namespace TuiRogGame::Domain::Model;
using namespace TuiRogGame::Domain::Event;

using ::testing::_;
using ::testing::ByRef;
using ::testing::Return;

class MockSaveGameStatePort : public ISaveGameStatePort {
public:
  MOCK_METHOD(void, saveGameState, (const GameStateDTO &game_state),
              (override));
};

class MockLoadGameStatePort : public ILoadGameStatePort {
public:
  MOCK_METHOD(std::unique_ptr<GameStateDTO>, loadGameState, (), (override));
};

class MockRenderPort : public IRenderPort {
public:
  MOCK_METHOD(void, render,
              (const GameStateDTO &game_state,
               const std::vector<std::unique_ptr<DomainEvent>> &events),
              (override));
};

class MockGenerateDescriptionPort : public IGenerateDescriptionPort {
public:
  MOCK_METHOD(std::string, generateDescription,
              (const GameStateDTO &game_state,
               const TuiRogGame::Domain::Event::DomainEvent &event),
              (override));
};

class GameEngineTest : public ::testing::Test {
protected:
  std::shared_ptr<MockSaveGameStatePort> mock_save_port_;
  std::shared_ptr<MockLoadGameStatePort> mock_load_port_;

  MockGenerateDescriptionPort *primary_desc_port_ptr_;
  MockGenerateDescriptionPort *alternative_desc_port_ptr_;
  MockRenderPort mock_render_port_;

  std::unique_ptr<GameEngine> game_engine_;

  void SetUp() override {
    mock_save_port_ = std::make_shared<MockSaveGameStatePort>();
    mock_load_port_ = std::make_shared<MockLoadGameStatePort>();

    auto mock_primary_desc_port =
        std::make_unique<MockGenerateDescriptionPort>();
    primary_desc_port_ptr_ =
        mock_primary_desc_port.get(); // Get raw pointer before move

    auto mock_alternative_desc_port =
        std::make_unique<MockGenerateDescriptionPort>();
    alternative_desc_port_ptr_ =
        mock_alternative_desc_port.get(); // Get raw pointer before move

    game_engine_ = std::make_unique<GameEngine>(
        mock_save_port_, mock_load_port_, std::move(mock_primary_desc_port),
        std::move(mock_alternative_desc_port));
    game_engine_->setRenderPort(&mock_render_port_);
  }
};

TEST_F(GameEngineTest, InitializeNewGame) {

  EXPECT_CALL(*mock_load_port_, loadGameState())
      .WillOnce(Return(nullptr)); // Return nullptr for no saved game

  EXPECT_CALL(mock_render_port_, render(_, _));
  EXPECT_CALL(*mock_save_port_, saveGameState(_));

  PlayerActionCommand command(PlayerActionCommand::INITIALIZE);
  game_engine_->handlePlayerAction(command);
}

TEST_F(GameEngineTest, PlayerMoves) {

  Player initialPlayer("TestPlayer", Stats{}, {1, 1});
  Map initialMap(10, 10);
  initialMap.generate(); // Ensure map has floor tiles

  EXPECT_CALL(*mock_load_port_, loadGameState())
      .WillOnce(Return(std::make_unique<GameStateDTO>(
          initialMap, initialPlayer))); // Return a new unique_ptr

  EXPECT_CALL(*primary_desc_port_ptr_, generateDescription(_, _))
      .Times(2) // Expect two calls: GameLoaded, PlayerMoved (action)
      .WillOnce(
          Return("게임이 로드되었습니다. 당신은 던전 깊은 곳에 서 있습니다. "
                 "평범한 던전 복도입니다. 특별한 것은 보이지 않습니다. "))
      .WillOnce(Return("새로운 지역으로 이동했습니다. 평범한 던전 복도입니다. "
                       "특별한 것은 보이지 않습니다. "));

  EXPECT_CALL(mock_render_port_, render(_, _))
      .Times(2); // Initialize and then move
  EXPECT_CALL(*mock_save_port_, saveGameState(_))
      .Times(2); // Initialize and then move

  PlayerActionCommand initCommand(PlayerActionCommand::INITIALIZE);
  game_engine_->handlePlayerAction(initCommand);

  PlayerActionCommand moveCommand(PlayerActionCommand::MOVE_DOWN);
  game_engine_->handlePlayerAction(moveCommand);
}

TEST_F(GameEngineTest, ComplexScenario) {

  Position playerStartPos = {1, 1};
  Position itemPos = {1, 2};
  Position enemyPos = {1, 3};
  Position exitPos = {1, 4};

  Player initialPlayer("Hero", Stats{}, playerStartPos);
  Map initialMap(10, 10);
  initialMap.generate(); // Generate base map with floor tiles

  initialMap.setTile(itemPos.x, itemPos.y, Tile::ITEM);
  initialMap.setTile(enemyPos.x, enemyPos.y,
                     Tile::FLOOR); // Ensure enemy position is floor
  initialMap.setTile(exitPos.x, exitPos.y, Tile::EXIT);
  initialMap.setStartPlayerPosition(playerStartPos);

  initialMap.addEnemy(enemyPos, std::make_unique<Orc>(enemyPos));
  initialMap.addItem(
      itemPos,
      std::make_unique<Item>(Item::ItemType::HealthPotion, "Health Potion"));

  auto initialGameState =
      std::make_unique<GameStateDTO>(initialMap, initialPlayer);

  EXPECT_CALL(*mock_load_port_, loadGameState())
      .WillOnce(Return(std::move(initialGameState)));

  EXPECT_CALL(*primary_desc_port_ptr_, generateDescription(_, _))
      .WillOnce(Return("Initial description."))       // After init
      .WillOnce(Return("Found a health potion."))     // After moving to item
      .WillRepeatedly(Return("Combat description.")); // During combat

  EXPECT_CALL(mock_render_port_, render(_, _))
      .Times(::testing::AtLeast(1)); // At least one render call per action

  EXPECT_CALL(*mock_save_port_, saveGameState(_))
      .Times(::testing::AtLeast(1)); // At least one save call per action

  PlayerActionCommand initCommand(PlayerActionCommand::INITIALIZE);
  game_engine_->handlePlayerAction(initCommand);

  PlayerActionCommand moveDown1(PlayerActionCommand::MOVE_DOWN);
  game_engine_->handlePlayerAction(moveDown1);

  PlayerActionCommand moveDown2(PlayerActionCommand::MOVE_DOWN);
  game_engine_->handlePlayerAction(moveDown2);

  for (int i = 0; i < 10; ++i) { // Simulate 10 attacks
    PlayerActionCommand attackCommand(PlayerActionCommand::ATTACK,
                                      0); // Attack with dummy payload
    game_engine_->handlePlayerAction(attackCommand);
  }

  PlayerActionCommand moveDown3(PlayerActionCommand::MOVE_DOWN);
  game_engine_->handlePlayerAction(moveDown3);
  PlayerActionCommand moveDown4(PlayerActionCommand::MOVE_DOWN);
  game_engine_->handlePlayerAction(moveDown4);
}
