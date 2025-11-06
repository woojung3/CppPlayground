#include "LlmAdapter.h"
#include "GameStateDTO.h" // GameStateDTO.h 추가
#include "Map.h"          // Map.h 추가
#include "Player.h"       // Player.h 추가
#include "gtest/gtest.h"
#include <cstdlib> // For std::getenv
#include <spdlog/spdlog.h>
#include <string>

// Define a test fixture for LlmAdapter
class LlmAdapterTest : public ::testing::Test {
protected:
  TuiRogGame::Adapter::Out::Description::LlmAdapter adapter;

  void SetUp() override {
    // Ensure GEMINI_API_KEY is set for the test
    if (std::getenv("GROQ_API_KEY") == nullptr) {
      spdlog::warn("GROQ_API_KEY not set. Gemini API tests might fail.");
      // Optionally, skip tests if API key is not set
      // GTEST_SKIP() << "GROQ_API_KEY not set. Skipping Gemini API tests.";
    }
  }
};

// Test case for generateDescription
TEST_F(LlmAdapterTest, GeneratesNonEmptyDescription) {
  spdlog::info("Running LlmAdapterTest.GeneratesNonEmptyDescription");

  // 더미 Map, Player, GameStateDTO 생성
  TuiRogGame::Domain::Model::Map dummy_map(10, 10); // 10x10 맵 생성
  TuiRogGame::Domain::Model::Player dummy_player(
      "test_player", TuiRogGame::Domain::Model::Stats{10, 10, 10, 10},
      TuiRogGame::Domain::Model::Position{5, 5});
  TuiRogGame::Port::Out::GameStateDTO dummy_game_state(dummy_map, dummy_player);

  std::string description = adapter.generateDescription(dummy_game_state);
  spdlog::info("Generated Description: {}", description);

  // Assert that the description is not empty
  ASSERT_FALSE(description.empty());

  // Further assertions could be added here, e.g., checking for specific
  // keywords but LLM output is non-deterministic, so exact matches are not
  // reliable. We can check for error messages if the API call failed.
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
