#ifndef GOMOKUUI_GAMECONTROLLER_H
#define GOMOKUUI_GAMECONTROLLER_H

#include <wx/sound.h>
#include "windows/GameWindow.h"
#include "panels/ConnectionPanel.h"
#include "panels/MainGamePanel.h"
#include "network/ResponseListenerThread.h"
#include "../common/game_state/game_state.h"


class GameController {

public:
    static void init(GameWindow* gameWindow);

    static void connectToServer();
    static void updateGameState(game_state* newGameState);
    static void startGame();
    static void placeStone(unsigned int x, unsigned int y, field_type colour, std::string &err);
    static void set_game_rules(std::string ruleset_string,  std::string &err);

    static wxEvtHandler* getMainThreadEventHandler();
    static void showError(const std::string& title, const std::string& message);
    static void showStatus(const std::string& message);
    static void showNewRoundMessage(game_state* oldGameState, game_state* newGameState);
    static void showGameOverMessage();

private:
    static GameWindow* _gameWindow;
    static ConnectionPanel* _connectionPanel;
    static MainGamePanel* _mainGamePanel;

    static player* _me;
    static game_state* _currentGameState;
};


#endif //GOMOKUUI_GAMECONTROLLER_H