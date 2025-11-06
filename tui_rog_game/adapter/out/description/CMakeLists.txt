# Description Adapter는 정적(STATIC) 라이브러리로 정의합니다.
add_library(description_adapter STATIC
    src/LlmAdapter.cc
    src/HardcodedDescAdapter.cc
)

# 네임스페이스 별칭을 생성합니다.
add_library(tui_rog_game::adapter::out::description ALIAS description_adapter)

# 이 라이브러리의 헤더를 다른 모듈에서 찾을 수 있도록 include 디렉터리를 PUBLIC으로 지정합니다.
# 또한, 모듈 내부의 소스 파일들이 내부 헤더를 찾을 수 있도록 src 디렉터리를 PRIVATE으로 추가합니다.
target_include_directories(description_adapter
    PUBLIC
        include
    PRIVATE
        src
)

# port/out 모듈, cpp-httplib, nlohmann/json에 의존합니다.
target_link_libraries(description_adapter
    PUBLIC
        tui_rog_game::port::out
    PRIVATE
        httplib # cpp-httplib FetchContent target
        nlohmann_json::nlohmann_json # nlohmann/json FetchContent target
        spdlog::spdlog # spdlog
)
