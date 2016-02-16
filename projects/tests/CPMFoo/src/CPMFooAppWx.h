#ifdef CPMPROCESS_WX

#pragma once;

#include "CPMFooFrame.h"
#include "CPMCore/CPMAppLayer.h"

#ifdef ACE_WIN32
#include <shellapi.h> // CommandLineToArgvW
#endif

class CPMFrameWx;

class CPMFooAppWx: public wxApp
{
public:
    CPMFooAppWx(CPMFooAppLayer * app_layer, bool show);
    bool OnInit();
    void terminated();

    bool exited_by_corba();

    //int FilterEvent(wxEvent& event);

protected:
    CPMFooAppLayer * app_layer_;
    CPMFooFrame * frame_;
    CPMFrameWx * cpm_frame_;
    bool show_;
};


#endif // CPMPROCESS_WX

