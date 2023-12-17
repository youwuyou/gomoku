//
// Created by Manuel on 25.01.2021.
//
// The game_instance class is a wrapper around the game_state of an active instance of the game.
// This class contains functions to modify the contained game_state.

#include "game_instance.h"

#include "server_network_manager.h"
#include "../common/network/responses/full_state_response.h"


game_instance::game_instance() {
    _game_state = new game_state();
}

game_state *game_instance::get_game_state() {
    return _game_state;
}

std::string game_instance::get_id() {
    return _game_state->get_id();
}

bool game_instance::is_player_allowed_to_play(player *player) {
    return _game_state->is_allowed_to_play_now(player);
}

bool game_instance::is_full() {
    return _game_state->is_full();
}

bool game_instance::is_started() {
    return _game_state->is_started();
}

bool game_instance::is_finished() {
    return _game_state->is_finished();
}



bool game_instance::start_game(player* player, std::string &err) {
    modification_lock.lock();
    if (_game_state->get_opening_rules() != ruleset_type::uninitialized) {
        if (_game_state->start_game(err)) {
            // send state update to all other players
            full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
            server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
            modification_lock.unlock();
            return true;
        }
    } else {
        err = _game_state->get_current_player()->get_player_name() + " needs to choose a ruleset before the game can start.";
        modification_lock.unlock();
        return false;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_remove_player(player *player, std::string &err) {
    modification_lock.lock();
    if (_game_state->remove_player(player, err)) {
        player->set_game_id("");
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_add_player(player *new_player, std::string &err) {
    modification_lock.lock();
    if (_game_state->add_player(new_player, err)) {
        new_player->set_game_id(get_id());
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), new_player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::place_stone(player *player, unsigned int x, unsigned int y, field_type colour, std::string &err) {
    modification_lock.lock();
    if (_game_state->place_stone(x, y, colour, err)){
        if (_game_state->check_win_condition(x, y, colour) ||
           (_game_state->get_turn_number() >= playing_board::MAX_NUM_STONES-1 && _game_state->check_for_tie())) { // -1 because turn number starts at 0 -> first turn that a tie can occur on is 224 in freestyle
            _game_state->wrap_up_round(err);
            full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
            server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
            modification_lock.unlock();
            return true;
        } else if (_game_state->update_current_player(err)){
            _game_state->iterate_turn();
            full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
            server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
            modification_lock.unlock();
            return true;
        } else {
            err = "game_instance: Unable to update current player.";
        }
    } else {
        err = "game_instance: Unable to place stone.";
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::do_swap_decision(player *player, swap_decision_type swap_decision, std::string &err) {
    // NOTE: This method expects swap_decision to be "do_swap", "do_not_swap", or "defer_swap".
    // Anything else will result in an error, or return false, to occur.
    modification_lock.lock();
    if (_game_state->determine_swap_decision(swap_decision, err)) {
        if (_game_state->update_current_player(err)){
            _game_state->iterate_turn();
            full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
            server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
            modification_lock.unlock();
            return true;
        } else {
            err = "game_instance: Unable to update current player.";
        }
    } else {
        err = "game_instance: Unable to carry out swap decision";
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::do_forfeit(player* player, std::string &err){
    modification_lock.lock();
    if(_game_state->alternate_current_player(err)){
        _game_state->wrap_up_round(err);
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    } else {
        err = "game_instance: Unable to do forfeit.";
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::set_game_mode(player* player, const std::string& ruleset_string, std::string& err) {
    modification_lock.lock();
    if (_game_state->set_game_mode(ruleset_string, err)) {
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    err = "game_instance: Unable to set game rule.";
    modification_lock.unlock();
    return false;
}
