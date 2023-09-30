//
// Created by Manuel on 25.01.2021.
//

#ifndef GOMOKU_DRAW_PILE_H
#define GOMOKU_DRAW_PILE_H

#include "card.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../../serialization/serializable.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../game_state/player/player.h"
#include "../../../../rapidjson/include/rapidjson/document.h"


class draw_pile : public unique_serializable {
private:
    std::vector<card*> _cards;

    /*
     * Deserialization constructor
     */
    draw_pile(std::string id, std::vector<card*>& cards);

    // from_diff constructor
    draw_pile(std::string id);

// pile functions
    void shuffle();

public:
// constructors
    draw_pile();
    draw_pile(std::vector<card*>& cards);
    ~draw_pile();


// accessors
    bool is_empty() const noexcept;
    int get_nof_cards() const noexcept;

#ifdef GOMOKU_SERVER
// state update functions
    void setup_game(std::string& err);  // Fills the stack with all cards of the game
    bool draw(player* player, card*& drawn_card, std::string& err);
    card* remove_top(std::string& err);
#endif

// serialization
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static draw_pile* from_json(const rapidjson::Value& json);
};


#endif //GOMOKU_DRAW_PILE_H
