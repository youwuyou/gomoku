//
// Created by Manuel Nowack on 10.04.21.
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/LamaException.h"
#include "../src/common/game_state/cards/card.h"
#include "../src/common/serialization/json_utils.h"


// A card can be played on another card if and only if the new card has the
// same value or the same value plus one than the previous card or the previous card
// has value 7 and the new card has value 1.
TEST(CardTest, PlayCardsOn1) {
    card c_1(1);
    card c_2(2);
    card c_3(3);
    card c_4(4);
    card c_5(5);
    card c_6(6);
    card c_7(7);
    EXPECT_TRUE(c_1.can_be_played_on((&c_1)));
    EXPECT_TRUE(c_2.can_be_played_on((&c_1)));
    EXPECT_TRUE(c_1.can_be_played_on((&c_7)));

    EXPECT_FALSE(c_3.can_be_played_on((&c_1)));
    EXPECT_FALSE(c_4.can_be_played_on((&c_1)));
    EXPECT_FALSE(c_5.can_be_played_on((&c_1)));
    EXPECT_FALSE(c_6.can_be_played_on((&c_1)));
    EXPECT_FALSE(c_7.can_be_played_on((&c_1)));

    EXPECT_FALSE(c_2.can_be_played_on((&c_7)));
    EXPECT_FALSE(c_3.can_be_played_on((&c_7)));
    EXPECT_FALSE(c_4.can_be_played_on((&c_7)));
    EXPECT_FALSE(c_5.can_be_played_on((&c_7)));
    EXPECT_FALSE(c_6.can_be_played_on((&c_7)));
}

// Serialization and subsequent deserialization must yield the same object
TEST(CardTest, SerializationEquality) {
    card card_send(1);
    rapidjson::Document* json_send = card_send.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    card* card_recv = card::from_json(json_recv);
    EXPECT_EQ(card_send.get_id(), card_recv->get_id());
    EXPECT_EQ(card_send.get_value(), card_recv->get_value());
    delete card_recv;
}

// Deserializing an invalid string must throw a LamaException
TEST(CardTest, SerializationException) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(card::from_json(json), LamaException);
}
