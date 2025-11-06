#ifndef TUI_ROG_GAME_ASSEMBLY_INCLUDE_APPLICATIONBUILDER_H
#define TUI_ROG_GAME_ASSEMBLY_INCLUDE_APPLICATIONBUILDER_H

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

#endif // TUI_ROG_GAME_ASSEMBLY_INCLUDE_APPLICATIONBUILDER_H
