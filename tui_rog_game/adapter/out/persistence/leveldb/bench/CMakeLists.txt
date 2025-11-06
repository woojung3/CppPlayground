add_executable(LevelDbAdapterBenchmark LevelDbAdapterBenchmark.cc)

target_link_libraries(LevelDbAdapterBenchmark
    PRIVATE
    benchmark::benchmark_main
    tui_rog_game::adapter::out::persistence::leveldb # Link to the leveldb adapter
    spdlog::spdlog
)