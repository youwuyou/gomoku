//
// Created by Manuel on 15.02.2021.
//

#include "request_response.h"
#include "../../serialization/json_utils.h"
#include "../../exceptions/GomokuException.h"
#include "../../game_state/game_state.h"

#ifdef GOMOKU_CLIENT
#include "../../../client/GameController.h"
#endif


request_response::request_response(const server_response::base_class_properties& props, std::string req_id, const bool success, rapidjson::Value* state_json, std::string err) :
    server_response(props),
    _req_id(std::move(req_id)),
    _state_json(state_json),
    _success(success),
    _err(std::move(err))
{ }

request_response::request_response(const std::string& game_id, std::string req_id, bool success, rapidjson::Value* state_json, std::string err):
    server_response(server_response::create_base_class_properties(ResponseType::req_response, game_id)),
    _req_id(std::move(req_id)),
    _state_json(state_json),
    _success(success),
    _err(std::move(err))
{ }


request_response::~request_response() {
    if (_state_json != nullptr) {
        delete _state_json;
        _state_json = nullptr;
    }
}

void request_response::write_into_json(rapidjson::Value &json,
                                       rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    server_response::write_into_json(json, allocator);

    rapidjson::Value err_val(_err.c_str(), allocator);
    json.AddMember("err", err_val, allocator);

    rapidjson::Value req_id_val(_req_id.c_str(), allocator);
    json.AddMember("req_id", req_id_val, allocator);

    json.AddMember("success", _success, allocator);

    if (_state_json != nullptr) {
        json.AddMember("state_json", *_state_json, allocator);
    }
}


request_response *request_response::from_json(const rapidjson::Value& json) {
    if (json.HasMember("err") && json.HasMember("success")) {
        std::string err = json["err"].GetString();

        rapidjson::Value* state_json = nullptr;
        if (json.HasMember("state_json")) {
            state_json = json_utils::clone_value(json["state_json"].GetObject());
        }
        return new request_response(
                server_response::extract_base_class_properties(json),
                json["req_id"].GetString(),
                json["success"].GetBool(),
                state_json,
                err);
    } else {
        throw GomokuException("Could not parse request_response from json. err or success is missing.");
    }
}

#ifdef GOMOKU_CLIENT

void request_response::Process() const {
    if (_success) {
        if (this->_state_json != nullptr) {
            game_state* state = game_state::from_json(*_state_json);
            GameController::updateGameState(state);

        } else {
            GameController::showError("Network error", "Expected a state as JSON inside the request_response. But there was none.");
        }
    } else {
        GameController::showError("Warning!", _err);
    }

}

#endif
