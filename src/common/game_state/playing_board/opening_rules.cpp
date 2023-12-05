//
// Created by stephenlincon on 11.11.23.
//

#include "opening_rules.h"
#include "../../exceptions/gomoku_exception.h"

// for deserialization
const std::unordered_map<std::string, ruleset_type> opening_rules::_string_to_ruleset_type = {
        {"freestyle", ruleset_type::freestyle },
        {"swap2", ruleset_type::swap2 },
        {"swap_after_first_move", ruleset_type::swap_after_first_move },
        {"uninitialized", ruleset_type::uninitialized },
};

// for serialization
const std::unordered_map<ruleset_type, std::string> opening_rules::_ruleset_type_to_string = {
        { ruleset_type::freestyle, "freestyle" },
        { ruleset_type::swap2, "swap2"},
        { ruleset_type::swap_after_first_move, "swap_after_first_move"},
        { ruleset_type::uninitialized, "uninitialized"},
};

// Deserialization Constructor
opening_rules::opening_rules(std::string id, ruleset_type ruleset,
                             serializable_value<std::string>* description) :
        unique_serializable(id),
        _ruleset(ruleset),
        _description(description)
{  }

// Client Constructor
opening_rules::opening_rules(ruleset_type ruleset) : unique_serializable(_ruleset_type_to_string.at(ruleset)) {
    this->_ruleset = ruleset;
    this->_description = new serializable_value<std::string>(generate_description_from_ruleset(ruleset));
}

// Ruleset and Description Helper Function
std::string opening_rules::generate_description_from_ruleset(ruleset_type ruleset) {
    switch(ruleset) {
        case ruleset_type::freestyle:
            return "freestyle description placeholder";
        case ruleset_type::swap2:
            return "swap2 description placeholder";
        case ruleset_type::swap_after_first_move:
            return "swap_after_first_move description placeholder";
        case ruleset_type::uninitialized:
            return "uninitialized description placeholder";
        default:
            throw gomoku_exception("Failed to instantiate opening_rules object. Invalid ruleset name.");
    }
}

// Destructor
opening_rules::~opening_rules() {
    _description = nullptr;
}

#ifdef GOMOKU_SERVER
// Server Constructor
opening_rules::opening_rules(std::string id, ruleset_type ruleset) :
        unique_serializable(id)
{
    this->_ruleset = ruleset;
    this->_description = new serializable_value<std::string>(generate_description_from_ruleset(ruleset));
}

std::string opening_rules::get_game_id() {
    return _game_id;
}

void opening_rules::set_game_id(std::string game_id) {
    _game_id = game_id;
}
#endif

// Accessors
ruleset_type opening_rules::get_ruleset() const noexcept {
    return this->_ruleset;
}

std::string opening_rules::get_description() const noexcept {
    return this->_description->get_value();
}

#ifdef GOMOKU_SERVER
// state update functions
void opening_rules::set_opening_rule(const std::string& ruleset_string) {
    ruleset_type ruleset = _string_to_ruleset_type.at(ruleset_string);
    this->_ruleset = ruleset;
    this->_description = new serializable_value<std::string>(generate_description_from_ruleset(ruleset));
}
#endif

// Serialization
void opening_rules::write_into_json(rapidjson::Value &json,
                                    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    serializable_value<std::string> ruleset_string = serializable_value<std::string>(_ruleset_type_to_string.at(_ruleset));
    rapidjson::Value ruleset_val(rapidjson::kObjectType);
    ruleset_string.write_into_json(ruleset_val, allocator);
    json.AddMember("ruleset", ruleset_val, allocator);

    rapidjson::Value description_val(rapidjson::kObjectType);
    _description->write_into_json(description_val, allocator);
    json.AddMember("description", description_val, allocator);
}

opening_rules *opening_rules::from_json(const rapidjson::Value &json) {

    if (json.HasMember("id")
        && json.HasMember("ruleset")
        && json.HasMember("description"))
    {
        ruleset_type ruleset = _string_to_ruleset_type.at(serializable_value<std::string>::from_json(json["ruleset"].GetObject())->get_value());
        return new opening_rules(
                json["id"].GetString(),
                ruleset,
                serializable_value<std::string>::from_json(json["description"].GetObject()));
    } else {
        throw gomoku_exception("Failed to deserialize opening_rules from json. Required json entries were missing.");
    }
}