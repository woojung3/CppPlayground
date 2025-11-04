add_library(port_in STATIC
    include/IGetPlayerActionUseCase.h
)

# 네임스페이스 별칭을 생성합니다.
add_library(tui_rog_game::port::in ALIAS port_in)

target_include_directories(port_in
    PUBLIC
        include
    PRIVATE
        src
)

target_link_libraries(port_in
    PUBLIC
        tui_rog_game::domain::model
)
