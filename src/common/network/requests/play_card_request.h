//
// Created by Manuel on 28.01.2021.
//

#ifndef LAMA_PLAY_CARD_REQUEST_H
#define LAMA_PLAY_CARD_REQUEST_H


#include "client_request.h"
#include <string>
#include "../../../../rapidjson/include/rapidjson/document.h"

class play_card_request : public client_request {

private:

    std::string _card_id;

    //Private constructor for deserialization
    play_card_request(base_class_properties, std::string card_id);

public:
    [[nodiscard]] std::string get_card_id() const { return this->_card_id; }

    play_card_request(std::string game_id, std::string player_id, std::string card_id);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static play_card_request* from_json(const rapidjson::Value& json);
};


#endif //LAMA_PLAY_CARD_REQUEST_H
