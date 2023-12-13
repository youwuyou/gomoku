#include "image_panel.h"


image_panel::image_panel(wxWindow* parent, wxString file, wxBitmapType format, wxPoint position, wxSize size, double rotation) :
        wxPanel(parent, wxID_ANY, position, size)
{
    if (!wxFileExists(file)) {
        wxMessageBox("Could not find file: " + file, "File error", wxICON_ERROR);
        return;
    }

    if (!this->_image.LoadFile(file, format)) {
        wxMessageBox("Could not load file: " + file, "File error", wxICON_ERROR);
        return;
    }

    this->_rotation = rotation;

    this->_width = -1;
    this->_height = -1;

    this->Bind(wxEVT_PAINT, &image_panel::paint_event, this);
    this->Bind(wxEVT_SIZE, &image_panel::on_size, this);
}


void image_panel::paint_event(wxPaintEvent& event) {
    // this code is called when the system requests this panel to be redrawn.

    if (!this->_image.IsOk()) {
        return;
    }

    wxPaintDC device_context = wxPaintDC(this);

    int new_width;
    int new_height;
    device_context.GetSize(&new_width, &new_height);

    if (new_width != this->_width || new_height != this->_height) {

        wxImage transformed;

        if (this->_rotation == 0.0) {
            transformed = this->_image.Scale(new_width, new_height, wxIMAGE_QUALITY_BILINEAR);

        } else {
            wxPoint center_of_rotation = wxPoint(this->_image.GetWidth() / 2, this->_image.GetHeight() / 2);
            transformed = this->_image.Rotate(this->_rotation, center_of_rotation, true);
            transformed = transformed.Scale(new_width, new_height, wxIMAGE_QUALITY_BILINEAR);
        }
        this->_bitmap = wxBitmap(transformed);
        this->_width = transformed.GetWidth();
        this->_height = transformed.GetHeight();

        device_context.DrawBitmap(this->_bitmap, 0, 0, false);
    } else {
        device_context.DrawBitmap(this->_bitmap, 0, 0, false);
    }
}


void image_panel::on_size(wxSizeEvent& event) {

    // when the user resizes this panel, the image should redraw itself
    Refresh();

    // skip any other effects of this event.
    event.Skip();
}