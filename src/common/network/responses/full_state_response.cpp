//
// Created by Manuel on 15.02.2021.
//

#include "full_state_response.h"

#include "../../exceptions/gomoku_exception.h"
#include "../../serialization/json_utils.h"

#ifdef GOMOKU_CLIENT
#include "../../../client/game_controller.h"
#endif

full_state_response::full_state_response(server_response::base_class_properties props, rapidjson::Value* state_json) :
        server_response(props),
        _state_json(state_json)
{ }

full_state_response::full_state_response(std::string game_id, const game_state& state) :
        server_response(server_response::create_base_class_properties(ResponseType::full_state_msg, game_id))
{
    this->_state_json = state.to_json();
}


void full_state_response::write_into_json(rapidjson::Value &json,
                                       rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    server_response::write_into_json(json, allocator);
    json.AddMember("state_json", *_state_json, allocator);
}

full_state_response *full_state_response::from_json(const rapidjson::Value& json) {
    if (json.HasMember("state_json")) {
        return new full_state_response(server_response::extract_base_class_properties(json),
                                       json_utils::clone_value(json["state_json"].GetObject()));
    } else {
        throw gomoku_exception("Could not parse full_state_response from json. state is missing.");
    }
}

full_state_response::~full_state_response() {
    if (_state_json != nullptr) {
        delete _state_json;
        _state_json = nullptr;
    }
}

rapidjson::Value* full_state_response::get_state_json() const {
    return _state_json;
}

#ifdef GOMOKU_CLIENT

void full_state_response::Process() const {
    try {
        game_state* state = game_state::from_json(*_state_json);
        game_controller::update_game_state(state);

    } catch(std::exception& e) {
        std::cerr << "Failed to extract game_state from full_state_response" << std::endl
                  << e.what() << std::endl;
    }
}

#endif
