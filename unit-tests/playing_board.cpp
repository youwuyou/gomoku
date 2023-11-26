//
// Created by Julius König on 26.11.23.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/playing_board/playing_board.h"
#include "../src/common/serialization/json_utils.h"
#include "../src/common/exceptions/GomokuException.h"



/* A test fixture allows to reuse the same configuration of objects for all
 * tests in a test suite. The name of the fixture must match the test suite.
 *
 * For each test defined with TEST_F(), googletest will create a fresh test
 * fixture at runtime, immediately initialize it via SetUp(), run the test,
 * clean up by calling TearDown(), and then delete the test fixture.
 * Note that different tests in the same test suite have different test fixture
 * objects, and googletest always deletes a test fixture before it creates the
 * next one. googletest does not reuse the same test fixture for multiple
 * tests. Any changes one test makes to the fixture do not affect other tests.
 */
class playing_board_test : public ::testing::Test {

protected:
    /* Any object and subroutine declared here can be accessed in the tests */

    // cards[i][j] holds a pointer to the j-th copy of a card of value i
    playing_board board;
    std::string err;
};

// Placing a stone on an empty board must succeed
TEST_F(playing_board_test, add_one_stone) {
    EXPECT_TRUE(board.place_stone(0, 0, field_type::black_stone, err));
    field_type expected_stone = field_type::black_stone;
    EXPECT_EQ(expected_stone, board.get_playing_board().at(0).at(0));
}

// The initial state must be an empty board
TEST_F(playing_board_test, add_no_stones) {
    std::vector<int> row(15, field_type::empty);
    std::vector<std::vector<int>> expected_board(15, row);
    EXPECT_EQ(expected_board, board.get_playing_board());
}

// Adding several stones with different values to an empty board at different spots must succeed
TEST_F(playing_board_test, add_many_stones_different_positions) {
    EXPECT_TRUE(board.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_TRUE(board.place_stone(5, 0, field_type::white_stone, err));
    EXPECT_TRUE(board.place_stone(0, 5, field_type::black_stone, err));

    EXPECT_EQ(field_type::black_stone, board.get_playing_board().at(0).at(0));
    EXPECT_EQ(field_type::white_stone, board.get_playing_board().at(0).at(5));
    EXPECT_EQ(field_type::black_stone, board.get_playing_board().at(5).at(0));
}

// Adding two stones to an empty board at the same position must first succeed, then fail
TEST_F(playing_board_test, add_two_stones_stones_position) {
    EXPECT_TRUE(board.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_FALSE(board.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_FALSE(board.place_stone(0, 0, field_type::white_stone, err));

    EXPECT_EQ(field_type::black_stone, board.get_playing_board().at(0).at(0));
}

// Placing a stone outside the board dimensions must fail
TEST_F(playing_board_test, place_outside_board) {
    EXPECT_FALSE(board.place_stone(15, 0, field_type::black_stone, err));
    EXPECT_FALSE(board.place_stone(0, 15, field_type::white_stone, err));
    EXPECT_FALSE(board.place_stone(15, 15, field_type::black_stone, err));

    std::vector<int> row(15, field_type::empty);
    std::vector<std::vector<int>> expected_board(15, row);
    EXPECT_EQ(expected_board, board.get_playing_board());
}

// Placing an empty stone anywhere on the board must fail
TEST_F(playing_board_test, place_empty_stone) {
    EXPECT_FALSE(board.place_stone(0, 0, field_type::empty, err));
    EXPECT_FALSE(board.place_stone(5, 5, field_type::empty, err));

    std::vector<int> row(15, field_type::empty);
    std::vector<std::vector<int>> expected_board(15, row);
    EXPECT_EQ(expected_board, board.get_playing_board());
}

// Resetting a board with several stones on it must result in an empty board
TEST_F(playing_board_test, reset_board) {
    EXPECT_TRUE(board.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_TRUE(board.place_stone(5, 5, field_type::white_stone, err));

    board.setup_round(err);

    std::vector<int> row(15, field_type::empty);
    std::vector<std::vector<int>> expected_board(15, row);
    EXPECT_EQ(expected_board, board.get_playing_board());
}

// serialising and deserialising a playing board must result in the initial playing board
TEST_F(playing_board_test, serialization_equality) {
    EXPECT_TRUE(board.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_TRUE(board.place_stone(5, 5, field_type::white_stone, err));

    rapidjson::Document* json_send = board.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    class playing_board* playing_board_recv = playing_board::from_json(json_recv);

    EXPECT_EQ(board.get_id(), playing_board_recv->get_id());
    EXPECT_EQ(board.get_playing_board(), playing_board_recv->get_playing_board());
}

// Deserializing an invalid string must throw a GomokuException
TEST_F(playing_board_test, serialization_exception) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(playing_board::from_json(json), GomokuException);
}