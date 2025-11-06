#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H

// [Note] 이 모듈의 JSON 처리 방식은 명시적인 이해를 돕기 위해 수동
// 직렬화/역직렬화 방식을 사용하고 있습니다. nlohmann::json 라이브러리의
// `to_json` 및 `from_json` 자유 함수를 활용하면 더 간결하고 효율적인 JSON
// 처리가 가능하며, 이는 향후 개선될 수 있는 부분입니다.

#include "ILoadGameStatePort.h"
#include "ISaveGameStatePort.h"
#include <memory> // For std::unique_ptr
#include <string>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

class LevelDbAdapter : public Port::Out::ISaveGameStatePort,
                       public Port::Out::ILoadGameStatePort {
public:
  explicit LevelDbAdapter(const std::string &db_path);
  ~LevelDbAdapter() override;

  void saveGameState(const Port::Out::GameStateDTO &game_state) override;
  std::unique_ptr<Port::Out::GameStateDTO> loadGameState() override;

private:
  struct Impl; // Forward-declaration of the implementation struct
  std::unique_ptr<Impl> impl_; // Pointer to implementation
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H
