#pragma once

#include "TuiAdapter.h" // For TuiAdapter type, which has a run() method
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

