//
// Created by Julius König on 26.11.23.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/game_state.h"
#include "../src/common/serialization/json_utils.h"
#include "../src/common/exceptions/gomoku_exception.h"



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
class game_state_test : public ::testing::Test {

protected:
    /* Any object and subroutine declared here can be accessed in the tests */

    game_state test_game_state;
    player* player1 = new player("player1", black);
    player* player2 = new player("player2", white);

    std::string err;
};

//// CHAPTER 1: basic functionality - placing stones
// Placing a stone on an empty board must succeed
TEST_F(game_state_test, add_one_stone) {
    EXPECT_TRUE(test_game_state.place_stone(0, 0, field_type::black_stone, err));
    field_type expected_stone = field_type::black_stone;
    EXPECT_EQ(expected_stone, test_game_state.get_playing_board().at(0).at(0));
}

// The initial state must be an empty board
TEST_F(game_state_test, add_no_stones) {
    std::vector<field_type> row(15, field_type::empty);
    std::vector<std::vector<field_type>> expected_board(15, row);
    EXPECT_EQ(expected_board, test_game_state.get_playing_board());
}

// Adding several stones with different values to an empty board at different spots must succeed
TEST_F(game_state_test, add_many_stones_different_positions) {
    EXPECT_TRUE(test_game_state.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_TRUE(test_game_state.place_stone(5, 0, field_type::white_stone, err));
    EXPECT_TRUE(test_game_state.place_stone(0, 5, field_type::black_stone, err));

    EXPECT_EQ(field_type::black_stone, test_game_state.get_playing_board().at(0).at(0));
    EXPECT_EQ(field_type::white_stone, test_game_state.get_playing_board().at(0).at(5));
    EXPECT_EQ(field_type::black_stone, test_game_state.get_playing_board().at(5).at(0));
}

// Adding two stones to an empty board at the same position must first succeed, then fail
TEST_F(game_state_test, add_two_stones_stones_position) {
    EXPECT_TRUE(test_game_state.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_FALSE(test_game_state.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_FALSE(test_game_state.place_stone(0, 0, field_type::white_stone, err));

    EXPECT_EQ(field_type::black_stone, test_game_state.get_playing_board().at(0).at(0));
}

// Placing a stone outside the board dimensions must fail
TEST_F(game_state_test, place_outside_board) {
    EXPECT_FALSE(test_game_state.place_stone(15, 0, field_type::black_stone, err));
    EXPECT_FALSE(test_game_state.place_stone(0, 15, field_type::white_stone, err));
    EXPECT_FALSE(test_game_state.place_stone(15, 15, field_type::black_stone, err));
    EXPECT_FALSE(test_game_state.place_stone(20, 20, field_type::black_stone, err));
    EXPECT_FALSE(test_game_state.place_stone(-1, -1, field_type::black_stone, err));

    std::vector<field_type> row(15, field_type::empty);
    std::vector<std::vector<field_type>> expected_board(15, row);
    EXPECT_EQ(expected_board, test_game_state.get_playing_board());
}

// Placing an empty stone anywhere on the board must fail
TEST_F(game_state_test, place_empty_stone) {
    EXPECT_FALSE(test_game_state.place_stone(0, 0, field_type::empty, err));
    EXPECT_FALSE(test_game_state.place_stone(5, 5, field_type::empty, err));

    std::vector<field_type> row(15, field_type::empty);
    std::vector<std::vector<field_type>> expected_board(15, row);
    EXPECT_EQ(expected_board, test_game_state.get_playing_board());
}

//// CHAPTER 2: basic functionality - player switching
// swapping the player's colours must change them
TEST_F(game_state_test, swap_colours) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);

    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);

    EXPECT_TRUE(test_game_state.execute_swap(err));
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_TRUE(test_game_state.execute_swap(err));
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
}

// alternate_current_player must successfully alternate the current player
TEST_F(game_state_test, alternate_current_player) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);

    EXPECT_EQ(test_game_state.get_current_player(), player1);
    EXPECT_TRUE(test_game_state.alternate_current_player(err));
    EXPECT_EQ(test_game_state.get_current_player(), player2);
    EXPECT_TRUE(test_game_state.alternate_current_player(err));
    EXPECT_EQ(test_game_state.get_current_player(), player1);
}

TEST_F(game_state_test, swap_starting_player){
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);

    EXPECT_EQ(test_game_state.get_starting_player_idx(), 0);
    EXPECT_TRUE(test_game_state.switch_starting_player(err));
    EXPECT_EQ(test_game_state.get_starting_player_idx(), 1);
    EXPECT_TRUE(test_game_state.switch_starting_player(err));
    EXPECT_EQ(test_game_state.get_starting_player_idx(), 0);
}

//// CHAPTER 3: basic functionality - other functions
TEST_F(game_state_test, is_full){
    EXPECT_FALSE(test_game_state.is_full());
    test_game_state.add_player(player1, err);
    EXPECT_FALSE(test_game_state.is_full());
    test_game_state.add_player(player2, err);
    EXPECT_TRUE(test_game_state.is_full());
}

TEST_F(game_state_test, set_opening_rules){
    EXPECT_EQ(test_game_state.get_opening_rules()->get_ruleset(), uninitialized);
    test_game_state.set_game_mode("freestyle", err);
    EXPECT_EQ(test_game_state.get_opening_rules()->get_ruleset(), freestyle);
    test_game_state.set_game_mode("swap_after_first_move", err);
    EXPECT_EQ(test_game_state.get_opening_rules()->get_ruleset(), swap_after_first_move);
    test_game_state.set_game_mode("swap2", err);
    EXPECT_EQ(test_game_state.get_opening_rules()->get_ruleset(), swap2);
}

//// CHAPTER 3: swap decision functionality
// wrong inputs to determine_swap_decision must return an error
TEST_F(game_state_test, determine_swap_decision) {
    EXPECT_FALSE(test_game_state.determine_swap_decision(swap_decision_type::no_decision_yet, err));
    EXPECT_FALSE(test_game_state.determine_swap_decision(swap_decision_type::deferred_do_not_swap, err));
    EXPECT_FALSE(test_game_state.determine_swap_decision(swap_decision_type::deferred_do_swap, err));
}

TEST_F(game_state_test, uninitialised){
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);

    EXPECT_THROW(test_game_state.update_current_player(err), gomoku_exception);
}


// test freestyle functionality
TEST_F(game_state_test, freestyle) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("freestyle", err);

    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player1);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    EXPECT_EQ(test_game_state.get_current_player(), player2);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 1);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    EXPECT_EQ(test_game_state.get_current_player(), player1);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 2);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
}

// test swap after first move functionality - do swap
TEST_F(game_state_test, swap_after_first_move_do_swap) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap_after_first_move", err);

    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);

    // place first stone
    EXPECT_EQ(test_game_state.get_current_player(), player1);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // player 2 chooses to swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::do_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 1);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // colours have changed
    EXPECT_EQ(test_game_state.get_turn_number(), 2);
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_swap_decision(), do_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
}

// test swap after first move functionality - do not swap
TEST_F(game_state_test, swap_after_first_move_do_not_swap) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap_after_first_move", err);

    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);

    // place first stone (black)
    EXPECT_EQ(test_game_state.get_current_player(), player1);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // player 2 chooses not to swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::do_not_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 1);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    EXPECT_EQ(test_game_state.get_current_player(), player2);
    EXPECT_EQ(test_game_state.get_turn_number(), 2);

    // players still on same colours
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_swap_decision(), do_not_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
}

// test swap2 functionality - do swap
TEST_F(game_state_test, swap2_do_swap) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);

    // players start as black & white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place first stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player still black
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    //place second stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 1);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place third stone (white)
    EXPECT_EQ(test_game_state.get_turn_number(), 2);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is black again
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // second player chooses to swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::do_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 3);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // players have switched colours
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // normal playing
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 4);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_swap_decision(), do_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player2);
}

// test swap2 functionality - do not swap
TEST_F(game_state_test, swap2_do_not_swap) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);

    // players start as black & white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place first stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player still black
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    //place second stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 1);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place third stone (white)
    EXPECT_EQ(test_game_state.get_turn_number(), 2);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is black again
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // second player chooses not to swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::do_not_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 3);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // players have not switched colours
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // normal playing
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 4);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_swap_decision(), do_not_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player1);
}

// test swap2 functionality - defer, do swap
TEST_F(game_state_test, swap2_defer_do_swap) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);

    // players start as black & white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place first stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player still black
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    //place second stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 1);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place third stone (white)
    EXPECT_EQ(test_game_state.get_turn_number(), 2);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is black again
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // second player chooses to defer swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::defer_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 3);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // second player is still white
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_swap_decision(), defer_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // place fourth stone (white)
    EXPECT_EQ(test_game_state.get_turn_number(), 4);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // second player is black
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // place fifth stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 5);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // second player is white again
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // first player chooses to swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::do_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 6);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // players have switched colours
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // normal playing
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 7);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_swap_decision(), deferred_do_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player2);
}

// test swap2 functionality - defer, do not swap
TEST_F(game_state_test, swap2_defer_do_not_swap) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);

    // players start as black & white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place first stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player still black
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    //place second stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 1);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is white
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // place third stone (white)
    EXPECT_EQ(test_game_state.get_turn_number(), 2);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // first player is black again
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // second player chooses to defer swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::defer_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 3);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // second player is still white
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_swap_decision(), defer_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // place fourth stone (white)
    EXPECT_EQ(test_game_state.get_turn_number(), 4);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // second player is black
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // place fifth stone (black)
    EXPECT_EQ(test_game_state.get_turn_number(), 5);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // second player is white again
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player1);

    // first player chooses not to swap
    EXPECT_TRUE(test_game_state.get_swap_next_turn());
    EXPECT_TRUE(test_game_state.determine_swap_decision(swap_decision_type::do_not_swap, err));
    EXPECT_EQ(test_game_state.get_turn_number(), 6);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    test_game_state.iterate_turn();
    // players have not switched colours
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_current_player(), player2);

    // normal playing
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_turn_number(), 7);
    EXPECT_TRUE(test_game_state.update_current_player(err));
    EXPECT_EQ(test_game_state.get_players().at(0)->get_colour(), player_colour_type::black);
    EXPECT_EQ(test_game_state.get_players().at(1)->get_colour(), player_colour_type::white);
    EXPECT_EQ(test_game_state.get_swap_decision(), deferred_do_not_swap);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), player1);
}

//// CHAPTER 4 - Round setup / wrap-up
TEST_F(game_state_test, wrap_up_round_1) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);
    test_game_state.place_stone(2, 2, black_stone, err);
    test_game_state.place_stone(2, 3, black_stone, err);
    test_game_state.place_stone(2, 4, white_stone, err);
    test_game_state.place_stone(2, 5, black_stone, err);

    int old_starting_player_idx = test_game_state.get_starting_player_idx();
    EXPECT_EQ(player1->get_score(), 0);
    EXPECT_EQ(player2->get_score(), 0);
    test_game_state.wrap_up_round(err);
    EXPECT_EQ(player1->get_score(), 1);
    EXPECT_EQ(player2->get_score(), 0);
    EXPECT_TRUE(test_game_state.is_finished());
    EXPECT_FALSE(test_game_state.is_started());
    EXPECT_NE(old_starting_player_idx, test_game_state.get_starting_player_idx());
}

TEST_F(game_state_test, wrap_up_round_2) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    for(int i = 0; i<playing_board::_playing_board_size; ++i){
        for(int j = 0; j<playing_board::_playing_board_size; ++j){
            test_game_state.place_stone(i, j, black_stone, err);
        }
    }
    EXPECT_TRUE(test_game_state.check_for_tie());
    EXPECT_TRUE(test_game_state.is_tied());

    int old_starting_player_idx = test_game_state.get_starting_player_idx();
    EXPECT_EQ(player1->get_score(), 0);
    EXPECT_EQ(player2->get_score(), 0);
    test_game_state.wrap_up_round(err);
    EXPECT_EQ(player1->get_score(), 0);
    EXPECT_EQ(player2->get_score(), 0);
    EXPECT_TRUE(test_game_state.is_finished());
    EXPECT_FALSE(test_game_state.is_started());
    EXPECT_NE(old_starting_player_idx, test_game_state.get_starting_player_idx());
}

TEST_F(game_state_test, prepare_game_1) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);
    test_game_state.place_stone(2, 2, black_stone, err);
    test_game_state.place_stone(2, 3, black_stone, err);
    test_game_state.place_stone(2, 4, white_stone, err);
    test_game_state.place_stone(2, 5, black_stone, err);

    // old state
    EXPECT_EQ(test_game_state.get_players().at(test_game_state.get_starting_player_idx()), player1);

    test_game_state.wrap_up_round(err);
    EXPECT_TRUE(test_game_state.prepare_game(player2, err));

    // new state
    EXPECT_EQ(test_game_state.get_players().at(test_game_state.get_starting_player_idx()), player2);
    EXPECT_FALSE(test_game_state.is_started());
    EXPECT_FALSE(test_game_state.is_finished());
}

TEST_F(game_state_test, prepare_game_2) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);
    test_game_state.place_stone(2, 2, black_stone, err);

    // old state
    EXPECT_EQ(test_game_state.get_players().at(test_game_state.get_starting_player_idx()), player1);

    test_game_state.wrap_up_round(err);
    EXPECT_TRUE(test_game_state.prepare_game(player1, err));

    // new state
    EXPECT_EQ(test_game_state.get_players().at(test_game_state.get_starting_player_idx()), player1);
}

TEST_F(game_state_test, setup_round) {
    test_game_state.add_player(player1, err);
    test_game_state.add_player(player2, err);
    test_game_state.set_game_mode("swap2", err);
    test_game_state.place_stone(2, 2, black_stone, err);
    test_game_state.place_stone(2, 3, black_stone, err);
    test_game_state.place_stone(2, 4, white_stone, err);
    test_game_state.place_stone(2, 5, black_stone, err);

    // old state
    EXPECT_EQ(test_game_state.get_players().at(test_game_state.get_starting_player_idx()), player1);

    test_game_state.wrap_up_round(err);
    test_game_state.setup_round(err);

    // new state
    EXPECT_EQ(test_game_state.get_players().at(test_game_state.get_starting_player_idx()), player2);
    EXPECT_EQ(test_game_state.get_turn_number(), 0);
    EXPECT_EQ(test_game_state.get_playing_board(), playing_board().get_playing_board());
    EXPECT_FALSE(test_game_state.is_finished());
    EXPECT_EQ(test_game_state.get_swap_decision(), no_decision_yet);
    EXPECT_FALSE(test_game_state.get_swap_next_turn());
    EXPECT_EQ(test_game_state.get_current_player(), test_game_state.get_players().at(test_game_state.get_starting_player_idx()));
}

//// CHAPTER 5 - game end checking
// test diagonal stone placement for win condition
TEST_F(game_state_test, check_win_condition_diagonal){
    test_game_state.place_stone(0, 0, black_stone, err);
    test_game_state.place_stone(1, 1, black_stone, err);
    test_game_state.place_stone(2, 2, black_stone, err);
    test_game_state.place_stone(3, 3, black_stone, err);
    test_game_state.place_stone(4, 4, white_stone, err);
    test_game_state.place_stone(6, 6, black_stone, err);
    test_game_state.place_stone(7, 7, black_stone, err);
    test_game_state.place_stone(8, 6, black_stone, err);
    test_game_state.place_stone(9, 5, black_stone, err);
    test_game_state.place_stone(10, 4, black_stone, err);
    test_game_state.place_stone(11, 3, black_stone, err);
    test_game_state.place_stone(12, 2, black_stone, err);
    EXPECT_FALSE(test_game_state.check_win_condition(3, 3, black_stone));
    EXPECT_FALSE(test_game_state.check_win_condition(6, 6, black_stone));
    EXPECT_TRUE(test_game_state.check_win_condition(7, 7, black_stone));
    EXPECT_TRUE(test_game_state.check_win_condition(11, 3, black_stone));
}

// test straight stone placement for win condition
TEST_F(game_state_test, check_win_condition_straight){
    test_game_state.place_stone(0, 0, black_stone, err);
    test_game_state.place_stone(0, 1, black_stone, err);
    test_game_state.place_stone(0, 2, black_stone, err);
    test_game_state.place_stone(0, 3, white_stone, err);
    test_game_state.place_stone(6, 6, black_stone, err);
    test_game_state.place_stone(7, 6, black_stone, err);
    test_game_state.place_stone(8, 6, black_stone, err);
    test_game_state.place_stone(9, 6, black_stone, err);
    test_game_state.place_stone(10, 6, black_stone, err);
    test_game_state.place_stone(11, 6, black_stone, err);
    test_game_state.place_stone(12, 6, black_stone, err);
    EXPECT_FALSE(test_game_state.check_win_condition(0, 3, black_stone));
    EXPECT_FALSE(test_game_state.check_win_condition(0, 3, white_stone));
    EXPECT_TRUE(test_game_state.check_win_condition(7, 6, black_stone));
    EXPECT_TRUE(test_game_state.check_win_condition(11, 6, black_stone));
}

// check for a tie
TEST_F(game_state_test, check_tie){
    for(int i = 0; i<playing_board::_playing_board_size; ++i){
        for(int j = 0; j<playing_board::_playing_board_size; ++j){
            EXPECT_FALSE(test_game_state.check_for_tie());
            test_game_state.place_stone(i, j, black_stone, err);
        }
    }
    EXPECT_TRUE(test_game_state.check_for_tie());
    EXPECT_TRUE(test_game_state.is_tied());
}

//// CHAPTER 6 - Serialisation
// serialising and deserialising a game state must result in the initial game state
TEST_F(game_state_test, serialization_equality) {
    EXPECT_TRUE(test_game_state.place_stone(0, 0, field_type::black_stone, err));
    EXPECT_TRUE(test_game_state.place_stone(5, 5, field_type::white_stone, err));

    rapidjson::Document* json_send = test_game_state.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    class game_state* game_state_recv = game_state::from_json(json_recv);

    EXPECT_EQ(test_game_state.get_id(), game_state_recv->get_id());
    EXPECT_EQ(test_game_state.get_playing_board(), game_state_recv->get_playing_board());
    EXPECT_EQ(test_game_state.get_players(), game_state_recv->get_players());
    EXPECT_EQ(test_game_state.get_current_player(), game_state_recv->get_current_player());
    EXPECT_EQ(test_game_state.get_turn_number(), game_state_recv->get_turn_number());
}

// Deserializing an invalid string must throw a gomoku_exception
TEST_F(game_state_test, serialization_exception) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(playing_board::from_json(json), gomoku_exception);
}