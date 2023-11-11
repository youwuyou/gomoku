#include "GameController.h"
#include "../common/network/requests/join_game_request.h"
#include "../common/network/requests/start_game_request.h"
#include "../common/network/requests/draw_card_request.h"
#include "../common/network/requests/fold_request.h"
#include "../common/network/requests/play_card_request.h"
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
    if(inputServerAddress.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's address");
        return;
    }
    if(inputServerPort.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's port number");
        return;
    }
    if(inputPlayerName.IsEmpty()) {
        GameController::showError("Input error", "Please enter your desired player name");
        return;
    }

    // convert host from wxString to std::string
    std::string host = inputServerAddress.ToStdString();

    // convert port from wxString to uint16_t
    unsigned long portAsLong;
    if(!inputServerPort.ToULong(&portAsLong) || portAsLong > 65535) {
        GameController::showError("Connection error", "Invalid port");
        return;
    }
    uint16_t port = (uint16_t) portAsLong;

    // convert player name from wxString to std::string
    std::string playerName = inputPlayerName.ToStdString();

    // connect to network
    ClientNetworkManager::init(host, port);

    // send request to join game
    GameController::_me = new player(playerName);
    join_game_request request = join_game_request(GameController::_me->get_id(), GameController::_me->get_player_name());
    ClientNetworkManager::sendRequest(request);

}


void GameController::updateGameState(game_state* newGameState) {

    // the existing game state is now old
    game_state* oldGameState = GameController::_currentGameState;

    // save the new game state as our current game state
    GameController::_currentGameState = newGameState;

    if(oldGameState != nullptr) {

        // check if a new round started, and display message accordingly
        if(oldGameState->get_turn_number() > 0 && oldGameState->get_turn_number() < newGameState->get_turn_number()) {
            GameController::showNewRoundMessage(oldGameState, newGameState);
        }

        // delete the old game state, we don't need it anymore
        delete oldGameState;
    }

    if(GameController::_currentGameState->is_finished()) {
        GameController::showGameOverMessage();
    }

    // make sure we are showing the main game panel in the window (if we are already showing it, nothing will happen)
    GameController::_gameWindow->showPanel(GameController::_mainGamePanel);

    // command the main game panel to rebuild itself, based on the new game state
    GameController::_mainGamePanel->buildGameState(GameController::_currentGameState, GameController::_me);
}


void GameController::startGame() {
    start_game_request request = start_game_request(GameController::_currentGameState->get_id(), GameController::_me->get_id());
    ClientNetworkManager::sendRequest(request);
}


void GameController::drawCard() {
    draw_card_request request = draw_card_request(GameController::_currentGameState->get_id(), GameController::_me->get_id());
    ClientNetworkManager::sendRequest(request);
}


void GameController::fold() {
    fold_request request = fold_request(GameController::_currentGameState->get_id(), GameController::_me->get_id());
    ClientNetworkManager::sendRequest(request);
}


void GameController::playCard(card* cardToPlay) {
    play_card_request request = play_card_request(GameController::_currentGameState->get_id(), GameController::_me->get_id(), cardToPlay->get_id());
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


void GameController::showNewRoundMessage(game_state* oldGameState, game_state* newGameState) {
    std::string title = "Round Completed";
    std::string message = "The players gained the following minus points:\n";
    std::string buttonLabel = "Start next round";

    // add the point differences of all players to the messages
    for(int i = 0; i < oldGameState->get_players().size(); i++) {

        player* oldPlayerState = oldGameState->get_players().at(i);
        player* newPlayerState = newGameState->get_players().at(i);

        int scoreDelta = newPlayerState->get_score() - oldPlayerState->get_score();
        std::string scoreText = std::to_string(scoreDelta);
        if(scoreDelta > 0) {
            scoreText = "+" + scoreText;
        }

        std::string playerName = newPlayerState->get_player_name();
        if(newPlayerState->get_id() == GameController::_me->get_id()) {
            playerName = "You";
        }
        message += "\n" + playerName + ":     " + scoreText;
    }

    wxMessageDialog dialogBox = wxMessageDialog(nullptr, message, title, wxICON_NONE);
    dialogBox.SetOKLabel(wxMessageDialog::ButtonLabel(buttonLabel));
    dialogBox.ShowModal();
}


void GameController::showGameOverMessage() {
    std::string title = "Game Over!";
    std::string message = "Final score:\n";
    std::string buttonLabel = "Close Game";

    // sort players by score
    std::vector<player*> players = GameController::_currentGameState->get_players();
    std::sort(players.begin(), players.end(), [](const player* a, const player* b) -> bool {
        return a->get_score() < b->get_score();
    });

    // list all players
    for(int i = 0; i < players.size(); i++) {

        player* playerState = players.at(i);
        std::string scoreText = std::to_string(playerState->get_score());

        // first entry is the winner
        std::string winnerText = "";
        if(i == 0) {
            winnerText = "     Winner!";
        }

        std::string playerName = playerState->get_player_name();
        if(playerState->get_id() == GameController::_me->get_id()) {
            playerName = "You";

            if(i == 0) {
                winnerText = "     You won!!!";
            }
        }
        message += "\n" + playerName + ":     " + scoreText + winnerText;
    }

    wxMessageDialog dialogBox = wxMessageDialog(nullptr, message, title, wxICON_NONE);
    dialogBox.SetOKLabel(wxMessageDialog::ButtonLabel(buttonLabel));
    int buttonClicked = dialogBox.ShowModal();
    if(buttonClicked == wxID_OK) {
        GameController::_gameWindow->Close();
    }
}