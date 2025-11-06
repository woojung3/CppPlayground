add_library(leveldb_provider STATIC
    src/LevelDbProvider.cc
)

add_library(tui_rog_game::adapter::out::persistence::leveldb::provider ALIAS leveldb_provider)

target_include_directories(leveldb_provider
    PUBLIC
        include
    PRIVATE
        src
)

target_link_libraries(leveldb_provider
    PRIVATE
        leveldb
        spdlog::spdlog
)

add_library(leveldb_adapter STATIC
    src/LevelDbAdapter.cc
    src/StandardLayoutCrudRepository.cc
    src/PlayerRepository.cc
    src/MapRepository.cc
    src/ItemRepository.cc
    src/EnemyRepository.cc
)

add_library(tui_rog_game::adapter::out::persistence::leveldb ALIAS leveldb_adapter)

target_include_directories(leveldb_adapter
    PUBLIC
        include
    PRIVATE
        src
)

target_link_libraries(leveldb_adapter
    PUBLIC
        tui_rog_game::port::out
        nlohmann_json::nlohmann_json # Linked publicly as serialization might be considered a domain concern, and domain entities might eventually use nlohmann_json directly for to_json/from_json macros/functions.
        tui_rog_game::common
        tui_rog_game::adapter::out::persistence::leveldb::provider # Link to the provider
    PRIVATE
        spdlog::spdlog
)

add_subdirectory(bench)
