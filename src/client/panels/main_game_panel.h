#ifndef GOMOKU_CLIENT_MAINGAMEPANEL_H
#define GOMOKU_CLIENT_MAINGAMEPANEL_H

#include <wx/wx.h>
#include <wx/mediactrl.h> // background music
#include <wx/notebook.h>
#include "../../common/game_state/game_state.h"


enum class IconType {
    About,
    Settings,
    Help
};


class main_game_panel : public wxPanel {

public:
    main_game_panel(wxWindow* parent);

    void build_game_state(game_state* game_state, player* me);

    std::vector<wxDialog*> _open_dialogs;


    static const std::unordered_map<std::string, std::string> _pretty_string_to_ruleset_string;
    static const std::unordered_map<std::string, std::string> _ruleset_string_to_pretty_string;

private:
    wxColor white = wxColor(255, 255, 255);
    wxColor black = wxColor(0, 0, 0);

    wxMediaCtrl* backgroundMusicPlayer; // wxMediaCtrl for background music
    void on_music_stop(wxMediaEvent& WXUNUSED(event));

    void build_playing_board(game_state* game_state, player* me);
    void build_turn_indicator(game_state* game_state, player* me);
    void build_this_player(game_state* game_state, player* me);
    void build_icons(game_state* gameState, player *me, IconType iconType, std::string path, wxPoint position);
    void close_all_dialogs();

    void build_about_text(wxCommandEvent& event);
    void build_help_text(wxCommandEvent& event);
    wxStaticText* build_static_text(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold = false);

    // define key constant layout values
    wxSize const panel_size = wxSize(960, 680); // also set in the constructor implementation
    wxPoint const table_center = wxPoint(480, 325);

    int const scale_factor  = 1000/400;
    wxSize const stone_size = wxSize(50/scale_factor, 50/scale_factor);
    wxSize const board_size = wxSize(1000/scale_factor, 1000/scale_factor);
    const int grid_spacing = 70;
    wxSize const grid_corner_offset = wxSize(12/scale_factor,  12/scale_factor);


    wxPoint const turn_indicator_offset = wxPoint(1000 / (scale_factor * 2) - 100, -30);
    wxPoint const turn_indicator_stone_offset = wxPoint(200, -5);


};


#endif //GOMOKU_CLIENT_MAINGAMEPANEL_H
