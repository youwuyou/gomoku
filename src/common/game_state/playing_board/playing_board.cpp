//
// Created by Manuel on 27.01.2021.
//

#include "playing_board.h"

#include "../../exceptions/gomoku_exception.h"
#include "../../serialization/vector_utils.h"

playing_board::playing_board() : unique_serializable() {
    std::vector<field_type> row(_playing_board_size, field_type::empty);
    std::vector<std::vector<field_type>> playing_board (_playing_board_size, row);
    this->_playing_board = playing_board;
}

playing_board::playing_board(std::string id) : unique_serializable(id) {
    std::vector<field_type> row(_playing_board_size, field_type::empty);
    std::vector<std::vector<field_type>> playing_board (_playing_board_size, row);
    this->_playing_board = playing_board;
}

// deserialization constructor
playing_board::playing_board(std::string id, std::vector<std::vector<field_type>> playing_board) : unique_serializable(id) {
    this->_playing_board = playing_board;
}

playing_board::~playing_board() {
    _playing_board.clear();
}

void playing_board::reset() {
    for (int i = 0; i < _playing_board.size(); i++) {
        for (int j = 0; j < _playing_board.size(); j++) {
            _playing_board.at((i)).at((j)) = field_type::empty;
        }
    }
}

/*
 * Changes the value of a position on the board indicated by its x and y coordinates only if:
 *   - the requested field type is "black" or "white"
 *   - the stones coordinates are valid (smaller than the size of the board)
 *   - the spot is not occupied (== field_type::empty)
 */
bool playing_board::place_stone(const unsigned int x, const unsigned int y, field_type colour, std::string &err) {
    if (x < _playing_board_size && y < _playing_board_size && colour != field_type::empty) {
        if (this->_playing_board.at(y).at(x) == field_type::empty) {
            this->_playing_board.at(y).at(x) = colour;
            return true;
        } else {
            err = "Stone coordinates on board are already taken.";
            return false;
        }
    } else {
        err = "Stone coordinates are outside of board dimensions, or invalid stone colour.";
        return false;
    }
}

// for deserialisation
const std::unordered_map<std::string, field_type> playing_board::_string_to_field_type = {
        {"empty", field_type::empty},
        {"black_stone", field_type::black_stone},
        {"white_stone", field_type::white_stone},
};

//for serialisation
const std::unordered_map<field_type, std::string> playing_board::_field_type_to_string = {
        {field_type::empty, "empty"},
        {field_type::black_stone, "black_stone"},
        {field_type::white_stone, "white_stone"},
};


std::vector<std::vector<field_type>> playing_board::get_playing_board() const {
    return _playing_board;
}

std::vector<std::vector<field_type>>::iterator playing_board::get_playing_board_iterator() {
    return _playing_board.begin();
}


#ifdef GOMOKU_SERVER
void playing_board::setup_round(std::string& err) {
    this->reset();
}

#endif

void playing_board::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);
    std::vector<serializable_value<std::string>> flattened_playing_board;
    for (int i=0; i<_playing_board.size(); ++i) {
        for (int j=0; j<_playing_board.size(); ++j) {
            serializable_value<std::string> current_value = serializable_value<std::string>(_field_type_to_string.at(
                    static_cast<const field_type>(_playing_board.at(i).at(j))));
            flattened_playing_board.push_back(current_value);
        }
    }
    json.AddMember("playing_board", vector_utils::serialize_vector(flattened_playing_board, allocator), allocator);
}

playing_board *playing_board::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("playing_board")) {
        std::vector<int> deserialized_flattened_playing_board;
        for (auto &serialized_field : json["playing_board"].GetArray()) {
            int current_field = _string_to_field_type.at(serializable_value<std::string>::from_json(serialized_field.GetObject())->get_value());
            deserialized_flattened_playing_board.push_back(current_field);
        }
        std::vector<field_type> row(_playing_board_size, field_type::empty);
        std::vector<std::vector<field_type>> deserialized_playing_board (_playing_board_size, row);
        for (int i=0; i<_playing_board_size; i++) {
            for (int j=0; j<_playing_board_size; j++) {
                deserialized_playing_board.at(i).at(j) = static_cast<field_type>(deserialized_flattened_playing_board.at(
                        i * _playing_board_size + j));
            }
        }
        return new playing_board(json["id"].GetString(), deserialized_playing_board);
    } else {
        throw gomoku_exception("Could not parse playing board from json. 'playing_board' was missing.");
    }
}



