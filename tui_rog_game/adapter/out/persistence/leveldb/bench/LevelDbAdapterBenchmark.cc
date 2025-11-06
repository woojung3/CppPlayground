#include <benchmark/benchmark.h>
#include "LevelDbAdapter.h"
#include "IPersistencePort.h"
#include "GameStateDTO.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "Stats.h"
#include "Position.h"
#include "PlayerCoreStats.h"

#include <string>
#include <vector>
#include <memory>
#include <filesystem> // For creating and removing temporary directories
#include <spdlog/spdlog.h>

namespace {
void addCounters(benchmark::State &state, uint64_t cnt)
{
    state.counters["OPS"] = benchmark::Counter(cnt, benchmark::Counter::kIsRate);
    state.counters["Latency"] = benchmark::Counter(cnt, benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}
}

namespace TuiRogGame {
namespace Benchmark {

// Global initializer to turn off logging for benchmarks
struct BenchmarkInitializer {
    BenchmarkInitializer() {
        spdlog::set_level(spdlog::level::off);
    }
};
static BenchmarkInitializer benchmark_initializer;

// Helper function to create a dummy GameStateDTO
Port::Out::GameStateDTO createDummyGameState() {
    // Create Player
    Domain::Model::Stats playerStats{20, 15, 10, 25};
    Domain::Model::Position playerPos{5, 5};
    Domain::Model::PlayerCoreStats playerCoreStats{10, 1500, 250}; // level, xp, hp
    std::vector<std::unique_ptr<Domain::Model::Item>> playerInventory;
    playerInventory.push_back(std::make_unique<Domain::Model::Item>(Domain::Model::Item::ItemType::HealthPotion, "Small Health Potion"));
    playerInventory.push_back(std::make_unique<Domain::Model::Item>(Domain::Model::Item::ItemType::StrengthScroll, "Scroll of Strength"));

    Domain::Model::Player player("player1", playerCoreStats, playerStats, playerPos, std::move(playerInventory));

    // Create Map
    int mapWidth = 50;
    int mapHeight = 50;
    Domain::Model::Position startPlayerPos{0, 0};
    std::vector<std::vector<Domain::Model::Tile>> tiles(mapHeight, std::vector<Domain::Model::Tile>(mapWidth, Domain::Model::Tile::FLOOR));
    // Add some walls
    for (int i = 0; i < mapWidth; ++i) {
        tiles[0][i] = Domain::Model::Tile::WALL;
        tiles[mapHeight - 1][i] = Domain::Model::Tile::WALL;
    }
    for (int i = 0; i < mapHeight; ++i) {
        tiles[i][0] = Domain::Model::Tile::WALL;
        tiles[i][mapWidth - 1] = Domain::Model::Tile::WALL;
    }
    tiles[10][10] = Domain::Model::Tile::EXIT;

    std::map<Domain::Model::Position, std::unique_ptr<Domain::Model::Enemy>> enemies;
    Domain::Model::Stats goblinStats{5, 5, 0, 10};
    enemies.emplace(Domain::Model::Position{1, 1}, std::make_unique<Domain::Model::Enemy>(Domain::Model::Enemy::EnemyType::Goblin, "GoblinGrunt", goblinStats, Domain::Model::Position{1, 1}));
    enemies.emplace(Domain::Model::Position{2, 2}, std::make_unique<Domain::Model::Enemy>(Domain::Model::Enemy::EnemyType::Goblin, "GoblinArcher", goblinStats, Domain::Model::Position{2, 2}));

    std::map<Domain::Model::Position, std::unique_ptr<Domain::Model::Item>> items;
    items.emplace(Domain::Model::Position{3, 3}, std::make_unique<Domain::Model::Item>(Domain::Model::Item::ItemType::HealthPotion, "Large Health Potion"));

    Domain::Model::Map gameMap(mapWidth, mapHeight, startPlayerPos, std::move(tiles), std::move(enemies), std::move(items));

    return Port::Out::GameStateDTO(std::move(gameMap), std::move(player));
}

// Fixture for LevelDbAdapter benchmarks
class LevelDbAdapterFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state) override {
        db_path_ = std::filesystem::temp_directory_path() / "leveldb_benchmark_db";
        std::filesystem::remove_all(db_path_); // Clean up previous runs
        adapter_ = std::make_unique<Adapter::Out::Persistence::LevelDbAdapter>(db_path_.string());
        dummy_game_state_ = std::make_unique<Port::Out::GameStateDTO>(createDummyGameState());
    }

    void TearDown(const ::benchmark::State& state) override {
        adapter_.reset(); // Close DB before removing directory
        std::filesystem::remove_all(db_path_);
    }

protected:
    std::unique_ptr<Adapter::Out::Persistence::LevelDbAdapter> adapter_;
    std::filesystem::path db_path_;
    std::unique_ptr<Port::Out::GameStateDTO> dummy_game_state_;
};

// --- Benchmarks for direct LevelDbAdapter usage ---

BENCHMARK_F(LevelDbAdapterFixture, BM_LevelDbAdapter_SaveGame_Direct)(benchmark::State& state) {
    for (auto _ : state) {
        adapter_->saveGame(*dummy_game_state_);
    }

    addCounters(state, state.iterations());
}

BENCHMARK_F(LevelDbAdapterFixture, BM_LevelDbAdapter_LoadGame_Direct)(benchmark::State& state) {
    // Pre-save a game state to load
    adapter_->saveGame(*dummy_game_state_);
    for (auto _ : state) {
        std::unique_ptr<Port::Out::GameStateDTO> loaded_state = adapter_->loadGame();
        benchmark::DoNotOptimize(loaded_state); // Prevent compiler from optimizing away the load
    }

    addCounters(state, state.iterations());
}

// --- Benchmarks for IPersistencePort usage ---

BENCHMARK_F(LevelDbAdapterFixture, BM_LevelDbAdapter_SaveGame_Port)(benchmark::State& state) {
    Port::Out::IPersistencePort* port = adapter_.get(); // Use the port interface
    for (auto _ : state) {
        port->saveGame(*dummy_game_state_);
    }

    addCounters(state, state.iterations());
}

BENCHMARK_F(LevelDbAdapterFixture, BM_LevelDbAdapter_LoadGame_Port)(benchmark::State& state) {
    // Pre-save a game state to load
    adapter_->saveGame(*dummy_game_state_);
    Port::Out::IPersistencePort* port = adapter_.get(); // Use the port interface
    for (auto _ : state) {
        std::unique_ptr<Port::Out::GameStateDTO> loaded_state = port->loadGame();
        benchmark::DoNotOptimize(loaded_state); // Prevent compiler from optimizing away the load
    }

    addCounters(state, state.iterations());
}

} // namespace Benchmark
} // namespace TuiRogGame

BENCHMARK_MAIN();
