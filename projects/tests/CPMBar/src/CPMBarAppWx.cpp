#ifdef CPMPROCESS_WX

#include "CPMBarAppWx.h"

#include "CPMFrameWx.h"

CPMBarAppWx::CPMBarAppWx(CPMBarAppLayer * app_layer, bool show)
: app_layer_(app_layer)
, show_(show)
{
}

bool CPMBarAppWx::OnInit()
{
    // Create the main frame window
    frame_ = new CPMBarFrame(0, -1, wxT("CPMBar"));
    frame_->set_app_layer(app_layer_);
    cpm_frame_ = new CPMFrameWx(frame_);
    app_layer_->set_frame(cpm_frame_);
    frame_->Show(show_);

    return true;
}

void CPMBarAppWx::terminated()
{
    cpm_frame_->terminated();
}

bool CPMBarAppWx::exited_by_corba()
{
    return cpm_frame_->exited_by_corba();
}

//int CPMBarAppWx::FilterEvent(wxEvent& event)
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
