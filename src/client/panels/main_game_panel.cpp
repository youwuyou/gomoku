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


main_game_panel::main_game_panel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {}

void main_game_panel::build_game_state(game_state* game_state, player* me) {

    // remove any existing UI
    this->DestroyChildren();
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

    // show the board at the center
    this->build_playing_board(game_state, me);

    // show turn indicator below card piles
    this->build_turn_indicator(game_state, me);

    // show our own player
    this->build_this_player(game_state, me);

    // build About button
    // this->build_about(game_state, me); 
    // this->build_settings(game_state, me);
    // this->build_help(game_state, me);

    this->build_icons(game_state, me, "assets/icons/info.png", wxPoint(30, 30));
    this->build_icons(game_state, me, "assets/icons/cog.png", wxPoint(30, 100));
    this->build_icons(game_state, me, "assets/icons/help.png", wxPoint(30, 170));

    // update layout
    this->Layout();
}

void main_game_panel::build_playing_board(game_state* game_state, player *me) {

    // only show the board, stones and buttons if the game has started
    if (game_state->is_started()) {

        // show background
        std::string background_image = "assets/background.jpg";
        wxSize panel_size = this->GetSize();
        image_panel *background_panel = new image_panel(this, background_image, wxBITMAP_TYPE_ANY, wxDefaultPosition, panel_size);
        background_panel->Lower(); // This ensures the background is behind all other elements

        // show board image
        std::string board_image = "assets/playing_board.png";
        wxPoint playing_board_position = main_game_panel::table_center - main_game_panel::board_size / 2;
        image_panel *playing_board_panel = new image_panel(this, board_image, wxBITMAP_TYPE_ANY, playing_board_position, main_game_panel::board_size);

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
                                                                       current_stone_position, main_game_panel::stone_size);

                    // add drop shadow to stone
                    std::string stone_shadow_image = "assets/stone_shadow.png";
                    wxPoint current_stone_shadow_position = current_stone_position + stone_size/17;
                    image_panel *current_stone_shadow_panel = new image_panel(this, stone_shadow_image, wxBITMAP_TYPE_ANY,
                                                                              current_stone_shadow_position, main_game_panel::stone_size);

                } else {
                    // if no stone is present, show a transparent button on each spot, if it is currently our turn
                    if (game_state->get_current_player() == me && !game_state->get_swap_next_turn()) {
                        player_colour_type current_player_colour = game_state->get_current_player()->get_colour();
                        std::string transparent_stone_image = "assets/stone_transparent.png";
                        wxPoint current_stone_position = table_center - board_size / 2 + grid_corner_offset +
                                                         i * wxPoint(0, grid_spacing / scale_factor) +
                                                         j * wxPoint(grid_spacing / scale_factor, 0) - stone_size / 2;
                        image_panel *new_stone_button = new image_panel(this, transparent_stone_image, wxBITMAP_TYPE_ANY,
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

                        new_stone_button->Bind(wxEVT_LEFT_UP, [x, y, new_stone_colour, &err](wxMouseEvent &event) {
                        wxSound stone_place_sound("assets/music/place-stone-sound.wav");
                        stone_place_sound.Play(wxSOUND_ASYNC);

                            game_controller::place_stone(x, y, new_stone_colour, err);
                        });
                    }
                }
            }
        }
    }

}

void main_game_panel::build_turn_indicator(game_state *game_state, player *me) {
    if (game_state->is_started() && game_state->get_current_player() != nullptr) {

        std::string turn_indicator_text_string = "It's " + game_state->get_current_player()->get_player_name() + "'s turn!";
        if (game_state->get_current_player() == me) {
            turn_indicator_text_string = "It's your turn!";
        }

        wxPoint turn_indicator_position = main_game_panel::table_center - main_game_panel::board_size / 2 + main_game_panel::turn_indicator_offset;

        wxStaticText* turn_indicator_text = this->build_static_text(
                turn_indicator_text_string,
                turn_indicator_position,
                wxSize(200, 18),
                wxALIGN_CENTER,
                true
        );
        turn_indicator_text->SetForegroundColour(black);

        std::string current_player_colour = player::_player_colour_type_to_string.at(game_state->get_current_player()->get_colour());
        std::string current_player_stone_image = "assets/stone_" + current_player_colour + ".png";
        wxPoint turn_indicator_stone_position = turn_indicator_position + main_game_panel::turn_indicator_stone_offset;
        image_panel* turn_indicator_stone = new image_panel(this, current_player_stone_image, wxBITMAP_TYPE_ANY, turn_indicator_stone_position, main_game_panel::stone_size);
        turn_indicator_stone->SetToolTip("Colour to play: " + current_player_colour);

        // add drop shadow to stone
        std::string stone_shadow_image = "assets/stone_shadow.png";
        wxPoint turn_indicator_stone_shadow_position = turn_indicator_stone_position + stone_size / 17;
        image_panel *turn_indicator_stone_shadow_panel = new image_panel(this, stone_shadow_image, wxBITMAP_TYPE_ANY,
                                                                         turn_indicator_stone_shadow_position, main_game_panel::stone_size);
    }
}


void main_game_panel::build_this_player(game_state* game_state, player* me) {
    // close all open dialogs
    main_game_panel::close_all_dialogs();

    // Setup two nested box sizers, in order to align our player's UI to the bottom center
    wxBoxSizer* outer_layout = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(outer_layout);
    wxBoxSizer* inner_layout = new wxBoxSizer(wxVERTICAL);
    outer_layout->Add(inner_layout, 1, wxALIGN_BOTTOM);

    // show dropdown for game mode selection for first player if the game has not yet started and the opening ruleset is "uninitialized"
    if(!game_state->is_started()){
        // show background for game mode selection
        std::string background_image = "assets/gameselec_background.png";
        wxSize panel_size = this->GetSize();
        image_panel *background_panel = new image_panel(this, background_image, wxBITMAP_TYPE_ANY, wxDefaultPosition, panel_size);
        background_panel->Lower(); // background at bottom-most layer

        if (game_state->get_current_player() == me && game_state->get_opening_rules()->get_ruleset() == ruleset_type::uninitialized) {

            wxStaticText* game_rule_dropdown_text = build_static_text(
                    "Please choose a game_window style:",
                    wxDefaultPosition,
                    wxSize(200, 18),
                    wxALIGN_CENTER,
                    true
            );
            game_rule_dropdown_text->SetForegroundColour(white);
            inner_layout->Add(game_rule_dropdown_text, 0, wxALIGN_CENTER);

            wxArrayString game_rule_choices;
            game_rule_choices.Add("Freestyle");
            game_rule_choices.Add("Swap after first move");
            game_rule_choices.Add("Swap2");

            wxComboBox* game_rule_dropdown = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, game_rule_choices, wxCB_DROPDOWN | wxCB_READONLY);
            inner_layout->Add(game_rule_dropdown, 0, wxALIGN_CENTER, 10);
            game_rule_dropdown->SetSelection(0);

            // add a spacer for the gap
            inner_layout->AddSpacer(10);

            // add a button for confirming the ruleset choice
            std::string err;
            wxButton* choose_rules_button = new wxButton(this, wxID_ANY, "Confirm choice", wxDefaultPosition, wxSize(150, 40));
            choose_rules_button->Bind(wxEVT_BUTTON, [game_rule_dropdown, &err](wxCommandEvent& event) {
                wxSound button_click_sound("assets/music/click-button.wav");
                button_click_sound.Play(wxSOUND_ASYNC);
                game_controller::set_game_rules(_pretty_string_to_ruleset_string.at(std::string(game_rule_dropdown->GetValue())), err);
            });
            inner_layout->Add(choose_rules_button, 0, wxALIGN_CENTER, 10);

            // create a buffer before the start game button
            inner_layout->AddSpacer(90);
        }

    }


    // show chosen ruleset type if game has not yet started but ruleset has been chosen
    if (!game_state->is_started() && game_state->get_opening_rules()->get_ruleset() != ruleset_type::uninitialized) {
        std::string chosen_game_mode_string = std::string("Chosen game style: ");
        std::string ruleset_string = _ruleset_string_to_pretty_string.at(opening_rules::_ruleset_type_to_string.at(game_state->get_opening_rules()->get_ruleset()));
        chosen_game_mode_string.append(ruleset_string);
        wxStaticText* game_rule_chosen_text = build_static_text(
                chosen_game_mode_string,
                wxDefaultPosition,
                wxSize(400, 18),
                wxALIGN_CENTER,
                true
        );
        game_rule_chosen_text->SetForegroundColour(white);
        inner_layout->Add(game_rule_chosen_text, 0, wxALIGN_CENTER);
        inner_layout->AddSpacer(170);
    }

    // Show the label with our player name
    wxStaticText* player_name = build_static_text(
            me->get_player_name(),
            wxDefaultPosition,
            wxSize(200, 18),
            wxALIGN_CENTER,
            true
    );
    player_name->SetForegroundColour(white);
    inner_layout->Add(player_name, 0, wxALIGN_CENTER);

    // if the game has not yet started we say so
    if (!game_state->is_started()) {

        wxStaticText* player_points = build_static_text(
                "waiting for game_window to start...",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER
        );
        player_points->SetForegroundColour(white);
        inner_layout->Add(player_points, 0, wxALIGN_CENTER, 8);
        inner_layout->AddSpacer(10);

        // show button that allows our player to start the game
        wxButton* start_game_button = new wxButton(this, wxID_ANY, "Start Game!", wxDefaultPosition, wxSize(160, 64));
        start_game_button->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
            wxSound button_click_sound("assets/music/click-button.wav");
            button_click_sound.Play(wxSOUND_ASYNC);
            game_controller::start_game();
        });
        inner_layout->Add(start_game_button, 0, wxALIGN_CENTER, 8);
        inner_layout->AddSpacer(65);

    } else {
        // show our player's points
        wxStaticText *player_points = build_static_text(
                std::to_string(me->get_score()) + " points",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER
        );
        player_points->SetForegroundColour(white);
        inner_layout->Add(player_points, 0, wxALIGN_CENTER, 8);

        // show our player's colour next to their name and points
        std::string my_stone_colour_image = "assets/stone_" + player::_player_colour_type_to_string.at(me->get_colour()) + ".png";
        wxPoint my_colour_stone_position = wxDefaultPosition;
        image_panel* my_colour_stone = new image_panel(this, my_stone_colour_image, wxBITMAP_TYPE_ANY, my_colour_stone_position, main_game_panel::stone_size);

        inner_layout->Add(my_colour_stone, 0, wxALIGN_CENTER, 10);

        // add drop shadow to stone
        std::string stone_shadow_image = "assets/stone_shadow.png";
        wxPoint player_stone_shadow_position = my_colour_stone_position + stone_size / 17;
        image_panel *turn_indicator_stone_shadow_panel = new image_panel(this, stone_shadow_image, wxBITMAP_TYPE_ANY,
                                                                         player_stone_shadow_position, main_game_panel::stone_size);

        inner_layout->AddSpacer(5);

        /* might be re-usable if we want to have a "give up" button
         *
        // if we haven't folded yet, and it's our turn, display Fold button
        } else if (game_state->get_current_player() == me) {
            wxButton *fold_button = new wxButton(this, wxID_ANY, "Fold", wxDefaultPosition, wxSize(80, 32));
            fold_button->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
                game_controller::fold();
            });
            inner_layout->Add(foldButton, 0, wxALIGN_CENTER | wxBOTTOM, 8);
        */

        // if it's not our turn, display "waiting..."
        if (game_state->get_current_player() != me) {
            wxStaticText *player_status = build_static_text(
                    "waiting...",
                    wxDefaultPosition,
                    wxSize(200, 32),
                    wxALIGN_CENTER
            );
            player_status->SetForegroundColour(white);
            inner_layout->Add(player_status, 0, wxALIGN_CENTER, 8);
        }

        else {
            wxStaticText *player_status = build_static_text(
                    "",
                    wxDefaultPosition,
                    wxSize(200, 32),
                    wxALIGN_CENTER
            );
            player_status->SetForegroundColour(white);
            inner_layout->Add(player_status, 0, wxALIGN_CENTER, 8);
        }


    }
}

// build icons for about, settings and help
void main_game_panel::build_icons(game_state* gameState, player *me, std::string path, wxPoint position) {
    wxBitmap about_button_bitmap(path, wxBITMAP_TYPE_PNG);
    wxBitmapButton* about_button = new wxBitmapButton(this, wxID_ANY, about_button_bitmap, wxDefaultPosition, wxSize(70, 70), wxBORDER_NONE);
    about_button->SetPosition(position); // Set position to top-left corner with margin
    about_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
        wxSound button_click_sound("assets/music/click-button.wav");
        button_click_sound.Play(wxSOUND_ASYNC);
        this->build_about_text(event);
    });
}

void main_game_panel::build_about_text(wxCommandEvent& event) {
    wxString about_info = wxT("Gomoku Game\n\nAuthors: Haoanqin Gao, Julius König, Stephen Lincon, \n                Nicolas Müller, Rana Singh, You Wu \nVersion: 1.0.0\n\n© 2023 Wizards of the C Inc.");
    wxMessageBox(about_info, wxT("About Gomoku"), wxOK | wxICON_INFORMATION, this);
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