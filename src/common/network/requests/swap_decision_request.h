//
// Created by Manuel on 29.01.2021.
//

#ifndef GOMOKU_SWAP_DECISION_REQUEST_H
#define GOMOKU_SWAP_DECISION_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../game_state/game_state.h"

class swap_decision_request : public client_request {

private:
     swap_decision_type _swap_decision;

    /*
     * Private constructor for deserialization
     */
    swap_decision_request(base_class_properties, swap_decision_type swap_decision);

public:

    [[nodiscard]] swap_decision_type get_swap_decision() const { return this->_swap_decision; }

    swap_decision_request(std::string game_id, std::string player_id, swap_decision_type swap_decision);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static swap_decision_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_SWAP_DECISION_REQUEST_H
