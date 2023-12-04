//
// Created by Manuel on 29.01.2021.
//

#include "swap_colour_request.h"

#ifdef GOMOKU_SERVER
#include "../../../server/game_instance_manager.h"
#include "../../../server/game_instance.h"
#endif

// Public constructor
swap_colour_request::swap_colour_request(std::string player_id, std::string game_id, swap_decision_type swap_decision)
        : client_request( client_request::create_base_class_properties(RequestType::swap_colour, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{
    _swap_decision = swap_decision;
}

// private constructor for deserialization
swap_colour_request::swap_colour_request(client_request::base_class_properties props, swap_decision_type swap_decision) :
        client_request(props),
        _swap_decision(swap_decision)
{ }

swap_colour_request* swap_colour_request::from_json(const rapidjson::Value &json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("swap_decision") ) {
        return new swap_colour_request(props, game_state::_string_to_swap_decision_type.at(json["swap_decision"].GetString()));
    } else {
        throw GomokuException("Could not find 'nof_cards' in swap_colour_request");
    }
}

void swap_colour_request::write_into_json(rapidjson::Value &json,
                                          rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    rapidjson::Value swap_decision_val(game_state::_swap_decision_type_to_string.at(static_cast<const swap_decision_type>(_swap_decision)), allocator);
    json.AddMember("swap_decision", swap_decision_val,allocator);
}
