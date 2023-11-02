//
// Created by Manuel on 29.01.2021.
//

#ifndef GOMOKU_SELECT_GAME_MODE_REQUEST_H
#define GOMOKU_SELECT_GAME_MODE_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class select_game_mode_request : public client_request{

private:

    /*
     * Private constructor for deserialization
     */
    explicit select_game_mode_request(base_class_properties);

public:
    select_game_mode_request(std::string game_id, std::string player_id);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static select_game_mode_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_SELECT_GAME_MODE_REQUEST_H
