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

#ifndef XYZFRAME_H
#define XYZFRAME_H

#include "xyzAppLayer.h"
#include "DPOConsole/DPOConsole.h"

#include "ace/os_include/sys/os_types.h"
#include <wx/wx.h>
#include <wx/image.h>
#include "wx/intl.h"

#ifndef APP_CATALOG
#define APP_CATALOG "app"  // replace with the appropriate catalog name
#endif


// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade


class XyzFrame: public wxFrame {
public:
    // begin wxGlade: XyzFrame::ids
    // end wxGlade

    XyzFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE);
    void set_app_layer(XyzAppLayer * app_layer)
    {
        app_layer_ = app_layer;
    }

protected:
    XyzAppLayer * app_layer_;
    DPOConsole_ns::DPOConsole console_;

private:
    // begin wxGlade: XyzFrame::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: XyzFrame::attributes
    wxButton* button_dummy;
    wxCheckBox* checkbox_console;
    // end wxGlade

    DECLARE_EVENT_TABLE();

public:
    virtual void on_button_dummy(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void on_checkbox_console(wxCommandEvent &event); // wxGlade: <event_handler>

protected:
    virtual void on_close(wxCloseEvent& event);

}; // wxGlade: end class


#endif // XYZFRAME_H

#endif // CPMPROCESS_WX
