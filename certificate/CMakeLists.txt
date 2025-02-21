# 이 예시의 구조화의 핵심은 이 CMakeLists.txt 파일입니다
# 이 코드를 쉽게 고칠 수 있도록 허용하면 안 됩니다! 제일 꼼꼼한 검토(및 그에 수반되는 구조 유지)가 필요합니다

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

# Domain module
add_library(${PROJECT_NAME}_domain STATIC
  domain/src/Certificate.cc
)
add_library(playground::${PROJECT_NAME}_domain ALIAS ${PROJECT_NAME}_domain)
target_include_directories(${PROJECT_NAME}_domain
  PUBLIC
    domain/include
  PRIVATE
    domain/src
)

# Adapter module - in.tui
add_library(${PROJECT_NAME}_adapter_in_tui STATIC
  adapter/in/tui/src/CertificateController.cc
)
add_library(playground::${PROJECT_NAME}_adapter_in_tui ALIAS ${PROJECT_NAME}_adapter_in_tui)
target_include_directories(${PROJECT_NAME}_adapter_in_tui
  PUBLIC
    adapter/in/tui/include
  PRIVATE
    adapter/in/tui/src
)

target_link_libraries(${PROJECT_NAME}_adapter_in_tui
  PUBLIC
    playground::${PROJECT_NAME}_application_inport
)

# Adapter module - out.persistence
add_library(${PROJECT_NAME}_adapter_out_persistence STATIC
  adapter/out/persistence/src/CertificatePersistenceAdapter.cc
  adapter/out/persistence/src/LevelDbAccessor.cc
)
add_library(playground::${PROJECT_NAME}_adapter_out_persistence ALIAS ${PROJECT_NAME}_adapter_out_persistence)

target_include_directories(${PROJECT_NAME}_adapter_out_persistence
  PRIVATE
    adapter/out/persistence/src
)

target_link_libraries(${PROJECT_NAME}_adapter_out_persistence
  PUBLIC
    playground::${PROJECT_NAME}_application_outport
  # PRIVATE leveldb <- inmemory로 대체함(데모용)
)

# Application module - service
add_library(${PROJECT_NAME}_application_service STATIC
  application/service/src/AddCertificateService.cc
)
add_library(playground::${PROJECT_NAME}_application_service ALIAS ${PROJECT_NAME}_application_service)
target_include_directories(${PROJECT_NAME}_application_service
  PRIVATE
    application/service/src
)

target_link_libraries(${PROJECT_NAME}_application_service
  PUBLIC
    playground::${PROJECT_NAME}_application_inport
  PRIVATE
    playground::${PROJECT_NAME}_domain
    playground::${PROJECT_NAME}_application_outport
    playground::${PROJECT_NAME}_adapter_out_persistence
)

# Application module - inport
add_library(${PROJECT_NAME}_application_inport INTERFACE
  application/inport/include/AddCertificateUseCase.h
)
add_library(playground::${PROJECT_NAME}_application_inport ALIAS ${PROJECT_NAME}_application_inport)
target_include_directories(${PROJECT_NAME}_application_inport
  INTERFACE
    application/inport/include
)

# Application module - outport
add_library(${PROJECT_NAME}_application_outport INTERFACE
  application/outport/include/UpdateCertificatePort.h
)
add_library(playground::${PROJECT_NAME}_application_outport ALIAS ${PROJECT_NAME}_application_outport)
target_include_directories(${PROJECT_NAME}_application_outport
  INTERFACE
    application/outport/include
)

target_link_libraries(${PROJECT_NAME}_application_outport
  INTERFACE
    playground::${PROJECT_NAME}_domain
)

# Test execurable
file(GLOB DEPS CONFIGURE_DEPENDS "test/*")
add_executable(${PROJECT_NAME}_test
  ${DEPS}
)

target_link_libraries(${PROJECT_NAME}_test
  PRIVATE
    ${PROJECT_NAME}_adapter_in_tui
    ${PROJECT_NAME}_application_service
    gtest
)
