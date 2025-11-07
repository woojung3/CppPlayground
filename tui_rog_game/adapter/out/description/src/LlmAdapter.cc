#include "LlmAdapter.h"
#include "IGenerateDescriptionPort.h"
#include "PlayerDiedEvent.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Description {

struct LlmAdapter::Impl {
  std::unique_ptr<httplib::Client> cli_;
};

LlmAdapter::LlmAdapter() : impl_(std::make_unique<Impl>()) {
  impl_->cli_ = std::make_unique<httplib::Client>("https://api.groq.com");
  impl_->cli_->set_keep_alive(true);
  impl_->cli_->set_connection_timeout(3, 0);
  impl_->cli_->set_read_timeout(5, 0);
  impl_->cli_->set_write_timeout(3, 0);
}

LlmAdapter::~LlmAdapter() = default;

std::string

LlmAdapter::generateDescription(const Port::Out::GameStateDTO &game_state,
                                const Domain::Event::DomainEvent &event) {
  const auto &player = game_state.player;
  const auto &map = game_state.map;
  const auto &player_position = player.getPosition();

  spdlog::info(
      "AI Adapter: Generating description for player at ({}, {}) for event: {}",
      player_position.x, player_position.y, event.toString());

  const char *groq_api_key = std::getenv("GROQ_API_KEY");

  if (!groq_api_key) {
    spdlog::error("AI Adapter: GROQ_API_KEY environment variable not set.");
    return "An ancient, echoing chamber, but the magic seems to have failed. "
           "(API Key Missing)";
  }

  std::string path = "/openai/v1/chat/completions";
  nlohmann::json request_body;
  std::string prompt =
      "당신은 던전 마스터입니다. 다음 게임 상태 정보와 발생한 이벤트를 "
      "바탕으로 플레이어 주변 "
      "상황을 한국어로 간결하고 생생하게 묘사해주세요. 묘사는 50단어 이내로 "
      "해주세요.\n\n";

  if (dynamic_cast<const Domain::Event::PlayerDiedEvent*>(&event)) {
      prompt += "플레이어가 방금 죽었지만, 신비한 힘으로 즉시 부활했습니다. "
          "이 기적적인 부활의 순간을 묘사해주세요.\n";
  }

  prompt += "발생한 이벤트: " + event.toString() + ".\n";
  prompt += "플레이어 정보: 레벨 " + std::to_string(player.getLevel()) +
            ", 경험치 " + std::to_string(player.getXp()) + ", 체력 " +
            std::to_string(player.getHp()) + "/" +
            std::to_string(player.getMaxHp()) + ".\n";

  prompt += "인벤토리: ";

  if (player.getInventory().empty()) {
    prompt += "비어있음.\n";
  } else {
    for (size_t i = 0; i < player.getInventory().size(); ++i) {
      prompt += player.getInventory()[i]->getName();

      if (i < player.getInventory().size() - 1) {
        prompt += ", ";
      }
    }

    prompt += ".\n";
  }

  std::vector<std::string> nearby_elements;
  int px = player_position.x;
  int py = player_position.y;
  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0)
        continue; // 플레이어 자신 위치 제외

      int nx = px + dx;
      int ny = py + dy;

      if (nx >= 0 && nx < map.getWidth() && ny >= 0 && ny < map.getHeight()) {
        Domain::Model::Tile tile = map.getTile(nx, ny);

        if (tile == Domain::Model::Tile::ENEMY) {
          nearby_elements.push_back("몬스터");
        } else if (tile == Domain::Model::Tile::ITEM) {
          nearby_elements.push_back("아이템");
        } else if (tile == Domain::Model::Tile::EXIT) {
          nearby_elements.push_back("출구");
        }
      }
    }
  }

  if (!nearby_elements.empty()) {
    prompt += "현재 위치 (" + std::to_string(px) + ", " + std::to_string(py) +
              ") 주변에는 ";

    for (size_t i = 0; i < nearby_elements.size(); ++i) {
      prompt += nearby_elements[i];

      if (i < nearby_elements.size() - 1) {
        prompt += ", ";
      }
    }

    prompt += "이(가) 있습니다.\n";
    prompt += "몬스터가 가까이 있다면 위협적인 분위기를, 아이템이 가까이 "
              "있다면 흥미로운 발견을, 출구가 가까이 있다면 탈출 또는 다음 "
              "단계로의 기회를 암시하는 묘사를 포함해주세요.\n";

  } else {
    prompt += "현재 위치 (" + std::to_string(px) + ", " + std::to_string(py) +
              ") 주변에는 특별한 것이 없습니다.\n";
  }

  request_body["model"] = "llama-3.3-70b-versatile";
  request_body["max_tokens"] = 150;
  request_body["temperature"] = 0.7;
  request_body["messages"] = {{{"role", "user"}, {"content", prompt}}};

  httplib::Headers headers = {
      {"Authorization", "Bearer " + std::string(groq_api_key)},
      {"Content-Type", "application/json"}};

  auto res = impl_->cli_->Post(path.c_str(), headers, request_body.dump(),
                               "application/json");

  if (res && res->status == 200) {
    try {
      nlohmann::json response_json = nlohmann::json::parse(res->body);
      if (response_json.contains("choices") &&
          !response_json["choices"].empty()) {

        const auto &choice = response_json["choices"][0];
        if (choice.contains("message") &&
            choice["message"].contains("content")) {
          spdlog::info("AI Adapter: Successfully generated description.");
          return choice["message"]["content"].get<std::string>();
        }
      }

      spdlog::error("AI Adapter: LLM response missing expected fields.");
      return "An ancient, echoing chamber, but the magic seems to have failed. "
             "(Invalid LLM Response)";

    } catch (const nlohmann::json::parse_error &e) {
      spdlog::error("AI Adapter: Failed to parse LLM response JSON: {}",
                    e.what());

      return "An ancient, echoing chamber, but the magic seems to have failed. "
             "(JSON Parse Error)";
    }

  }

  else {
    spdlog::error("AI Adapter: Failed to get response from LLM API. Status: "
                  "{}, Error: {}",
                  res ? res->status : 0,
                  res ? res->body : "(no response body)");

    return "An ancient, echoing chamber, but the magic seems to have failed. "
           "(API Request Failed)";
  }
}
} // namespace Description
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
