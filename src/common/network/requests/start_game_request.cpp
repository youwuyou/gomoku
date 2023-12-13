//
// Created by Manuel on 29.01.2021.
//

#include "start_game_request.h"

#ifdef GOMOKU_SERVER
#include "../../../server/game_instance_manager.h"
#include "../../../server/game_instance.h"
#endif

// Public constructor
start_game_request::start_game_request(std::string game_id, std::string player_id)
        : client_request( client_request::create_base_class_properties(request_type::start_game, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{ }

// private constructor for deserialization
start_game_request::start_game_request(client_request::base_class_properties props) :
        client_request(props)
{ }

start_game_request* start_game_request::from_json(const rapidjson::Value& json) {
    return new start_game_request(client_request::extract_base_class_properties(json));
}

void start_game_request::write_into_json(rapidjson::Value &json,
                                         rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
}
