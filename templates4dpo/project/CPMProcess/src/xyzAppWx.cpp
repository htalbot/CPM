#ifdef CPMPROCESS_WX

#include "xyzAppWx.h"

#include "CPMFrameWx.h"

XyzAppWx::XyzAppWx(XyzAppLayer * app_layer, bool show)
: app_layer_(app_layer)
, show_(show)
{
}

bool XyzAppWx::OnInit()
{
    // Create the main frame window
    frame_ = new XyzFrame(0, -1, wxT("Xyz"));
    frame_->set_app_layer(app_layer_);
    cpm_frame_ = new CPMFrameWx(frame_);
    app_layer_->set_frame(cpm_frame_);
    frame_->Show(show_);

    return true;
}

void XyzAppWx::terminated()
{
    cpm_frame_->terminated();
}

bool XyzAppWx::exited_by_corba()
{
    return cpm_frame_->exited_by_corba();
}

//int XyzAppWx::FilterEvent(wxEvent& event)
//{
//    if ((event.GetEventType() == wxEVT_KEY_DOWN) &&
//        (((wxKeyEvent&)event).GetKeyCode() == WXK_F12) &&
//        ((wxKeyEvent&)event).ShiftDown() &&
//        ((wxKeyEvent&)event).ControlDown())
//    {
//        // ...
//    }
//
//    return -1;
//}

#endif // CPMPROCESS_WX
