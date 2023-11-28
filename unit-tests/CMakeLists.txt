project(Gomoku-unit-tests)

set(TEST_SOURCE_FILES
        playing_board.cpp
        opening_rules.cpp
        player.cpp)

add_executable(Gomoku-tests ${TEST_SOURCE_FILES})

target_compile_definitions(Gomoku-tests PRIVATE GOMOKU_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

target_link_libraries(Gomoku-tests gtest gtest_main Gomoku-lib)

# Add the tests
add_test(NAME tests COMMAND Gomoku-tests)