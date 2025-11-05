#include "gtest/gtest.h"
#include "ChatGptAdapter.h" // This is now our GeminiAdapter
#include "Position.h"
#include <string>
#include <cstdlib> // For std::getenv
#include <spdlog/spdlog.h>

// Define a test fixture for ChatGptAdapter
class ChatGptAdapterTest : public ::testing::Test {
protected:
    TuiRogGame::Adapter::Out::Description::ChatGptAdapter adapter;
    TuiRogGame::Domain::Model::Position test_position = {5, 10};

    void SetUp() override {
        // Ensure GEMINI_API_KEY is set for the test
        if (std::getenv("GEMINI_API_KEY") == nullptr) {
            spdlog::warn("GEMINI_API_KEY not set. Gemini API tests might fail.");
            // Optionally, skip tests if API key is not set
            // GTEST_SKIP() << "GEMINI_API_KEY not set. Skipping Gemini API tests.";
        }
    }
};

// Test case for generateDescription
TEST_F(ChatGptAdapterTest, GeneratesNonEmptyDescription) {
    spdlog::info("Running ChatGptAdapterTest.GeneratesNonEmptyDescription");
    std::string description = adapter.generateDescription(test_position);
    spdlog::info("Generated Description: {}", description);

    // Assert that the description is not empty
    ASSERT_FALSE(description.empty());

    // Further assertions could be added here, e.g., checking for specific keywords
    // but LLM output is non-deterministic, so exact matches are not reliable.
    // We can check for error messages if the API call failed.
    ASSERT_EQ(description.find("(API Key Missing)"), std::string::npos) << "Description indicates API Key Missing, but it should not if key is set.";
    ASSERT_EQ(description.find("(Invalid LLM Response)"), std::string::npos) << "Description indicates Invalid LLM Response, but it should not.";
    ASSERT_EQ(description.find("(JSON Parse Error)"), std::string::npos) << "Description indicates JSON Parse Error, but it should not.";
    ASSERT_EQ(description.find("(API Request Failed)"), std::string::npos) << "Description indicates API Request Failed, but it should not.";
}
