#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <wx/wx.h>
#include <wx/sizer.h>

// This class can be used to display an image. It can be scaled with parameter <size> and rotated with <rotation> (in radian)
class image_panel : public wxPanel
{
    wxImage _image;
    wxBitmap _bitmap;

    double _rotation;

    int _width;
    int _height;

public:
    image_panel(wxWindow* parent, wxString file, wxBitmapType format, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize, double rotation = 0.0);

    void paint_event(wxPaintEvent& event);
    void on_size(wxSizeEvent& event);
};

#endif // IMAGEPANEL_H
