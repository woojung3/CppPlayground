add_library(domain_event
    src/DomainEvent.cc
    src/PlayerMovedEvent.cc
)

add_library(tui_rog_game::domain::event ALIAS domain_event)

target_include_directories(domain_event PUBLIC include)

target_link_libraries(domain_event
    PUBLIC
        tui_rog_game::domain::model
)