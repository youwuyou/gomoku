//
// Created by Manuel on 27.01.2021.
//

#ifndef GOMOKU_PLAYING_BOARD_H
#define GOMOKU_PLAYING_BOARD_H

#include <vector>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../playing_board/stone.h"

class playing_board : public unique_serializable {

private:
    static const int _playing_board_size = 15;
    std::vector<std::vector<stone*>> _playing_board;

    playing_board(std::string id);
    playing_board(std::string id, std::vector<std::vector<stone*>> playing_board);
    void reset();
    void place_stone(stone* stone);

public:
    playing_board();
    ~playing_board();


// serializable interface
    static playing_board* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

// accessors
    std::vector<std::vector<stone*>> get_playing_board() const;

#ifdef GOMOKU_SERVER
// state update functions
    //void setup_round(std::string& err);
#endif

    std::vector<std::vector<stone*>>::iterator get_playing_board_iterator();
};


#endif //GOMOKU_PLAYING_BOARD_H
