//
// Created by Manuel Nowack on 11.04.21.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/player/hand.h"


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
class HandTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        cards.resize(8);
        for (int i = 1; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                cards[i].push_back(new card(i));
            }
        }
    }

    /* Any object and subroutine declared here can be accessed in the tests */

    // cards[i][j] holds a pointer to the j-th copy of a card of value i
    std::vector<std::vector<card*>> cards;
    hand player_hand;
    std::string err;
};

// Adding one card to an empty hand must succeed
TEST_F(HandTest, AddOneCard) {
    EXPECT_TRUE(player_hand.add_card(cards[1][0], err));
    std::vector<card*> expected_hand = {cards[1][0]};
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// The initial state must be an empty hand
TEST_F(HandTest, AddNoCards) {
    std::vector<card*> expected_hand = {};
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// Adding several cards with different values to an empty hand must succeed
TEST_F(HandTest, AddManyCards) {
    EXPECT_TRUE(player_hand.add_card(cards[1][0], err));
    EXPECT_TRUE(player_hand.add_card(cards[3][0], err));
    EXPECT_TRUE(player_hand.add_card(cards[7][0], err));
    std::vector<card*> expected_hand = {cards[1][0], cards[3][0], cards[7][0]};
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// Adding several cards with duplicate values to an empty hand must succeed
TEST_F(HandTest, AddManyCardsWithDuplicates) {
    EXPECT_TRUE(player_hand.add_card(cards[1][0], err));
    EXPECT_TRUE(player_hand.add_card(cards[1][1], err));
    EXPECT_TRUE(player_hand.add_card(cards[1][2], err));
    EXPECT_TRUE(player_hand.add_card(cards[3][0], err));
    EXPECT_TRUE(player_hand.add_card(cards[3][1], err));
    EXPECT_TRUE(player_hand.add_card(cards[7][0], err));
    std::vector<card*> expected_hand = {cards[1][0], cards[1][1], cards[1][2],
                                        cards[3][0], cards[3][1], cards[7][0]};
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// Removing one card from a hand of three cards must succeed
TEST_F(HandTest, RemoveOneCard) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[7][0], err);

    card* to_remove = cards[1][0];
    card* removed_card = nullptr;
    EXPECT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {cards[3][0], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// Removing a card from an empty hand must fail
// Removing a nonexistent card from a hand of three cards must fail
TEST_F(HandTest, RemoveNonexistentCards) {
    card* to_remove = cards[1][0];
    card* removed_card = cards[1][0];
    EXPECT_FALSE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {};
    EXPECT_EQ(nullptr, removed_card);
    // Use ASSERT instead of EXPECT to abort the current function (test)
    // because adding and removing additional cards in an already incorrect
    // hand makes no sense.
    ASSERT_EQ(expected_hand, player_hand.get_cards());

    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[7][0], err);

    to_remove = cards[1][1];
    removed_card = cards[1][1];
    EXPECT_FALSE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][0], cards[3][0], cards[7][0]};
    EXPECT_EQ(nullptr, removed_card);
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// Removing the final card from a hand must succeed
TEST_F(HandTest, RemoveAllCards) {
    player_hand.add_card(cards[1][0], err);

    card* to_remove = cards[1][0];
    card* removed_card = nullptr;
    EXPECT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {};
    EXPECT_EQ(to_remove, removed_card);
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// Removing several cards of different values from a hand of seven cards must succeed
TEST_F(HandTest, RemoveManyCards) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[1][1], err);
    player_hand.add_card(cards[1][2], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[3][1], err);
    player_hand.add_card(cards[7][0], err);

    card* to_remove = cards[1][0];
    card* removed_card = nullptr;
    EXPECT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {cards[1][1], cards[1][2], cards[3][0],
                                        cards[3][1], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());

    to_remove = cards[3][0];
    removed_card = nullptr;
    EXPECT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][1], cards[1][2], cards[3][1], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());

    to_remove = cards[7][0];
    removed_card = nullptr;
    EXPECT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][1], cards[1][2], cards[3][1]};
    EXPECT_EQ(to_remove, removed_card);
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// Removing several cards of the same value from a hand of seven cards must succeed
TEST_F(HandTest, RemoveManyDuplicateCards) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[1][1], err);
    player_hand.add_card(cards[1][2], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[3][1], err);
    player_hand.add_card(cards[7][0], err);

    card* to_remove = cards[3][0];
    card* removed_card = nullptr;
    EXPECT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {cards[1][0], cards[1][1], cards[1][2],
                                        cards[3][1], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());

    to_remove = cards[3][1];
    removed_card = nullptr;
    EXPECT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][0], cards[1][1], cards[1][2], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    EXPECT_EQ(expected_hand, player_hand.get_cards());
}

// The score of a hand with a single card of value one must be equal to 1
TEST_F(HandTest, ScoreOneCard) {
    player_hand.add_card(cards[1][0], err);
    EXPECT_EQ(1, player_hand.get_score());
}

// The score of a hand with a single Lama card must be equal to 10
TEST_F(HandTest, ScoreLama) {
    player_hand.add_card(cards[7][0], err);
    EXPECT_EQ(10, player_hand.get_score());
}

// The score of an empty hand must be zero
TEST_F(HandTest, ScoreNoCards) {
    EXPECT_EQ(0, player_hand.get_score());
}

// Each addition of a card must increase the score by that card's value
TEST_F(HandTest, ScoreManyCards) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(4, player_hand.get_score());
    player_hand.add_card(cards[7][0], err);
    EXPECT_EQ(14, player_hand.get_score());
}

// Each addition of a card must increase the score by that card's value even
// if several cards have the same value
TEST_F(HandTest, ScoreManyCardsWithDuplicates) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[1][1], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[1][2], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(4, player_hand.get_score());
    player_hand.add_card(cards[3][1], err);
    ASSERT_EQ(4, player_hand.get_score());
    player_hand.add_card(cards[7][0], err);
    EXPECT_EQ(14, player_hand.get_score());
}

// A hand of one card must have count 1
TEST_F(HandTest, CountOneCard) {
    player_hand.add_card(cards[1][0], err);
    EXPECT_EQ(1, player_hand.get_nof_cards());
}

// An empty hand must have count 0
TEST_F(HandTest, CountNoCards) {
    EXPECT_EQ(0, player_hand.get_nof_cards());
}

// Each addition of a card must increase the count by 1
TEST_F(HandTest, CountManyCards) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_nof_cards());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(2, player_hand.get_nof_cards());
    player_hand.add_card(cards[7][0], err);
    EXPECT_EQ(3, player_hand.get_nof_cards());
}

// Each addition of a card must increase the count by 1 even if several cards
// have the same value
TEST_F(HandTest, CountManyCardsWithDuplicates) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_nof_cards());
    player_hand.add_card(cards[1][1], err);
    ASSERT_EQ(2, player_hand.get_nof_cards());
    player_hand.add_card(cards[1][2], err);
    ASSERT_EQ(3, player_hand.get_nof_cards());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(4, player_hand.get_nof_cards());
    player_hand.add_card(cards[3][1], err);
    ASSERT_EQ(5, player_hand.get_nof_cards());
    player_hand.add_card(cards[7][0], err);
    EXPECT_EQ(6, player_hand.get_nof_cards());
}
