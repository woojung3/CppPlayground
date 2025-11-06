# TUI Adapter는 정적(STATIC) 라이브러리로 정의합니다.
add_library(adapter_in_tui
    src/TuiAdapter.cc
)

# 네임스페이스 별칭을 생성합니다.
add_library(tui_rog_game::adapter::in::tui ALIAS adapter_in_tui)

# 이 라이브러리의 헤더를 다른 모듈에서 찾을 수 있도록 include 디렉터리를 PUBLIC으로 지정합니다.
# 또한, 모듈 내부의 소스 파일들이 내부 헤더를 찾을 수 있도록 src 디렉터리를 PRIVATE으로 추가합니다.
target_include_directories(adapter_in_tui
    PUBLIC
        include
    PRIVATE
        src
)

# port/in, port/out 모듈 및 FTXUI에 의존합니다.
target_link_libraries(adapter_in_tui
    PRIVATE
        tui_rog_game::port::in
        tui_rog_game::port::out
        tui_rog_game::domain::event
        spdlog::spdlog
    PUBLIC
        ftxui::screen
        ftxui::dom
        ftxui::component
)
