#include "connection_panel.h"


#include "../uiElements/image_panel.h"
#include "../../common/network/default.conf"
#include "../game_controller.h"


connection_panel::connection_panel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition) {

    wxBoxSizer* vertical_layout = new wxBoxSizer(wxVERTICAL);
    image_panel* background = new image_panel(this, "assets/background_connection_withlogo.png", wxBITMAP_TYPE_ANY, wxDefaultPosition, panel_size);
    vertical_layout->Add(background, 0, wxALIGN_CENTER, 10);

    // Server
    this->_server_address_field = new input_field(
        this, // parent element
        wxPoint(280, 420), // position
        100, // width of label
        default_server_host, // default value (variable from "default.conf")
        240 // width of field
    );

    // Server port
    this->_server_port_field = new input_field(
        this, // parent element
        wxPoint(280, 470), // position
        100, // width of label
        wxString::Format("%i", default_port), // default value (variable from "default.conf")
        240 // width of field
    );

    // Player name
    this->_player_name_field = new input_field(
        this, // parent element
        wxPoint(280, 520), // position
        100, // width of label
        "", // default value
        240 // width of field
    );

    image_panel* connect_button = new image_panel(this, "assets/buttons/button_connect.png", wxBITMAP_TYPE_ANY,
                                                        wxPoint(430, 600),
                                                        button_size);
    connect_button->SetCursor(wxCursor(wxCURSOR_HAND));
    connect_button->Bind(wxEVT_LEFT_UP, [](wxMouseEvent &event) {
        wxSound button_click_sound("assets/music/click-button.wav");
        button_click_sound.Play(wxSOUND_ASYNC);
        game_controller::connect_to_server();
    });


    this->SetSizerAndFit(vertical_layout);
}


wxString connection_panel::get_server_address() {
    return this->_server_address_field->getValue();
}


wxString connection_panel::get_server_port() {
    return this->_server_port_field->getValue();
}


wxString connection_panel::get_player_name() {
    return this->_player_name_field->getValue();
}
