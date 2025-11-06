#pragma once

#include "PlayerActionCommand.h"

namespace TuiRogGame {
namespace Port {
namespace In {




class IGetPlayerActionUseCase {
public:
  virtual ~IGetPlayerActionUseCase() = default;



  virtual void handlePlayerAction(
      const TuiRogGame::Port::In::PlayerActionCommand &command) = 0;

  virtual void toggleDescriptionPort() = 0;
};

} // namespace In
} // namespace Port
} // namespace TuiRogGame

