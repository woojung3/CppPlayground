add_library(inmemory_adapter STATIC
    src/InMemoryAdapter.cc
)

add_library(tui_rog_game::adapter::out::persistence::inmemory ALIAS inmemory_adapter)

target_include_directories(inmemory_adapter
    PUBLIC
        include
    PRIVATE
        src
)

target_link_libraries(inmemory_adapter
    PUBLIC
        tui_rog_game::port::out
    PRIVATE
        spdlog::spdlog
)
