#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H

#include "IPersistencePort.h"
#include <string>
#include <optional>
#include <memory> // For std::unique_ptr and std::shared_ptr

#include <leveldb/db.h> // LevelDB header
#include <leveldb/write_batch.h> // For leveldb::WriteBatch

// Include full headers for repository types
#include "ItemRepository.h"
#include "EnemyRepository.h"
#include "PlayerRepository.h"
#include "MapRepository.h"

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                class LevelDbAdapter : public Port::Out::IPersistencePort {
                public:
                    explicit LevelDbAdapter(const std::string& db_path);
                    ~LevelDbAdapter() override;

                    void saveGame(const Port::Out::GameStateDTO& game_state) override;
                    std::unique_ptr<Port::Out::GameStateDTO> loadGame() override;

                private:
                    std::shared_ptr<leveldb::DB> db_; // Shared DB pointer
                    
                    // Repository instances as direct members
                    ItemRepository item_repo_;
                    EnemyRepository enemy_repo_;
                    PlayerRepository player_repo_;
                    MapRepository map_repo_;
                };

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_LEVELDBADAPTER_H
