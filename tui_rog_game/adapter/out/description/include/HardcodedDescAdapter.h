#ifndef TUI_ROG_GAME_ADAPTER_OUT_DESCRIPTION_HARDCODEDDESCADAPTER_H
#define TUI_ROG_GAME_ADAPTER_OUT_DESCRIPTION_HARDCODEDDESCADAPTER_H

#include "IGenerateDescriptionPort.h"
#include "GameStateDTO.h" // GameStateDTO를 포함
#include <string>

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Description {

                // HardcodedDescAdapter is a concrete implementation of IGenerateDescriptionPort
                // that provides hardcoded descriptions based on simple logic.
                // This is useful for initial development and testing.
                class HardcodedDescAdapter : public Port::Out::IGenerateDescriptionPort {
                public:
                    HardcodedDescAdapter() = default;
                    ~HardcodedDescAdapter() override = default;

                    std::string generateDescription(const Port::Out::GameStateDTO& game_state) override;
                };

            } // namespace Description
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_DESCRIPTION_HARDCODEDDESCADAPTER_H
