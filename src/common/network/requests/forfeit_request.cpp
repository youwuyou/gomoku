//
// Created by Julius on 13.12.2023.
//

#include "forfeit_request.h"

// Public constructor
forfeit_request::forfeit_request(std::string player_id, std::string game_id)
        : client_request( client_request::create_base_class_properties(request_type::forfeit, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{ }

// private constructor for deserialization
forfeit_request::forfeit_request(client_request::base_class_properties props) :
        client_request(props)
{ }

forfeit_request* forfeit_request::from_json(const rapidjson::Value &json) {
    return new forfeit_request(client_request::extract_base_class_properties((json)));
}

void forfeit_request::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
}