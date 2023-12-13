#ifndef TEST_INPUTFIELD_H
#define TEST_INPUTFIELD_H

#include <wx/wx.h>


class input_field : public wxPanel {

public:
    input_field(wxWindow* parent, const wxString& label_text, int label_width, const wxString& field_value, int field_width);
    wxString getValue();

private:
    wxStaticText* _label;
    wxTextCtrl* _field;

};


#endif //TEST_INPUTFIELD_H
