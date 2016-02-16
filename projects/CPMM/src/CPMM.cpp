
// Include the application layer header.
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/ARGV.h"
#include "ace/Get_Opt.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMCore/CPMUtils.h"
#include "CPMMAppLayer.h"
#include "CPMM/CPMM.evc.h"

// We don't use CPMEventRaiser in CPMApps because CPMEventMgrObj uses
// CPMEventHandlerMgr that is a Corba object. So, if a CPMApp uses RAISE_EVENT
// when the orb is not availbale yet, it can take some time for the CPMApp to
// reach CPMEventHandlerMgr and the event goes to the system log in a non
// standard format afterward. Thus, in every CPMApp we log directly into the 
// system log. This is fatser and produces a more formal event entry.
// We still include CPMEventRaiser.h because we need to free the object that

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
    CPMMAppLayer app_layer;

    bool exception(false);

    bool overwrite_autostart_to_false(false);
    try
    {
        if (argc == 2)
        {
            if (strcmp(argv[1], "overwrite_autostart_to_false") == 0)
            {
                overwrite_autostart_to_false = true;
            }
        }

        std::string node = CPMUtils_ns::CPMUtils::get_local_addr();

        std::string cpmm_port;
        if (!CPMUtils_ns::CPMUtils::get_cpmm_port(cpmm_port))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMMEvents::ev_cpmm_port_env_var_not_defined,
                LM_ERROR);
            throw 0;
        }

        std::string iiop = std::string("iiop://") + node + ":" + cpmm_port;

        //ACE_DEBUG((LM_DEBUG, "iiop = %s\n", iiop.c_str()));

        ACE_ARGV_T<ACE_TCHAR> args;
        args.add(ACE_TEXT("-ORBListenEndPoints"));
        args.add(ACE_TEXT(iiop.c_str()));

#ifdef _DEBUG
        app_layer.cpmo_id("CPMMd");
#else
        app_layer.cpmo_id("CPMM");
#endif
        app_layer.set_overwrite_autostart_to_false(overwrite_autostart_to_false);

        CPMTAOLayer tao_layer;

        if (tao_layer.init(args.argc(), args.argv()))
        {
            app_layer.tao_layer(&tao_layer);

            try
            {
                if (!app_layer.run())
                {
                    throw 0;
                }
            }
            catch(...)
            {
                EVENT_HANDLER_SYSTEMLOG->handle_event(
                    _SRC,
                    CPMMEvents::ev_cpmm_app_layer_exception,
                    LM_ERROR);
                throw 0;;
            }
        }
        else
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMMEvents::ev_cpmm_tao_layer_init_failure,
                LM_ERROR);
            throw 0;
        }

        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMMEvents::ev_server_stopped,
            LM_INFO,
            "CPMM");
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMMEvents::ev_main_exception,
            LM_ERROR,
            "CPMM");

        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMMEvents::ev_server_stopped,
            LM_ERROR,
            "CPMM");

        exception = true;
    }

    if (exception)
    {
        //CPMTRACER->trace("%s - exception!!!", _SRC);
        return -1;
    }

    //CPMTRACER->trace("%s - return.", _SRC);
    return 0;
}
