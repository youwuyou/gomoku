//
// Created by stephenlincon on 11.11.23.
//

#include "opening_rules.h"
#include "../../exceptions/GomokuException.h"

// Deserialization Constructor
opening_rules::opening_rules(std::string id, serializable_value<std::string>* ruleset,
                             serializable_value<std::string>* description) :
        unique_serializable(id),
        _ruleset(ruleset),
        _description(description)
{ }

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

// Ruleset and Description Helper Function
std::string opening_rules::generate_description_from_ruleset(std::string ruleset) {
    if (ruleset == "freestyle") {
        return "freestyle description placeholder";
    } else if (ruleset == "swap2") {
        return "swap2 description placeholder";
    } else if (ruleset == "swap_after_first_move") {
        return "swap_after_first_move description placeholder";
    } else if (ruleset == "uninitialized") {
        return "uninitialized description placeholder";
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
// Server Constructor
opening_rules::opening_rules(std::string id, std::string ruleset) :
        unique_serializable(id)
{
    std::string generated_description = generate_description_from_ruleset(ruleset);
    if (generated_description == "") {
        throw GomokuException("Failed to instantiate opening_rules object. Invalid ruleset name.");
    } else {
        this->_ruleset = new serializable_value<std::string>(ruleset);
        this->_description = new serializable_value<std::string>(generated_description);
    }
}

std::string opening_rules::get_game_id() {
    return _game_id;
}

void opening_rules::set_game_id(std::string game_id) {
    _game_id = game_id;
}
#endif

// Accessors
std::string opening_rules::get_ruleset() const noexcept {
    return this->_ruleset->get_value();
}

std::string opening_rules::get_description() const noexcept {
    return this->_description->get_value();
}

#ifdef GOMOKU_SERVER
// state update functions
void opening_rules::set_opening_rule(std::string ruleset) {
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