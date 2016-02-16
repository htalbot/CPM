#ifdef CPMPROCESS_WX

#pragma once;

#include "CPMProcWxFrame.h"
#include "CPMCore/CPMAppLayer.h"

#ifdef ACE_WIN32
#include <shellapi.h> // CommandLineToArgvW
#endif

class CPMFrameWx;

class CPMProcWxAppWx: public wxApp
{
public:
    CPMProcWxAppWx(CPMProcWxAppLayer * app_layer, bool show);
    bool OnInit();
    void terminated();

    bool exited_by_corba();

    //int FilterEvent(wxEvent& event);

protected:
    CPMProcWxAppLayer * app_layer_;
    CPMProcWxFrame * frame_;
    CPMFrameWx * cpm_frame_;
    bool show_;
};


#endif // CPMPROCESS_WX

