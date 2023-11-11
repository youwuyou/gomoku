//
// Created by Manuel on 25.01.2021.
//

#include "player.h"

#include "../../exceptions/GomokuException.h"

player::player(std::string name, bool colour) : unique_serializable() {
    this->_player_name = new serializable_value<std::string>(name);
    this->_colour = new serializable_value<bool>(colour);
    this->_score = new serializable_value<int>(0);
}

player::player(std::string id, serializable_value<std::string>* name,
               serializable_value<int>* score, serializable_value<bool>* colour) :
        unique_serializable(id),
        _player_name(name),
        _score(score),
        _colour(colour)
{ }

player::~player() {
    if (_player_name != nullptr) {
        delete _player_name;
        delete _score;
        delete _colour;

        _player_name = nullptr;
        _score = nullptr;
        _colour = nullptr;

    }
}

#ifdef GOMOKU_SERVER
player::player(std::string id, std::string name) :
        unique_serializable(id)
{
    this->_player_name = new serializable_value<std::string>(name);
    this->_colour = new serializable_value<bool>(colour);
    this->_score = new serializable_value<int>(0);
}

std::string player::get_game_id() {
    return _game_id;
}

void player::set_game_id(std::string game_id) {
    _game_id = game_id;
}
#endif

std::string player::get_player_name() const noexcept {
    return this->_player_name->get_value();
}

int player::get_score() const noexcept {
    return _score->get_value();
}

bool player::get_colour() const noexcept {
    return this->_colour->get_value();
}


#ifdef GOMOKU_SERVER
void player::setup_round(std::string& err) {
    _has_folded->set_value(false);
    _hand->setup_round(err);
}

void player::wrap_up_round(std::string &err) {
    int cards_value = _hand->get_score();
    int new_score = _score->get_value();
    if (cards_value > 0) {
        new_score = _score->get_value() + cards_value;
    } else {
        // The player got rid of all cards. Deduct 10 points
        new_score = std::max(0, _score->get_value() - 10);
    }
    _score->set_value(new_score);
}

bool player::fold(std::string &err) {
    if (has_folded()) {
        err = "This player has already folded.";
        return false;
    }
    _has_folded->set_value(true);
    return true;
}

bool player::add_card(card *card, std::string &err) {
    if (has_folded()) {
        err = "Player has already folded and is not allowed to draw any cards";
        return false;
    }
    return _hand->add_card(card, err);
}

bool player::remove_card(std::string card_id, card*& card, std::string &err) {
    card = nullptr;
    if (has_folded()) {
        err = "Player has already folded and is not allowed to play any cards";
        return false;
    }
    return _hand->remove_card(card_id, card, err);
}

#endif


void player::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    rapidjson::Value name_val(rapidjson::kObjectType);
    _player_name->write_into_json(name_val, allocator);
    json.AddMember("player_name", name_val, allocator);

    rapidjson::Value score_val(rapidjson::kObjectType);
    _score->write_into_json(score_val, allocator);
    json.AddMember("score", score_val, allocator);

    rapidjson::Value colour_val(rapidjson::kObjectType);
    _colour->write_into_json(colour_val, allocator);
    json.AddMember("colour", colour_val, allocator);
}


player *player::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("player_name")
        && json.HasMember("score")
        && json.HasMember("colour"))
    {
        return new player(
                json["id"].GetString(),
                serializable_value<std::string>::from_json(json["player_name"].GetObject()),
                serializable_value<int>::from_json(json["score"].GetObject()),
                serializable_value<bool>::from_json(json["colour"].GetObject()));
    } else {
        throw GomokuException("Failed to deserialize player from json. Required json entries were missing.");
    }
}
