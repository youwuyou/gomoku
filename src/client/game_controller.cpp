#include "game_controller.h"
#include "../common/network/requests/join_game_request.h"
#include "../common/network/requests/start_game_request.h"
#include "../common/network/requests/place_stone_request.h"
#include "../common/network/requests/swap_decision_request.h"
#include "../common/network/requests/select_game_mode_request.h"
#include "../common/network/requests/restart_game_request.h"
#include "../common/network/requests/forfeit_request.h"
#include "network/client_network_manager.h"


// initialize static members
game_window* game_controller::_game_window = nullptr;
connection_panel* game_controller::_connection_panel = nullptr;
main_game_panel* game_controller::_main_game_panel = nullptr;
player* game_controller::_me = nullptr;
game_state* game_controller::_current_game_state = nullptr;

void game_controller::init(game_window* game_window) {

    game_controller::_game_window = game_window;

    // Set up main panels
    game_controller::_connection_panel = new connection_panel(game_window);
    game_controller::_main_game_panel = new main_game_panel(game_window);

    // Hide all panels
    game_controller::_connection_panel->Show(false);
    game_controller::_main_game_panel->Show(false);

    // Only show connection panel at the start of the game
    game_controller::_game_window->show_panel(game_controller::_connection_panel);

    // Set status bar
    game_controller::show_status("Not connected");
}


void game_controller::connect_to_server() {

    // get values form UI input fields
    wxString input_server_address = game_controller::_connection_panel->get_server_address().Trim();
    wxString input_server_port = game_controller::_connection_panel->get_server_port().Trim();
    wxString input_player_name = game_controller::_connection_panel->get_player_name().Trim();

    // check that all values were provided
    if (input_server_address.IsEmpty()) {
        game_controller::show_error("Input error", "Please provide the server's address");
        return;
    }
    if (input_server_port.IsEmpty()) {
        game_controller::show_error("Input error", "Please provide the server's port number");
        return;
    }
    if (input_player_name.IsEmpty()) {
        game_controller::show_error("Input error", "Please enter your desired player name");
        return;
    }

    // convert host from wxString to std::string
    std::string host = input_server_address.ToStdString();

    // convert port from wxString to uint16_t
    unsigned long port_as_long;
    if (!input_server_port.ToULong(&port_as_long) || port_as_long > 65535) {
        game_controller::show_error("Connection error", "Invalid port");
        return;
    }
    uint16_t port = (uint16_t) port_as_long;

    // convert player name from wxString to std::string
    std::string player_name = input_player_name.ToStdString();

    // connect to network
    client_network_manager::init(host, port);

    // send request to join game
    game_controller::_me = new player(player_name, player_colour_type::white);
    join_game_request request = join_game_request(game_controller::_me->get_id(), game_controller::_me->get_player_name());
    client_network_manager::send_request(request);

}


void game_controller::update_game_state(game_state* new_game_state) {

    // the existing game state is now old
    game_state* oldGameState = game_controller::_current_game_state;

    // save the new game state as our current game state
    game_controller::_current_game_state = new_game_state;

    // make sure we are showing the main game panel in the window (if we are already showing it, nothing will happen)
    game_controller::_game_window->show_panel(game_controller::_main_game_panel);

    // save current music state for continuation
    if(game_controller::_main_game_panel->music_is_started) {
        game_controller::_main_game_panel->save_music_state();
    }

    // command the main game panel to rebuild itself, based on the new game state
    game_controller::_main_game_panel->build_game_state(game_controller::_current_game_state, game_controller::_me);

}


void game_controller::start_game() {
    start_game_request request = start_game_request(game_controller::_current_game_state->get_id(), game_controller::_me->get_id());
    client_network_manager::send_request(request);
}

void game_controller::place_stone(unsigned int x, unsigned int y, field_type colour, std::string &err) {
    if (x < 15 && y < 15 && (colour == field_type::black_stone || colour == field_type::white_stone)) {
        place_stone_request request = place_stone_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), x, y, colour);
        client_network_manager::send_request(request);
    } else {
        err = "invalid position or colour";
    }
}

void game_controller::set_game_rules(std::string ruleset_string, std::string &err) {
    select_game_mode_request request = select_game_mode_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), ruleset_string);
    client_network_manager::send_request(request);
}


wxEvtHandler* game_controller::get_main_thread_event_handler() {
    return game_controller::_game_window->GetEventHandler();
}


void game_controller::show_error(const std::string& title, const std::string& message) {
    wxMessageBox(message, title, wxICON_ERROR);
}


void game_controller::show_status(const std::string& message) {
    game_controller::_game_window->set_status(message);
}

void game_controller::send_swap_decision(swap_decision_type decision){
    swap_decision_request request = swap_decision_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), decision);
    client_network_manager::send_request(request);
}

void game_controller::send_restart_decision(bool change_ruleset){
    restart_game_request request = restart_game_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), change_ruleset);
    client_network_manager::send_request(request);
}

void game_controller::close_game() {
    game_controller::_game_window->Close();
}

void game_controller::forfeit(){
    forfeit_request request = forfeit_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id());
    client_network_manager::send_request(request);
}