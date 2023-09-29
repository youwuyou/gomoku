//
// Created by Manuel on 25.01.2021.
//

#include "discard_pile.h"
#include "../../serialization/vector_utils.h"
#include "../../exceptions/LamaException.h"


discard_pile::discard_pile(std::string id) : unique_serializable(id) { }

discard_pile::discard_pile(std::string id, std::vector<card *> &cards):
        unique_serializable(id),
        _cards(cards)
{ }

discard_pile::discard_pile() : unique_serializable() { }

discard_pile::~discard_pile() {
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}


bool discard_pile::can_play(const card *card)  {
    return _cards.empty() || card->can_be_played_on(_cards.back());
}

const card* discard_pile::get_top_card() const  {
    if (_cards.size() > 0) {
        return _cards.back();
    } else {
        return nullptr;
    }
}

#ifdef LAMA_SERVER
void discard_pile::setup_game(std::string &err) {
    // remove all cards (if any) and clear it
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}

bool discard_pile::try_play(const std::string& card_id, player* player, std::string& err) {
    card* played_card = nullptr;
    if (player->get_hand()->try_get_card(card_id, played_card)) {
        if (can_play(played_card)) {
            card* local_system_card;
            if (player->remove_card(played_card->get_id(), local_system_card, err)) {
                _cards.push_back(local_system_card);
                return true;
            } else {
                err = "Could not play card " + played_card->get_id() + " because player does not have this card.";
            }
        } else {
            err = "The desired card with value " + std::to_string(played_card->get_value())
                  + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
        }
    } else {
        err = "The player does not possess the card " + card_id + ", which was requested to be played.";
    }
    return false;
}

bool discard_pile::try_play(card* played_card, std::string& err) {
    if (can_play(played_card)) {
        _cards.push_back(played_card);
        return true;
    } else {
        err = "The desired card with value " + std::to_string(played_card->get_value())
              + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
    }
    return false;
}
#endif


discard_pile *discard_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("cards")) {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        return new discard_pile(json["id"].GetString(), deserialized_cards);
    } else {
        throw LamaException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }
}

void discard_pile::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}





