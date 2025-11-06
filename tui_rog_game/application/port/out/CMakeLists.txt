add_library(port_out STATIC
    include/IGenerateDescriptionPort.h
    include/ISaveGameStatePort.h
    include/ILoadGameStatePort.h
)

# 네임스페이스 별칭을 생성합니다.
add_library(tui_rog_game::port::out ALIAS port_out)

# 이 라이브러리를 사용하는 다른 모듈이 헤더를 찾을 수 있도록 include 경로를 INTERFACE로 지정합니다.
target_include_directories(port_out
    PUBLIC
        include
    PRIVATE
        src
)

target_link_libraries(port_out
    PUBLIC
        tui_rog_game::domain::model
)
