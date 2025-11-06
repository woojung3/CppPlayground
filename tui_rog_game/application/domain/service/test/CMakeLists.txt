add_executable(GameEngineTest GameEngineTest.cc)
target_link_libraries(GameEngineTest
    PRIVATE
        gtest_main
        gmock_main
        tui_rog_game::domain::service
        tui_rog_game::domain::model
        tui_rog_game::domain::event
        tui_rog_game::port::in
        tui_rog_game::port::out
)
include(GoogleTest)
gtest_discover_tests(GameEngineTest)