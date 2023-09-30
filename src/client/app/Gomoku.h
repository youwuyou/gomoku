#ifndef GOMOKUUI_GOMOKU_H
#define GOMOKUUI_GOMOKU_H

#include <wx/wx.h>
#include "../windows/GameWindow.h"
#include "../GameController.h"


// Main app class
class Gomoku : public wxApp
{
public:
    virtual bool OnInit();
};


#endif //GOMOKUUI_GOMOKU_H
