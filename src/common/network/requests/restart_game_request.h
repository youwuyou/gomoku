//
// Created by Julius on 28.11.2023.
//

#ifndef GOMOKU_RESTART_GAME_REQUEST_H
#define GOMOKU_RESTART_GAME_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/serializable_value.h"

class restart_game_request : public client_request{

private:

    bool _change_ruleset;

    /*
     * Private constructor for deserialization
     */
    explicit restart_game_request(base_class_properties, bool change_ruleset);

public:
    restart_game_request(std::string game_id, std::string player_id, bool change_ruleset);
    [[nodiscard]] bool get_change_ruleset() const { return this->_change_ruleset; }

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static restart_game_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_RESTART_GAME_REQUEST_H
