//
// Created by Manuel on 27.01.2021.
//

#include "game_state.h"

#include "../exceptions/gomoku_exception.h"
#include "../serialization/vector_utils.h"
#include "playing_board/playing_board.h"

// for deserialization
const std::unordered_map<std::string, swap_decision_type> game_state::_string_to_swap_decision_type = {
        {"do_swap", swap_decision_type::do_swap },
        {"do_not_swap", swap_decision_type::do_not_swap },
        {"defer_swap", swap_decision_type::defer_swap },
        {"no_decision_yet", swap_decision_type::no_decision_yet },
        {"deferred_do_swap", swap_decision_type::deferred_do_swap },
        {"deferred_do_not_swap", swap_decision_type::deferred_do_not_swap },
};

// for serialization
const std::unordered_map<swap_decision_type, std::string> game_state::_swap_decision_type_to_string = {
        { swap_decision_type::do_swap, "do_swap" },
        { swap_decision_type::do_not_swap, "do_not_swap"},
        { swap_decision_type::defer_swap, "defer_swap"},
        { swap_decision_type::no_decision_yet, "no_decision_yet"},
        { swap_decision_type::deferred_do_swap, "deferred_do_swap"},
        { swap_decision_type::deferred_do_not_swap, "deferred_do_not_swap"},
};

game_state::game_state() : unique_serializable() {
    this->_players = std::vector<player*>();
    this->_playing_board = new playing_board();
    this->_opening_ruleset = new opening_rules(ruleset_type::uninitialized);
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_is_tied = new serializable_value<bool>(false);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_starting_player_idx = new serializable_value<int>(0);
    this->_turn_number = new serializable_value<int>(0);
    this->_swap_next_turn = new serializable_value<bool>(false);
    this->_swap_decision = swap_decision_type::no_decision_yet;

}

// deserialization constructor
game_state::game_state (std::string id, playing_board* _playing_board, opening_rules* _opening_ruleset,
                        std::vector<player *> &players, serializable_value<bool> *is_started,
                        serializable_value<bool> *is_finished, serializable_value<bool> *is_tied,
                        serializable_value<int> *current_player_idx, serializable_value<int> *starting_player_idx,
                        serializable_value<int>* turn_number,
                        serializable_value<bool>* swap_next_turn, swap_decision_type swap_decision)
        : unique_serializable(id),
        _playing_board(_playing_board),
        _opening_ruleset(_opening_ruleset),
        _players(players),
        _is_started(is_started),
        _is_finished(is_finished),
        _is_tied(is_tied),
        _current_player_idx(current_player_idx),
        _starting_player_idx(starting_player_idx),
        _turn_number(turn_number),
        _swap_next_turn(swap_next_turn),
        _swap_decision(swap_decision)
{  }

game_state::game_state(std::string id) : unique_serializable(id) {
    this->_playing_board = new playing_board();
    this->_opening_ruleset = new opening_rules(ruleset_type::uninitialized);
    this->_players = std::vector<player*>();
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_is_tied = new serializable_value<bool>(false);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_starting_player_idx = new serializable_value<int>(0);
    this->_turn_number = new serializable_value<int>(0);
    this->_swap_next_turn = new serializable_value<bool>(false);
    this->_swap_decision = swap_decision_type::no_decision_yet;
}

game_state::~game_state() {
    if (_is_started != nullptr) {
        delete _is_started;
        delete _is_finished;
        delete _is_tied;
        delete _playing_board;
        delete _opening_ruleset;
        delete _current_player_idx;
        delete _starting_player_idx;
        delete _turn_number;
        delete _swap_next_turn;

        _is_started = nullptr;
        _is_finished = nullptr;
        _is_tied = nullptr;
        _playing_board = nullptr;
        _opening_ruleset = nullptr;
        _current_player_idx = nullptr;
        _starting_player_idx = nullptr;
        _turn_number = nullptr;
        _swap_next_turn = nullptr;
    }
}

// accessors
player* game_state::get_current_player() const {
    if (_current_player_idx == nullptr || _players.size() == 0) {
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

bool game_state::is_tied() const {
    return _is_tied->get_value();
}

int game_state::get_turn_number() const {
    return _turn_number->get_value();
}

int game_state::get_starting_player_idx() const {
    return _starting_player_idx->get_value();
}

std::vector<std::vector<field_type>> game_state::get_playing_board() const{
    return _playing_board->get_playing_board();
}

opening_rules* game_state::get_opening_rules() const {
    return _opening_ruleset;
}

bool game_state::get_swap_next_turn() const {
    return _swap_next_turn->get_value();
}

swap_decision_type game_state::get_swap_decision() const {
    return _swap_decision;
}

int game_state::get_player_index(player *player) const {
    auto it = std::find(_players.begin(), _players.end(), player);
    if (it == _players.end()) {
        return -1;
    } else {
        return it - _players.begin();
    }
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
    this->_is_finished->set_value(false);
    this->_playing_board->setup_round(err);
    this->_turn_number->set_value(0);
    this->_swap_decision = swap_decision_type::no_decision_yet;
    if (this->_current_player_idx->get_value() != this->_starting_player_idx->get_value()){
        this->_current_player_idx->set_value(this->_starting_player_idx->get_value());
    }
    // set starting player colour as black, other player colour as white
    if(this->_players.at(this->_starting_player_idx->get_value())->get_colour() != player_colour_type::black){
        this->_players.at(0)->change_colour(err);
        this->_players.at(1)->change_colour(err);
    }
}

void game_state::wrap_up_round(std::string& err) {
    if(!this->is_tied()){
        this->get_current_player()->increment_score(err);
    }
    this->switch_starting_player(err);
    this->_is_started->set_value(false);
    this->_is_finished->set_value(true);
}

bool game_state::update_current_player(std::string& err) {
    bool result;
    int current_turn_val = this->get_turn_number();

    switch(_opening_ruleset->get_ruleset()) {
        case freestyle:
            result = alternate_current_player(err);
            break;
        case swap_after_first_move:
            switch(current_turn_val) {
                case 0:
                    _swap_next_turn->set_value(true);
                    result = alternate_current_player(err);
                    break;
                case 1:
                    switch(_swap_decision) {
                        case swap_decision_type::do_swap:
                            result = execute_swap(err);
                            break;
                        case swap_decision_type::do_not_swap:
                            _swap_next_turn->set_value(false);
                            result = true;
                            break;
                        default:
                            result = false;
                            break;
                    }
                    break;
                default:
                    result = alternate_current_player(err);
                    break;
            }
            break;
        case swap2:
            switch(current_turn_val) {
                case 0:
                    result = true;
                    break;
                case 1:
                    this->get_current_player()->change_colour(err);
                    result = true;
                    break;
                case 2:
                    this->get_current_player()->change_colour(err);
                    _swap_next_turn->set_value(true);
                    result = alternate_current_player(err);
                    break;
                case 3:
                    switch(_swap_decision) {
                        case swap_decision_type::do_swap:
                            result = execute_swap(err);
                            break;
                        case swap_decision_type::do_not_swap:
                            _swap_next_turn->set_value(false);
                            result = true;
                            break;
                        case swap_decision_type::defer_swap:
                            _swap_next_turn->set_value(false);
                            result = true;
                            break;
                        default:
                            result = false;
                            break;
                    }
                    break;
                default:
                    // If swap has not been deferred
                    if (_swap_decision == do_swap || _swap_decision == do_not_swap) {
                        result = alternate_current_player(err);
                        break;
                    // If swap has been deferred
                    } else if (_swap_decision == defer_swap
                        || _swap_decision == deferred_do_swap
                        || _swap_decision == deferred_do_not_swap) {
                        switch(current_turn_val) {
                            case 4:
                                this->get_current_player()->change_colour(err);
                                result = true;
                                break;
                            case 5:
                                this->get_current_player()->change_colour(err);
                                _swap_next_turn->set_value(true);
                                result = alternate_current_player(err);
                                break;
                            case 6:
                                switch (_swap_decision) {
                                    case swap_decision_type::deferred_do_swap:
                                        result = execute_swap(err);
                                        break;
                                    case swap_decision_type::deferred_do_not_swap:
                                        _swap_next_turn->set_value(false);
                                        result = true;
                                        break;
                                    default:
                                        return false;
                                }
                            default:
                                result = alternate_current_player(err);
                                break;
                        }
                    } else {
                        result = false;
                        break;
                    }
            }
            break;
        default:
            throw gomoku_exception("Failed to update current player. Invalid ruleset name.");
    }
    return result;
}

// Helper function of update_current_player, which makes the other player the new current player
bool game_state::alternate_current_player(std::string& err) {
    if (_current_player_idx->get_value() == 0){
        _current_player_idx->set_value(1);
        return true;
    } else if (_current_player_idx->get_value() == 1) {
        _current_player_idx->set_value(0);
        return true;
    } else {
        err = "Invalid current player index for player index update.";
        return false;
    }
}

// Helper function of update_current_player, which swaps the colours of both players
bool game_state::execute_swap(std::string& err) {
    _players.at(0)->change_colour(err);
    _players.at(1)->change_colour(err);
    _swap_next_turn->set_value(false);
    bool res = alternate_current_player(err);
    return res;
}

void game_state::iterate_turn() {
    this->_turn_number->set_value(this->get_turn_number()+1);
}

bool game_state::determine_swap_decision(swap_decision_type swap_decision, std::string &err) {
    if (swap_decision == do_swap || swap_decision == do_not_swap || swap_decision == defer_swap) {
        // If swap has been deferred in a prior turn, set _swap_decision to the deferred enums after second swap
        if (_swap_decision == defer_swap) {
            if (swap_decision == do_swap) {
                _swap_decision = deferred_do_swap;
                return true;
            } else if (swap_decision == do_not_swap) {
                _swap_decision = deferred_do_not_swap;
                return true;
            }
        }
        // Otherwise, set _swap_decision directly as the enum parameter
        _swap_decision = swap_decision;
        return true;
    }
    err = "GameState: Unable to carry out swap decision";
    return false;
}

bool game_state::switch_starting_player(std::string& err) {
    if (_starting_player_idx->get_value() == 0){
        _starting_player_idx->set_value(1);
        return true;
    } else if (_starting_player_idx->get_value() == 1) {
        _starting_player_idx->set_value(0);
        return true;
    } else {
        err = "Invalid starting player index for player index update.";
        return false;
    }
}

bool game_state::prepare_game(player* player, std::string &err) {
    if(_players.at(_starting_player_idx->get_value()) != player){
        this->switch_starting_player(err);
    }
    //if(_players.at(_current_player_idx->get_value())->get_colour() != player_colour_type::black){
    //    this->_players.at(0)->change_colour(err);
    //    this->_players.at(1)->change_colour(err);
    //}
    _is_finished->set_value(false);
    return true;
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

bool game_state::set_game_mode(const std::string& rule_name, std::string& err) {
    this->_opening_ruleset->set_opening_rule(rule_name);
    return true;
}

bool game_state::place_stone(unsigned int x, unsigned int y, field_type colour, std::string& err) {
    if (this->_playing_board->place_stone(x, y, colour, err)) {
        return true;
    }
    err = "GameState: Unable to place stone.";
    return false;
}

bool game_state::check_win_condition(unsigned int x, unsigned int y, int colour) {
    /*
     * order of directions in vector
     *   0  1  2
     *   3     4
     *   5  6  7
     */
    std::vector<unsigned int> stones_in_directions;
    for (int i = -1; i<2; ++i) {
        for (int j = -1; j<2; ++j) {
            if (i != 0 || j != 0) {
                stones_in_directions.push_back(count_stones_one_direction(x, y, i, j, colour));
            }
        }
    }
    if (stones_in_directions.at(0)+stones_in_directions.at(7)+1 >= 5 ||
        stones_in_directions.at(1)+stones_in_directions.at(6)+1 >= 5 ||
        stones_in_directions.at(2)+stones_in_directions.at(5)+1 >= 5 ||
        stones_in_directions.at(3)+stones_in_directions.at(4)+1 >= 5) {
        return true;
    } else {
        return false;
    }
}

// recursive function to find the number of same-colour stones in a direction from a given location
unsigned int game_state::count_stones_one_direction(unsigned int x, unsigned int y, int direction_x, int direction_y, int colour) {
    // edge checking
    if (x == 0 && direction_x == -1 ||
        y == 0 && direction_y == -1 ||
        x == 14 && direction_x == 1 ||
        y == 14 && direction_y == 1){
        return 0;
    } else {
        int next_stone_colour = _playing_board->get_playing_board().at(y + direction_y).at(x + direction_x);
        //return zero if we have reached the end of the line
        if (next_stone_colour != colour) {
            return 0;
        } else {
            return 1 +
                   game_state::count_stones_one_direction(x + direction_x, y + direction_y, direction_x, direction_y,
                                                          colour);
        }
    }
}

bool game_state::check_for_tie(){
    int playing_board_size = playing_board::_playing_board_size;
    for(int i = 0; i<playing_board_size; ++i){
        for(int j = 0; j<playing_board_size; ++j){
            if(this->get_playing_board().at(i).at(j) == field_type::empty){
                return false;
            }
        }
    }
    this->_is_tied->set_value(true);
    return true;
}

#endif


// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value is_started_val(rapidjson::kObjectType);
    _is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    _is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    rapidjson::Value is_tied_val(rapidjson::kObjectType);
    _is_tied->write_into_json(is_tied_val, allocator);
    json.AddMember("is_tied", is_tied_val, allocator);

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

    rapidjson::Value swap_next_turn_val(rapidjson::kObjectType);
    _swap_next_turn->write_into_json(swap_next_turn_val, allocator);
    json.AddMember("swap_next_turn", swap_next_turn_val, allocator);

    serializable_value<std::string> swap_decision_string = serializable_value<std::string>(_swap_decision_type_to_string.at(_swap_decision));
    rapidjson::Value swap_decision_val(rapidjson::kObjectType);
    swap_decision_string.write_into_json(swap_decision_val, allocator);
    json.AddMember("swap_decision", swap_decision_val, allocator);
}

game_state* game_state::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("is_started")
        && json.HasMember("is_finished")
        && json.HasMember("is_tied")
        && json.HasMember("current_player_idx")
        && json.HasMember("starting_player_idx")
        && json.HasMember("turn_number")
        && json.HasMember("players")
        && json.HasMember("playing_board")
        && json.HasMember("opening_ruleset")
        && json.HasMember("swap_next_turn")
        && json.HasMember("swap_decision"))
    {

        std::vector<player*> deserialized_players;
        for (auto &serialized_player : json["players"].GetArray()) {
            deserialized_players.push_back(player::from_json(serialized_player.GetObject()));
        }
        swap_decision_type swap_decision = _string_to_swap_decision_type.at(serializable_value<std::string>::from_json(json["swap_decision"].GetObject())->get_value());
        return new game_state(json["id"].GetString(),
                              playing_board::from_json(json["playing_board"].GetObject()),
                              opening_rules::from_json(json["opening_ruleset"].GetObject()),
                              deserialized_players,
                              serializable_value<bool>::from_json(json["is_started"].GetObject()),
                              serializable_value<bool>::from_json(json["is_finished"].GetObject()),
                              serializable_value<bool>::from_json(json["is_tied"].GetObject()),
                              serializable_value<int>::from_json(json["current_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["starting_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["turn_number"].GetObject()),
                              serializable_value<bool>::from_json(json["swap_next_turn"].GetObject()),
                              swap_decision);
    } else {
        throw gomoku_exception("Failed to deserialize game_state. Required entries were missing.");
    }
}

