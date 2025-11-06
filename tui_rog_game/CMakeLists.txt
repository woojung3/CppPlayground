cmake_minimum_required(VERSION 3.15)
project(TuiRogGame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For LSP support

# Add subdirectories for each module
add_subdirectory(adapter/in/tui)
add_subdirectory(adapter/out/persistence)
add_subdirectory(adapter/out/description)
add_subdirectory(adapter/out/description/test)
add_subdirectory(application/domain/model)
add_subdirectory(application/domain/event)
add_subdirectory(application/domain/service)
add_subdirectory(application/port/in)
add_subdirectory(application/port/out)
add_subdirectory(common)
add_subdirectory(assembly)

############################
# Fetch External Dependencies
############################
include(FetchContent)

# FTXUI
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG        v5.0.0
  GIT_PROGRESS   TRUE
  GIT_SHALLOW    TRUE
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ftxui)

# cpp-httplib
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG        v0.15.3
)
FetchContent_MakeAvailable(httplib)
# httplib is header-only, linking not strictly needed but good practice

# nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# googletest (includes googlemock)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)
# These are for tests, not for the main executable
# We will link them in test-specific CMakeLists.txt later

# googlebenchmark
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_TAG        v1.8.3
)
FetchContent_MakeAvailable(googlebenchmark)
# Also for tests/benchmarks, not main executable

# cxxopts
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG        v3.2.0
)
FetchContent_MakeAvailable(cxxopts)

# leveldb
set(LEVELDB_BUILD_TESTS OFF)
set(LEVELDB_BUILD_BENCHMARKS OFF)
FetchContent_Declare(
  leveldb
  GIT_REPOSITORY https://github.com/google/leveldb.git
  GIT_TAG        1.23
)
FetchContent_MakeAvailable(leveldb)

#################################
# End of External Dependencies
#################################

# Main executable
add_executable(tui_rog_game main.cc)

# Link all libraries to the main executable
target_link_libraries(tui_rog_game
    PRIVATE
        adapter_in_tui
        persistence_adapter
        description_adapter
        domain_model
        domain_event
        domain_service
        port_in
        port_out
        assembly
)

target_link_libraries(tui_rog_game 
    PRIVATE 
        httplib 
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        cxxopts::cxxopts
)

# Set include directories for the entire project
target_include_directories(tui_rog_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter/in/tui/include
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter/out/persistence/include
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter/out/description/include
    ${CMAKE_CURRENT_SOURCE_DIR}/application/domain/model/include
    ${CMAKE_CURRENT_SOURCE_DIR}/application/domain/event/include
    ${CMAKE_CURRENT_SOURCE_DIR}/application/domain/service/include
    ${CMAKE_CURRENT_SOURCE_DIR}/application/port/in/include
    ${CMAKE_CURRENT_SOURCE_DIR}/application/port/out/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common/include
)
