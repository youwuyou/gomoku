#ifndef GOMOKU_CLIENT_MAINGAMEPANEL_H
#define GOMOKU_CLIENT_MAINGAMEPANEL_H

#include <wx/wx.h>
#include "../../common/game_state/game_state.h"


class MainGamePanel : public wxPanel {

public:
    MainGamePanel(wxWindow* parent);

    void buildGameState(game_state* gameState, player* me);


private:

    void buildPlayingBoard(game_state* gameState, player* me);
    void buildTurnIndicator(game_state* gameState, player* me);
    void buildThisPlayer(game_state* gameState, player* me);
    void buildAbout(game_state* gameState, player* me);

    wxStaticText* buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold = false);

    wxSize getBoundsOfRotatedSquare(double edgeLength, double rotationAngle);
    double getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle);

    // might be cut-able
    wxPoint getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle);

    // define key constant layout values
    wxSize const panelSize = wxSize(960, 680); // also set in the constructor implementation
    wxPoint const tableCenter = wxPoint(480, 300);

    int const scale_factor  = 1000/400;
    wxSize const stoneSize = wxSize(50/scale_factor, 50/scale_factor);
    wxSize const boardSize = wxSize(1000/scale_factor, 1000/scale_factor);


    wxPoint const turnIndicatorOffset = wxPoint(-70, 98);
    wxPoint const turnIndicatorStoneOffset = wxPoint(10, 0);

    // might be cut-able
    double const twoPi = 6.28318530718;

};


#endif //GOMOKU_CLIENT_MAINGAMEPANEL_H
