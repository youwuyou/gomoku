#include "main_game_panel.h"
#include "../uiElements/image_panel.h"
#include "../game_controller.h"
#include "../../common/exceptions/gomoku_exception.h"

// for game mode choice
const std::unordered_map<std::string, std::string> main_game_panel::_pretty_string_to_ruleset_string = {
        {"Freestyle", "freestyle"},
        {"Swap2", "swap2"},
        {"Swap after first move","swap_after_first_move"},
};

// for game mode choice
const std::unordered_map<std::string, std::string> main_game_panel::_ruleset_string_to_pretty_string = {
        {"freestyle", "Freestyle"},
        {"swap2", "Swap2"},
        {"swap_after_first_move", "Swap after first move"},
};

// for sound playing
const std::unordered_map<sound_type, std::string> main_game_panel::_sound_type_to_string = {
        {click_button_sound, "assets/music/click-button.wav"},
        {place_stone_sound, "assets/music/place-stone-sound.wav"},
        {rematch_sound, "assets/music/rematch.wav"},
        {forfeit_sound, "assets/music/forfeit.wav"},
};

main_game_panel::main_game_panel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 760)) {
    // setup media control for background music
    this->background_music_player = new wxMediaCtrl();
    wxString filePath = "assets/music/chinese-journey.wav";
    this->background_music_player->Create(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    (long) wxMEDIABACKEND_WMP10);
    this->background_music_player->Load(filePath);
    this->background_music_player->Play();
    this->background_music_player->SetVolume(1.0);
}

void main_game_panel::build_game_state(game_state* game_state, player* me) {

    // remove any existing UI
    this->DestroyChildren();
    // close all open dialogs
    main_game_panel::close_all_dialogs();

    std::vector<player*> players = game_state->get_players();

    // find our own player object in the list of players
    int my_position = -1;
    std::vector<player*>::iterator it = std::find_if (players.begin(), players.end(), [me](const player* x) {
        return x->get_id() == me->get_id();
    });
    if (it < players.end()) {
        me = *it;
        my_position = it - players.begin();
    } else {
        game_controller::show_error("Game state error", "Could not find this player among players of server game.");
        return;
    }

    // show game mode choice and start screen if the game is not yet started
    if(!game_state->is_started() && !game_state->is_finished()){
        this->build_before_start(game_state, me);
    } else {

        // show the board at the center
        this->build_playing_board(game_state, me);

        // show the scoreboard
        this->build_scoreboard(game_state, me);

        if(!game_state->is_finished()){
            build_forfeit_button(game_state, me);
        }

        // show swap field if applicable
        if (game_state->get_swap_next_turn() && game_state->get_current_player() == me) {
            this->build_swap_field(game_state, me);
        }

        // show the restart options if the game is finished
        if (game_state->is_finished()) {
            this->build_game_over_field(game_state, me);
        }
    }

    // build icon buttons for about, settings and help
    this->build_icons(icon_type::About, "assets/buttons/button_about.png", wxPoint(30, 30));
    this->build_icons(icon_type::Settings, "assets/buttons/button_settings.png", wxPoint(30, 100));
    this->build_icons(icon_type::Help, "assets/buttons/button_help.png", wxPoint(30, 170));

    // update layout
    this->Layout();
}

void main_game_panel::build_before_start(game_state* game_state, player* me){

    // Setup two nested box sizers, in order to align our UI to the bottom center
    wxBoxSizer* outer_layout = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(outer_layout);

    outer_layout->AddSpacer(220);

    wxBoxSizer* inner_layout = new wxBoxSizer(wxVERTICAL);
    outer_layout->Add(inner_layout, 1, wxALIGN_TOP);


    // show background for game mode selection
    wxSize panel_size = this->GetSize();
    image_panel *background_panel = new image_panel(this, "assets/background_gameselection.jpg", wxBITMAP_TYPE_ANY, wxDefaultPosition, panel_size);
    background_panel->Lower(); // background at bottom-most layer

    // add a spacer for the gap at the top
    inner_layout->AddSpacer(155);

    // show dropdown for game mode choice to first player
    if (game_state->get_current_player() == me && game_state->get_opening_rules()->get_ruleset() == ruleset_type::uninitialized) {

        wxStaticText* game_rule_dropdown_text = build_static_text(
                "Please choose a game style:",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER,
                true
        );
        game_rule_dropdown_text->SetForegroundColour(dark_green);
        inner_layout->Add(game_rule_dropdown_text, 0, wxALIGN_CENTER);

        wxArrayString game_rule_choices;
        game_rule_choices.Add("Freestyle");
        game_rule_choices.Add("Swap after first move");
        game_rule_choices.Add("Swap2");

        wxComboBox* game_rule_dropdown = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, game_rule_choices, wxCB_DROPDOWN | wxCB_READONLY);
        inner_layout->Add(game_rule_dropdown, 0, wxALIGN_CENTER, 10);
        game_rule_dropdown->SetSelection(0);

        // add a spacer for the gap
        inner_layout->AddSpacer(5);

        // add a button for confirming the ruleset choice
        std::string err;
        image_panel* choose_rules_button = new image_panel(this, "assets/buttons/button_confirm_choice.png", wxBITMAP_TYPE_ANY,
                                                         wxDefaultPosition,
                                                         main_game_panel::button_size);

        choose_rules_button->SetCursor(wxCursor(wxCURSOR_HAND));
        choose_rules_button->Bind(wxEVT_LEFT_UP, [game_rule_dropdown, this, &err](wxMouseEvent &event) {
            this->play_sound(click_button_sound);
            game_controller::set_game_rules(_pretty_string_to_ruleset_string.at(std::string(game_rule_dropdown->GetValue())), err);
        });
        inner_layout->Add(choose_rules_button, 0, wxALIGN_CENTER, 10);
    }

    // show chosen ruleset type if ruleset has been chosen
    if (game_state->get_opening_rules()->get_ruleset() != ruleset_type::uninitialized) {
        inner_layout->AddSpacer(40);
        std::string ruleset_string = _ruleset_string_to_pretty_string.at(opening_rules::_ruleset_type_to_string.at(game_state->get_opening_rules()->get_ruleset()));
        wxStaticText* game_rule_chosen_text = build_static_text(
                std::string("Chosen game style: "),
                wxDefaultPosition,
                wxSize(400, 18),
                wxALIGN_CENTER,
                true
        );
        wxStaticText* game_rule_text = build_static_text(
                ruleset_string,
            wxDefaultPosition,
            wxSize(400, 18),
            wxALIGN_CENTER,
            true
        );
        game_rule_chosen_text->SetForegroundColour(dark_green);
        game_rule_text->SetForegroundColour(dark_green);
        inner_layout->Add(game_rule_chosen_text, 0, wxALIGN_CENTER);
        inner_layout->Add(game_rule_text, 0, wxALIGN_CENTER);
        inner_layout->AddSpacer(35);
    }

    // Show the label with our player name
    wxStaticText* player_name = build_static_text(
            me->get_player_name(),
            wxDefaultPosition,
            wxSize(200, 18),
            wxALIGN_CENTER,
            true
    );
    player_name->SetForegroundColour(dark_green);
    inner_layout->Add(player_name, 0, wxALIGN_CENTER);

    // say that the game has not yet started
    wxStaticText* waiting_text = build_static_text(
            "waiting for game to start...",
            wxDefaultPosition,
            wxSize(200, 18),
            wxALIGN_CENTER
            );
    waiting_text->SetForegroundColour(dark_green);
    inner_layout->Add(waiting_text, 0, wxALIGN_CENTER, 8);
    inner_layout->AddSpacer(5);

    // show button that allows our player to start the game
    image_panel* start_game_button = new image_panel(this, "assets/buttons/button_start_game.png", wxBITMAP_TYPE_ANY,
                                                    wxDefaultPosition,
                                                    main_game_panel::button_size);

    start_game_button->SetCursor(wxCursor(wxCURSOR_HAND));
    start_game_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
        this->play_sound(click_button_sound);
        game_controller::start_game();
    });
    inner_layout->Add(start_game_button, 0, wxALIGN_CENTER, 8);
}


void main_game_panel::build_playing_board(game_state* game_state, player *me) {

        // show background
        wxSize panel_size = this->GetSize();
        image_panel *background_panel = new image_panel(this, "assets/background.jpg", wxBITMAP_TYPE_ANY, wxDefaultPosition, panel_size);
        background_panel->Lower(); // This ensures the background is behind all other elements

        // show board image
        wxPoint playing_board_position = main_game_panel::table_center - main_game_panel::board_size / 2;
        image_panel *playing_board_panel = new image_panel(this, "assets/playing_board.png", wxBITMAP_TYPE_ANY, playing_board_position, main_game_panel::board_size);

        // show stones on the playing board
        std::vector<std::vector<field_type>> playing_board = game_state->get_playing_board();
        unsigned int board_spot_num = playing_board.size();

        for (unsigned int i = 0; i < board_spot_num; i++) {
            for (unsigned int j = 0; j < board_spot_num; j++) {
                // if a stone is placed at that spot, render it
                if (playing_board.at(i).at(j) != field_type::empty) {
                    int current_stone = playing_board.at(i).at(j);
                    std::string current_stone_image;
                    if (current_stone == field_type::white_stone) {
                        current_stone_image = "assets/stone_white.png";
                    } else if (current_stone == field_type::black_stone) {
                        current_stone_image = "assets/stone_black.png";
                    } else {
                        throw gomoku_exception("Invalid stone colour for current_stone in rendering.");
                    }
                    wxPoint current_stone_position = table_center - board_size / 2 + grid_corner_offset +
                                                     i * wxPoint(0, grid_spacing / scale_factor) +
                                                     j * wxPoint(grid_spacing / scale_factor, 0) - stone_size / 2;
                    image_panel *current_stone_panel = new image_panel(this, current_stone_image, wxBITMAP_TYPE_ANY,
                                                                       current_stone_position,
                                                                       main_game_panel::stone_size);

                    // add drop shadow to stone
                    std::string stone_shadow_image = "assets/stone_shadow.png";
                    wxPoint current_stone_shadow_position = current_stone_position + stone_size / 17;
                    image_panel *current_stone_shadow_panel = new image_panel(this, stone_shadow_image,
                                                                              wxBITMAP_TYPE_ANY,
                                                                              current_stone_shadow_position,
                                                                              main_game_panel::stone_size);

                } else {
                    // if no stone is present, show a transparent button on each spot, if it is currently our turn
                    if (game_state->get_current_player() == me && !game_state->get_swap_next_turn() &&
                        !game_state->is_finished()) {
                        player_colour_type current_player_colour = game_state->get_current_player()->get_colour();
                        std::string transparent_stone_image = "assets/stone_transparent.png";
                        wxPoint current_stone_position = table_center - board_size / 2 + grid_corner_offset +
                                                         i * wxPoint(0, grid_spacing / scale_factor) +
                                                         j * wxPoint(grid_spacing / scale_factor, 0) - stone_size / 2;
                        image_panel *new_stone_button = new image_panel(this, transparent_stone_image,
                                                                        wxBITMAP_TYPE_ANY,
                                                                        current_stone_position,
                                                                        main_game_panel::stone_size);

                        new_stone_button->SetCursor(wxCursor(wxCURSOR_HAND));
                        std::string err;

                        field_type new_stone_colour = field_type::empty;

                        if (current_player_colour == player_colour_type::black) {
                            new_stone_colour = field_type::black_stone;
                        } else if (current_player_colour == player_colour_type::white) {
                            new_stone_colour = field_type::white_stone;
                        } else {
                            throw gomoku_exception("Invalid current player colour in new stone button rendering.");
                        }
                        unsigned int x = j;
                        unsigned int y = i;

                        new_stone_button->Bind(wxEVT_LEFT_UP, [x, y, new_stone_colour, this, &err](wxMouseEvent &event) {
                            this->play_sound(place_stone_sound);

                            game_controller::place_stone(x, y, new_stone_colour, err);
                        });
                    }
                }
            }
        }
}

void main_game_panel::build_scoreboard(game_state *game_state, player *me) {

    std::string scoreboard_image_string = "assets/button_frame_half.png";
    image_panel* scoreboard_frame = new image_panel(this, scoreboard_image_string, wxBITMAP_TYPE_ANY,
                                                     main_game_panel::scoreboard_position,
                                                     main_game_panel::scoreboard_size);

    for (int i = 0; i<game_state->get_players().size(); ++i){
        player* player = game_state->get_players().at(i);
        std::string player_name = player->get_player_name();
        if (player == me){
            player_name +=" (You)";
        }
        std::string player_points = std::to_string(player->get_score());
        player_points += " Points";

        wxStaticText* player_text = this->build_static_text(
                player_name,
                main_game_panel::scoreboard_position + wxPoint(53, 32 + i*30),
                wxSize(100, 20),
                wxALIGN_LEFT,
                true
        );
        player_text->SetForegroundColour(black);
        wxStaticText* player_points_text = this->build_static_text(
                player_points,
                main_game_panel::scoreboard_position + wxPoint(140, 32 + i*30),
                wxSize(80, 20),
                wxALIGN_LEFT,
                true
        );
        player_points_text->SetForegroundColour(black);

        if(game_state->get_current_player() == player){
            std::string current_player_colour = player::_player_colour_type_to_string.at(game_state->get_current_player()->get_colour());
            std::string current_player_stone_image = "assets/stone_" + current_player_colour + ".png";
            wxPoint turn_indicator_stone_position = main_game_panel::scoreboard_position + wxPoint(23, 30 + i*30);
            image_panel* turn_indicator_stone = new image_panel(this, current_player_stone_image, wxBITMAP_TYPE_ANY, turn_indicator_stone_position, main_game_panel::stone_size);
            turn_indicator_stone->SetToolTip("Colour to play: " + current_player_colour);

            // add drop shadow to stone
            std::string stone_shadow_image = "assets/stone_shadow.png";
            wxPoint turn_indicator_stone_shadow_position = turn_indicator_stone_position + stone_size / 17;
            image_panel *turn_indicator_stone_shadow_panel = new image_panel(this, stone_shadow_image, wxBITMAP_TYPE_ANY,
                                                                             turn_indicator_stone_shadow_position, main_game_panel::stone_size);
        }
    }
}

void main_game_panel::build_forfeit_button(game_state* game_state, player* me){

    std::string forfeit_button_string = "assets/buttons/button_forfeit.png";
    wxPoint forfeit_position = wxPoint(scoreboard_position.x + 20, scoreboard_position.y + scoreboard_size.y + 20);
    image_panel* forfeit_button = new image_panel(this, forfeit_button_string, wxBITMAP_TYPE_ANY,
                                                  forfeit_position,
                                                  main_game_panel::button_size);
    forfeit_button->SetCursor(wxCursor(wxCURSOR_HAND));
    forfeit_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
        this->play_sound(forfeit_sound);
        game_controller::forfeit();
    });
}

void main_game_panel::build_swap_field(game_state* game_state, player *me){
    ruleset_type ruleset_name = game_state->get_opening_rules()->get_ruleset();

    wxSize current_swap_field_size = main_game_panel::swap_field_size;
    if (ruleset_name == swap2 && game_state->get_swap_decision() == no_decision_yet){
        current_swap_field_size = wxSize(main_game_panel::swap_field_size.x, main_game_panel::swap_field_size.y + 50);
    }

    std::string swap_field_image_string = "assets/button_frame_full.png";
    image_panel* scoreboard_frame = new image_panel(this, swap_field_image_string, wxBITMAP_TYPE_ANY,
                                                    main_game_panel::swap_field_position,
                                                    current_swap_field_size);

    std::string title_string = main_game_panel::_ruleset_string_to_pretty_string.at(opening_rules::_ruleset_type_to_string.at(ruleset_name));

    wxStaticText* title_text = this->build_static_text(
            title_string,
            main_game_panel::swap_field_position + wxPoint((swap_field_size.x/2) - 190/2, 30),
            wxSize(190, 20),
            wxALIGN_CENTER,
            true
    );
    title_text->SetForegroundColour(black);

    std::string swap_button_string = "assets/buttons/button_swap_black.png";
    std::string no_swap_button_string = "assets/buttons/button_continue_white.png";
    if(game_state->get_swap_decision() == defer_swap){
        swap_button_string = "assets/buttons/button_swap_white.png";
        no_swap_button_string = "assets/buttons/button_continue_black.png";
    }

    image_panel *swap_button = new image_panel(this, swap_button_string, wxBITMAP_TYPE_ANY,
                                               main_game_panel::swap_field_position +
                                               wxPoint((swap_field_size.x/2) - (button_size.x / 2), 50),
                                               main_game_panel::button_size);
    swap_button->SetCursor(wxCursor(wxCURSOR_HAND));
    swap_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
        this->play_sound(click_button_sound);
        game_controller::send_swap_decision(swap_decision_type::do_swap);
    });

    image_panel *no_swap_button = new image_panel(this, no_swap_button_string, wxBITMAP_TYPE_ANY,
                                                  main_game_panel::swap_field_position +
                                                  wxPoint((swap_field_size.x/2) - (button_size.x / 2), 100),
                                                  main_game_panel::button_size);
    no_swap_button->SetCursor(wxCursor(wxCURSOR_HAND));
    no_swap_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
        this->play_sound(click_button_sound);
        game_controller::send_swap_decision(swap_decision_type::do_not_swap);
        });

    if(ruleset_name == swap2 && game_state->get_swap_decision() == no_decision_yet){
        std::string defer_swap_button_string = "assets/buttons/button_defer_choice.png";
        image_panel *defer_swap_button = new image_panel(this, defer_swap_button_string, wxBITMAP_TYPE_ANY,
                                                      main_game_panel::swap_field_position +
                                                      wxPoint((swap_field_size.x/2) - (button_size.x / 2), 150),
                                                      main_game_panel::button_size);
        defer_swap_button->SetCursor(wxCursor(wxCURSOR_HAND));
        defer_swap_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
            this->play_sound(click_button_sound);
            game_controller::send_swap_decision(swap_decision_type::defer_swap);
        });

    }
}

void main_game_panel::build_game_over_field(game_state* game_state, player* me){

    std::string swap_field_image_string = "assets/button_frame_full.png";
    image_panel* scoreboard_frame = new image_panel(this, swap_field_image_string, wxBITMAP_TYPE_ANY,
                                                    main_game_panel::swap_field_position,
                                                    wxSize(230, 260));

    std::string title_string = "Game Over!";

    std::string winner_string = game_state->get_current_player()->get_player_name();
    winner_string += " won!";
    if(game_state->is_tied()){
        winner_string = "Tied!";
    }

    wxStaticText* title_text = this->build_static_text(
            title_string,
            main_game_panel::swap_field_position + wxPoint((swap_field_size.x/2) - 190/2, 33),
            wxSize(190, 20),
            wxALIGN_CENTER,
            true
    );
    title_text->SetForegroundColour(black);
    wxStaticText* winner_text = this->build_static_text(
            winner_string,
            main_game_panel::swap_field_position + wxPoint((swap_field_size.x/2) - 190/2, 53),
            wxSize(190, 20),
            wxALIGN_CENTER,
            true
    );
    winner_text->SetForegroundColour(black);

    std::string rematch_button_string = "assets/buttons/button_rematch.png";
    std::string change_ruleset_button_string = "assets/buttons/button_change_game_mode.png";
    std::string close_game_button_string = "assets/buttons/button_close_game.png";

    image_panel *rematch_button = new image_panel(this, rematch_button_string, wxBITMAP_TYPE_ANY,
                                               main_game_panel::swap_field_position +
                                               wxPoint((swap_field_size.x/2) - (button_size.x / 2), 73),
                                               main_game_panel::button_size);
    rematch_button->SetCursor(wxCursor(wxCURSOR_HAND));
    rematch_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
        this->play_sound(rematch_sound);
        game_controller::send_restart_decision(false);
    });

    image_panel *change_ruleset_button = new image_panel(this, change_ruleset_button_string, wxBITMAP_TYPE_ANY,
                                                  main_game_panel::swap_field_position +
                                                  wxPoint((swap_field_size.x/2) - (button_size.x / 2), 123),
                                                  main_game_panel::button_size);
    change_ruleset_button->SetCursor(wxCursor(wxCURSOR_HAND));
    change_ruleset_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
        this->play_sound(rematch_sound);
        game_controller::send_restart_decision(true);
    });

    image_panel *close_game_button = new image_panel(this, close_game_button_string, wxBITMAP_TYPE_ANY,
                                                         main_game_panel::swap_field_position +
                                                         wxPoint((swap_field_size.x/2) - (button_size.x / 2), 173),
                                                         main_game_panel::button_size);
    close_game_button->SetCursor(wxCursor(wxCURSOR_HAND));
    close_game_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
        this->play_sound(click_button_sound);
        game_controller::close_game();
    });
}

// build icons for about, settings and help
void main_game_panel::build_icons(/*game_state* gameState, player *me, */icon_type iconType, std::string path, wxPoint position) {
    image_panel* icon_button = new image_panel(this, path, wxBITMAP_TYPE_ANY,
                                               position,
                                               wxSize(70,70));
    icon_button->SetCursor(wxCursor(wxCURSOR_HAND));
    //icon_button->SetPosition(position); // Set position to top-left corner with margin

    // switch cases based on the icon type to be displayed
    switch (iconType) {
        case icon_type::About:
            icon_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {
                this->play_sound(click_button_sound);
                // this->build_about_image(event);
                this->build_about_text(event);
            });
            break;

        case icon_type::Settings:
            if(is_muted) {
                icon_button->SetToolTip("Unmute sounds");
                icon_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {
                    is_muted = false;
                    this->play_sound(click_button_sound);
                    this->build_icons(icon_type::Settings, "assets/buttons/button_settings.png", wxPoint(30, 100));
                });
            } else if (!is_muted) {
                icon_button->SetToolTip("Mute sounds");
                icon_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {
                    is_muted = true;
                    this->build_icons(icon_type::Settings, "assets/buttons/button_settings.png", wxPoint(30, 100));
                });
            }
            break;

        case icon_type::Help:
            icon_button->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {
                this->play_sound(click_button_sound);
                this->build_help_text(event);
            });
            break;
    }
}

void main_game_panel::build_about_image(wxMouseEvent& event) {

    image_panel *about_image = new image_panel(this, 
                                                "assets/about_gomoku.png", 
                                                wxBITMAP_TYPE_ANY, 
                                                wxPoint(300, 150), 
                                                wxSize(400, 400));
    // wxFrame *imageFrame = new wxFrame(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, 450), wxBORDER_NONE);
    // wxImage aboutImage("assets/about_gomoku.png", wxBITMAP_TYPE_PNG);
    // wxBitmap aboutBitmap(aboutImage);


    // // Set the bitmap to display
    // wxStaticBitmap *imageDisplay = new wxStaticBitmap(this, wxID_ANY, aboutImage);

    // // Position and show the frame
    // imageFrame->SetPosition(wxPoint(250, 250)); // Set this to your desired position
    // imageFrame->Show(true);
    // about_image->Show(true);
}

void main_game_panel::build_about_text(wxMouseEvent& event) {
    wxString about_info = wxT("Authors: Haoanqin Gao, Julius König, Stephen Lincon, \n                Nicolas Müller, Rana Singh, You Wu \nVersion: 1.0.0\n\n© 2023 Wizards of the C Inc.");
    wxMessageBox(about_info, wxT("About Gomoku"), wxOK | wxICON_INFORMATION, this);
}

void main_game_panel::build_help_text(wxMouseEvent& event) {
    wxDialog ruleDialog(this, wxID_ANY, wxT("Game Rules"), wxDefaultPosition, wxDefaultSize);
    wxNotebook* notebook = new wxNotebook(&ruleDialog, wxID_ANY);

    // helper function to create a panel with text and image for the notebook
    auto createRulePanel = [notebook](const wxString& text, const wxString& rule) -> wxPanel* {
        wxPanel* panel = new wxPanel(notebook, wxID_ANY);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* staticText = new wxStaticText(panel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        sizer->Add(staticText, 0, wxALL, 5);

        wxStaticText* ruleText = new wxStaticText(panel, wxID_ANY, rule, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        sizer->Add(ruleText, 0, wxALL, 5);
        
        panel->SetSizer(sizer);
        return panel;
    };

    // create tabs for the notebook
    // wxPanel* freestylePanel = createRulePanel(wxT("Freestyle rules..."), wxT("assets/rules_freestyle.png"));
    // wxPanel* swapPanel = createRulePanel(wxT("Swap rules..."), wxT("assets/rules_swap1.png"));
    // wxPanel* swap2Panel = createRulePanel(wxT("Swap2 rules..."), wxT("assets/rules_swap2.png"));
    wxPanel* freestylePanel = createRulePanel(wxT(""), wxT("Black starts, and both players play in an alternating \n fashion until one player has won or there is a tie."));
    wxPanel* swapPanel = createRulePanel(wxT(""), wxT("After the first move, the second player has the option \n to switch to playing with black, or continuing as white. Free style rules from then on."));
    wxPanel* swap2Panel = createRulePanel(wxT(""), wxT("The first player starts by placing two black \n and one white stones. The second player then has the \n option to switch to black, continue as white, or place one more stone of each colour \n and defer the swapping choice to the first player, \n who then gets to swap to white or continue as black. Free style rules from then on."));

    // panels as tabs to the notebook
    notebook->AddPage(freestylePanel, wxT("Freestyle"), true); // true to make this the selected tab
    notebook->AddPage(swapPanel, wxT("Swap"));
    notebook->AddPage(swap2Panel, wxT("Swap2"));

    // sizer for the dialog and add the notebook to it
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(notebook, 1, wxEXPAND | wxALL, 5);

    wxButton* continueButton = new wxButton(&ruleDialog, wxID_OK, wxT("Continue"));
    vbox->Add(continueButton, 0, wxALIGN_CENTER | wxALL, 5);

    // set the sizer for the rule dialog and fit the dialog to the contents of the sizer
    ruleDialog.SetSizerAndFit(vbox);
    ruleDialog.CentreOnParent();
    ruleDialog.ShowModal();
}

wxStaticText* main_game_panel::build_static_text(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold) {
    wxStaticText* static_text = new wxStaticText(this, wxID_ANY, content, position, size, textAlignment);
    if (bold) {
        wxFont font = static_text->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        static_text->SetFont(font);
    }
    return static_text;
}

void main_game_panel::close_all_dialogs(){
    for (wxDialog* dialog : this->_open_dialogs){
        if(dialog && dialog->IsModal()){
            dialog->EndModal(wxID_OK);
        }
    }
}

void main_game_panel::play_sound(sound_type sound) {
    if(!is_muted){
        wxSound sound_to_play(_sound_type_to_string.at(sound));
        sound_to_play.Play(wxSOUND_ASYNC);
    }
}