//
// Created by Manuel on 15.02.2021.
//

#ifndef GOMOKU_REQUEST_RESPONSE_H
#define GOMOKU_REQUEST_RESPONSE_H

#include <string>
#include "server_response.h"


class request_response : public server_response {
private:
    bool _success;
    std::string _err;
    std::string _req_id;
    rapidjson::Value* _state_json = nullptr;

    request_response(base_class_properties props, std::string req_id, bool success, rapidjson::Value* state_json, std::string& err);

public:

    request_response(std::string game_id, std::string req_id, bool success, rapidjson::Value* state_json, std::string err);
    ~request_response();

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static request_response* from_json(const rapidjson::Value& json);

#ifdef GOMOKU_CLIENT
    virtual void Process() const override;
#endif
};


#endif //GOMOKU_REQUEST_RESPONSE_H
