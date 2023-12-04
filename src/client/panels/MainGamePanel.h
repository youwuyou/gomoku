#ifndef GOMOKU_CLIENT_MAINGAMEPANEL_H
#define GOMOKU_CLIENT_MAINGAMEPANEL_H

#include <wx/wx.h>
#include <wx/mediactrl.h> // background music
#include "../../common/game_state/game_state.h"


class MainGamePanel : public wxPanel {

public:
    MainGamePanel(wxWindow* parent);

    void buildGameState(game_state* gameState, player* me);

    std::vector<wxDialog*> _open_dialogs;


    static const std::unordered_map<std::string, std::string> _pretty_string_to_ruleset_string;
    static const std::unordered_map<std::string, std::string> _ruleset_string_to_pretty_string;

private:
    wxMediaCtrl* backgroundMusicPlayer; // wxMediaCtrl for background music
    void OnMusicStop(wxMediaEvent& WXUNUSED(event));

    void buildPlayingBoard(game_state* gameState, player* me);
    void buildTurnIndicator(game_state* gameState, player* me);
    void buildThisPlayer(game_state* gameState, player* me);
    void buildAbout(game_state* gameState, player* me);

    void close_all_dialogs();

    void buildAboutText(wxCommandEvent& event);
    wxStaticText* buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold = false);

    wxSize getBoundsOfRotatedSquare(double edgeLength, double rotationAngle);
    double getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle);

    // might be cut-able
    wxPoint getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle);

    // define key constant layout values
    wxSize const panelSize = wxSize(960, 680); // also set in the constructor implementation
    wxPoint const table_center = wxPoint(480, 300);

    int const scale_factor  = 1000/400;
    wxSize const stone_size = wxSize(50/scale_factor, 50/scale_factor);
    wxSize const board_size = wxSize(1000/scale_factor, 1000/scale_factor);
    const int grid_spacing = 70;
    wxSize const grid_corner_offset = wxSize(12/scale_factor,  12/scale_factor);


    wxPoint const turnIndicatorOffset = wxPoint(1000/(scale_factor*2)-100, -30);
    wxPoint const turnIndicatorStoneOffset = wxPoint(200, -5);

    // might be cut-able
    double const twoPi = 6.28318530718;

};


#endif //GOMOKU_CLIENT_MAINGAMEPANEL_H
