#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "GameEngine.h"
#include "ISaveGameStatePort.h"
#include "ILoadGameStatePort.h"
#include "IRenderPort.h"
#include "IGenerateDescriptionPort.h"
#include "PlayerActionCommand.h"
#include "GameStateDTO.h"
#include "Player.h"
#include "Map.h"
#include "DomainEvent.h"
#include "Orc.h"
#include "Goblin.h"
#include "Item.h"

using namespace TuiRogGame::Domain::Service;
using namespace TuiRogGame::Port::Out;
using namespace TuiRogGame::Port::In;
using namespace TuiRogGame::Domain::Model;
using namespace TuiRogGame::Domain::Event;

using ::testing::_; // Matcher for any argument
using ::testing::Return; // Action to return a specific value
using ::testing::ByRef; // Matcher for reference arguments

// Mock classes for the ports
class MockSaveGameStatePort : public ISaveGameStatePort {
public:
    MOCK_METHOD(void, saveGameState, (const GameStateDTO& game_state), (override));
};

class MockLoadGameStatePort : public ILoadGameStatePort {
public:
    MOCK_METHOD(std::unique_ptr<GameStateDTO>, loadGameState, (), (override));
};

class MockRenderPort : public IRenderPort {
public:
    MOCK_METHOD(void, render, (const GameStateDTO& game_state, const std::vector<std::unique_ptr<DomainEvent>>& events), (override));
};

class MockGenerateDescriptionPort : public IGenerateDescriptionPort {
public:
    MOCK_METHOD(std::string, generateDescription, (const GameStateDTO& game_state), (override));
};

// Test fixture for GameEngine
class GameEngineTest : public ::testing::Test {
protected:
    std::shared_ptr<MockSaveGameStatePort> mock_save_port_;
    std::shared_ptr<MockLoadGameStatePort> mock_load_port_;
    // Store raw pointers to the mock description ports for setting expectations
    MockGenerateDescriptionPort* primary_desc_port_ptr_;
    MockGenerateDescriptionPort* alternative_desc_port_ptr_;
    MockRenderPort mock_render_port_;

    std::unique_ptr<GameEngine> game_engine_;

    void SetUp() override {
        mock_save_port_ = std::make_shared<MockSaveGameStatePort>();
        mock_load_port_ = std::make_shared<MockLoadGameStatePort>();

        auto mock_primary_desc_port = std::make_unique<MockGenerateDescriptionPort>();
        primary_desc_port_ptr_ = mock_primary_desc_port.get(); // Get raw pointer before move

        auto mock_alternative_desc_port = std::make_unique<MockGenerateDescriptionPort>();
        alternative_desc_port_ptr_ = mock_alternative_desc_port.get(); // Get raw pointer before move

        game_engine_ = std::make_unique<GameEngine>(
            mock_save_port_,
            mock_load_port_,
            std::move(mock_primary_desc_port),
            std::move(mock_alternative_desc_port)
        );
        game_engine_->setRenderPort(&mock_render_port_);
    }
};

TEST_F(GameEngineTest, InitializeNewGame) {
    // Expect loadGameState to be called and return no saved game
    EXPECT_CALL(*mock_load_port_, loadGameState())
        .WillOnce(Return(nullptr)); // Return nullptr for no saved game

    // No description generation expected during initial game setup

    // Expect render to be called with the initial game state and events
    EXPECT_CALL(mock_render_port_, render(_, _));
    EXPECT_CALL(*mock_save_port_, saveGameState(_));

    PlayerActionCommand command(PlayerActionCommand::INITIALIZE);
    game_engine_->handlePlayerAction(command);

    // Verify that player and map are initialized (can't directly check private members, but can infer from behavior)
    // For example, if a PlayerMovedEvent is generated, it implies player initialization.
}

TEST_F(GameEngineTest, PlayerMoves) {
    // Setup initial game state for movement test
    Player initialPlayer("TestPlayer", Stats{}, {1, 1});
    Map initialMap(10, 10);
    initialMap.generate(); // Ensure map has floor tiles

    EXPECT_CALL(*mock_load_port_, loadGameState())
        .WillOnce(Return(std::make_unique<GameStateDTO>(initialMap, initialPlayer))); // Return a new unique_ptr

    // Expect description generation when player moves
    EXPECT_CALL(*primary_desc_port_ptr_, generateDescription(_))
        .Times(1) // Expect one call for the move action
        .WillOnce(Return("Moved description."));

    EXPECT_CALL(mock_render_port_, render(_, _))
        .Times(2); // Initialize and then move
    EXPECT_CALL(*mock_save_port_, saveGameState(_))
        .Times(2); // Initialize and then move

    // Initialize game first
    PlayerActionCommand initCommand(PlayerActionCommand::INITIALIZE);
    game_engine_->handlePlayerAction(initCommand);

    // Now, test player movement
    PlayerActionCommand moveCommand(PlayerActionCommand::MOVE_DOWN);
    game_engine_->handlePlayerAction(moveCommand);

    // Further assertions could check the player's new position if GameEngine exposed it,
    // or by inspecting the GameStateDTO passed to render.
}

TEST_F(GameEngineTest, ComplexScenario) {
    // --- 1. Initial State Setup ---
    Position playerStartPos = {1, 1};
    Position itemPos = {1, 2};
    Position enemyPos = {1, 3};
    Position exitPos = {1, 4};

    Player initialPlayer("Hero", Stats{}, playerStartPos);
    Map initialMap(10, 10);
    initialMap.generate(); // Generate base map with floor tiles

    // Manually set up specific tiles and entities after generation
    initialMap.setTile(itemPos.x, itemPos.y, Tile::ITEM);
    initialMap.setTile(enemyPos.x, enemyPos.y, Tile::FLOOR); // Ensure enemy position is floor
    initialMap.setTile(exitPos.x, exitPos.y, Tile::EXIT);
    initialMap.setStartPlayerPosition(playerStartPos);

    initialMap.addEnemy(enemyPos, std::make_unique<Orc>(enemyPos));
    initialMap.addItem(itemPos, std::make_unique<Item>(Item::ItemType::HealthPotion, "Health Potion"));

    auto initialGameState = std::make_unique<GameStateDTO>(initialMap, initialPlayer);

    // --- 2. Mock Expectations ---
    // Initialize game: loadGameState returns the prepared state
    EXPECT_CALL(*mock_load_port_, loadGameState())
        .WillOnce(Return(std::move(initialGameState)));

    // Description generation for various actions
    EXPECT_CALL(*primary_desc_port_ptr_, generateDescription(_))
        .WillOnce(Return("Initial description.")) // After init
        .WillOnce(Return("Found a health potion.")) // After moving to item
        .WillRepeatedly(Return("Combat description.")); // During combat

    // Render calls
    EXPECT_CALL(mock_render_port_, render(_, _))
        .Times(::testing::AtLeast(1)); // At least one render call per action

    // Save calls
    EXPECT_CALL(*mock_save_port_, saveGameState(_))
        .Times(::testing::AtLeast(1)); // At least one save call per action

    // --- 3. Simulate Player Actions ---

    // Action 1: Initialize game
    PlayerActionCommand initCommand(PlayerActionCommand::INITIALIZE);
    game_engine_->handlePlayerAction(initCommand);

    // Action 2: Move to item position (1,2)
    PlayerActionCommand moveDown1(PlayerActionCommand::MOVE_DOWN);
    game_engine_->handlePlayerAction(moveDown1);

    // Action 3: Attack to enemy position (1,3)
    PlayerActionCommand moveDown2(PlayerActionCommand::MOVE_DOWN);
    game_engine_->handlePlayerAction(moveDown2);

    // Action 4: Attack enemy until defeated (assuming Orc has 100 health, player does 10 damage)
    // We'll simulate enough attacks to defeat a typical enemy.
    for (int i = 0; i < 10; ++i) { // Simulate 10 attacks
        PlayerActionCommand attackCommand(PlayerActionCommand::ATTACK, 0); // Attack with dummy payload
        game_engine_->handlePlayerAction(attackCommand);
    }

    // Action 5: Move to exit position (1,4)
    PlayerActionCommand moveDown3(PlayerActionCommand::MOVE_DOWN);
    game_engine_->handlePlayerAction(moveDown3);
    PlayerActionCommand moveDown4(PlayerActionCommand::MOVE_DOWN);
    game_engine_->handlePlayerAction(moveDown4);

    // --- 4. Assertions (Implicit via EXPECT_CALLs) ---
    // Additional explicit assertions could be added here if GameEngine exposed more state.
}
