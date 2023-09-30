//
// Created by Manuel on 29.01.2021.
//
// The game_instance_manager only exists on the server side. It stores all currently active games and offers
// functionality to retrieve game instances by id and adding players to games.
// If a new player requests to join a game but no valid game_instance is available, then this class
// will generate a new game_instance and add it to the unordered_map of (active) game instances.

#ifndef GOMOKU_GAME_INSTANCE_MANAGER_H
#define GOMOKU_GAME_INSTANCE_MANAGER_H

#include <string>
#include <shared_mutex>
#include <unordered_map>

#include "game_instance.h"

class game_instance_manager {

private:

    inline static std::shared_mutex games_lut_lock;
    static std::unordered_map<std::string, game_instance*> games_lut;

    static game_instance* create_new_game();
    static game_instance* find_joinable_game_instance();

public:

    // returns true if the desired game_instance 'game_id' was found or false otherwise.
    // The found game instance is written into game_instance_ptr.
    static bool try_get_game_instance(const std::string& game_id, game_instance*& game_instance_ptr);
    // returns true if the desired player 'player_id' was found and is connected to a game_instance.
    // The found player and game_instance will be written into 'player' and 'game_instance_ptr'
    static bool try_get_player_and_game_instance(const std::string& player_id, player*& player, game_instance*& game_instance_ptr, std::string& err);

    // Try to add 'player' to any game. Returns true if 'player' is successfully added to a game_instance.
    // The joined game_instance will be written into 'game_instance_ptr'.
    static bool try_add_player_to_any_game(player* player, game_instance*& game_instance_ptr, std::string& err);
    // Try to add 'player' to the provided 'game_instance_ptr'. Returns true if success and false otherwise.
    static bool try_add_player(player* player, game_instance*& game_instance_ptr, std::string& err);


    static bool try_remove_player(player* player, const std::string& game_id, std::string& err);
    static bool try_remove_player(player* player, game_instance*& game_instance_ptr, std::string& err);

};


#endif //GOMOKU_GAME_INSTANCE_MANAGER_H
