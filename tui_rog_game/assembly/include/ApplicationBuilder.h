#pragma once

#include "TuiAdapter.h"
#include <memory>

namespace TuiRogGame {
namespace Assembly {

class ApplicationBuilder {
public:
  static std::unique_ptr<Adapter::In::Tui::TuiAdapter>
  build(ftxui::ScreenInteractive &screen);
};

} // namespace Assembly
} // namespace TuiRogGame
