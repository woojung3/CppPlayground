add_executable(LlmAdapterTest LlmAdapterTest.cc)

target_link_libraries(LlmAdapterTest
    PRIVATE
        gtest_main
        description_adapter
        httplib
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        tui_rog_game::domain::model
        tui_rog_game::adapter::out::description
)
