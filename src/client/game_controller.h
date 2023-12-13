#ifndef GOMOKUUI_GAMECONTROLLER_H
#define GOMOKUUI_GAMECONTROLLER_H

#include <wx/sound.h>
#include "windows/game_window.h"
#include "panels/connection_panel.h"
#include "panels/main_game_panel.h"
#include "network/response_listener_thread.h"
#include "../common/game_state/game_state.h"


class game_controller {

public:
    static void init(game_window* game_window);

    static void connect_to_server();
    static void update_game_state(game_state* new_game_state);
    static void start_game();
    static void place_stone(unsigned int x, unsigned int y, field_type colour, std::string &err);
    static void set_game_rules(std::string ruleset_string,  std::string &err);
    static void send_swap_decision(swap_decision_type decision);
    static void send_restart_decision(bool change_ruleset);
    static void close_game();
    static void forfeit();

    static wxEvtHandler* get_main_thread_event_handler();
    static void show_error(const std::string& title, const std::string& message);
    static void show_status(const std::string& message);

private:
    static game_window* _game_window;
    static connection_panel* _connection_panel;
    static main_game_panel* _main_game_panel;

    static player* _me;
    static game_state* _current_game_state;
};


#endif //GOMOKUUI_GAMECONTROLLER_H
