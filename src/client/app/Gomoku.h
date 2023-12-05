#ifndef GOMOKUUI_GOMOKU_H
#define GOMOKUUI_GOMOKU_H

#include <wx/wx.h>
#include "../windows/game-window.h"
#include "../game_controller.h"


// Main app class
class Gomoku : public wxApp
{
public:
    virtual bool OnInit();
};


#endif //GOMOKUUI_GOMOKU_H
