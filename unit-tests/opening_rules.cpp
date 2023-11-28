//
// Created by Julius König on 26.11.23.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/playing_board/opening_rules.h"
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
class opening_rules_test : public ::testing::Test {

protected:
    /* Any object and subroutine declared here can be accessed in the tests */

    opening_rules ruleset_uninit = opening_rules(ruleset_type::uninitialized);;
    opening_rules ruleset_freestyle = opening_rules(ruleset_type::freestyle);;
    opening_rules ruleset_swap_first_move = opening_rules(ruleset_type::swap_after_first_move);;
    opening_rules ruleset_swap2 = opening_rules(ruleset_type::swap2);;
    std::string err;
};

// initialising from a valid ruleset_type must work
TEST_F(opening_rules_test, initialize_valid) {
    opening_rules test_rules = opening_rules(ruleset_type::freestyle);
    EXPECT_EQ(test_rules.get_ruleset(), ruleset_freestyle.get_ruleset());
}

// serialising and deserialising a opening ruleset must result in the initial opening ruleset
TEST_F(opening_rules_test, serialization_equality) {
    rapidjson::Document* json_send = ruleset_freestyle.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    class opening_rules* opening_rules_recv = opening_rules::from_json(json_recv);

    EXPECT_EQ(ruleset_freestyle.get_id(), opening_rules_recv->get_id());
    EXPECT_EQ(ruleset_freestyle.get_ruleset(), opening_rules_recv->get_ruleset());
    EXPECT_EQ(ruleset_freestyle.get_description(), opening_rules_recv->get_description());
}

// Deserializing an invalid string must throw a GomokuException
TEST_F(opening_rules_test, serialization_exception) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(opening_rules::from_json(json), GomokuException);
}