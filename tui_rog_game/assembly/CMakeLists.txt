add_library(assembly
    src/ApplicationBuilder.cc
)
add_library(tui_rog_game::assembly ALIAS assembly)

target_include_directories(assembly PUBLIC include)

target_link_libraries(assembly
    PUBLIC
        adapter_in_tui
        persistence_adapter
        description_adapter
        domain_model
        domain_event
        domain_service
        port_in
        port_out
        spdlog::spdlog
        ftxui::screen
)
