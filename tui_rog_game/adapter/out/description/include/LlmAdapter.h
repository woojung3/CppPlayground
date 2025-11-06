#ifndef TUI_ROG_GAME_ADAPTER_OUT_DESCRIPTION_CHATGPTADAPTER_H
#define TUI_ROG_GAME_ADAPTER_OUT_DESCRIPTION_CHATGPTADAPTER_H

#include "IGenerateDescriptionPort.h"
#include <string>
#include <memory> // Required for std::unique_ptr
#include <httplib.h> // Required for httplib::Client

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Description {

                // LlmAdapter is a concrete implementation of IGenerateDescriptionPort
                // that interacts with the ChatGPT API to generate descriptions.
                class LlmAdapter : public Port::Out::IGenerateDescriptionPort {
                public:
                    LlmAdapter();
                    ~LlmAdapter() override = default;

                    std::string generateDescription(const TuiRogGame::Domain::Model::Position& player_position) override;

                private:
                    std::unique_ptr<httplib::Client> cli_;
                };

            } // namespace Description
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_DESCRIPTION_CHATGPTADAPTER_H
