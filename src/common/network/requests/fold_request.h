//
// Created by Manuel on 29.01.2021.
//

#ifndef GOMOKU_FOLD_REQUEST_H
#define GOMOKU_FOLD_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class fold_request : public client_request{

private:

    /*
     * Private constructor for deserialization
     */
    explicit fold_request(base_class_properties);

public:
    fold_request(std::string game_id, std::string player_id);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static fold_request* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_FOLD_REQUEST_H
