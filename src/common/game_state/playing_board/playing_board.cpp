//
// Created by Manuel on 27.01.2021.
//

#include "playing_board.h"

#include "../../exceptions/GomokuException.h"
#include "../../serialization/vector_utils.h"

playing_board::playing_board() : unique_serializable() { }

playing_board::playing_board(std::string id) : unique_serializable(id) {
    this->_playing_board = std::array<std::array<stone*, _playing_board_size>, _playing_board_size>;
}

// deserialization constructor
playing_board::playing_board(std::string id, std::array<std::array<stone*, _playing_board_size>, _playing_board_size> playing_board) : unique_serializable(id) {
    this->_playing_board = playing_board;
}

playing_board::~playing_board() {
    for (int i = 0; i < this->_playing_board_size; i++) {
         for (int j = 0; j < this->_playing_board_size; j++) {
            delete _playing_board.at((i)).at((j));
            _playing_board.at(i).at((j)) = nullptr;
        }
    }
}

// NOTE: Array size should be initialised with _playing_board_size, not a constant integer, WIP
std::array<std::array<stone*, 15>, 15> playing_board::get_playing_board() const {
    return _playing_board;
}

// NOTE: Array size should be initialised with _playing_board_size, not a constant integer, WIP
std::array<std::array<stone*, 15>, 15>::iterator playing_board::get_playing_board_iterator() {
    return _playing_board.begin();
}


#ifdef GOMOKU_SERVER

#endif


void playing_board::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);
    std::vector<stone*> flattened_playing_board;
    for (auto const &v: _playing_board) {
        flattened_playing_board.insert(flattened_playing_board.end(), v.begin(), v.end());
    }

    json.AddMember("playing_board", vector_utils::serialize_vector(flattened_playing_board, allocator), allocator);
}

playing_board *playing_board::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("playing_board")) {
        std::vector<stone*> deserialized_flattened_playing_board;
        for (auto &serialized_stone : json["playing_board"].GetArray()) {
            deserialized_flattened_playing_board.push_back(stone::from_json(serialized_stone.GetObject()));
        }
        std::array<std::array<stone*, _playing_board_size>, _playing_board_size> deserialized_playing_board;
        for (int i=0; i<_playing_board_size; i++){
            for (int j=0; j<_playing_board_size; j++){
                deserialized_playing_board.at(i).at(j) = deserialized_flattened_playing_board.at(i*_playing_board_size + j);
            }
        }
        return new playing_board(json["id"].GetString(), deserialized_playing_board);
    } else {
        throw GomokuException("Could not parse hand from json. 'cards' were missing.");
    }
}



