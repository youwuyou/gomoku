#ifndef GOMOKUUI_GAMECONTROLLER_H
#define GOMOKUUI_GAMECONTROLLER_H

#include <wx/sound.h>
#include "windows/game-window.h"
#include "panels/connection_panel.h"
#include "panels/main_game_panel.h"
#include "network/response_listener_thread.h"
#include "../common/game_state/game_state.h"


class game_controller {

public:
    static void init(game* game_window);

    static void connect_to_server();
    static void update_game_state(game_state* new_game_state);
    static void start_game();
    static void place_stone(unsigned int x, unsigned int y, field_type colour, std::string &err);
    static void set_game_rules(std::string ruleset_string,  std::string &err);

    static wxEvtHandler* get_main_thread_event_handler();
    static void show_error(const std::string& title, const std::string& message);
    static void show_status(const std::string& message);
    static void show_game_over_message();
    static void show_swap_message();

private:
    static game* _game_window;
    static connection_panel* _connection_panel;
    static main_game_panel* _main_game_panel;

    static player* _me;
    static game_state* _current_game_state;
};


#endif //GOMOKUUI_GAMECONTROLLER_H
