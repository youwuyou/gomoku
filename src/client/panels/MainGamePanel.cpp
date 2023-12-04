#include "MainGamePanel.h"
#include "../uiElements/ImagePanel.h"
#include "../GameController.h"
#include "../../common/exceptions/GomokuException.h"

// for game mode choice
const std::unordered_map<std::string, std::string> MainGamePanel::_pretty_string_to_ruleset_string = {
        {"Freestyle", "freestyle"},
        {"Swap2", "swap2"},
        {"Swap after first move","swap_after_first_move"},
};

// for game mode choice
const std::unordered_map<std::string, std::string> MainGamePanel::_ruleset_string_to_pretty_string = {
        {"freestyle", "Freestyle"},
        {"swap2", "Swap2"},
        {"swap_after_first_move", "Swap after first move"},
};


MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {}

void MainGamePanel::buildGameState(game_state* gameState, player* me) {

    // remove any existing UI
    this->DestroyChildren();
    std::vector<player*> players = gameState->get_players();

    // find our own player object in the list of players
    int myPosition = -1;
    std::vector<player*>::iterator it = std::find_if (players.begin(), players.end(), [me](const player* x) {
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
    this->buildAbout(gameState, me);

    // update layout
    this->Layout();
}

void MainGamePanel::buildPlayingBoard(game_state* gameState, player *me) {

    // only show the board, stones and buttons if the game has started
    if (gameState->is_started()) {

        // show background
        std::string backgroundImage = "assets/background.jpg"; // replace with your image file path
        wxSize panelSize = this->GetSize();
        ImagePanel *backgroundPanel = new ImagePanel(this, backgroundImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, panelSize);
        backgroundPanel->Lower(); // This ensures the background is behind all other elements

        // show board image
        std::string boardImage = "assets/playing_board.png";
        wxPoint playingBoardPosition = MainGamePanel::table_center - MainGamePanel::board_size / 2;
        ImagePanel *playingBoard = new ImagePanel(this, boardImage, wxBITMAP_TYPE_ANY, playingBoardPosition, MainGamePanel::board_size);

        // show stones on the playing board
        std::vector<std::vector<field_type>> playing_board = gameState->get_playing_board();
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
                        throw GomokuException("Invalid stone colour for current_stone in rendering.");
                    }
                    wxPoint current_stone_position = table_center - board_size / 2 + grid_corner_offset +
                                                     i * wxPoint(0, grid_spacing / scale_factor) +
                                                     j * wxPoint(grid_spacing / scale_factor, 0) - stone_size / 2;
                    ImagePanel *current_stone_panel = new ImagePanel(this, current_stone_image, wxBITMAP_TYPE_ANY,
                                                                     current_stone_position, MainGamePanel::stone_size);

                    // add drop shadow to stone
                    std::string stone_shadow_image = "assets/stone_shadow.png";
                    wxPoint current_stone_shadow_position = current_stone_position + stone_size/17;
                    ImagePanel *current_stone_shadow_panel = new ImagePanel(this, stone_shadow_image, wxBITMAP_TYPE_ANY,
                                                                     current_stone_shadow_position, MainGamePanel::stone_size);

                    //current_stone_panel->SetToolTip(current_stone.get_colour() + " stone at (" + std::to_string(i) + ", " + std::to_string(j) + ")");
                } else {
                    // if no stone is present, show a transparent button on each spot, if it is currently our turn
                    if (gameState->get_current_player() == me) {
                        player_colour_type current_player_colour = gameState->get_current_player()->get_colour();
                        std::string transparent_stone_image = "assets/stone_transparent.png";
                        wxPoint current_stone_position = table_center - board_size / 2 + grid_corner_offset +
                                                         i * wxPoint(0, grid_spacing / scale_factor) +
                                                         j * wxPoint(grid_spacing / scale_factor, 0) - stone_size / 2;
                        ImagePanel *new_stone_button = new ImagePanel(this, transparent_stone_image, wxBITMAP_TYPE_ANY,
                                                                      current_stone_position,
                                                                      MainGamePanel::stone_size);
                        new_stone_button->SetCursor(wxCursor(wxCURSOR_HAND));
                        //new_stone_button->SetToolTip("Place stone at (" + std::to_string(i) + ", " + std::to_string(j) + ")");
                        std::string err;

                        field_type new_stone_colour = field_type::empty;

                        if (current_player_colour == player_colour_type::black) {
                            new_stone_colour = field_type::black_stone;
                        } else if (current_player_colour == player_colour_type::white) {
                            new_stone_colour = field_type::white_stone;
                        } else {
                            throw GomokuException("Invalid current player colour in new stone button rendering.");
                        }
                        unsigned int x = j;
                        unsigned int y = i;

                        new_stone_button->Bind(wxEVT_LEFT_UP, [x, y, new_stone_colour, &err](wxMouseEvent &event) {
                        wxSound stonePlaceSound("assets/music/place-stone-sound.wav");
                        stonePlaceSound.Play(wxSOUND_ASYNC);

                        GameController::placeStone(x, y, new_stone_colour, err);
                        });
                    }
                }
            }
        }
    }

}

void MainGamePanel::buildTurnIndicator(game_state *gameState, player *me) {
    if (gameState->is_started() && gameState->get_current_player() != nullptr) {

        std::string turnIndicatorText = "It's " + gameState->get_current_player()->get_player_name() + "'s turn!";
        if (gameState->get_current_player() == me) {
            turnIndicatorText = "It's your turn!";
        }

        wxPoint turnIndicatorPosition = MainGamePanel::table_center - MainGamePanel::board_size/2 + MainGamePanel::turnIndicatorOffset;

        wxStaticText* turn_indicator_text = this->buildStaticText(
                turnIndicatorText,
                turnIndicatorPosition,
                wxSize(200, 18),
                wxALIGN_CENTER,
                true
        );
        turn_indicator_text->SetForegroundColour(wxColour(255,255,255));

        std::string current_player_colour = player::_player_colour_type_to_string.at(gameState->get_current_player()->get_colour());
        std::string currentPlayerStoneImage = "assets/stone_" + current_player_colour + ".png";
        wxPoint turnIndicatorStonePosition = turnIndicatorPosition + MainGamePanel::turnIndicatorStoneOffset;
        ImagePanel* turnIndicatorStone = new ImagePanel(this, currentPlayerStoneImage, wxBITMAP_TYPE_ANY, turnIndicatorStonePosition, MainGamePanel::stone_size);
        turnIndicatorStone->SetToolTip("Colour to play: " + current_player_colour);

        // add drop shadow to stone
        std::string stone_shadow_image = "assets/stone_shadow.png";
        wxPoint turn_indicator_stone_shadow_position = turnIndicatorStonePosition + stone_size/17;
        ImagePanel *turn_indicator_stone_shadow_panel = new ImagePanel(this, stone_shadow_image, wxBITMAP_TYPE_ANY,
                                                                turn_indicator_stone_shadow_position, MainGamePanel::stone_size);
    }
}


void MainGamePanel::buildThisPlayer(game_state* gameState, player* me) {
    // close all open dialogs
    MainGamePanel::close_all_dialogs();

    // Setup two nested box sizers, in order to align our player's UI to the bottom center
    wxBoxSizer* outerLayout = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(outerLayout);
    wxBoxSizer* innerLayout = new wxBoxSizer(wxVERTICAL);
    outerLayout->Add(innerLayout, 1, wxALIGN_BOTTOM);

    // show dropdown for game mode selection for first player if the game has not yet started and the opening ruleset is "uninitialized"
    if(!gameState->is_started()){
        // show background for game mode selection
        std::string backgroundImage = "assets/gameselec_background.png"; // Replace with your image file path
        wxSize panelSize = this->GetSize();
        ImagePanel *backgroundPanel = new ImagePanel(this, backgroundImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, panelSize);
        backgroundPanel->Lower(); // This ensures the background is behind all other elements

        if (gameState->get_current_player() == me && gameState->get_opening_rules()->get_ruleset() == ruleset_type::uninitialized) {

            wxStaticText* game_rule_dropdown_text = buildStaticText(
                    "Please choose a game style:",
                    wxDefaultPosition,
                    wxSize(200, 18),
                    wxALIGN_CENTER,
                    true
            );
            game_rule_dropdown_text->SetForegroundColour(wxColour(255,255,255));
            innerLayout->Add(game_rule_dropdown_text, 0, wxALIGN_CENTER);

            wxArrayString game_rule_choices;
            game_rule_choices.Add("Freestyle");
            game_rule_choices.Add("Swap after first move");
            game_rule_choices.Add("Swap2");

            wxComboBox* game_rule_dropdown = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, game_rule_choices, wxCB_DROPDOWN | wxCB_READONLY);
            innerLayout->Add(game_rule_dropdown, 0, wxALIGN_CENTER, 10);
            game_rule_dropdown->SetSelection(0);

            // add a spacer for the gap
            int gapHeight = 10;
            innerLayout->AddSpacer(gapHeight);

            // add a button for confirming the ruleset choice
            std::string err;
            wxButton* choose_rules_button = new wxButton(this, wxID_ANY, "Confirm choice", wxDefaultPosition, wxSize(150, 40));
            choose_rules_button->Bind(wxEVT_BUTTON, [game_rule_dropdown, &err](wxCommandEvent& event) {
                wxSound buttonClickSound("assets/music/click-button.wav");
                buttonClickSound.Play(wxSOUND_ASYNC);
                GameController::set_game_rules(_pretty_string_to_ruleset_string.at(std::string(game_rule_dropdown->GetValue())), err);
            });
            innerLayout->Add(choose_rules_button, 0, wxALIGN_CENTER, 10);

            // create a buffer before the start game button
            innerLayout->AddSpacer(100);
        }

    }


    // show chosen ruleset type if game has not yet started but ruleset has been chosen
    if (!gameState->is_started() && gameState->get_opening_rules()->get_ruleset() != ruleset_type::uninitialized) {
        std::string chosen_game_mode_string = std::string("Chosen game style: ");
        std::string ruleset_string = _ruleset_string_to_pretty_string.at(opening_rules::_ruleset_type_to_string.at(gameState->get_opening_rules()->get_ruleset()));
        chosen_game_mode_string.append(ruleset_string);
        wxStaticText* game_rule_chosen_text = buildStaticText(
                chosen_game_mode_string,
                wxDefaultPosition,
                wxSize(400, 18),
                wxALIGN_CENTER,
                true
        );
        game_rule_chosen_text->SetForegroundColour(wxColour(255,255,255));
        innerLayout->Add(game_rule_chosen_text, 0, wxALIGN_CENTER);
        // create a buffer before the start game button
        innerLayout->AddSpacer(100);
    }

    // Show the label with our player name
    wxStaticText* playerName = buildStaticText(
            me->get_player_name(),
            wxDefaultPosition,
            wxSize(200, 18),
            wxALIGN_CENTER,
            true
    );
    playerName->SetForegroundColour(wxColour(255,255,255));
    innerLayout->Add(playerName, 0, wxALIGN_CENTER);

    // if the game has not yet started we say so
    if (!gameState->is_started()) {

        wxStaticText* playerPoints = buildStaticText(
                "waiting for game to start...",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER
        );
        playerPoints->SetForegroundColour(wxColour(255,255,255));
        innerLayout->Add(playerPoints, 0, wxALIGN_CENTER, 8);
        innerLayout->AddSpacer(10);

        // show button that allows our player to start the game
        wxButton* startGameButton = new wxButton(this, wxID_ANY, "Start Game!", wxDefaultPosition, wxSize(160, 64));
        startGameButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
            wxSound buttonClickSound("assets/music/click-button.wav");
            buttonClickSound.Play(wxSOUND_ASYNC);
            GameController::startGame();
        });
        innerLayout->Add(startGameButton, 0, wxALIGN_CENTER, 8);
        innerLayout->AddSpacer(10);

    } else {

        // show our player's points
        wxStaticText *playerPoints = buildStaticText(
                std::to_string(me->get_score()) + " points",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER
        );
        playerPoints->SetForegroundColour(wxColour(255,255,255));
        innerLayout->Add(playerPoints, 0, wxALIGN_CENTER, 8);

        // show our player's colour next to their name and points
        std::string my_stone_colour_image = "assets/stone_" + player::_player_colour_type_to_string.at(me->get_colour()) + ".png";
        wxPoint myColourStonePosition = wxDefaultPosition;
        ImagePanel* myColourStone = new ImagePanel(this, my_stone_colour_image, wxBITMAP_TYPE_ANY, myColourStonePosition, MainGamePanel::stone_size);

        innerLayout->Add(myColourStone, 0, wxALIGN_CENTER, 10);

        // add drop shadow to stone
        std::string stone_shadow_image = "assets/stone_shadow.png";
        wxPoint player_stone_shadow_position = myColourStonePosition + stone_size/17;
        ImagePanel *turn_indicator_stone_shadow_panel = new ImagePanel(this, stone_shadow_image, wxBITMAP_TYPE_ANY,
                                                                       player_stone_shadow_position, MainGamePanel::stone_size);

        innerLayout->AddSpacer(10);

        /* might be re-usable if we want to have a "give up" button
         *
        // if we haven't folded yet, and it's our turn, display Fold button
        } else if (gameState->get_current_player() == me) {
            wxButton *foldButton = new wxButton(this, wxID_ANY, "Fold", wxDefaultPosition, wxSize(80, 32));
            foldButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
                GameController::fold();
            });
            innerLayout->Add(foldButton, 0, wxALIGN_CENTER | wxBOTTOM, 8);
        */

        // if it's not our turn, display "waiting..."
        if (gameState->get_current_player() != me) {
            wxStaticText *playerStatus = buildStaticText(
                    "waiting...",
                    wxDefaultPosition,
                    wxSize(200, 32),
                    wxALIGN_CENTER
            );
            playerStatus->SetForegroundColour(wxColour(255,255,255));
            innerLayout->Add(playerStatus, 0, wxALIGN_CENTER, 8);
        }

    }
}

void MainGamePanel::buildAbout(game_state* gameState, player *me) {
    // create the About button with a default position
    wxButton* aboutButton = new wxButton(this, wxID_ANY, wxT("About"), wxDefaultPosition, wxSize(100, 30));

    // manually set the position of the About button to the upper left corner
    int margin = 20;
    wxPoint buttonPosition(margin, margin); // setting the position to the top-left corner with margin
    aboutButton->SetPosition(buttonPosition);

    // bind the event handler with sound
    aboutButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
        wxSound buttonClickSound("assets/music/info-click.wav");
        buttonClickSound.Play(wxSOUND_ASYNC);
        this->buildAboutText(event); // call the original handler
    });
}




void MainGamePanel::buildAboutText(wxCommandEvent& event) {
    wxString aboutInfo = wxT("Gomoku Game\n\nAuthors: Haoanqin Gao, Julius König, Stephen Lincon, \n                Nicolas Müller, Rana Singh, You Wu \nVersion: 1.0.0\n\n© 2023 Wizards of the C Inc.");
    wxMessageBox(aboutInfo, wxT("About Gomoku"), wxOK | wxICON_INFORMATION, this);
}

wxStaticText* MainGamePanel::buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold) {
    wxStaticText* staticText = new wxStaticText(this, wxID_ANY, content, position, size, textAlignment);
    if (bold) {
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

void MainGamePanel::close_all_dialogs(){
    for (wxDialog* dialog : this->_open_dialogs){
        if(dialog){
            dialog->Close();
        }
    }
}