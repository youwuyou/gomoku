//
// Created by Julius König on 26.11.23.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/player/player.h"
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
class player_test : public ::testing::Test {

protected:
    /* Any object and subroutine declared here can be accessed in the tests */

    player player_black = player("player_black", player_colour_type::black);;
    player player_white = player("player_white", player_colour_type::white);;
    std::string err;
};

// initialising from a valid player colour must work
TEST_F(player_test, initialize_valid) {
    player test_player = player("test_player", player_colour_type::black);
    EXPECT_EQ(test_player.get_colour(), player_black.get_colour());
    EXPECT_EQ(test_player.get_score(), 0);
}

// swapping a valid player's colour must result in a player with the opposite colour
TEST_F(player_test, swap_colour) {
    player test_player = player("test_player", player_colour_type::black);
    test_player.swap_colour(err);
    EXPECT_EQ(test_player.get_colour(), player_colour_type::white);
    test_player.swap_colour(err);
    EXPECT_EQ(test_player.get_colour(), player_colour_type::black);
}

// incrementing a valid player's colour must result in a player with a one higher score
TEST_F(player_test, increment_score) {
    player test_player = player("test_player", player_colour_type::black);
    EXPECT_EQ(test_player.get_score(), 0);
    test_player.increment_score(err);
    EXPECT_EQ(test_player.get_score(), 1);
    test_player.increment_score(err);
    EXPECT_EQ(test_player.get_score(), 2);
}

// resetting a valid player's colour must result in a player with a score of 0
TEST_F(player_test, reset_score) {
    player test_player = player("test_player", player_colour_type::black);
    EXPECT_EQ(test_player.get_score(), 0);
    test_player.increment_score(err);
    test_player.increment_score(err);
    test_player.reset_score(err);
    EXPECT_EQ(test_player.get_score(), 0);
}

// serialising and deserialising a opening ruleset must result in the initial opening ruleset
TEST_F(player_test, serialization_equality) {
    rapidjson::Document* json_send = player_white.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    class player* player_recv = player::from_json(json_recv);

    EXPECT_EQ(player_white.get_id(), player_recv->get_id());
    EXPECT_EQ(player_white.get_player_name(), player_recv->get_player_name());
    EXPECT_EQ(player_white.get_colour(), player_recv->get_colour());
}

// Deserializing an invalid string must throw a GomokuException
TEST_F(player_test, serialization_exception) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(player::from_json(json), GomokuException);
}