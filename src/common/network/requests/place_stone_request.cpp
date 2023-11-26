//
// Created by Manuel on 28.01.2021.
//

#include "place_stone_request.h"

// Public constructor
place_stone_request::place_stone_request(std::string player_id, std::string game_id, unsigned int x, unsigned int y, int colour)
        : client_request(client_request::create_base_class_properties(RequestType::place_stone, uuid_generator::generate_uuid_v4(), player_id, game_id) ),
        _x(x),
        _y(y),
        _colour(colour)
{ }

// private constructor for deserialization
place_stone_request::place_stone_request(client_request::base_class_properties props, unsigned int x, unsigned int y, field_type colour) :
        client_request(props),
        _x(x),
        _y(y),
        _colour(colour)
{ }

place_stone_request* place_stone_request::from_json(const rapidjson::Value& json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("x") && json.HasMember("y") && json.HasMember("colour")) {
        return new place_stone_request(props, std::stoul(json["x"].GetString()), std::stoul(json["y"].GetString()), playing_board::_string_to_field_type.at(json["colour"].GetString()));
    } else {
        throw GomokuException("Could not find 'x', 'y' or 'colour' in place_stone_request");
    }
}

void place_stone_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    rapidjson::Value x_val(std::to_string(_x), allocator);
    rapidjson::Value y_val(std::to_string(_y), allocator);
    rapidjson::Value colour_val(playing_board::_field_type_to_string.at(static_cast<const field_type>(_colour)), allocator);
    json.AddMember("x", x_val,allocator);
    json.AddMember("y", y_val,allocator);
    json.AddMember("colour", colour_val,allocator);
}
