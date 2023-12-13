#include "input_field.h"


input_field::input_field(wxWindow* parent, const wxString& label_text, int label_width, const wxString& field_value, int field_width)
    : wxPanel(parent, wxID_ANY) {

    wxBoxSizer* horizontal_layout = new wxBoxSizer(wxHORIZONTAL);

    this->_label = new wxStaticText(
            this, // parent element
            wxID_ANY, // element id
            label_text, // text that's displayed as label
            wxDefaultPosition, // position
            wxSize(label_width, -1) // size (-1 means default size)
     );
    horizontal_layout->Add(this->_label, 0, wxALIGN_CENTER);

    this->_field = new wxTextCtrl(
            this, // parent element
            wxID_ANY, // element id
            field_value, // default value
            wxDefaultPosition, // position
            wxSize(field_width, -1) // size (-1 means default size)
    );
    horizontal_layout->Add(this->_field, 0, wxALIGN_CENTER);

    this->SetSizerAndFit(horizontal_layout);
}


wxString input_field::getValue() {
    return this->_field->GetValue();
}