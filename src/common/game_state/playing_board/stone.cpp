//
// Created by Julius on 30.10.2023.
//

#include "stone.h"

#include "../../exceptions/GomokuException.h"


stone::stone(std::string id) : unique_serializable(id) { }

stone::stone(std::string id, serializable_value<std::string>* colour, serializable_value<int>* position_x, serializable_value<int>* position_y)
        : unique_serializable(id), _colour(colour), _position_x(position_x), _position_y(position_y)
{ }

stone::stone(serializable_value<std::string> colour, serializable_value<int> position_x, serializable_value<int> position_y) :
        unique_serializable(),
        _colour(new serializable_value<std::string>(colour)),
        _position_x(new serializable_value<int>(position_x)),
        _position_y(new serializable_value<int>(position_y))
{ }


stone::~stone() { }


std::string stone::get_colour() const noexcept {
    return _colour->get_value();
}

int stone::get_position_x() const noexcept {
    return _position_x->get_value();
}
int stone::get_position_y() const noexcept {
    return _position_y->get_value();
}

stone *stone::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("colour") && json.HasMember("position_x") && json.HasMember("position_y")) {
        return new stone(json["id"].GetString(),
                         serializable_value<std::string>::from_json(json["colour"].GetObject()),
                         serializable_value<int>::from_json(json["position_x"].GetObject()),
                         serializable_value<int>::from_json(json["position_y"].GetObject()));
    } else {
        throw GomokuException("Could not parse json of stone. Was missing 'id', 'val' or 'position'.");
    }
}


void stone::write_into_json(rapidjson::Value &json,
                            rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value colour_val(rapidjson::kObjectType);
    rapidjson::Value position_x_val(rapidjson::kObjectType);
    rapidjson::Value position_y_val(rapidjson::kObjectType);
    _colour->write_into_json(colour_val, allocator);
    _position_x->write_into_json(position_x_val, allocator);
    _position_y->write_into_json(position_y_val, allocator);
    json.AddMember("colour", colour_val, allocator);
    json.AddMember("position_x", position_x_val, allocator);
    json.AddMember("position_y", position_y_val, allocator);
}