#ifndef GOMOKU_CLIENT_MAINGAMEPANEL_H
#define GOMOKU_CLIENT_MAINGAMEPANEL_H

#include <wx/wx.h>
#include <wx/mediactrl.h> // background music
#include <wx/notebook.h>
#include "../../common/game_state/game_state.h"
#include "../windows/game_window.h"


enum class icon_type {
    About,
    Settings,
    Help
};

enum sound_type{
    click_button_sound,
    place_stone_sound,
    rematch_sound,
    forfeit_sound
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
    wxColor dark_green = wxColor(54, 81, 39);

    wxMediaCtrl* background_music_player; // wxMediaCtrl for background music
    void on_music_stop(wxMediaEvent& WXUNUSED(event));

    void build_before_start(game_state* game_state, player* me);
    void build_playing_board(game_state* game_state, player* me);
    void build_scoreboard(game_state* game_state, player* me);
    void build_forfeit_button(game_state* game_state, player* me);
    void build_swap_field(game_state* game_state, player* me);
    void build_game_over_field(game_state* game_state, player* me);
    void build_icons(icon_type iconType, std::string path, wxPoint position);
    void close_all_dialogs();

    bool is_muted = false;
    void play_sound(sound_type sound);
    static const std::unordered_map<sound_type, std::string> _sound_type_to_string;

    void build_about_text(wxMouseEvent& event);
    void build_help_text(wxMouseEvent& event);
    wxStaticText* build_static_text(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold = false);

    void build_about_image(wxMouseEvent& event);

    // define key constant layout values
    wxSize const panel_size = wxSize(960, 760); // also set in the constructor implementation
    wxPoint const table_center = wxPoint(475, 400);
    wxSize const button_size = wxSize(130, 130/2.56);

    wxPoint const scoreboard_position = wxPoint(730, 300);
    wxSize const scoreboard_size = wxSize(230, 230/1.93);

    wxPoint const swap_field_position = wxPoint(5, 350);
    wxSize const swap_field_size = wxSize(230, 180);

    int const scale_factor  = 1000/400;
    wxSize const stone_size = wxSize(50/scale_factor, 50/scale_factor);
    wxSize const board_size = wxSize(1000/scale_factor, 1000/scale_factor);
    const int grid_spacing = 70;
    wxSize const grid_corner_offset = wxSize(12/scale_factor,  12/scale_factor);


    wxPoint const turn_indicator_offset = wxPoint(1000 / (scale_factor * 2) - 100, -30);
    wxPoint const turn_indicator_stone_offset = wxPoint(200, -5);
};


#endif //GOMOKU_CLIENT_MAINGAMEPANEL_H
