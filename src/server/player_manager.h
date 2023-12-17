// The player_manager only exists on the server side. It stores all connected users since starting the server. It offers
// functionality to retrieve players by id or adding players when they first connect to the server.
//

#ifndef GOMOKU_PLAYER_MANAGER_H
#define GOMOKU_PLAYER_MANAGER_H

#include <string>
#include <shared_mutex>
#include <unordered_map>

#include "../common/game_state/player/player.h"

class player_manager {

private:

    inline static std::shared_mutex _rw_lock;
    static std::unordered_map<std::string, player*> _players_lut;

public:
    static bool try_get_player(const std::string& player_id, player*& player_ptr);
    static bool add_or_get_player(std::string name, const std::string& player_id, player*& player_ptr);
    static bool remove_player(const std::string& player_id, player*& player);  // not implemented
};


#endif //GOMOKU_PLAYER_MANAGER_H
