#ifdef CPMPROCESS_WX

#pragma once;

#include "CPMBarFrame.h"
#include "CPMCore/CPMAppLayer.h"

#ifdef ACE_WIN32
#include <shellapi.h> // CommandLineToArgvW
#endif

class CPMFrameWx;

class CPMBarAppWx: public wxApp
{
public:
    CPMBarAppWx(CPMBarAppLayer * app_layer, bool show);
    bool OnInit();
    void terminated();

    bool exited_by_corba();

    //int FilterEvent(wxEvent& event);

protected:
    CPMBarAppLayer * app_layer_;
    CPMBarFrame * frame_;
    CPMFrameWx * cpm_frame_;
    bool show_;
};


#endif // CPMPROCESS_WX

