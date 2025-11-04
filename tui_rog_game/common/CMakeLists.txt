# 'common' 모듈을 정적(STATIC) 라이브러리로 정의합니다.
# 향후 .cc 파일들이 이곳에 추가될 것입니다.
add_library(common_lib STATIC
    src/DomainEvent.cc
)

# 모던 CMake 활용을 위한 네임스페이스 별칭(ALIAS)을 생성합니다.
add_library(tui_rog_game::common ALIAS common_lib)

# 이 라이브러리의 헤더를 다른 모듈에서 찾을 수 있도록 include 디렉터리를 PUBLIC으로 지정합니다.
# 또한, 모듈 내부의 소스 파일들이 내부 헤더를 찾을 수 있도록 src 디렉터리를 PRIVATE으로 추가합니다.
target_include_directories(common_lib
    PUBLIC
        include
    PRIVATE
        src # 내부 구현 파일들이 사용할 헤더 경로
)
