#pragma once

#include "Map.h"    // Map의 완전한 정의를 포함
#include "Player.h" // Player의 완전한 정의를 포함

namespace TuiRogGame {
namespace Port {
namespace Out {

// A Data Transfer Object (DTO) to carry game state to the rendering adapter.
struct GameStateDTO {
  Domain::Model::Map map;
  Domain::Model::Player player;

  // Constructor to initialize the members
  GameStateDTO(Domain::Model::Map map_val, Domain::Model::Player player_val)
      : map(std::move(map_val)), player(std::move(player_val)) {}

  // 기본 복사/이동 생성자 및 할당 연산자는 컴파일러가 자동으로 생성하도록
  // 합니다. Map과 Player가 복사 가능해야 합니다.
};

} // namespace Out
} // namespace Port
} // namespace TuiRogGame
