get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

file(GLOB DEPS CONFIGURE_DEPENDS "src/*")
set(ALL_DEPS ${ALL_DEPS} ${DEPS})

add_library(${PROJECT_NAME} STATIC
  ${ALL_DEPS}
)
add_library(playground::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    include
)

# 의존성 설정
# target_link_libraries(${PROJECT_NAME}
#   PUBLIC
#     leveldb
# )

# 테스트 설정
file(GLOB DEPS CONFIGURE_DEPENDS "gtest/*")
add_executable(${PROJECT_NAME}_test
  ${DEPS}
)

target_link_libraries(${PROJECT_NAME}_test
  PRIVATE
    ${PROJECT_NAME} gtest
)

file(GLOB DEPS CONFIGURE_DEPENDS "gbench/*")
add_executable(${PROJECT_NAME}_bench
  ${DEPS}
)

target_link_libraries(${PROJECT_NAME}_bench
  PRIVATE
    ${PROJECT_NAME} benchmark::benchmark
)