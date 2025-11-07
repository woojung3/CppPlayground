# Domain Service는 정적(STATIC) 라이브러리로 정의합니다.
add_library(domain_service STATIC
    src/GameEngine.cc
)

# 네임스페이스 별칭을 생성합니다.
add_library(tui_rog_game::domain::service ALIAS domain_service)

# 이 라이브러리의 헤더를 다른 모듈에서 찾을 수 있도록 include 디렉터리를 PUBLIC으로 지정합니다.
# 또한, 모듈 내부의 소스 파일들이 내부 헤더를 찾을 수 있도록 src 디렉터리를 PRIVATE으로 추가합니다.
target_include_directories(domain_service
    PUBLIC
        include
    PRIVATE
        src
)

# domain/model, port/in, port/out 모듈에 의존합니다.
target_link_libraries(domain_service
    PUBLIC
        tui_rog_game::domain::model
    PRIVATE
        tui_rog_game::domain::event
        tui_rog_game::port::in
        tui_rog_game::port::out
        spdlog::spdlog
)
