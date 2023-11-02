//
// Created by Manuel on 28.01.2021.
//

#include "play_stone_request.h"

// Public constructor
play_stone_request::play_stone_request(std::string game_id, std::string player_id, std::string stone_id)
        : client_request(client_request::create_base_class_properties(RequestType::play_stone, uuid_generator::generate_uuid_v4(), player_id, game_id) ),
        _stone_id(stone_id_id)
{ }

// private constructor for deserialization
play_stone_request::play_stone_request(client_request::base_class_properties props, std::string stone_id) :
        client_request(props),
        _stone_id(stone_id)
{ }

play_stone_request* play_stone_request::from_json(const rapidjson::Value& json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("stone_id")) {
        return new play_stone_request(props, json["stone_id"].GetString());
    } else {
        throw GomokuException("Could not find 'stone_id' or 'value' in play_card_request");
    }
}

void play_stone_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    rapidjson::Value stone_id_val(_stone_id.c_str(), allocator);
    json.AddMember("stone_id", stone_id_val,allocator);
}
