//
// Created by Manuel on 27.01.2021.
//

#include "game_state.h"

#include "../exceptions/GomokuException.h"
#include "../serialization/vector_utils.h"
#include "playing_board/playing_board.h"

game_state::game_state() : unique_serializable() {
    this->_players = std::vector<player*>();
    this->_playing_board = new playing_board();
    this->_opening_ruleset = new opening_rules("uninitialized");
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_turn_number = new serializable_value<int>(0);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_starting_player_idx = new serializable_value<int>(0);
}

// deserialization constructor
game_state::game_state(std::string id, playing_board* _playing_board, opening_rules* _opening_ruleset,
                       std::vector<player *> &players, serializable_value<bool> *is_started,
                       serializable_value<bool> *is_finished, serializable_value<int> *current_player_idx,
                        serializable_value<int>* turn_number, serializable_value<int> *starting_player_idx)
        : unique_serializable(id),
        _playing_board(_playing_board),
        _opening_ruleset(_opening_ruleset),
        _players(players),
        _is_started(is_started),
        _is_finished(is_finished),
        _current_player_idx(current_player_idx),
        _turn_number(turn_number),
        _starting_player_idx(starting_player_idx)
    {  }

game_state::game_state(std::string id) : unique_serializable(id) {
    this->_playing_board = new playing_board();
    this->_opening_ruleset = new opening_rules("uninitialized");
    this->_players = std::vector<player*>();
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_turn_number = new serializable_value<int>(0);
    this->_starting_player_idx = new serializable_value<int>(0);
}

game_state::~game_state() {
    if (_is_started != nullptr) {
        delete _is_started;
        delete _is_finished;
        delete _playing_board;
        delete _opening_ruleset;
        delete _current_player_idx;
        delete _starting_player_idx;
        delete _turn_number;

        _is_started = nullptr;
        _is_finished = nullptr;
        _playing_board = nullptr;
        _opening_ruleset = nullptr;
        _current_player_idx = nullptr;
        _starting_player_idx = nullptr;
        _turn_number = nullptr;
    }
}

// accessors
player* game_state::get_current_player() const {
    if(_current_player_idx == nullptr || _players.size() == 0) {
        return nullptr;
    }
    return _players[_current_player_idx->get_value()];
}

bool game_state::is_full() const {
    return _players.size() == 2;
}

bool game_state::is_started() const {
    return _is_started->get_value();
}

bool game_state::is_finished() const {
    return _is_finished->get_value();
}

int game_state::get_turn_number() const {
    return _turn_number->get_value();
}

std::vector<std::vector<int>> game_state::get_playing_board() const{
    return _playing_board->get_playing_board();
}

int game_state::get_player_index(player *player) const {
    auto it = std::find(_players.begin(), _players.end(), player);
    if (it == _players.end()) {
        return -1;
    } else {
        return it - _players.begin();
    }
}

bool game_state::is_player_in_game(player *player) const {
    return std::find(_players.begin(), _players.end(), player) < _players.end();
}

bool game_state::is_allowed_to_play_now(player *player) const {
    return player == get_current_player();
}

std::vector<player*>& game_state::get_players() {
    return _players;
}


#ifdef GOMOKU_SERVER

// state modification functions without diff
void game_state::setup_round(std::string &err) {
    this->_playing_board->setup_round(err);
    this->_players.at(0)->swap_colour(err);
}

void game_state::wrap_up_round(std::string& err) {
    // TODO: Implementation Code
}

void game_state::update_current_player(std::string& err) {
    if(_current_player_idx->get_value() == 0){
        _current_player_idx->set_value(1);
    } else if (_current_player_idx->get_value() == 0){
        _current_player_idx->set_value(0);
    } else {
        err = "Invalid current player index for player index update.";
    }
}

bool game_state::start_game(std::string &err) {
    if (_players.size() < 2) {
        err = "You need at least " + std::to_string(2) + " players to start the game.";
        return false;
    }

    if (!_is_started->get_value()) {
        this->setup_round(err);
        this->_is_started->set_value(true);
        return true;
    } else {
        err = "Could not start game, as the game was already started";
        return false;
    }
}

bool game_state::remove_player(player *player_ptr, std::string &err) {
    int idx = get_player_index(player_ptr);
    if (idx != -1) {
        if (idx < _current_player_idx->get_value()) {
            // reduce current_player_idx if the player who left had a lower index
            _current_player_idx->set_value(_current_player_idx->get_value() - 1);
        }
        _players.erase(_players.begin() + idx);
        return true;
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
}

bool game_state::add_player(player* player_ptr, std::string& err) {
    if (_is_started->get_value()) {
        err = "Could not join game, because the requested game is already started.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not join game, because the requested game is already finished.";
        return false;
    }
    if (_players.size() >= 2) {
        err = "Could not join game, because the max number of players is already reached.";
        return false;
    }
    if (std::find(_players.begin(), _players.end(), player_ptr) != _players.end()) {
        err = "Could not join game, because this player is already subscribed to this game.";
        return false;
    }

    _players.push_back(player_ptr);
    return true;
}

bool set_game_mode(std::string rule_name) {
    // TODO: Implementation Code
    return false;
}

bool place_stone(player* player, std::string& err) {
    // TODO: Implementation Code
    return false;
}

bool check_win_condition() {
    // TODO: Implementation Code
    return false;
}

#endif


// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    _is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    rapidjson::Value is_started_val(rapidjson::kObjectType);
    _is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    rapidjson::Value current_player_idx_val(rapidjson::kObjectType);
    _current_player_idx->write_into_json(current_player_idx_val, allocator);
    json.AddMember("current_player_idx", current_player_idx_val, allocator);

    rapidjson::Value starting_player_idx_val(rapidjson::kObjectType);
    _starting_player_idx->write_into_json(starting_player_idx_val, allocator);
    json.AddMember("starting_player_idx", starting_player_idx_val, allocator);

    rapidjson::Value turn_number_val(rapidjson::kObjectType);
    _turn_number->write_into_json(turn_number_val, allocator);
    json.AddMember("turn_number", turn_number_val, allocator);

    rapidjson::Value playing_board_val(rapidjson::kObjectType);
    _playing_board->write_into_json(playing_board_val, allocator);
    json.AddMember("playing_board", playing_board_val, allocator);

    rapidjson::Value opening_ruleset_val(rapidjson::kObjectType);
    _opening_ruleset->write_into_json(opening_ruleset_val, allocator);
    json.AddMember("opening_ruleset", opening_ruleset_val, allocator);

    json.AddMember("players", vector_utils::serialize_vector(_players, allocator), allocator);
}

game_state* game_state::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("is_finished")
        && json.HasMember("is_started")
        && json.HasMember("current_player_idx")
        && json.HasMember("turn_number")
        && json.HasMember("starting_player_idx")
        && json.HasMember("players")
        && json.HasMember("playing_board")
        && json.HasMember("opening_ruleset"))
    {

        std::vector<player*> deserialized_players;
        for (auto &serialized_player : json["players"].GetArray()) {
            deserialized_players.push_back(player::from_json(serialized_player.GetObject()));
        }

        return new game_state(json["id"].GetString(),
                              playing_board::from_json(json["playing_board"].GetObject()),
                              opening_rules::from_json(json["opening_ruleset"].GetObject()),
                              deserialized_players,
                              serializable_value<bool>::from_json(json["is_started"].GetObject()),
                              serializable_value<bool>::from_json(json["is_finished"].GetObject()),
                              serializable_value<int>::from_json(json["current_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["turn_number"].GetObject()),
                              serializable_value<int>::from_json(json["starting_player_idx"].GetObject()));
    } else {
        throw GomokuException("Failed to deserialize game_state. Required entries were missing.");
    }
}

