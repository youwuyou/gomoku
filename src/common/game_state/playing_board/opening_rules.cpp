//
// Created by stephenlincon on 11.11.23.
//

#include "opening_rules.h"
#include "../../exceptions/GomokuException.h"

// for deserialization
const std::unordered_map<std::string, RulesetType> opening_rules::_string_to_ruleset_type = {
        {"freestyle", RulesetType::freestyle },
        {"swap2", RulesetType::swap2 },
        {"swap_after_first_move", RulesetType::swap_after_first_move },
        {"uninitialized", RulesetType::uninitialized },
};

// for serialization
const std::unordered_map<RulesetType, std::string> opening_rules::_ruleset_type_to_string = {
        { RulesetType::freestyle, "freestyle" },
        { RulesetType::swap2, "swap2"},
        { RulesetType::swap_after_first_move, "swap_after_first_move"},
        { RulesetType::uninitialized, "uninitialized"},
};

// Deserialization Constructor
opening_rules::opening_rules(std::string id, RulesetType ruleset,
                             serializable_value<std::string>* description) :
        unique_serializable(id),
        _ruleset(ruleset),
        _description(description)
{  }

// Client Constructor
opening_rules::opening_rules(RulesetType ruleset) : unique_serializable(_ruleset_type_to_string.at(ruleset)) {
    std::string generated_description = generate_description_from_ruleset(ruleset);
    this->_ruleset = ruleset;
    this->_description = new serializable_value<std::string>(generated_description);
}

// Ruleset and Description Helper Function
std::string opening_rules::generate_description_from_ruleset(RulesetType ruleset) {
    switch(ruleset) {
        case RulesetType::freestyle:
            return "freestyle description placeholder";
        case RulesetType::swap2:
            return "swap2 description placeholder";
        case RulesetType::swap_after_first_move:
            return "swap_after_first_move description placeholder";
        case RulesetType::uninitialized:
            return "uninitialized description placeholder";
        default:
            throw GomokuException("Failed to instantiate opening_rules object. Invalid ruleset name.");
    }
}

// Destructor
opening_rules::~opening_rules() {
    _description = nullptr;
}

#ifdef GOMOKU_SERVER
// Server Constructor
opening_rules::opening_rules(std::string id, RulesetType ruleset) :
        unique_serializable(id)
{
    std::string generated_description = generate_description_from_ruleset(ruleset);
    this->_ruleset = ruleset;
    this->_description = new serializable_value<std::string>(generated_description);
}

std::string opening_rules::get_game_id() {
    return _game_id;
}

void opening_rules::set_game_id(std::string game_id) {
    _game_id = game_id;
}
#endif

// Accessors
RulesetType opening_rules::get_ruleset() const noexcept {
    return this->_ruleset;
}

std::string opening_rules::get_description() const noexcept {
    return this->_description->get_value();
}

// WIP HERE
#ifdef GOMOKU_SERVER
// state update functions
void opening_rules::set_opening_rule(RulesetType ruleset) {
    std::string generated_description = generate_description_from_ruleset(ruleset);
    if (generated_description == "") {
        throw GomokuException("Failed to instantiate opening_rules object. Invalid ruleset name.");
    } else {
        this->_ruleset = new serializable_value<std::string>(ruleset);
        this->_description = new serializable_value<std::string>(generated_description);
    }
}
#endif

// Serialization
void opening_rules::write_into_json(rapidjson::Value &json,
                                    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    rapidjson::Value ruleset_val(rapidjson::kObjectType);
    _ruleset->write_into_json(ruleset_val, allocator);
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
        return new opening_rules(
                json["id"].GetString(),
                serializable_value<std::string>::from_json(json["ruleset"].GetObject()),
                serializable_value<std::string>::from_json(json["description"].GetObject()));
    } else {
        throw GomokuException("Failed to deserialize opening_rules from json. Required json entries were missing.");
    }
}