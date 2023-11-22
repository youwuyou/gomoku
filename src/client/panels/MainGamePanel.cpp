#include "MainGamePanel.h"
#include "../uiElements/ImagePanel.h"
#include "../GameController.h"
#include "../../common/exceptions/GomokuException.h"


MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {
}

void MainGamePanel::buildGameState(game_state* gameState, player* me) {

    // remove any existing UI
    this->DestroyChildren();

    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = 2;

    // find our own player object in the list of players
    int myPosition = -1;
    std::vector<player*>::iterator it = std::find_if(players.begin(), players.end(), [me](const player* x) {
        return x->get_id() == me->get_id();
    });
    if (it < players.end()) {
        me = *it;
        myPosition = it - players.begin();
    } else {
        GameController::showError("Game state error", "Could not find this player among players of server game.");
        return;
    }

    // show the board at the center
    this->buildPlayingBoard(gameState, me);

    // show turn indicator below card piles
    this->buildTurnIndicator(gameState, me);

    // show our own player
    this->buildThisPlayer(gameState, me);

    // build About button
    //this->buildAbout(gameState, me);

    // update layout
    this->Layout();
}


void MainGamePanel::buildPlayingBoard(game_state* gameState, player *me) {

    // show the empty board as soon as the game is started
    if(gameState->is_started()) {

        // Show discard pile
        std::string boardImage = "assets/playing_board.png";

        wxPoint playingBoardPosition = MainGamePanel::tableCenter - MainGamePanel::boardSize/2;

        ImagePanel* playingBoard = new ImagePanel(this, boardImage, wxBITMAP_TYPE_ANY, playingBoardPosition, MainGamePanel::boardSize);
        playingBoard->SetToolTip("Playing board");

    }

    // show stones on the playing board
    std::vector<std::vector<stone*>> playing_board = gameState->get_playing_board();
    unsigned int board_spot_num = playing_board.size();

    for(unsigned int i=0; i<board_spot_num; i++){
        for(unsigned int j=0; j<board_spot_num; j++){
            if(playing_board.at(i).at(j) != nullptr){
                stone current_stone = *playing_board.at(i).at(j);
                std::string current_stone_image;
                if(current_stone.get_colour() == "white"){
                    current_stone_image = "assets/stone_white.png";
                } else if (current_stone.get_colour() == "black") {
                    current_stone_image = "assets/stone_black.png";
                } else {
                    throw GomokuException("Invalid stone colour for current_stone in rendering.");
                }
                wxPoint current_stone_position = tableCenter - boardSize/2 + wxPoint(16/scale_factor, 16/scale_factor) + i*wxPoint(0, 63) + j*wxPoint(63,0);
                ImagePanel* current_stone_panel = new ImagePanel(this, current_stone_image, wxBITMAP_TYPE_ANY, current_stone_position, MainGamePanel::stoneSize);
                current_stone_panel->SetToolTip(current_stone.get_colour() + " stone at (" + std::to_string(i) + ", " + std::to_string(j) + ")");
            }
        }
    }

}

void MainGamePanel::buildTurnIndicator(game_state *gameState, player *me) {
    if(gameState->is_started() && gameState->get_current_player() != nullptr) {

        std::string turnIndicatorText = "It's " + gameState->get_current_player()->get_player_name() + "'s turn!";
        if(gameState->get_current_player() == me) {
            turnIndicatorText = "It's your turn!";
        }

        wxPoint turnIndicatorPosition = MainGamePanel::tableCenter - MainGamePanel::boardSize/2 + MainGamePanel::turnIndicatorOffset;

        this->buildStaticText(
                turnIndicatorText,
                turnIndicatorPosition,
                wxSize(200, 18),
                wxALIGN_CENTER,
                true
        );

        std::string current_player_colour = gameState->get_current_player()->get_colour();
        std::string currentPlayerStoneImage = "assets/stone_" + current_player_colour + ".png";
        wxPoint turnIndicatorStonePosition = turnIndicatorPosition + MainGamePanel::turnIndicatorStoneOffset;
        ImagePanel* turnIndicatorStone = new ImagePanel(this, currentPlayerStoneImage, wxBITMAP_TYPE_ANY, turnIndicatorStonePosition, MainGamePanel::stoneSize);
        turnIndicatorStone->SetToolTip("Colour to play: " + current_player_colour);
    }
}


void MainGamePanel::buildThisPlayer(game_state* gameState, player* me) {

    // Setup two nested box sizers, in order to align our player's UI to the bottom center
    wxBoxSizer* outerLayout = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(outerLayout);
    wxBoxSizer* innerLayout = new wxBoxSizer(wxVERTICAL);
    outerLayout->Add(innerLayout, 1, wxALIGN_BOTTOM);

    // Show the label with our player name
    wxStaticText* playerName = buildStaticText(
            me->get_player_name(),
            wxDefaultPosition,
            wxSize(200, 18),
            wxALIGN_CENTER,
            true
    );
    innerLayout->Add(playerName, 0, wxALIGN_CENTER);

    // if the game has not yet started we say so
    if(!gameState->is_started()) {

        wxStaticText* playerPoints = buildStaticText(
                "waiting for game to start...",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER
        );
        innerLayout->Add(playerPoints, 0, wxALIGN_CENTER | wxBOTTOM, 8);

        // show button that allows our player to start the game
        wxButton* startGameButton = new wxButton(this, wxID_ANY, "Start Game!", wxDefaultPosition, wxSize(160, 64));
        startGameButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
            GameController::startGame();
        });
        innerLayout->Add(startGameButton, 0, wxALIGN_CENTER | wxBOTTOM, 8);

    } else {

        // show our player's points
        wxStaticText *playerPoints = buildStaticText(
                std::to_string(me->get_score()) + " points",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER
        );
        innerLayout->Add(playerPoints, 0, wxALIGN_CENTER | wxBOTTOM, 8);

        /* might be re-usable if we want to have a "give up" button
         *
        // if we haven't folded yet, and it's our turn, display Fold button
        } else if (gameState->get_current_player() == me) {
            wxButton *foldButton = new wxButton(this, wxID_ANY, "Fold", wxDefaultPosition, wxSize(80, 32));
            foldButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
                GameController::fold();
            });
            innerLayout->Add(foldButton, 0, wxALIGN_CENTER | wxBOTTOM, 8);


        // if it's not our turn, display "waiting..."
        } else {*/
            wxStaticText *playerStatus = buildStaticText(
                    "waiting...",
                    wxDefaultPosition,
                    wxSize(200, 32),
                    wxALIGN_CENTER
            );
            innerLayout->Add(playerStatus, 0, wxALIGN_CENTER | wxBOTTOM, 8);
        //}

    }
}


wxStaticText* MainGamePanel::buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold) {
    wxStaticText* staticText = new wxStaticText(this, wxID_ANY, content, position, size, textAlignment);
    if(bold) {
        wxFont font = staticText->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        staticText->SetFont(font);
    }
    return staticText;
}


wxSize MainGamePanel::getBoundsOfRotatedSquare(double edgeLength, double rotationAngle) {
    double newEdgeLength = this->getEdgeLengthOfRotatedSquare(edgeLength, rotationAngle);
    return wxSize(newEdgeLength, newEdgeLength);
}


double MainGamePanel::getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle) {
    return originalEdgeLength * (abs(sin(rotationAngle)) + abs(cos(rotationAngle)));
}


wxPoint MainGamePanel::getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle) {
    return wxPoint((int) (sin(angle) * horizontalRadius), (int) (cos(angle) * verticalRadius));
}
