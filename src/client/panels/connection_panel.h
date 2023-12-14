#ifndef TEST_CONNECTIONPANEL_H
#define TEST_CONNECTIONPANEL_H

#include <wx/wx.h>
#include <wx/mediactrl.h>
#include <wx/sound.h>

#include "../uiElements/input_field.h"


class connection_panel : public wxPanel {

public:
    connection_panel(wxWindow* parent);

    wxString get_server_address();
    wxString get_server_port();
    wxString get_player_name();

private:
    input_field* _server_address_field;
    input_field* _server_port_field;
    input_field* _player_name_field;

    // positions
    wxSize const panel_size = wxSize(960, 760); // same as the main game panel
    wxSize const button_size = wxSize(130, 130/2.56);

    // colors
    wxColor white = wxColor(255, 255, 255);
    wxColor black = wxColor(0, 0, 0);
    wxColor dark_green = wxColor(54, 81, 39);
};


#endif //TEST_CONNECTIONPANEL_H
