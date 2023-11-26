//
// Created by Manuel on 28.01.2021.
//

#ifndef GOMOKU_PLACE_STONE_REQUEST_H
#define GOMOKU_PLACE_STONE_REQUEST_H


#include "client_request.h"
#include <string>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../game_state/playing_board/playing_board.h"

class place_stone_request : public client_request {

private:

    unsigned int _x;
    unsigned int _y;
    int _colour;

    //Private constructor for deserialization
    place_stone_request(base_class_properties, unsigned int x, unsigned int y, field_type colour);

public:
    place_stone_request(std::string player_id, std::string game_id, unsigned int x, unsigned int y, int colour);
    [[nodiscard]] unsigned int get_stone_x() const { return this->_x; }
    [[nodiscard]] unsigned int get_stone_y() const { return this->_y; }
    [[nodiscard]] int get_stone_colour() const { return this->_colour; }

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static place_stone_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_PLACE_STONE_REQUEST_H
