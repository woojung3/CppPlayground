add_library(assembly
    src/ApplicationBuilder.cc
)
add_library(tui_rog_game::assembly ALIAS assembly)

target_include_directories(assembly PUBLIC include)

target_link_libraries(assembly
    PUBLIC
        tui_rog_game::adapter::in::tui
        tui_rog_game::adapter::out::persistence
        tui_rog_game::adapter::out::description
        tui_rog_game::domain::model
        tui_rog_game::domain::event
        tui_rog_game::domain::service
        tui_rog_game::port::in
        tui_rog_game::port::out
        spdlog::spdlog
        ftxui::screen
)
