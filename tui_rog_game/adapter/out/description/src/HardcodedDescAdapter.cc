#include "HardcodedDescAdapter.h"
#include <string>
#include <vector>

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Description {

                std::string HardcodedDescAdapter::generateDescription(const Port::Out::GameStateDTO& game_state) {
                    const auto& player_position = game_state.player.getPosition();
                    const auto& map = game_state.map;

                    std::string description = "";

                    // 플레이어 위치 기반 묘사
                    if (player_position.x == 0 && player_position.y == 0) {
                        description += "어둡고 축축한 던전 입구에 서 있습니다. 차가운 바람이 안쪽에서 불어옵니다. ";
                    } else if (player_position.x == 5 && player_position.y == 5) {
                        description += "희미하게 불이 밝혀진 동굴입니다. 깊은 곳에서 이상한 소리가 울려 퍼집니다. ";
                    } else {
                        description += "평범한 던전 복도입니다. 특별한 것은 보이지 않습니다. ";
                    }

                    // 주변 환경 정보 추가
                    std::vector<std::string> nearby_elements;
                    int px = player_position.x;
                    int py = player_position.y;

                    // 주변 1칸 이내 탐색
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0) continue; // 플레이어 자신 위치 제외

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
