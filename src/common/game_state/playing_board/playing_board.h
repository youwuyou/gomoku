//
// Created by Manuel on 27.01.2021.
//

#ifndef GOMOKU_PLAYING_BOARD_H
#define GOMOKU_PLAYING_BOARD_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../../serialization/serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

enum field_type {
    empty,
    black_stone,
    white_stone,
};

class playing_board : public unique_serializable {

private:
    std::vector<std::vector<field_type>> _playing_board;

    playing_board(std::string id);
    playing_board(std::string id, std::vector<std::vector<field_type>> playing_board);
    void reset();

public:
    playing_board();
    ~playing_board();

    static const int _playing_board_size = 15;
    static const int MAX_NUM_STONES = _playing_board_size*_playing_board_size;

    bool place_stone(unsigned int x, unsigned int y, field_type colour, std::string &err);

// serializable interface
    static playing_board* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    // for deserialization
    static const std::unordered_map<std::string, field_type> _string_to_field_type;
    // for serialization
    static const std::unordered_map<field_type, std::string> _field_type_to_string;

// accessors
    std::vector<std::vector<field_type>> get_playing_board() const;

#ifdef GOMOKU_SERVER
// state update functions
    void setup_round(std::string& err);
#endif

    std::vector<std::vector<field_type>>::iterator get_playing_board_iterator();
};


#endif //GOMOKU_PLAYING_BOARD_H
