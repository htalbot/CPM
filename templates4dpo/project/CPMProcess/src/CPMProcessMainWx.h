#ifdef CPMPROCESS_WX

#pragma once

#ifdef _WIN64
    #pragma warning(push)
    #pragma warning(disable: 4800)
#endif
#include "ace/Process.h"
#ifdef _WIN64
    #pragma warning(pop)
#endif

#include "xyzAppWx.h"
#include "CPMProcessMain.h"
#include <wx/msgdlg.h>

bool restart = false;

void cpm_messagebox_wx(const std::string & msg)
{
    wxApp * p = new wxApp;
    wxApp::SetInstance( p );
    if (wxTheApp->OnInit())
    {
        wxMessageBox(msg.c_str());

    }
    wxTheApp->OnExit();
    wxEntryCleanup();
}

template<class T>
class CPMProcessBootWx: public CPMProcessBoot<T>, public ACE_Task_Base
{
public:
    CPMProcessBootWx(CPMProcessArgs * args, bool show)
        : CPMProcessBoot<T>(args)
        , args_(args)
        , show_(show)
    {
    }

    ~CPMProcessBootWx()
    {
    }

    virtual void terminate_cpm_process()
    {
        //if (app_layer_.as_gui())
        {
            CPMProcessBoot<T>::terminate_cpm_process();
            wait();
        }
    }

    virtual bool run()
    {
        if (!app_layer_.init_corba_objects())
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                XyzEvents::ev_corba_objects_init_failure,
                LM_ERROR);
            return false;
        }

        this->activate(); // Run orb in background

        // GUI...
        bool exited_by_corba = false;

        XyzAppWx * p = new XyzAppWx(&app_layer_, show_);
        wxApp::SetInstance( p );
        int argc = args_->argc();
        wxEntryStart(argc, args_->argv());
        if (wxTheApp->OnInit())
        {
            wxTheApp->OnRun();
            exited_by_corba = p->exited_by_corba();
            p->terminated();
        }
        wxTheApp->OnExit();
        wxEntryCleanup();

        if (exited_by_corba
            && app_layer_.as_gui())
        {
            wxApp * p = new wxApp;
            wxApp::SetInstance( p );
            if (wxTheApp->OnInit())
            {
                int rc = wxMessageBox("Termination initiated by CPM. Do you want to restart?",
                                        "",
                                        wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT);
                if (rc == wxYES)
                {
                    restart = true;
                }
            }
            wxTheApp->OnExit();
            wxEntryCleanup();
        }

        return true;
    }

    virtual int svc()
    {
        if (!app_layer_.run())
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                XyzEvents::ev_app_layer_failure,
                LM_ERROR);

            EVENT_HANDLER_SYSTEMLOG->handle_event(_SRC,
                XyzEvents::ev_server_stopped,
                LM_INFO,
                "xyz");

            return -1;
        }

        return 0;
    }

protected:
    CPMProcessArgs * args_;
    bool show_;
};


#ifdef ACE_WIN32

    // Useful for debugging (better than a macro)
    int WinMainInternal(HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    {
        int argc;
        LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
        if (argv == NULL)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                XyzEvents::ev_args_parsing_failure,
                LM_ERROR,
                "Xyz");
            return -1;
        }

        ACE_TCHAR ** argv_conv = new ACE_TCHAR *[argc];
        for (int i = 0; i != argc; ++i)
        {
            ACE_TCHAR * s = new ACE_TCHAR[MAX_PATH];
            wcstombs(s, argv[i], MAX_PATH);

            argv_conv[i] = s;
        }

        ACE::init();
        {
            // A server is started with cpm_process_port and cpmo_name arguments
            // while gui is started with cpmo_name argument only. For gui, cpm_process_port
            // is defined as environment variable: Xyz_<cpmo_name>_port. This
            // environement variable is also used by CPMProcessStarter (via CPMD)
            // to determine if new console (Windows) has to be created.
            CPMProcessArgs args(argc, argv_conv);

            std::string cpm_process_port;
            std::string cpmo_name;
            if (args.get_cpm_process_port_and_cpmo_name(cpm_process_port, cpmo_name))
            {
                // Started as server

                // Check if the process is already running
                std::stringstream ss_mutex;
                ss_mutex << szCPM_PROCESS_Mutex << cpmo_name;
                ACE_Process_Mutex mutex(ss_mutex.str().c_str());

                ACE_Time_Value timeout(ACE_OS::gettimeofday().sec() + 1 /*sec*/ );
                if (mutex.acquire(timeout) == -1)
                {
                    // The process is running.

                    // Make sure it is set as server
                    std::string this_node = CPMUtils_ns::CPMUtils::get_local_addr();
                    std::string location = std::string("corbaloc:iiop:") + this_node + ":" + cpm_process_port + "/" + cpmo_name;

                    CPMTAOLayer tao_layer;

                    // Initialize tao_layer
                    if (tao_layer.init(args.argc(), args.argv()))
                    {
                        CPMTAOLayer::GetRefStatus status;
                        ::ICPM_module::ICPMProcess_var cpm_process_obj =
                            tao_layer.get_ref_from_iortable<::ICPM_module::ICPMProcess>(location.c_str(), status);
                        if (cpm_process_obj)
                        {
                            cpm_process_obj->as_server(1);
                        }
                    }
                }
                else
                {
                    // The process is not running, keep booting.
                    CPMProcessBootWx<XyzAppLayer> cpm_process_boot(&args, false /*show*/);
                    if (cpm_process_boot.init_cpm_process())
                    {
                        cpm_process_boot.as_server(true);
                        cpm_process_boot.run();
                    }

                    cpm_process_boot.terminate_cpm_process();
                }
            }
            else
            {
                std::string cpmo_name;
                if (args.get_cpmo_name(cpmo_name))
                {
                    // This process has been launched with number of arguments not corresponding to a server.
                    // Maybe it is desired to launch it as a GUI.
                    // Two things can be done here:
                    //      1- if the CPMProcess is running, ask it to show GUI
                    //      2- if the CPMProcess is not running, keep running and show GUI

                    std::stringstream ss_port_env_var;
                    ss_port_env_var << cpmo_name << "_PORT";
                    std::transform(cpmo_name.begin(), cpmo_name.end(), cpmo_name.begin(), ::toupper);
                    char * port = getenv (ss_port_env_var.str().c_str());
                    if (!port)
                    {
                        std::stringstream ss_pid;
                        ss_pid << cpmo_name;

                        std::stringstream ss_msg;
                        ss_msg << "Process not started as server and "
                                << ss_port_env_var.str()
                                << " is not defined as environment variable.\n"
                                << ss_pid.str()
                                << " can't be started as GUI.";
                        cpm_messagebox_wx(ss_msg.str());
                    }
                    else
                    {
                        // Create the named mutex and try to acquire it.
                        std::stringstream ss_mutex;
                        ss_mutex << szCPM_PROCESS_Mutex << cpmo_name;
                        ACE_Process_Mutex mutex(ss_mutex.str().c_str());

                        ACE_Time_Value timeout(ACE_OS::gettimeofday().sec() + 1 /*sec*/ );
                        if (mutex.acquire(timeout) == -1)
                        {
                            // An instance of Xyz process is running.\n"));
                            // Try to get a ref on its CPMObj

                            std::string this_node = CPMUtils_ns::CPMUtils::get_local_addr();
                            std::string location = std::string("corbaloc:iiop:") + this_node + ":" + port + "/" + cpmo_name;

                            CPMTAOLayer tao_layer;

                            // Initialize tao_layer
                            if (tao_layer.init(args.argc(), args.argv()))
                            {
                                CPMTAOLayer::GetRefStatus status;
                                ::ICPM_module::ICPMProcess_var cpm_process_obj =
                                    tao_layer.get_ref_from_iortable<::ICPM_module::ICPMProcess>(location.c_str(), status);
                                if (cpm_process_obj)
                                {
                                    cpm_process_obj->show_gui(1);
                                }
                                else
                                {
                                    wxString msg = wxString::Format("Failed to get reference on object %s\n", location.c_str());
                                    cpm_messagebox_wx(msg.ToStdString());
                                }
                            }
                        }
                        else
                        {
                            // 2- if the CPMProcess is not running, keep running and show GUI
                            ACE_DEBUG((LM_DEBUG, "Process not running"));

                            ACE_ARGV_T<ACE_TCHAR> args;
                            args.add(ACE_TEXT(argv_conv[0]));
                            args.add(ACE_TEXT("--cpm_process_port"));
                            args.add(ACE_TEXT(port));
                            args.add(ACE_TEXT("--cpmo_name"));
                            args.add(ACE_TEXT(cpmo_name.c_str()));

                            CPMProcessArgs cpm_process_args(args.argc(), args.argv());

                            CPMProcessBootWx<XyzAppLayer> cpm_process_boot(&cpm_process_args, true /*show*/);
                            if (cpm_process_boot.init_cpm_process())
                            {
                                cpm_process_boot.as_gui(true);
                                cpm_process_boot.run();
                            }

                            cpm_process_boot.terminate_cpm_process();
                        }
                    }
                }
                else
                {
                    cpm_messagebox_wx("As GUI application, xyz must be started with 'cpmo_name' argument.\n\n     Syntax: xyz --cpmo_name <discriminant>");
                }
            }
        }
        ACE::fini();

        if (restart)
        {
            CPMProcessArgs args(argc, argv_conv);

            std::string cpmo_name;
            if (args.get_cpmo_name(cpmo_name))
            {
                std::stringstream ss;
                ss << args.argv()[0]
                    << " --cpmo_name " << cpmo_name;

                ACE_Process_Options options;
                options.command_line(ss.str().c_str());

                ACE_Process process;
                pid_t pid = process.spawn(options);
                if (pid == -1)
                {
                    cpm_messagebox_wx("Failed to spawn process xyz");
                }
            }
            else
            {
                cpm_messagebox_wx("Restart of xyz failed due to bad args");
            }
        }

        delete [] argv_conv;

        return 0;
    }

    #define CPMProcessMainWx\
        int __stdcall WinMain(HINSTANCE hInstance,\
        HINSTANCE hPrevInstance,\
        LPSTR lpCmdLine,\
            int nCmdShow)\
        {\
            return WinMainInternal(hPrevInstance, lpCmdLine,nCmdShow);\
        }

#else
// TO_DO: linux
#endif // ACE_WIN32


#endif // CPMPROCESS_WX
