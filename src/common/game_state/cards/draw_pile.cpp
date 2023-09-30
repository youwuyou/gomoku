//
// Created by Manuel on 25.01.2021.
//

#include "draw_pile.h"


#include "../../serialization/vector_utils.h"
#include "../../exceptions/GomokuException.h"


// deserialization constructor
draw_pile::draw_pile(std::string id, std::vector<card*> &cards)
        : unique_serializable(id),
          _cards(cards)
{ }

// from_diff constructor
draw_pile::draw_pile(std::string id) : unique_serializable(id) { }


draw_pile::draw_pile(std::vector<card*> &cards)
        : unique_serializable(), _cards(cards)
{ }


draw_pile::draw_pile() : unique_serializable() { }

draw_pile::~draw_pile() {
    for (card* & _card : _cards) {
        delete _card;
    }
    _cards.clear();
}


void draw_pile::shuffle() {
    std::shuffle(_cards.begin(), _cards.end(), std::mt19937(std::random_device()()));
}

bool draw_pile::is_empty() const noexcept  {
    return _cards.empty();
}

int draw_pile::get_nof_cards() const noexcept  {
    return _cards.size();
}


#ifdef GOMOKU_SERVER
void draw_pile::setup_game(std::string &err) {
    // remove all cards (if any) and add the change to the "cards" array_diff
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();

    // add a fresh set of cards
    for (int card_value = 1; card_value <= 7; card_value++) {
        for (int i = 0; i < 8; i ++) {
            _cards.push_back(new card(card_value));
        }
    }
    // shuffle them
    this->shuffle();
}

bool draw_pile::draw(player* player, card*& drawn_card, std::string& err)  {
    if (!_cards.empty()) {
        drawn_card = _cards.back();
        if (player->add_card(drawn_card, err)) {
            _cards.pop_back();
            return true;
        } else {
            drawn_card = nullptr;
        }
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return false;
}

card* draw_pile::remove_top(std::string& err) {
    card* drawn_card = nullptr;
    if (!_cards.empty()) {
        drawn_card = _cards.back();
        _cards.pop_back();
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return drawn_card;
}

#endif


void draw_pile::write_into_json(rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}


draw_pile *draw_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("cards")) {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        return new draw_pile(json["id"].GetString(), deserialized_cards);
    } else {
        throw GomokuException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }
}
