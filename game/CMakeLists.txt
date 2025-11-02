# 이 예시의 구조화의 핵심은 이 CMakeLists.txt 파일입니다
# 이 코드를 쉽게 고칠 수 있도록 허용하면 안 됩니다! 제일 꼼꼼한 검토(및 그에 수반되는 구조 유지)가 필요합니다

# 주의사항:
# 예시인 certificate 폴더가 통째로 하나의 vertical slice를 구성하므로, certificate 폴더 내의 최상단
# CMakeLists인 이 파일의 구조는 여러 vertical slice로 전파될 가능성이 있습니다(copy-paste를 통해)
# 이 파일을 통해 vertical slice 내의 아키텍처가 강제되기 때문에, 아키텍처에 대한 결정권이 없는 사람이 쉽게 바꿀 수 있도록 허용하면 안 됩니다
# 지금은 예시이기 때문에 _domain 등 접미사를 하드코딩하였지만, 실수가 일어나지 않도록 더 조정해야 합니다

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

# Domain module
# - 도메인 모듈은 외부로의 의존성을 가지지 않도록 주의해야 합니다
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
# - 어댑터 모듈은 inport의 인터페이스를 구현해야 합니다
# - 외부 입력을 직접적으로 수령하는 adapter.in 모듈은 application.inport에만 의존합니다
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
# - 어댑터 모듈은 outport의 인터페이스를 구현해야 합니다
# - 여기에서는 빌드도구(cmake)를 통해 정의와 선언을 분리하였습니다
# - (1) 런타임 분리 (2) 컴파일 타임에 PRIVATE 의존성 전파 완전 분리 <- 가 목적인 경우
# - application_inport에 완제 abstract class (interface class) 를 두는 방식으로 변형을 가해야 합니다
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
# out.persistence 모듈이 빌드도구(cmake)만을 통해 정의와 선언이 분리되었으므로,
# service에서는 application.outport의 정의 뿐만 아니라 adapter.out의 선언을 PRIVATE로 가져와야 합니다
# - (1) 런타임 분리 (2) 컴파일 타임에 PRIVATE 의존성 전파 완전 분리 <- 가 목적인 경우
# - application_inport에 완제 abstract class (interface class) 를 두는 방식으로 변형을 가해야 합니다
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
# adapter.in과 application.service의 연관관계를 끊기 위한 장벽입니다
# INTERFACE 선언시, IDE가 인식할 수 있도록 add_library에 헤더 파일을 기입합니다
add_library(${PROJECT_NAME}_application_inport INTERFACE
  application/inport/include/AddCertificateUseCase.h
)
add_library(playground::${PROJECT_NAME}_application_inport ALIAS ${PROJECT_NAME}_application_inport)
target_include_directories(${PROJECT_NAME}_application_inport
  INTERFACE
    application/inport/include
)

# Application module - outport
# adapter.out과 application.service의 연관관계를 끊기 위한 장벽입니다
# INTERFACE 선언시, IDE가 인식할 수 있도록 add_library에 헤더 파일을 기입합니다
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
# configuration 부입니다. 선언과 정의에 따른 모듈간의 조립을 이 곳에서 수행합니다
# 조립은 main, test 등에서 중복적으로 일어나야 합니다
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
