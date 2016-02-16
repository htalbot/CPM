
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/ARGV.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMCore/CPMUtils.h"
#include "CPMDAppLayer.h"
#include "CPMD/CPMD.evc.h"
#include "libConfig/libconfig.h++"
#include "FileShortcut/FileShortcut.h"

using namespace libconfig;

// We don't use CPMEventRaiser in CPMApps because CPMEventMgrObj uses
// CPMEventHandlerMgr that is a Corba object. So, if a CPMApp uses RAISE_EVENT
// when the orb is not availbale yet, it can take some time for the CPMApp to
// reach CPMEventHandlerMgr and the event goes to the system log in a non
// standard format afterward. Thus, in every CPMApp we log directly into the 
// system log. This is fatser and produces a more formal event entry.

bool get_timeout_params(int & connection_ms, int & request_ms);

int ACE_TMAIN (int, ACE_TCHAR *[])
{
    CPMDAppLayer app_layer;

    bool exception(false);

    try
    {
        int connection_timeout_ms;
        int request_timeout_ms;
        if (!get_timeout_params(connection_timeout_ms, request_timeout_ms))
        {
            throw 0;
        }

        std::string node = CPMUtils_ns::CPMUtils::get_local_addr();

        std::string cpmd_port;
        if (!CPMUtils_ns::CPMUtils::get_cpmd_port(cpmd_port))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMDEvents::ev_cpmd_port_env_var_not_defined,
                LM_ERROR);
            throw 0;
        }

        std::string iiop = std::string("iiop://") + node + ":" + cpmd_port;

        //ACE_DEBUG((LM_DEBUG, "CPMD::IIOP = %s\n", iiop.c_str()));

        ACE_ARGV_T<ACE_TCHAR> args;
        args.add(ACE_TEXT("-ORBListenEndPoints"));
        args.add(ACE_TEXT(iiop.c_str()));

        app_layer.cpmo_id("CPMD");

        CPMTAOLayer tao_layer;

        if (tao_layer.init(args.argc(), args.argv()))
        {
            tao_layer.set_request_timeout_orb(request_timeout_ms); // limit time for request to request_timeout_ms msec
            tao_layer.set_connection_timeout_orb(connection_timeout_ms); // limit time for connection to connection_timeout_ms msec

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
                    CPMDEvents::ev_cpmd_app_layer_failure,
                    LM_ERROR);
                throw 0;
            }
        }
        else
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMDEvents::ev_cpmd_tao_layer_init_failure,
                LM_ERROR);
            throw 0;
        }

        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_server_stopped,
            LM_INFO,
            "CPMD");
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_main_exception,
            LM_ERROR,
            "CPMD");

        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_server_stopped,
            LM_ERROR,
            "CPMD");

        exception = true;
    }

    if (exception)
    {
        return -1;
    }

    return 0;
}

bool get_timeout_params(int & connection_ms, int & request_ms)
{
    // Get starting_proc_timeout_ms parameter
    char * pPath = getenv ("CPM_ETC");
    if (!pPath)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_env_var_not_defined,
            LM_ERROR,
            "CPM_ETC");
        return false;
    }

    std::string path(pPath);
    std::string::iterator it;
    for (it = path.begin(); it != path.end(); it++)
    {
        if (*it == '\\')
        {
            *it = '/';
        }
    }

    std::string file = path + "/cpm.cfg";
    file = FileShortcut_ns::FileShortcut::resolve(file);

    Config cfg;

    try
    {
        cfg.readFile(file.c_str());

        // root
        Setting & root = cfg.getRoot();

        // boot
        Setting & cpmd = root["cpmd"];

        // getting_reference_timeout_ms
        if (!cpmd.lookupValue("connection_timeout_ms", connection_ms))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMDEvents::ev_field_not_found,
                LM_ERROR,
                file.c_str(),
                "cpmd/connection_timeout_ms");
            return false;
        }

        // getting_reference_tries
        if (!cpmd.lookupValue("request_timeout_ms", request_ms))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMDEvents::ev_field_not_found,
                LM_ERROR,
                file.c_str(),
                "cpmd/request_timeout_ms");
            return false;
        }
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_exception_with_settings_file,
            LM_ERROR,
            file.c_str());
        return false;
    }

    return true;
}
