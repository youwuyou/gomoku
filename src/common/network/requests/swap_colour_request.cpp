//
// Created by Manuel on 29.01.2021.
//

#include "swap_colour_request.h"

#ifdef GOMOKU_SERVER
#include "../../../server/game_instance_manager.h"
#include "../../../server/game_instance.h"
#endif

// Public constructor
swap_colour_request::swap_colour_request(std::string game_id, std::string player_id, int nof_cards)
        : client_request( client_request::create_base_class_properties(RequestType::swap_colour, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{
    _nof_cards = nof_cards;
}

// private constructor for deserialization
swap_colour_request::swap_colour_request(client_request::base_class_properties props, int nof_cards) :
        client_request(props),
        _nof_cards(nof_cards)
{ }

swap_colour_request* swap_colour_request::from_json(const rapidjson::Value &json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("nof_cards") ) {
        return new swap_colour_request(props, json["nof_cards"].GetInt());
    } else {
        throw GomokuException("Could not find 'nof_cards' in swap_colour_request");
    }
}

void swap_colour_request::write_into_json(rapidjson::Value &json,
                                          rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    json.AddMember("nof_cards", rapidjson::Value(this->_nof_cards),allocator);
}
