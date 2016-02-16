#ifdef CPMPROCESS_WX

#pragma once;

#include "xyzFrame.h"
#include "CPMCore/CPMAppLayer.h"

#ifdef ACE_WIN32
#include <shellapi.h> // CommandLineToArgvW
#endif

class CPMFrameWx;

class XyzAppWx: public wxApp
{
public:
    XyzAppWx(XyzAppLayer * app_layer, bool show);
    bool OnInit();
    void terminated();

    bool exited_by_corba();

    //int FilterEvent(wxEvent& event);

protected:
    XyzAppLayer * app_layer_;
    XyzFrame * frame_;
    CPMFrameWx * cpm_frame_;
    bool show_;
};


#endif // CPMPROCESS_WX

