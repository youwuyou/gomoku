#include "MainGamePanel.h"
#include "../uiElements/ImagePanel.h"
#include "../GameController.h"


MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {
}

void MainGamePanel::buildGameState(game_state* gameState, player* me) {

    // remove any existing UI
    this->DestroyChildren();

    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();

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

    double anglePerPlayer = MainGamePanel::twoPi / (double) numberOfPlayers;

    // show all other players
    for(int i = 1; i < numberOfPlayers; i++) {

        // get player at i-th position after myself
        player* otherPlayer = players.at((myPosition + i) % numberOfPlayers);

        double playerAngle = (double) i * anglePerPlayer;
        int side = (2 * i) - numberOfPlayers; // side < 0 => right, side == 0 => center, side > 0 => left

        this->buildOtherPlayerLabels(gameState, otherPlayer, playerAngle, side);
    }

    // show turn indicator below card piles
    this->buildTurnIndicator(gameState, me);

    // show our own player
    this->buildThisPlayer(gameState, me);

    // update layout
    this->Layout();
}


void MainGamePanel::buildOtherPlayerLabels(game_state* gameState, player* otherPlayer, double playerAngle, int side) {

    long textAlignment = wxALIGN_CENTER;
    int labelOffsetX = 0;

    if(side < 0) { // right side
        textAlignment = wxALIGN_LEFT;
        labelOffsetX = 85;

    } else if(side > 0) { // left side
        textAlignment = wxALIGN_RIGHT;
        labelOffsetX = -85;
    }

    // define the ellipse which represents the virtual player circle
    double horizontalRadius = MainGamePanel::otherPlayerLabelDistanceFromCenter * 1.25; // 1.25 to horizontally elongate players' circle (but less than the hands' circle)
    double verticalRadius = MainGamePanel::otherPlayerLabelDistanceFromCenter;

    // get this player's position on that ellipse
    wxPoint labelPosition = MainGamePanel::tableCenter;
    labelPosition += this->getPointOnEllipse(horizontalRadius, verticalRadius, playerAngle);
    labelPosition += wxSize(labelOffsetX, 0);

    // if game has not yet started, we only have two lines
    if(!gameState->is_started()) {
        this->buildStaticText(
                otherPlayer->get_player_name(),
                labelPosition + wxSize(-100, -18),
                wxSize(200, 18),
                textAlignment,
                true
        );
        this->buildStaticText(
                "waiting...",
                labelPosition + wxSize(-100, 0),
                wxSize(200, 18),
                textAlignment
        );

    } else {
        this->buildStaticText(
                otherPlayer->get_player_name(),
                labelPosition + wxSize(-100, -27),
                wxSize(200, 18),
                textAlignment,
                true
        );
        this->buildStaticText(
                std::to_string(otherPlayer->get_score()) + " minus points",
                labelPosition + wxSize(-100, -9),
                wxSize(200, 18),
                textAlignment
        );

        // Show other player's status label
        std::string statusText = "waiting...";
        bool bold = false;
        /*if(otherPlayer->has_folded()) {
            statusText = "Folded!";
        } else */if(otherPlayer == gameState->get_current_player()) {
            statusText = "their turn";
            bold = true;
        }
        this->buildStaticText(
                statusText,
                labelPosition + wxSize(-100, 9),
                wxSize(200, 18),
                textAlignment,
                bold
        );
    }
}


void MainGamePanel::buildTurnIndicator(game_state *gameState, player *me) {
    if(gameState->is_started() && gameState->get_current_player() != nullptr) {

        std::string turnIndicatorText = "It's " + gameState->get_current_player()->get_player_name() + "'s turn!";
        if(gameState->get_current_player() == me) {
            turnIndicatorText = "It's your turn!";
        }

        wxPoint turnIndicatorPosition = MainGamePanel::tableCenter + MainGamePanel::turnIndicatorOffset;

        this->buildStaticText(
                turnIndicatorText,
                turnIndicatorPosition,
                wxSize(200, 18),
                wxALIGN_CENTER,
                true
        );
    }
}


void MainGamePanel::buildThisPlayer(game_state* gameState, player* me) {
    //TODO
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
