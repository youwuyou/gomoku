#include "InputField.h"


InputField::InputField(wxWindow* parent, const wxString& labelText, int labelWidth, const wxString& fieldValue, int fieldWidth)
    : wxPanel(parent, wxID_ANY) {

    wxBoxSizer* horizontalLayout = new wxBoxSizer(wxHORIZONTAL);

    this->_label = new wxStaticText(
            this, // parent element
            wxID_ANY, // element id
            labelText, // text that's displayed as label
            wxDefaultPosition, // position
            wxSize(labelWidth, -1) // size (-1 means default size)
     );
    horizontalLayout->Add(this->_label, 0, wxALIGN_CENTER);

    this->_field = new wxTextCtrl(
            this, // parent element
            wxID_ANY, // element id
            fieldValue, // default value
            wxDefaultPosition, // position
            wxSize(fieldWidth, -1) // size (-1 means default size)
    );
    horizontalLayout->Add(this->_field, 0, wxALIGN_CENTER);

    this->SetSizerAndFit(horizontalLayout);
}


wxString InputField::getValue() {
    return this->_field->GetValue();
}