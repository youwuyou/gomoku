//
// Created by Manuel on 03.02.2021.
//
// Used to serialize game_state objects that need to be identifiable by a unique id.

#ifndef LAMA_UNIQUE_SERIALIZABLE_H
#define LAMA_UNIQUE_SERIALIZABLE_H


#include "serializable.h"

class unique_serializable : public serializable {
protected:

    std::string _id;       // unique identifier

    unique_serializable();
    unique_serializable(std::string id);

public:
// accessors
    std::string get_id() const;

// serializable interface
    virtual void write_into_json(rapidjson::Value& json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const override;

};

#endif //LAMA_UNIQUE_SERIALIZABLE_H
