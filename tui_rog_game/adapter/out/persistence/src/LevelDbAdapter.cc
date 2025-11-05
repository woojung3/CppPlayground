#include "LevelDbAdapter.h"
#include <spdlog/spdlog.h> // For logging
#include <leveldb/db.h> // LevelDB header
#include "ScopeGuard.h" // For TuiRogGame::Common::MakeGuard

// Include full headers for repository types
#include "CrudRepository.h" // CrudRepository is a template, so its implementation is in .h
#include "ItemRepository.h"
#include "EnemyRepository.h"
#include "PlayerRepository.h"
#include "MapRepository.h"

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                LevelDbAdapter::LevelDbAdapter(const std::string& db_path)
                    : db_([&]() -> std::shared_ptr<leveldb::DB> {
                        leveldb::Options options;
                        options.create_if_missing = true;
                        leveldb::DB* db_raw = nullptr;
                        leveldb::Status status = leveldb::DB::Open(options, db_path, &db_raw);

                        if (!status.ok()) {
                            spdlog::error("LevelDbAdapter: Failed to open LevelDB at {}: {}", db_path, status.ToString());
                            return nullptr;
                        } else {
                            spdlog::info("LevelDbAdapter: Successfully opened LevelDB at {}", db_path);
                            return std::shared_ptr<leveldb::DB>(db_raw);
                        }
                    }()), // Immediately invoked lambda to initialize db_
                      item_repo_(db_),
                      enemy_repo_(db_),
                      player_repo_(db_, item_repo_),
                      map_repo_(db_, enemy_repo_, item_repo_) {
                    // Constructor body is now empty as all members are initialized in the initializer list
                }

                LevelDbAdapter::~LevelDbAdapter() {
                    // db_ shared_ptr will handle closing the DB when its ref count drops to 0
                    spdlog::info("LevelDbAdapter: Closed LevelDB.");
                }

                void LevelDbAdapter::saveGame(const Port::Out::GameStateDTO& game_state) {
                    if (!db_) {
                        spdlog::error("LevelDbAdapter: Cannot save game, DB not open.");
                        return;
                    }

                    leveldb::WriteBatch batch;
                    auto guard = TuiRogGame::Common::MakeGuard([&]() {
                        spdlog::error("LevelDbAdapter: Save operation failed, WriteBatch not applied.");
                    });

                    player_repo_.save("main_player", game_state.player, batch);
                    map_repo_.save("main_map", game_state.map, batch);

                    leveldb::Status status = db_->Write(leveldb::WriteOptions(), &batch);

                    if (!status.ok()) {
                        spdlog::error("LevelDbAdapter: Failed to apply WriteBatch: {}", status.ToString());
                        // The guard's destructor will be called, logging the failure.
                    } else {
                        guard.commit(); // Mark the guard as committed, so its destructor won't log an error.
                        spdlog::info("LevelDbAdapter: Game state saved successfully with WriteBatch.");
                    }
                }

                std::unique_ptr<Port::Out::GameStateDTO> LevelDbAdapter::loadGame() {
                    if (!db_) {
                        spdlog::error("LevelDbAdapter: Cannot load game, DB not open.");
                        return nullptr;
                    }

                    auto player_opt = player_repo_.findById("main_player");
                    auto map_opt = map_repo_.findById("main_map");

                    if (player_opt && map_opt) {
                        spdlog::info("LevelDbAdapter: Game state loaded.");
                        return std::make_unique<Port::Out::GameStateDTO>(map_opt.value(), player_opt.value());
                    } else {
                        spdlog::info("LevelDbAdapter: No saved game found or partial data.");
                        return nullptr;
                    }
                }

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame