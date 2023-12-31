//
// Created by Manuel on 25.01.2021.
//

#include "player.h"

#include "../../exceptions/gomoku_exception.h"

player::player(std::string name, player_colour_type colour) : unique_serializable() {
    this->_player_name = new serializable_value<std::string>(name);
    this->_colour = colour;
    this->_score = new serializable_value<int>(0);
}

// deserialisation constructor
player::player(std::string id, serializable_value<std::string>* name,
               serializable_value<int>* score, player_colour_type colour) :
        unique_serializable(id),
        _player_name(name),
        _score(score),
        _colour(colour)
{  }

player::~player() {
    if (_player_name != nullptr) {
        delete _player_name;
        delete _score;

        _player_name = nullptr;
        _score = nullptr;
    }
}

#ifdef GOMOKU_SERVER
player::player(std::string id, std::string name, player_colour_type colour) :
        unique_serializable(std::move(id))
{
    this->_player_name = new serializable_value<std::string>(std::move(name));
    this->_colour = colour;
    this->_score = new serializable_value<int>(0);
}

std::string player::get_game_id() {
    return _game_id;
}

void player::set_game_id(std::string game_id) {
    _game_id = std::move(game_id);
}
#endif

std::string player::get_player_name() const noexcept {
    return this->_player_name->get_value();
}

int player::get_score() const noexcept {
    return _score->get_value();
}

player_colour_type player::get_colour() const noexcept {
    return this->_colour;
}

// for deserialisation
const std::unordered_map<std::string, player_colour_type> player::_string_to_player_colour_type = {
        {"black", player_colour_type::black},
        {"white", player_colour_type::white},
};

//for serialisation
const std::unordered_map<player_colour_type, std::string> player::_player_colour_type_to_string = {
        {player_colour_type::black, "black"},
        {player_colour_type::white, "white"},
};

#ifdef GOMOKU_SERVER

void player::increment_score(std::string& err){
    int current_score = this->_score->get_value();
    this->_score->set_value(current_score + 1);
}

bool player::reset_score(std::string& err){
    this->_score->set_value(0);
    return true;
}

void player::change_colour(std::string& err) {
    if (this->_colour == player_colour_type::black) {
        this->_colour = player_colour_type::white;
    } else if (this->_colour == player_colour_type::white) {
        this->_colour = player_colour_type::black;
    } else {
        throw gomoku_exception("Failed to swap player colour. Unknown player colour was given.");
    }
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

    serializable_value<std::string> colour_string = serializable_value<std::string>(_player_colour_type_to_string.at(_colour));
    rapidjson::Value colour_val(rapidjson::kObjectType);
    colour_string.write_into_json(colour_val, allocator);
    json.AddMember("colour", colour_val, allocator);
}


player *player::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("player_name")
        && json.HasMember("score")
        && json.HasMember("colour"))
    {
        player_colour_type colour = _string_to_player_colour_type.at(serializable_value<std::string>::from_json(json["colour"].GetObject())->get_value());
        return new player(
                json["id"].GetString(),
                serializable_value<std::string>::from_json(json["player_name"].GetObject()),
                serializable_value<int>::from_json(json["score"].GetObject()),
                colour);
    } else {
        throw gomoku_exception("Failed to deserialize player from json. Required json entries were missing.");
    }
}
