#ifdef CPMPROCESS_WX

// -*- C++ -*-
//
// generated by wxGlade 0.6.8 on Fri Apr 04 07:09:58 2014
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include "ace/os_include/sys/os_types.h"
#include <wx/wx.h>
#include "CPMBarFrame.h"
#include "CPMBarAppLayer.h"

// begin wxGlade: ::extracode
// end wxGlade


CPMBarFrame::CPMBarFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
, console_("CPMBar", 120, 40)
{
    // begin wxGlade: CPMBarFrame::CPMBarFrame
    button_dummy = new wxButton(this, wxID_ANY, _("Dummy..."));
    checkbox_console = new wxCheckBox(this, wxID_ANY, _("console"));

    set_properties();
    do_layout();
    // end wxGlade
}


void CPMBarFrame::set_properties()
{
    // begin wxGlade: CPMBarFrame::set_properties
    SetTitle(_("CPMBar"));
    // end wxGlade
}


void CPMBarFrame::do_layout()
{
    // begin wxGlade: CPMBarFrame::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    sizer_1->Add(button_dummy, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_1->Add(checkbox_console, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    Layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(CPMBarFrame, wxFrame)
    // begin wxGlade: CPMBarFrame::event_table
    EVT_BUTTON(wxID_ANY, CPMBarFrame::on_button_dummy)
    EVT_CHECKBOX(wxID_ANY, CPMBarFrame::on_checkbox_console)
    // end wxGlade

    EVT_CLOSE(CPMBarFrame::on_close)

END_EVENT_TABLE();


void CPMBarFrame::on_button_dummy(wxCommandEvent &event)
{
    wxMessageBox("You hit me!");
    ACE_DEBUG((LM_DEBUG, "You hit me!\n")); // To test console
}

void CPMBarFrame::on_checkbox_console(wxCommandEvent &event)
{
    if (checkbox_console->GetValue())
    {
        console_.show(true);
    }
    else
    {
        console_.show(false);
    }
}

void CPMBarFrame::on_close(wxCloseEvent& event)
{
    if (app_layer_->as_server())
    {
        this->Show(false);
        console_.show(false);
        checkbox_console->SetValue(false);
        app_layer_->as_gui(false);
    }
    else
    {
        this->Destroy();
    }
}


// wxGlade: add CPMBarFrame event handlers

#endif // CPMPROCESS_WX
