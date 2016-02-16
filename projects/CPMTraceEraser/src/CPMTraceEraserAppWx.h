#ifdef CPMPROCESS_WX

#pragma once;

#include "CPMTraceEraserFrame.h"
#include "CPMCore/CPMAppLayer.h"

#ifdef ACE_WIN32
#include <shellapi.h> // CommandLineToArgvW
#endif

class CPMFrameWx;

class CPMTraceEraserAppWx: public wxApp
{
public:
    CPMTraceEraserAppWx(CPMTraceEraserAppLayer * app_layer, bool show);
    bool OnInit();
    void terminated();

    bool exited_by_corba();

    //int FilterEvent(wxEvent& event);

protected:
    CPMTraceEraserAppLayer * app_layer_;
    CPMTraceEraserFrame * frame_;
    CPMFrameWx * cpm_frame_;
    bool show_;
};


#endif // CPMPROCESS_WX

