//
// Created by Manuel on 29.01.2021.
//

#include "select_game_mode_request.h"

// Public constructor
select_game_mode_request::select_game_mode_request(std::string player_id, std::string game_id, std::string ruleset_string)
        : client_request( client_request::create_base_class_properties(request_type::select_game_mode, uuid_generator::generate_uuid_v4(), player_id, game_id) ),
        _ruleset_string(ruleset_string)
{ }

// private constructor for deserialization
select_game_mode_request::select_game_mode_request(client_request::base_class_properties props, std::string ruleset_string) :
        client_request(props),
        _ruleset_string(ruleset_string)
{ }

select_game_mode_request* select_game_mode_request::from_json(const rapidjson::Value &json) {
    return new select_game_mode_request(client_request::extract_base_class_properties((json)), json["ruleset_string"].GetString());
}

void select_game_mode_request::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    rapidjson::Value ruleset_string_val(_ruleset_string, allocator);
    json.AddMember("ruleset_string", ruleset_string_val,allocator);
}