get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

file(GLOB DEPS CONFIGURE_DEPENDS "src/*")
set(ALL_DEPS ${ALL_DEPS} ${DEPS})

add_library(${PROJECT_NAME} STATIC
  ${ALL_DEPS}
)
add_library(keco::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    include
  PRIVATE
    src
)

find_package(OpenSSL REQUIRED)

# 의존성 설정
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    spdlog::spdlog
    OpenSSL::Crypto
)

# 테스트 설정
file(GLOB DEPS CONFIGURE_DEPENDS "test/*")
add_executable(${PROJECT_NAME}_test
  ${DEPS}
)

target_link_libraries(${PROJECT_NAME}_test
  PRIVATE
    ${PROJECT_NAME} gtest keco::sample
)
