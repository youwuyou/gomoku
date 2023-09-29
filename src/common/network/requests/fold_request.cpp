//
// Created by Manuel on 29.01.2021.
//

#include "fold_request.h"

// Public constructor
fold_request::fold_request(std::string game_id, std::string player_id)
        : client_request( client_request::create_base_class_properties(RequestType::fold, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{ }

// private constructor for deserialization
fold_request::fold_request(client_request::base_class_properties props) :
        client_request(props)
{ }

fold_request* fold_request::from_json(const rapidjson::Value &json) {
    return new fold_request(client_request::extract_base_class_properties((json)));
}

void fold_request::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
}