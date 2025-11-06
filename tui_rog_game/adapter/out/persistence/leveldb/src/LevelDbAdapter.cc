#include "LevelDbAdapter.h"
#include "LevelDbProvider.h"
#include "EnemyRepository.h"
#include "ItemRepository.h"
#include "MapRepository.h"
#include "PlayerRepository.h"
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

struct LevelDbAdapter::Impl {
    ItemRepository itemRepo;
    EnemyRepository enemyRepo;
    PlayerRepository playerRepo;
    MapRepository mapRepo;

    Impl()
        : itemRepo(),
          enemyRepo(),
          playerRepo(itemRepo),
          mapRepo(enemyRepo, itemRepo) {}
};

LevelDbAdapter::LevelDbAdapter(const std::string& db_path)
    : impl_(std::make_unique<Impl>()) {
        // The provider is a singleton and will initialize the DB on first use.
        // The db_path parameter is now implicitly handled by the provider.
        // We could pass it to the provider on first call if needed.
        (void)db_path; // Mark as unused for now
}

LevelDbAdapter::~LevelDbAdapter() = default;

void LevelDbAdapter::saveGameState(const Port::Out::GameStateDTO& game_state) {
    auto& provider = LevelDbProvider::getInstance();
    provider.startBatch();

    impl_->playerRepo.saveForBatch("main_player", game_state.player);
    impl_->mapRepo.saveForBatch("main_map", game_state.map);

    if (provider.commitBatch()) {
        spdlog::info(
            "LevelDbAdapter: Game state saved successfully with batch.");
    } else {
        spdlog::error(
            "LevelDbAdapter: Save operation failed, batch not applied.");
    }
}

std::unique_ptr<Port::Out::GameStateDTO> LevelDbAdapter::loadGameState() {
    auto player_opt = impl_->playerRepo.findById("main_player");
    auto map_opt = impl_->mapRepo.findById("main_map");

    if (player_opt && map_opt) {
        spdlog::info("LevelDbAdapter: Game state loaded.");
        return std::make_unique<Port::Out::GameStateDTO>(
            std::move(map_opt.value()), std::move(player_opt.value()));
    } else {
        spdlog::warn("LevelDbAdapter: No saved game found or data is partial.");
        return nullptr;
    }
}

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
