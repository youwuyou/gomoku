//
// Created by Manuel on 28.01.2021.
//

#ifndef GOMOKU_PLAY_STONE_REQUEST_H
#define GOMOKU_PLAY_STONE_REQUEST_H


#include "client_request.h"
#include <string>
#include "../../../../rapidjson/include/rapidjson/document.h"

class play_stone_request : public client_request {

private:

    std::string _stone_id;

    //Private constructor for deserialization
    play_stone_request(base_class_properties, std::string stone_id);

public:
    [[nodiscard]] std::string get_stone_id() const { return this->_stone_id; }

    play_card_request(std::string game_id, std::string player_id, std::string stone_id_id);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static play_stone_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_PLAY_STONE_REQUEST_H
