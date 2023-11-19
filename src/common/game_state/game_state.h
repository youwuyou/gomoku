//
// Created by Manuel on 27.01.2021.
//

#ifndef GOMOKU_GAME_STATE_H
#define GOMOKU_GAME_STATE_H

#include <vector>
#include <string>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "playing_board/stone.h"
#include "playing_board/playing_board.h"
#include "playing_board/opening_rules.h"
#include "../serialization/serializable.h"
#include "../serialization/serializable_value.h"
#include "../serialization/unique_serializable.h"


class game_state : public unique_serializable {
private:

    std::vector<player*> _players;
    playing_board* _playing_board;
    opening_rules* _opening_ruleset;
    serializable_value<bool>* _is_started;
    serializable_value<bool>* _is_finished;
    serializable_value<int>* _turn_number;
    serializable_value<int>* _current_player_idx;
    serializable_value<int>* _starting_player_idx;

    // from_diff constructor
    game_state(std::string id);

    // deserialization constructor
    game_state(
            std::string id,
            playing_board* _playing_board,
            opening_rules* _opening_ruleset,
            std::vector<player*>& players,
            serializable_value<bool>* is_started,
            serializable_value<bool>* is_finished,
            serializable_value<int>* current_player_idx,
            serializable_value<int>* turn_number,
            serializable_value<int>* starting_player_idx);

    // returns the index of 'player' in the '_players' vector
    int get_player_index(player* player) const;
public:
    game_state();
    ~game_state();

// accessors
    bool is_full() const;
    bool is_started() const;
    bool is_finished() const;
    bool is_player_in_game(player* player) const;
    bool is_allowed_to_play_now(player* player) const;
    std::vector<player*>& get_players();
    int get_turn_number() const;

    player* get_current_player() const;

#ifdef GOMOKU_SERVER
    // server-side state update functions
    //// lobby functionalities
    bool remove_player(player* player, std::string& err);
    bool add_player(player* player, std::string& err);
    bool start_game(std::string& err);
    bool set_game_mode(std::string rule_name);

    //// start of round functions
    void setup_round(std::string& err);

    //// in-round functionalities
    bool place_stone(player* player, std::string& err);
    bool check_win_condition();

    //// end of round functions
    void update_current_player(std::string& err);
    void wrap_up_round(std::string& err);

#endif

// serializable interface
    static game_state* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //GOMOKU_GAME_STATE_H
