//
// Created by Julius on 30.10.2023.
//

#ifndef GOMOKU_STONE_H
#define GOMOKU_STONE_H

#include <string>
#include <vector>
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../serialization/vector_utils.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class stone : public unique_serializable {
private:
    /*
     * Colour definition
     * Black = False (0)
     * White = True (1)
     */
    serializable_value<bool>* _colour;
    serializable_value<int>* _position_x;
    serializable_value<int>* _position_y;

    // from_diff constructor
    stone(std::string id);
    // deserialization constructor
    stone(std::string id, serializable_value<bool>* colour, serializable_value<int>* position_x, serializable_value<int>* position_y);
public:
    stone(serializable_value<bool> colour, serializable_value<int> position_x, serializable_value<int> position_y);
    ~stone();

// accessors
    bool get_colour() const noexcept;
    serializable_value<int> get_position_x() const noexcept;
    serializable_value<int> get_position_y() const noexcept;

// serializable interface
    void write_into_json(rapidjson::Value &json,
                         rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator,
                         rapidjson::Document::AllocatorType& doc_allocator) const;
    static stone* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_STONE_H
