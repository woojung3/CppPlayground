# Persistence Adapter는 정적(STATIC) 라이브러리로 정의합니다.
# 이 CMakeLists.txt는 이제 하위 디렉토리의 어댑터들을 관리합니다.

# InMemoryAdapter 서브디렉토리 추가
add_subdirectory(inmemory)

# LevelDbAdapter 서브디렉토리 추가
add_subdirectory(leveldb)

# persistence_adapter는 이제 하위 어댑터들을 묶는 역할을 합니다.
# 실제 구현은 inmemory/CMakeLists.txt와 leveldb/CMakeLists.txt에서 정의됩니다.
# 여기서는 편의를 위해 두 어댑터를 모두 링크하는 인터페이스 라이브러리를 정의합니다.
# 실제 사용 시에는 필요한 어댑터만 링크하도록 선택적으로 구성할 수 있습니다.
add_library(persistence_adapter INTERFACE)

# persistence_adapter 인터페이스 라이브러리에 하위 어댑터들을 링크합니다.
# 이렇게 하면 persistence_adapter를 링크하는 다른 모듈에서
# inmemory_adapter와 leveldb_adapter의 기능에 접근할 수 있습니다.
target_link_libraries(persistence_adapter INTERFACE
    tui_rog_game::adapter::out::persistence::inmemory
    tui_rog_game::adapter::out::persistence::leveldb
)

# 네임스페이스 별칭을 생성합니다.
add_library(tui_rog_game::adapter::out::persistence ALIAS persistence_adapter)