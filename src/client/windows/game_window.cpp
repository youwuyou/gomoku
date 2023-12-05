#include "game_window.h"


game_window::game_window(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    // Set up layout that will contain and center all content

    this->_main_layout = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* outer_layout = new wxBoxSizer(wxHORIZONTAL);
    outer_layout->Add(this->_main_layout, 1, wxCENTER);
    this->SetSizerAndFit(outer_layout);
    this->_current_panel = nullptr;

    // Set up status bar
    this->_status_bar = this->CreateStatusBar(1);

    // Set background
    wxColor light_grey = wxColor(200, 200, 200);
    this->SetBackgroundColour(light_grey);

    // Set the minimum size of the window. The user won't be able to resize the window to a size smaller than this
    this->SetMinSize(wxSize(1000, 720));

}


void game_window::show_panel(wxPanel* panel) {

    // if we are already showing the panel, we don't need to do anything
    if (this->_current_panel == panel) {
        return;
    }

    // remove previous panel
    if (this->_current_panel != nullptr) {
        this->_main_layout->Detach(this->_current_panel);
        this->_current_panel->Show(false);
        this->_current_panel = nullptr;
    }

    // add new panel
    this->_main_layout->Add(panel, 0, wxALIGN_CENTER | wxALL, 20); // 20 pixel spacing
    panel->Show(true);
    this->_current_panel = panel;

    // update layout
    this->_main_layout->Layout();

    // update window size
    this->Fit();
}


void game_window::set_status(const std::string& message) {
    this->_status_bar->SetStatusText(message, 0);
}

