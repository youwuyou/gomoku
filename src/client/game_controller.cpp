#include "game_controller.h"
#include "../common/network/requests/join_game_request.h"
#include "../common/network/requests/start_game_request.h"
#include "../common/network/requests/place_stone_request.h"
#include "../common/network/requests/swap_decision_request.h"
#include "../common/network/requests/select_game_mode_request.h"
#include "../common/network/requests/restart_game_request.h"
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

    if(game_controller::_current_game_state->is_finished()) {
        game_controller::show_game_over_message();
    }

    // make sure we are showing the main game panel in the window (if we are already showing it, nothing will happen)
    game_controller::_game_window->show_panel(game_controller::_main_game_panel);

    // command the main game panel to rebuild itself, based on the new game state
    game_controller::_main_game_panel->build_game_state(game_controller::_current_game_state, game_controller::_me);

    if(game_controller::_current_game_state->get_swap_next_turn() && game_controller::_current_game_state->get_current_player()->get_id() == game_controller::_me->get_id()){
        game_controller::show_swap_message();
    }
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

// for disabling closing with cross on dialogues
void on_close(wxCloseEvent& event){
    event.Veto();
}

void game_controller::show_game_over_message() {
    std::string title = "Round Over!";
    std::string message = "Current scores:\n";
    std::string rematch_button_label = "Rematch!";
    std::string change_ruleset_button_label = "Change game mode";
    std::string close_button_label = "Close Game";

    // sort players alphabetically
    std::vector<player*> players = game_controller::_current_game_state->get_players();
    std::sort(players.begin(), players.end(), [](const player* a, const player* b) -> bool {
        return a->get_player_name() < b->get_player_name();
    });

    // list all players
    for (int i = 0; i < players.size(); i++) {

        player* player_state = players.at(i);
        std::string score_text = std::to_string(player_state->get_score());

        // current player of game state is the winner
        std::string winner_text = "";
        if (game_controller::_current_game_state->get_current_player() == player_state
            && !game_controller::_current_game_state->is_tied()) {
            winner_text = "     Winner!";
        }

        std::string player_name = player_state->get_player_name();
        if (player_state->get_id() == game_controller::_me->get_id()) {
            player_name = "You";

            if (game_controller::_current_game_state->get_current_player() == player_state) {
                winner_text = "     You won!";
            }
        }
        message += "\n" + player_name + ":     " + score_text + winner_text;
    }

    // display the round results in a dialogue
    wxDialog* dialog_box = new wxDialog(_game_window, wxID_ANY, wxString(title), wxDefaultPosition, wxSize(400, 300));

    // Disable close button to force a choice
    dialog_box->Bind(wxEVT_CLOSE_WINDOW, on_close);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* text_label = new wxStaticText(dialog_box, wxID_ANY, wxString(message));

    wxButton* rematch_button = new wxButton(dialog_box, wxID_ANY, rematch_button_label);
    wxButton* change_ruleset_button = new wxButton(dialog_box, wxID_ANY, change_ruleset_button_label);
    wxButton* close_button = new wxButton(dialog_box, wxID_ANY, close_button_label);

    button_sizer->Add(rematch_button, 0, wxALL, 10);
    button_sizer->Add(change_ruleset_button, 0, wxALL, 10);

    main_sizer->Add(text_label, 0, wxALIGN_CENTER | wxALL, 10);
    main_sizer->Add(button_sizer, 0, wxALIGN_CENTER);
    main_sizer->Add(close_button, 0, wxALIGN_CENTER, 10);

    _main_game_panel->_open_dialogs.push_back(dialog_box);

    rematch_button->Bind(wxEVT_BUTTON, [dialog_box](wxCommandEvent& event) {
        //wxSound button_click_sound("assets/music/info-click.wav");
        //button_click_sound.Play(wxSOUND_ASYNC);
        restart_game_request request = restart_game_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), false);
        client_network_manager::send_request(request);
        dialog_box->EndModal(wxID_OK);
    });
    change_ruleset_button->Bind(wxEVT_BUTTON, [dialog_box](wxCommandEvent& event) {
        //wxSound button_click_sound("assets/music/info-click.wav");
        //button_click_sound.Play(wxSOUND_ASYNC);
        restart_game_request request = restart_game_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), true);
        client_network_manager::send_request(request);
        dialog_box->EndModal(wxID_OK);
    });
    close_button->Bind(wxEVT_BUTTON, [dialog_box](wxCommandEvent& event) {
        //wxSound button_click_sound("assets/music/info-click.wav");
        //button_click_sound.Play(wxSOUND_ASYNC);
        game_controller::_game_window->Close();
        dialog_box->EndModal(wxID_OK);
    });

    main_sizer->Layout();
    dialog_box->SetSizer(main_sizer);

    dialog_box->ShowModal();
}


void game_controller::show_swap_message() {
    ruleset_type ruleset_name = game_controller::_current_game_state->get_opening_rules()->get_ruleset();
    int turn_number = game_controller::_current_game_state->get_turn_number();

    std::string title = main_game_panel::_ruleset_string_to_pretty_string.at(opening_rules::_ruleset_type_to_string.at(ruleset_name));
    std::string dialog_text = "How do you wish to proceed?";

    wxSize dialog_box_size(300,200);
    if(ruleset_name == swap2 && game_controller::_current_game_state->get_swap_decision() == no_decision_yet){
        dialog_box_size = wxSize(300,260);
    }

    wxDialog* swap_dialog_box = new wxDialog(_game_window, wxID_ANY, wxString(title), wxPoint(500, 300), dialog_box_size);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Disable close button to force a choice
    swap_dialog_box->Bind(wxEVT_CLOSE_WINDOW, on_close);

    std::string swap_button_text = "Swap to black";
    std::string no_swap_button_text = "Continue as white";

    if(game_controller::_current_game_state->get_swap_decision() == swap_decision_type::defer_swap){
        swap_button_text = "Swap to white";
        no_swap_button_text = "Continue as black";
    }

    wxButton* swap_button = new wxButton(swap_dialog_box, wxID_ANY, swap_button_text);
    wxButton* no_swap_button = new wxButton(swap_dialog_box, wxID_ANY, no_swap_button_text);

    wxStaticText* text_label = new wxStaticText(swap_dialog_box, wxID_ANY, wxString(dialog_text));
    main_sizer->Add(text_label, 0, wxALIGN_CENTER | wxALL, 10);

    switch(ruleset_name){
        case swap_after_first_move:
            main_sizer->Add(swap_button, 0, wxALIGN_CENTER | wxALL, 1);
            main_sizer->Add(no_swap_button, 0, wxALIGN_CENTER | wxALL, 1);
            swap_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                //wxSound button_click_sound("assets/music/info-click.wav");
                //button_click_sound.Play(wxSOUND_ASYNC);
                swap_decision_request request = swap_decision_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), swap_decision_type::do_swap);
                client_network_manager::send_request(request);
                swap_dialog_box->Show(false);
            });
            no_swap_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                //wxSound button_click_sound("assets/music/info-click.wav");
                //button_click_sound.Play(wxSOUND_ASYNC);
                swap_decision_request request = swap_decision_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), swap_decision_type::do_not_swap);
                client_network_manager::send_request(request);
                swap_dialog_box->Show(false);;
            });

            break;

        case swap2:
            main_sizer->Add(swap_button, 0, wxALIGN_CENTER | wxALL, 1);
            main_sizer->Add(no_swap_button, 0, wxALIGN_CENTER | wxALL, 1);

            swap_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                //wxSound button_click_sound("assets/music/info-click.wav");
                //button_click_sound.Play(wxSOUND_ASYNC);
                swap_decision_request request = swap_decision_request(game_controller::_me->get_id(),
                                                                      game_controller::_current_game_state->get_id(), swap_decision_type::do_swap);
                client_network_manager::send_request(request);
                swap_dialog_box->Show(false);
            });
            no_swap_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                //wxSound button_click_sound("assets/music/info-click.wav");
                //button_click_sound.Play(wxSOUND_ASYNC);
                swap_decision_request request = swap_decision_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), swap_decision_type::do_not_swap);
                client_network_manager::send_request(request);
                swap_dialog_box->Show(false);
            });

            // add defer button if there hasn't been a swap decision yet
            if(game_controller::_current_game_state->get_swap_decision() == swap_decision_type::no_decision_yet) {
                wxButton *defer_button = new wxButton(swap_dialog_box, wxID_ANY, "Defer choice");
                main_sizer->Add(defer_button, 0, wxALIGN_CENTER | wxALL, 1);
                defer_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                    //wxSound button_click_sound("assets/music/info-click.wav");
                    //button_click_sound.Play(wxSOUND_ASYNC);
                    swap_decision_request request = swap_decision_request(game_controller::_me->get_id(), game_controller::_current_game_state->get_id(), swap_decision_type::defer_swap);
                    client_network_manager::send_request(request);
                    swap_dialog_box->Show(false);
                });
            }

            break;
    }
    swap_dialog_box->SetEscapeId(wxID_NONE);

    main_sizer->Layout();
    swap_dialog_box->SetSizer(main_sizer);
    swap_dialog_box->CentreOnParent(wxVERTICAL);

    swap_dialog_box->Show();
}