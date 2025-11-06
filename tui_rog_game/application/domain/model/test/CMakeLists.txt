add_executable(PositionTest PositionTest.cc)
target_link_libraries(PositionTest
    PRIVATE
        gtest_main
        tui_rog_game::domain::model
)
include(GoogleTest)
gtest_discover_tests(PositionTest)