//
// Created by Julius on 28.11.2023.
//

#include "restart_game_request.h"

// Public constructor
restart_game_request::restart_game_request(std::string player_id, std::string game_id, bool change_ruleset)
        : client_request( client_request::create_base_class_properties(RequestType::restart_game, uuid_generator::generate_uuid_v4(), player_id, game_id) ),
        _change_ruleset(change_ruleset)
{ }

// private constructor for deserialization
restart_game_request::restart_game_request(client_request::base_class_properties props, bool change_ruleset) :
        client_request(props),
        _change_ruleset(change_ruleset)
{ }

restart_game_request* restart_game_request::from_json(const rapidjson::Value &json) {
    return new restart_game_request(client_request::extract_base_class_properties((json)), serializable_value<bool>::from_json(json["change_ruleset"].GetObject())->get_value());
}

void restart_game_request::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    serializable_value<bool>* change_ruleset_serial = new serializable_value<bool>(_change_ruleset);
    client_request::write_into_json(json, allocator);
    rapidjson::Value change_ruleset_val(rapidjson::kObjectType);
    change_ruleset_serial->write_into_json(change_ruleset_val, allocator);
    json.AddMember("change_ruleset", change_ruleset_val,allocator);
}