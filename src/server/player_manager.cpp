//
// Created by Manuel on 29.01.2021.
//
// The player_manager only exists on the server side. It stores all connected users since starting the server. It offers
// functionality to retrieve players by id or adding players when they first connect to the server.
//

#include "player_manager.h"

// Initialize static map
std::unordered_map<std::string, player*> player_manager::_players_lut = {};

bool player_manager::try_get_player(const std::string& player_id, player *&player_ptr) {
    player_ptr = nullptr;
    _rw_lock.lock_shared();
    auto it = player_manager::_players_lut.find(player_id);
    if (it != _players_lut.end()) {
        player_ptr = it->second;
    }
    _rw_lock.unlock_shared();
    return player_ptr;
}

bool player_manager::add_or_get_player(std::string name, const std::string& player_id, player *&player_ptr) {
    if (try_get_player(player_id, player_ptr)) {
        return true;
    }
    if(_players_lut.empty()){
        player_ptr = new player(player_id, std::move(name), player_colour_type::white);
    } else {
        player_ptr = new player(player_id, std::move(name), player_colour_type::black);
    }
    _rw_lock.lock();    // exclusive
    player_manager::_players_lut.insert({player_id, player_ptr});
    _rw_lock.unlock();
    return true;
}

bool player_manager::remove_player(const std::string& player_id, player *&player) {
    if (try_get_player(player_id, player)) {
        _rw_lock.lock();    // exclusive
        int nof_removals = player_manager::_players_lut.erase(player_id);
        _rw_lock.unlock();
        return true;
    }
    return false;
}

