#include "GameController.h"
#include "../common/network/requests/join_game_request.h"
#include "../common/network/requests/start_game_request.h"
#include "../common/network/requests/place_stone_request.h"
#include "../common/network/requests/swap_colour_request.h"
#include "../common/network/requests/select_game_mode_request.h"
#include "../common/network/requests/restart_game_request.h"
#include "network/ClientNetworkManager.h"


// initialize static members
GameWindow* GameController::_gameWindow = nullptr;
ConnectionPanel* GameController::_connectionPanel = nullptr;
MainGamePanel* GameController::_mainGamePanel = nullptr;
player* GameController::_me = nullptr;
game_state* GameController::_currentGameState = nullptr;

void GameController::init(GameWindow* gameWindow) {

    GameController::_gameWindow = gameWindow;

    // Set up main panels
    GameController::_connectionPanel = new ConnectionPanel(gameWindow);
    GameController::_mainGamePanel = new MainGamePanel(gameWindow);

    // Hide all panels
    GameController::_connectionPanel->Show(false);
    GameController::_mainGamePanel->Show(false);

    // Only show connection panel at the start of the game
    GameController::_gameWindow->showPanel(GameController::_connectionPanel);

    // Set status bar
    GameController::showStatus("Not connected");
}


void GameController::connectToServer() {

    // get values form UI input fields
    wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
    wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
    wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();

    // check that all values were provided
    if (inputServerAddress.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's address");
        return;
    }
    if (inputServerPort.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's port number");
        return;
    }
    if (inputPlayerName.IsEmpty()) {
        GameController::showError("Input error", "Please enter your desired player name");
        return;
    }

    // convert host from wxString to std::string
    std::string host = inputServerAddress.ToStdString();

    // convert port from wxString to uint16_t
    unsigned long portAsLong;
    if (!inputServerPort.ToULong(&portAsLong) || portAsLong > 65535) {
        GameController::showError("Connection error", "Invalid port");
        return;
    }
    uint16_t port = (uint16_t) portAsLong;

    // convert player name from wxString to std::string
    std::string playerName = inputPlayerName.ToStdString();

    // connect to network
    ClientNetworkManager::init(host, port);

    // send request to join game
    GameController::_me = new player(playerName, player_colour_type::white);
    join_game_request request = join_game_request(GameController::_me->get_id(), GameController::_me->get_player_name());
    ClientNetworkManager::sendRequest(request);

}


void GameController::updateGameState(game_state* newGameState) {

    // the existing game state is now old
    game_state* oldGameState = GameController::_currentGameState;

    // save the new game state as our current game state
    GameController::_currentGameState = newGameState;

    if(GameController::_currentGameState->is_finished()) {
        GameController::showGameOverMessage();
    }

    // make sure we are showing the main game panel in the window (if we are already showing it, nothing will happen)
    GameController::_gameWindow->showPanel(GameController::_mainGamePanel);

    // command the main game panel to rebuild itself, based on the new game state
    GameController::_mainGamePanel->buildGameState(GameController::_currentGameState, GameController::_me);

    if(GameController::_currentGameState->get_swap_next_turn() && GameController::_currentGameState->get_current_player()->get_id() == GameController::_me->get_id()){
        GameController::show_swap_message();
    }
}


void GameController::startGame() {
    start_game_request request = start_game_request(GameController::_currentGameState->get_id(), GameController::_me->get_id());
    ClientNetworkManager::sendRequest(request);
}

void GameController::placeStone(unsigned int x, unsigned int y, field_type colour, std::string &err) {
    if (x < 15 && y < 15 && 0 < colour && colour <= 2) {
        place_stone_request request = place_stone_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), x, y, colour);
        ClientNetworkManager::sendRequest(request);
    } else {
        err = "invalid position or colour";
    }
}

void GameController::set_game_rules(std::string ruleset_string,  std::string &err) {
    select_game_mode_request request = select_game_mode_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), ruleset_string);
    ClientNetworkManager::sendRequest(request);
}


wxEvtHandler* GameController::getMainThreadEventHandler() {
    return GameController::_gameWindow->GetEventHandler();
}


void GameController::showError(const std::string& title, const std::string& message) {
    wxMessageBox(message, title, wxICON_ERROR);
}


void GameController::showStatus(const std::string& message) {
    GameController::_gameWindow->setStatus(message);
}

void GameController::showGameOverMessage() {
    std::string title = "Round Over!";
    std::string message = "Current scores:\n";
    std::string rematch_button_label = "Rematch!";
    std::string change_ruleset_button_label = "Change game mode";
    std::string close_button_label = "Close Game";

    // sort players alphabetically
    std::vector<player*> players = GameController::_currentGameState->get_players();
    std::sort(players.begin(), players.end(), [](const player* a, const player* b) -> bool {
        return a->get_player_name() < b->get_player_name();
    });

    // list all players
    for (int i = 0; i < players.size(); i++) {

        player* playerState = players.at(i);
        std::string scoreText = std::to_string(playerState->get_score());

        // current player of game state is the winner
        std::string winnerText = "";
        if (GameController::_currentGameState->get_current_player() == playerState
            && !GameController::_currentGameState->is_tied()) {
            winnerText = "     Winner!";
        }

        std::string playerName = playerState->get_player_name();
        if (playerState->get_id() == GameController::_me->get_id()) {
            playerName = "You";

            if (GameController::_currentGameState->get_current_player() == playerState) {
                winnerText = "     You won!";
            }
        }
        message += "\n" + playerName + ":     " + scoreText + winnerText;
    }

    // display the round results in a box, without the useless minimize button
    wxDialog* dialog_box = new wxDialog(_gameWindow, wxID_ANY, wxString(title), wxDefaultPosition, wxSize(400, 300));

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

    _mainGamePanel->_open_dialogs.push_back(dialog_box);

    rematch_button->Bind(wxEVT_BUTTON, [dialog_box](wxCommandEvent& event) {
        restart_game_request request = restart_game_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), false);
        ClientNetworkManager::sendRequest(request);
        dialog_box->EndModal(wxID_OK);
    });
    change_ruleset_button->Bind(wxEVT_BUTTON, [dialog_box](wxCommandEvent& event) {
        restart_game_request request = restart_game_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), true);
        ClientNetworkManager::sendRequest(request);
        dialog_box->EndModal(wxID_OK);
    });
    close_button->Bind(wxEVT_BUTTON, [dialog_box](wxCommandEvent& event) {
        GameController::_gameWindow->Close();
        dialog_box->EndModal(wxID_OK);
    });

    main_sizer->Layout();
    dialog_box->SetSizer(main_sizer);

    dialog_box->ShowModal();
}

void OnClose(wxCloseEvent& event){
    event.Veto();
}

void GameController::show_swap_message() {
    ruleset_type ruleset_name = GameController::_currentGameState->get_opening_rules()->get_ruleset();
    int turn_number = GameController::_currentGameState->get_turn_number();

    std::string title = MainGamePanel::_ruleset_string_to_pretty_string.at(opening_rules::_ruleset_type_to_string.at(ruleset_name));
    std::string dialog_text = "How do you wish to proceed?";

    wxSize dialog_box_size(300,200);
    if(ruleset_name == swap2 && GameController::_currentGameState->get_swap_decision() == no_decision_yet){
        dialog_box_size = wxSize(300,260);
    }

    wxDialog* swap_dialog_box = new wxDialog(_gameWindow, wxID_ANY, wxString(title), wxPoint(500, 300), dialog_box_size);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Disable close button to force a choice
    swap_dialog_box->Bind(wxEVT_CLOSE_WINDOW, OnClose);

    std::string swap_button_text = "Swap to black";
    std::string no_swap_button_text = "Continue as white";

    if(GameController::_currentGameState->get_swap_decision() == swap_decision_type::defer_swap){
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
                swap_colour_request request = swap_colour_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), swap_decision_type::do_swap);
                ClientNetworkManager::sendRequest(request);
                swap_dialog_box->Show(false);
            });
            no_swap_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                swap_colour_request request = swap_colour_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), swap_decision_type::do_not_swap);
                ClientNetworkManager::sendRequest(request);
                swap_dialog_box->Show(false);;
            });

            break;

        case swap2:
            main_sizer->Add(swap_button, 0, wxALIGN_CENTER | wxALL, 1);
            main_sizer->Add(no_swap_button, 0, wxALIGN_CENTER | wxALL, 1);

            swap_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                swap_colour_request request = swap_colour_request(GameController::_me->get_id(),
                                                                  GameController::_currentGameState->get_id(), swap_decision_type::do_swap);
                ClientNetworkManager::sendRequest(request);
                swap_dialog_box->Show(false);
            });
            no_swap_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                swap_colour_request request = swap_colour_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), swap_decision_type::do_not_swap);
                ClientNetworkManager::sendRequest(request);
                swap_dialog_box->Show(false);
            });

            // add defer button if there hasn't been a swap decision yet
            if(GameController::_currentGameState->get_swap_decision() == swap_decision_type::no_decision_yet) {
                wxButton *defer_button = new wxButton(swap_dialog_box, wxID_ANY, "Defer choice");
                main_sizer->Add(defer_button, 0, wxALIGN_CENTER | wxALL, 1);
                defer_button->Bind(wxEVT_BUTTON, [swap_dialog_box](wxCommandEvent& event) {
                    swap_colour_request request = swap_colour_request(GameController::_me->get_id(), GameController::_currentGameState->get_id(), swap_decision_type::defer_swap);
                    ClientNetworkManager::sendRequest(request);
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