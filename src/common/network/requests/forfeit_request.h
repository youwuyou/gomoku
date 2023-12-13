//
// Created by Julius on 13.12.2023.
//

#ifndef GOMOKU_FORFEIT_REQUEST_H
#define GOMOKU_FORFEIT_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class forfeit_request : public client_request{

private:

    /*
     * Private constructor for deserialization
     */
    explicit forfeit_request(base_class_properties);

public:
    forfeit_request(std::string game_id, std::string player_id);

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static forfeit_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_FORFEIT_REQUEST_H
