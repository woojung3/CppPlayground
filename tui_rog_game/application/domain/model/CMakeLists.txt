# Domain Model은 정적(STATIC) 라이브러리로 정의합니다.
add_library(domain_model STATIC
    src/Player.cc
    src/Enemy.cc
    src/Item.cc
    src/Stats.cc
    src/Position.cc
)

# 네임스페이스 별칭을 생성합니다.
add_library(tui_rog_game::domain::model ALIAS domain_model)

# 이 라이브러리의 헤더를 다른 모듈에서 찾을 수 있도록 include 디렉터리를 PUBLIC으로 지정합니다.
# 또한, 모듈 내부의 소스 파일들이 내부 헤더를 찾을 수 있도록 src 디렉터리를 PRIVATE으로 추가합니다.
target_include_directories(domain_model
    PUBLIC
        include
    PRIVATE
        src
)

target_link_libraries(domain_model
)
