#include "ConnectionPanel.h"


#include "../uiElements/ImagePanel.h"
#include "../../common/network/default.conf"
#include "../GameController.h"


ConnectionPanel::ConnectionPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    // play bgm
    wxSound connectSound("assets/music/chinese-journey.wav");
    connectSound.Play(wxSOUND_ASYNC | wxSOUND_LOOP);

    // wxColor white = wxColor(255, 255, 255);
    // this->SetBackgroundColour(white);
    wxBoxSizer* verticalLayout = new wxBoxSizer(wxVERTICAL);
    ImagePanel* logo = new ImagePanel(this, "assets/gomoku_logo.png", wxBITMAP_TYPE_ANY, wxDefaultPosition, wxSize(400, 400));
    verticalLayout->Add(logo, 0, wxALIGN_CENTER, 10);


    this->_serverAddressField = new InputField(
        this, // parent element
        "Server address:", // label
        100, // width of label
        default_server_host, // default value (variable from "default.conf")
        240 // width of field
    );
    verticalLayout->Add(this->_serverAddressField, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    this->_serverPortField = new InputField(
        this, // parent element
        "Server port:", // label
        100, // width of label
        wxString::Format("%i", default_port), // default value (variable from "default.conf")
        240 // width of field
    );
    verticalLayout->Add(this->_serverPortField, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    this->_playerNameField = new InputField(
        this, // parent element
        "Player name:", // label
        100, // width of label
        "", // default value
        240 // width of field
    );
    verticalLayout->Add(this->_playerNameField, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    wxButton* connectButton = new wxButton(this, wxID_ANY, "Connect", wxDefaultPosition, wxSize(100, 40));
    connectButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
        GameController::connectToServer();
    });
    verticalLayout->Add(connectButton, 0, wxALIGN_RIGHT | wxALL, 10);

    this->SetSizerAndFit(verticalLayout);
}


wxString ConnectionPanel::getServerAddress() {
    return this->_serverAddressField->getValue();
}


wxString ConnectionPanel::getServerPort() {
    return this->_serverPortField->getValue();
}


wxString ConnectionPanel::getPlayerName() {
    return this->_playerNameField->getValue();
}
