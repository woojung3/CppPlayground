#include "HardcodedDescAdapter.h"
#include "CombatStartedEvent.h"
#include "EnemyAttackedEvent.h"
#include "EnemyDefeatedEvent.h"
#include "GameLoadedEvent.h"
#include "ItemFoundEvent.h"
#include "ItemUsedEvent.h"
#include "MapChangedEvent.h"
#include "PlayerAttackedEvent.h"
#include "PlayerDiedEvent.h"
#include "PlayerLeveledUpEvent.h"
#include "PlayerMovedEvent.h"
#include <string>
#include <vector>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Description {

std::string HardcodedDescAdapter::generateDescription(
    const Port::Out::GameStateDTO &game_state,
    const Domain::Event::DomainEvent &event) {
  const auto &player_position = game_state.player.getPosition();
  const auto &map = game_state.map;

  std::string description = "";
  std::vector<std::string> nearby_elements; // Moved declaration here

  // Event-specific descriptions
  if (auto gameLoadedEvent =
          dynamic_cast<const Domain::Event::GameLoadedEvent *>(&event)) {
    description += "게임이 로드되었습니다. 당신은 던전 깊은 곳에 서 있습니다. ";
  } else if (auto playerMovedEvent =
                 dynamic_cast<const Domain::Event::PlayerMovedEvent *>(
                     &event)) {
    description += "새로운 지역으로 이동했습니다. ";
  } else if (auto combatStartedEvent =
                 dynamic_cast<const Domain::Event::CombatStartedEvent *>(
                     &event)) {
    description +=
        combatStartedEvent->getEnemyName() + "와(과) 전투가 시작되었습니다! ";
  } else if (auto enemyDefeatedEvent =
                 dynamic_cast<const Domain::Event::EnemyDefeatedEvent *>(
                     &event)) {
    description += enemyDefeatedEvent->getEnemyName() + "를(을) 물리쳤습니다! ";
  } else if (auto itemFoundEvent =
                 dynamic_cast<const Domain::Event::ItemFoundEvent *>(&event)) {
    description += itemFoundEvent->getItemName() + "을(를) 발견했습니다! ";
  } else if (auto mapChangedEvent =
                 dynamic_cast<const Domain::Event::MapChangedEvent *>(&event)) {
    description += "새로운 층으로 내려왔습니다. ";
  } else if (auto playerLeveledUpEvent =
                 dynamic_cast<const Domain::Event::PlayerLeveledUpEvent *>(
                     &event)) {
    description += "레벨업했습니다! 현재 레벨: " +
                   std::to_string(playerLeveledUpEvent->getNewLevel()) + ". ";
  } else if (auto playerDiedEvent =
                 dynamic_cast<const Domain::Event::PlayerDiedEvent *>(&event)) {
    description += "마술같은 은혜로 부활했습니다. ";
  } else if (auto playerAttackedEvent =
                 dynamic_cast<const Domain::Event::PlayerAttackedEvent *>(
                     &event)) {
    description += playerAttackedEvent->getEnemyName() + "에게 " +
                   std::to_string(playerAttackedEvent->getDamageDealt()) +
                   "의 피해를 입혔습니다. ";
  } else if (auto enemyAttackedEvent =
                 dynamic_cast<const Domain::Event::EnemyAttackedEvent *>(
                     &event)) {
    description += enemyAttackedEvent->getEnemyName() + "에게 " +
                   std::to_string(enemyAttackedEvent->getDamageDealt()) +
                   "의 피해를 입었습니다. ";
  } else if (auto itemUsedEvent =
                 dynamic_cast<const Domain::Event::ItemUsedEvent *>(&event)) {
    description += itemUsedEvent->getItemName() + "을(를) 사용했습니다. ";
  } else {
    description += "알 수 없는 이벤트가 발생했습니다. ";
  }

  // Generic location description (can be refined or removed if event-specific
  // is enough)
  if (player_position.x == 0 && player_position.y == 0) {
    description += "어둡고 축축한 던전 입구에 서 있습니다. 차가운 바람이 "
                   "안쪽에서 불어옵니다. ";
  } else if (player_position.x == 5 && player_position.y == 5) {
    description += "희미하게 불이 밝혀진 동굴입니다. 깊은 곳에서 이상한 소리가 "
                   "울려 퍼집니다. ";
  } else {
    description += "평범한 던전 복도입니다. 특별한 것은 보이지 않습니다. ";
  }

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
    description += "주변에는 ";
    for (size_t i = 0; i < nearby_elements.size(); ++i) {
      description += nearby_elements[i];
      if (i < nearby_elements.size() - 1) {
        description += ", ";
      }
    }
    description += "이(가) 있습니다.";
  }

  return description;
}

} // namespace Description
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
