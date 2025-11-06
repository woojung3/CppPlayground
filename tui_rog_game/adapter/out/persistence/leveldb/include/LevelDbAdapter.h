#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H

#include "ISaveGameStatePort.h"
#include "ILoadGameStatePort.h"
#include <string>
#include <memory> // For std::unique_ptr

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                class LevelDbAdapter : public Port::Out::ISaveGameStatePort, public Port::Out::ILoadGameStatePort {
                public:
                    explicit LevelDbAdapter(const std::string& db_path);
                    ~LevelDbAdapter() override;

                    void saveGameState(const Port::Out::GameStateDTO& game_state) override;
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
