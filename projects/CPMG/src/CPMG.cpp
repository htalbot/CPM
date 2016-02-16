#include "CPMGFrame.h"
#include "CPMCore/CPMAppLayer.h"
#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMEventRaiser.h"
#include "CPMG/CPMG.evc.h"
#include "ace/Init_ACE.h"

#ifdef ACE_WIN32
#include <shellapi.h>
#endif

class CPMGApp: 
    public wxApp,
    public CPMAppLayer_ns::CPMAppLayerBase
{
public:
    CPMGApp()
    {
        CPMAPPLAYER->set(this);

        tao_layer_ = new CPMTAOLayer;
    }

    ~CPMGApp()
    {
        delete tao_layer_;
    }

    bool OnInit();
    int FilterEvent(wxEvent& event);

    virtual bool objects_register() { return true; }
    virtual bool objects_init() { return true; }
    virtual void terminate_() {}

    virtual void show(bool show)
    {
        frame_->Show(show);
    }

protected:
    CPMGFrame * frame_;
};


bool CPMGApp::OnInit()
{
    // use standard command line handling:
    if ( !wxApp::OnInit() )
        return false;

    if (!CPMUtils_ns::CPMUtils::boot_cpmd())
    {
        wxMessageBox("Failed to boot CPMD.");
        return false;
    }

    try
    {
        if (!tao_layer_->init(this->argc, this->argv))
        {
            return false;
        }

        tao_layer(tao_layer_);

        tao_layer_->set_request_timeout_orb(200); 

        wxBusyCursor wait_cursor;

        INIT_RAISER_RETURN(false);

        // Create the main frame window
        frame_ = new CPMGFrame(0, -1, wxT("CPMM"));
        if (!frame_->init())
        {
            return false;
        }
    }
    catch(...)
    {
        wxMessageBox("Failed to initialize CPMG");
        return false;
    }

    frame_->Show(true); 

    return true;
}

int CPMGApp::FilterEvent(wxEvent& event)
{
    if ((event.GetEventType() == wxEVT_KEY_DOWN) &&
        (((wxKeyEvent&)event).GetKeyCode() == WXK_F12) &&
        ((wxKeyEvent&)event).ShiftDown() &&
        ((wxKeyEvent&)event).ControlDown())
    {
    }

    return -1;
}

#ifdef ACE_WIN32
int __stdcall WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
    ACE::init();

    LPWSTR *argv;
    int argc;
 
     argv = CommandLineToArgvW(GetCommandLine(), &argc);
     if (argv == NULL)
     {
         return 10;
     }
 
    wxApp::SetInstance( new CPMGApp() );
    wxEntryStart( argc, argv );
    if (wxTheApp->OnInit())
    {
        wxTheApp->OnRun();
    }
    wxTheApp->OnExit();
    wxEntryCleanup();

    ACE::fini();

    return 0;
}
#else
// TO_DO: check if there is problems with RAISE_EVENT...
IMPLEMENT_APP(CPMGApp)
#endif