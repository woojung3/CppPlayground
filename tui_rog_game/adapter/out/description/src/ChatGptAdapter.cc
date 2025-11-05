#include "ChatGptAdapter.h"
#include "IGenerateDescriptionPort.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Description {

                ChatGptAdapter::ChatGptAdapter() 
                    : cli_(std::make_unique<httplib::Client>("https://generativelanguage.googleapis.com")) {
                    // Constructor implementation
                }

                std::string ChatGptAdapter::generateDescription(const TuiRogGame::Domain::Model::Position& player_position) {
                    spdlog::info("Gemini Adapter: Generating description for position ({}, {})", player_position.x, player_position.y);

                    const char* gemini_api_key = std::getenv("GEMINI_API_KEY");
                    if (!gemini_api_key) {
                        spdlog::error("Gemini Adapter: GEMINI_API_KEY environment variable not set.");
                        return "An ancient, echoing chamber, but the magic seems to have failed. (API Key Missing)";
                    }

                    std::string path = "/v1beta/models/gemini-2.5-flash:generateContent?key=" + std::string(gemini_api_key);

                    nlohmann::json request_body;
                    std::string prompt = "Describe a fantasy dungeon location at coordinates (" + std::to_string(player_position.x) + ", " + std::to_string(player_position.y) + ") in a concise and evocative manner.";
                    request_body["contents"] = {{{"parts", {{{"text", prompt}}}}}};

                    auto res = cli_->Post(path.c_str(), request_body.dump(), "application/json");

                    if (res && res->status == 200) {
                        try {
                            nlohmann::json response_json = nlohmann::json::parse(res->body);
                            if (response_json.contains("candidates") && !response_json["candidates"].empty()) {
                                const auto& candidate = response_json["candidates"][0];
                                if (candidate.contains("content") && candidate["content"].contains("parts") && !candidate["content"]["parts"].empty()) {
                                    const auto& part = candidate["content"]["parts"][0];
                                    if (part.contains("text")) {
                                        spdlog::info("Gemini Adapter: Successfully generated description.");
                                        return part["text"].get<std::string>();
                                    }
                                }
                            }
                            spdlog::error("Gemini Adapter: LLM response missing expected fields.");
                            return "An ancient, echoing chamber, but the magic seems to have failed. (Invalid LLM Response)";
                        } catch (const nlohmann::json::parse_error& e) {
                            spdlog::error("Gemini Adapter: Failed to parse LLM response JSON: {}", e.what());
                            return "An ancient, echoing chamber, but the magic seems to have failed. (JSON Parse Error)";
                        }
                    } else {
                        spdlog::error("Gemini Adapter: Failed to get response from LLM API. Status: {}, Error: {}", res ? res->status : 0, res ? res->body : "(no response body)");
                        return "An ancient, echoing chamber, but the magic seems to have failed. (API Request Failed)";
                    }
                }

            } // namespace Description
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame
