add_executable(GeminiAdapterTest GeminiAdapterTest.cc)

target_link_libraries(GeminiAdapterTest
    PRIVATE
        gtest_main
        description_adapter
        httplib
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

target_include_directories(GeminiAdapterTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include # For ChatGptAdapter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../application/domain/model/include # For Position.h
    ${gtest_SOURCE_DIR}/include
)