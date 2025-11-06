#pragma once
#include "ILoadGameStatePort.h"
#include "ISaveGameStatePort.h"
#include <memory>
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
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame

