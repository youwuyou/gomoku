//
// Created by Manuel on 27.01.2021.
//

#include "hand.h"

#include "../../exceptions/LamaException.h"
#include "../../serialization/vector_utils.h"

hand::hand() : unique_serializable() { }

hand::hand(std::string id) : unique_serializable(id) { }

// deserialization constructor
hand::hand(std::string id, std::vector<card*> cards) : unique_serializable(id) {
    this->_cards = cards;
}

hand::~hand() {
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards.at((i));
        _cards.at(i) = nullptr;
    }
    _cards.clear();
}

int hand::get_nof_cards() const {
    return _cards.size();
}

int hand::get_score() const {
    int res = 0;
    bool already_counted[7] = { false, false, false, false, false, false, false }; // for the 7 card types
    for (int i = 0; i < _cards.size(); i++) {
        int card_value = _cards[i]->get_value();
        if(!already_counted[card_value - 1]) {
            already_counted[card_value - 1] = true;
            if(card_value == 7) {
                res += 10;
            } else {
                res += card_value;
            }
        }
    }
    return res;
}

const std::vector<card*> hand::get_cards() const {
    return _cards;
}

bool hand::try_get_card(const std::string &card_id, card *&hand_card) const {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card_id](const card* x) { return x->get_id() == card_id;});
    if (it < _cards.end()) {
        hand_card = *it;
        return true;
    }
    return false;
}


card* hand::remove_card(int idx) {
    return remove_card(_cards.begin() + idx);
}

card* hand::remove_card(card* card) {
    auto pos = std::find(_cards.begin(), _cards.end(), card);
    return remove_card(pos);
}

card* hand::remove_card(std::vector<card*>::iterator pos) {
    if (pos >= _cards.begin() && pos < _cards.end()) {
        card* res = *pos;
        _cards.erase(pos);
        return res;
    }
    return nullptr;
}

std::vector<card*>::iterator hand::get_card_iterator() {
    return _cards.begin();
}


#ifdef LAMA_SERVER
void hand::setup_round(std::string &err) {
    // remove all cards (if any) and clear it
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}

bool hand::add_card(card* new_card, std::string &err) {
    _cards.push_back(new_card);
    return true;
}

bool hand::remove_card(std::string card_id, card*& played_card, std::string &err) {
    played_card = nullptr;
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card_id](const card* x) { return x->get_id() == card_id;});
    if (it < _cards.end()) {
        played_card = remove_card(it);
        return true;
    } else {
        err = "Could not play card, as the requested card was not on the player's hand.";
        return false;
    }
}
#endif


void hand::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}

hand *hand::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("cards")) {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        return new hand(json["id"].GetString(), deserialized_cards);
    } else {
        throw LamaException("Could not parse hand from json. 'cards' were missing.");
    }
}



