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

};


#endif //TEST_CONNECTIONPANEL_H
