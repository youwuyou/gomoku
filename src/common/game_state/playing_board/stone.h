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
     * Colour Definition: "black" and "white"
     * all lowercase
     */
    serializable_value<std::string>* _colour;
    serializable_value<int>* _position_x;
    serializable_value<int>* _position_y;

    // from_diff constructor
    stone(std::string id);
    // deserialization constructor
    stone(std::string id, serializable_value<std::string>* colour, serializable_value<int>* position_x, serializable_value<int>* position_y);
public:
    stone(serializable_value<std::string> colour, serializable_value<int> position_x, serializable_value<int> position_y);
    ~stone();

// accessors
    std::string get_colour() const noexcept;
    int get_position_x() const noexcept;
    int get_position_y() const noexcept;

// serializable interface
    void write_into_json(rapidjson::Value &json,
                         rapidjson::Document::AllocatorType& doc_allocator) const override;
    static stone* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_STONE_H
