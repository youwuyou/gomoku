//
// Created by Manuel on 01.02.2021.
//
// abstract class for a serializable object.

#ifndef LAMA_SERIALIZABLE_H
#define LAMA_SERIALIZABLE_H

#include "../../rapidjson/include/rapidjson/document.h"

class serializable {
public:

    virtual rapidjson::Document* to_json() const {
        rapidjson::Document* json = new rapidjson::Document();
        json->SetObject();
        this->write_into_json(*json, json->GetAllocator());
        return json;
    }

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const = 0;
};

#endif //LAMA_SERIALIZABLE_H
