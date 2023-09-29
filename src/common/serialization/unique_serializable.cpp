//
// Created by Manuel on 19.02.2021.
//
// Used to serialize game_state objects that need to be identifiable by a unique id.

#include "unique_serializable.h"

#include "uuid_generator.h"
#include "../exceptions/LamaException.h"


unique_serializable::unique_serializable()
    : _id(uuid_generator::generate_uuid_v4())
{ }

unique_serializable::unique_serializable(std::string id)
    : _id(id)
{ }

std::string unique_serializable::get_id() const {
    return this->_id;
}

void unique_serializable::write_into_json(rapidjson::Value &json,
                                          rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

}
