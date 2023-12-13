#include "connection_panel.h"


#include "../uiElements/image_panel.h"
#include "../../common/network/default.conf"
#include "../game_controller.h"


connection_panel::connection_panel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition) {

    wxBoxSizer* vertical_layout = new wxBoxSizer(wxVERTICAL);
    image_panel* logo = new image_panel(this, "assets/gomoku_logo.png", wxBITMAP_TYPE_ANY, wxDefaultPosition, wxSize(350, 350));
    vertical_layout->Add(logo, 0, wxALIGN_CENTER, 10);

    this->_server_address_field = new input_field(
        this, // parent element
        "Server address:", // label
        100, // width of label
        default_server_host, // default value (variable from "default.conf")
        240 // width of field
    );
    this->_server_address_field->SetForegroundColour(wxColor(0, 0, 0));
    vertical_layout->Add(this->_server_address_field, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    this->_server_port_field = new input_field(
        this, // parent element
        "Server port:", // label
        100, // width of label
        wxString::Format("%i", default_port), // default value (variable from "default.conf")
        240 // width of field
    );
    this->_server_port_field->SetForegroundColour(wxColor(0, 0, 0));
    vertical_layout->Add(this->_server_port_field, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    this->_player_name_field = new input_field(
        this, // parent element
        "Player name:", // label
        100, // width of label
        "", // default value
        240 // width of field
    );
    this->_player_name_field->SetForegroundColour(wxColor(0, 0, 0));
    vertical_layout->Add(this->_player_name_field, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    wxButton* connect_button = new wxButton(this, wxID_ANY, "Connect", wxDefaultPosition, wxSize(100, 40));
    connect_button->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
        wxSound button_click_sound("assets/music/click-button.wav");
        button_click_sound.Play(wxSOUND_ASYNC);
        game_controller::connect_to_server();
    });

    vertical_layout->Add(connect_button, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 10);

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
