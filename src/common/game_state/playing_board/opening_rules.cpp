//
// Created by stephenlincon on 11.11.23.
//

#include "opening_rules.h"
#include "../../exceptions/GomokuException.h"

// Client Constructor
opening_rules::opening_rules(std::string ruleset) : unique_serializable(ruleset) {
    std::string generated_description = generate_description_from_ruleset(ruleset);
    if (generated_description == "") {
        throw GomokuException("Failed to instantiate opening_rules object. Invalid ruleset name.");
    } else {
        this->_ruleset = new serializable_value<std::string>(ruleset);
        this->_description = new serializable_value<std::string>(generated_description);
    }
}

// Deserialization Constructor
opening_rules::opening_rules(std::string id, serializable_value<std::string>* ruleset,
                             serializable_value<std::string>* description) :
        unique_serializable(id),
        _ruleset(ruleset),
        _description(description)
{ }

// Ruleset and Description Helper Function
std::string opening_rules::generate_description_from_ruleset(std::string ruleset) const {
    if (ruleset == "freestyle") {
        return "freestyle description placeholder";
    } else if (ruleset == "swap2") {
        return "swap2 description placeholder";
    } else if (ruleset == "swap_after_first_move") {
        return "swap_after_first_move description placeholder";
    } else {
        return "";
    }
}

// Destructor
opening_rules::~opening_rules() {
    _ruleset = nullptr;
    _description = nullptr;
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