//
// Created by Manuel on 28.01.2021.
//

#ifndef GOMOKU_PLACE_STONE_REQUEST_H
#define GOMOKU_PLACE_STONE_REQUEST_H


#include "client_request.h"
#include <string>
#include "../../../../rapidjson/include/rapidjson/document.h"

class place_stone_request : public client_request {

private:

    std::string _stone_id;

    //Private constructor for deserialization
    place_stone_request(base_class_properties, std::string stone_id);

public:
    place_stone_request(std::string player_id, std::string game_id, std::string stone_id);
    [[nodiscard]] std::string get_stone_id() const { return this->_stone_id; }

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static place_stone_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_PLACE_STONE_REQUEST_H
