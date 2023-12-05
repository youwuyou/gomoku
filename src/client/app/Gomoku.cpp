#include "Gomoku.h"


// Application entry point
bool Gomoku::OnInit()
{
    // Allow loading of JPEG  and PNG image files
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());

    // Open main game window
    game* gameWindow = new game(
            "Gomoku", // title of window,
            wxDefaultPosition, // position of the window
            wxDefaultSize // size of the window
    );
    gameWindow->Show(true);

    // Initialize game controller
    game_controller::init(gameWindow);

    return true;
}