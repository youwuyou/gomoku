//
// Created by Manuel on 29.01.2021.
//

#ifndef GOMOKU_SWAP_COLOR_REQUEST_H
#define GOMOKU_SWAP_COLOR_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class swap_color_request : public client_request {

private:
    int _nof_cards;

    /*
     * Private constructor for deserialization
     */
    swap_color_request(base_class_properties, int val);

public:

    [[nodiscard]] int get_nof_cards() const { return this->_nof_cards; }

    swap_color_request(std::string game_id, std::string player_id, int nof_cards = 1);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static swap_color_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_SWAP_COLOR_REQUEST_H
