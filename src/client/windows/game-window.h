#ifndef GOMOKUUI_GAMEWINDOW_H
#define GOMOKUUI_GAMEWINDOW_H

#include "../panels/connection_panel.h"


class game : public wxFrame
{
public:
    game(const wxString& title, const wxPoint& pos, const wxSize& size);

    void show_panel(wxPanel* panel);
    void set_status(const std::string& message);

private:
    wxBoxSizer* _main_layout;
    wxStatusBar* _status_bar;

    wxPanel* _current_panel;

};


#endif //GOMOKUUI_GAMEWINDOW_H
