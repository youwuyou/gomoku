//
// Created by Manuel on 28.01.2021.
//

#include "play_card_request.h"

// Public constructor
play_card_request::play_card_request(std::string game_id, std::string player_id, std::string card_id)
        : client_request(client_request::create_base_class_properties(RequestType::play_card, uuid_generator::generate_uuid_v4(), player_id, game_id) ),
        _card_id(card_id)
{ }

// private constructor for deserialization
play_card_request::play_card_request(client_request::base_class_properties props, std::string card_id) :
        client_request(props),
        _card_id(card_id)
{ }

play_card_request* play_card_request::from_json(const rapidjson::Value& json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("card_id")) {
        return new play_card_request(props, json["card_id"].GetString());
    } else {
        throw LamaException("Could not find 'card_id' or 'value' in play_card_request");
    }
}

void play_card_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    rapidjson::Value card_id_val(_card_id.c_str(), allocator);
    json.AddMember("card_id", card_id_val,allocator);
}
