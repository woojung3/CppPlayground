#include "LlmAdapter.h"
#include "DomainEvent.h"
#include "GameStateDTO.h"
#include "Map.h"
#include "Player.h"
#include "PlayerMovedEvent.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <string>

class LlmAdapterTest : public ::testing::Test {
protected:
  TuiRogGame::Adapter::Out::Description::LlmAdapter adapter;

  void SetUp() override {

    if (std::getenv("GROQ_API_KEY") == nullptr) {
      spdlog::warn("GROQ_API_KEY not set. Gemini API tests might fail.");
    }
  }
};

TEST_F(LlmAdapterTest, GeneratesNonEmptyDescription) {
  spdlog::info("Running LlmAdapterTest.GeneratesNonEmptyDescription");

  TuiRogGame::Domain::Model::Map dummy_map(10, 10); // 10x10 맵 생성
  TuiRogGame::Domain::Model::Player dummy_player(
      "test_player", TuiRogGame::Domain::Model::Stats{10, 10, 10, 10},
      TuiRogGame::Domain::Model::Position{5, 5});
  TuiRogGame::Port::Out::GameStateDTO dummy_game_state(dummy_map, dummy_player);

  // Create a dummy event
  TuiRogGame::Domain::Event::PlayerMovedEvent dummy_event(
      dummy_player.getPosition());

  std::string description =
      adapter.generateDescription(dummy_game_state, dummy_event);
  spdlog::info("Generated Description: {}", description);

  ASSERT_FALSE(description.empty());

  ASSERT_EQ(description.find("(API Key Missing)"), std::string::npos)
      << "Description indicates API Key Missing, but it should not if key is "
         "set.";
  ASSERT_EQ(description.find("(Invalid LLM Response)"), std::string::npos)
      << "Description indicates Invalid LLM Response, but it should not.";
  ASSERT_EQ(description.find("(JSON Parse Error)"), std::string::npos)
      << "Description indicates JSON Parse Error, but it should not.";
  ASSERT_EQ(description.find("(API Request Failed)"), std::string::npos)
      << "Description indicates API Request Failed, but it should not.";
}
