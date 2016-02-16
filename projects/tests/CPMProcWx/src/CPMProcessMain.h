#pragma once

#include "ace/Process_Mutex.h"

char szCPM_PROCESS_Mutex[] = "CPM_PROCESS_Mutex_";

class CPMProcessArgs: public ACE_ARGV_T<ACE_TCHAR>
{
public:
    CPMProcessArgs(int argc, ACE_TCHAR * argv[])
    : ACE_ARGV_T<ACE_TCHAR>(argc, argv)
    {
    }

    virtual bool get_cpm_process_port_and_cpmo_name(std::string & port, std::string & name)
    {
        ACE_Get_Opt cmd_opts(argc(), argv());

        if (cmd_opts.long_option (ACE_TEXT ("cpm_process_port"),
                                    0,
                                    ACE_Get_Opt::ARG_REQUIRED) == -1)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_cant_add_option,
                LM_ERROR,
                "cpm_process_port");
            return false;
        }

        if (cmd_opts.long_option (ACE_TEXT ("cpmo_name"), 1, ACE_Get_Opt::ARG_REQUIRED))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_cant_add_option,
                LM_ERROR,
                "cpmo_name");
            return false;
        }

        // Get process port and cpmo name
        const long max_port_len(16);
        ACE_TCHAR cpm_process_port[max_port_len];

        const long max_name_len(64);
        ACE_TCHAR cpmo_name[max_name_len];

        int option;
        while ((option = cmd_opts ()) != EOF)
        {
            switch (option)
            {
                case 0:
                    ACE_OS::strncpy (cpm_process_port,
                                        cmd_opts.opt_arg (),
                                        max_port_len);
                    port = cpm_process_port;
                    break;

                case 1:
                    ACE_OS::strncpy (cpmo_name,
                                        cmd_opts.opt_arg (),
                                        max_name_len);
                    name = cpmo_name;
                    break;

                default:
                    EVENT_HANDLER_SYSTEMLOG->handle_event(_SRC,
                                CPMProcWxEvents::ev_arg_parsing_failure,
                                LM_ERROR,
                                option);
                    return false;
            }
        }

        if (port.empty() || name.empty())
        {
            return false;
        }

        return true;
    }

    virtual bool get_cpmo_name(std::string & name)
    {
        ACE_Get_Opt cmd_opts(argc(), argv());

        if (cmd_opts.long_option (ACE_TEXT ("cpm_process_port"),
                                    0,
                                    ACE_Get_Opt::ARG_OPTIONAL) == -1)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_cant_add_option,
                LM_ERROR,
                "cpm_process_port");
            return false;
        }

        if (cmd_opts.long_option (ACE_TEXT ("cpmo_name"),
                                    1,
                                    ACE_Get_Opt::ARG_REQUIRED))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_cant_add_option,
                LM_ERROR,
                "cpmo_name");
            return false;
        }

        // Get cpmo name
        const long max_name_len(64);
        ACE_TCHAR cpmo_name[max_name_len];

        int option;
        while ((option = cmd_opts ()) != EOF)
        {
            switch (option)
            {
                case 0:
                    break;

                case 1:
                    ACE_OS::strncpy (cpmo_name,
                                        cmd_opts.opt_arg (),
                                        max_name_len);
                    name = cpmo_name;
                    break;

                default:
                    EVENT_HANDLER_SYSTEMLOG->handle_event(_SRC,
                                CPMProcWxEvents::ev_arg_parsing_failure,
                                LM_ERROR,
                                option);
                    return false;
            }
        }

        if (name.empty())
        {
            return false;
        }

        return true;
    }
};


template<class APPLAYER>
class CPMProcessBoot
{
public:
    CPMProcessBoot(CPMProcessArgs * args)
    : args_(args)
    , tao_layer_initialized_(false)
    {
    }

    virtual ~CPMProcessBoot()
    {
    }

    virtual bool init_cpm_process()
    {
        std::string cpm_process_port;
        std::string cpmo_name;
        if (!args_->get_cpm_process_port_and_cpmo_name(cpm_process_port, cpmo_name))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_bad_parameters,
                LM_ERROR,
                "cpm_process_port/cpmo_name",
                "CPMProcWx");

            return false;
        }

        // TAO options
        std::string node = CPMUtils_ns::CPMUtils::get_local_addr();

        std::string iiop = std::string("iiop://") + node + ":" + cpm_process_port;

        ACE_ARGV_T<ACE_TCHAR> args;
        args.add(ACE_TEXT("-ORBListenEndPoints"));
        args.add(ACE_TEXT(iiop.c_str()));

        // Initialize app layer cpmo_id
        app_layer_.cpmo_id(cpmo_name);

        // Initialize ORB
        if (tao_layer_.init(args.argc(), args.argv()))
        {
            tao_layer_initialized_ = true;
            app_layer_.tao_layer(&tao_layer_);
        }
        else
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_tao_layer_init_failure,
                LM_ERROR);
            return false;
        }

        return true;
    }

    virtual void as_server(bool flag)
    {
        app_layer_.as_server(flag);
    }

    virtual bool is_as_server()
    {
        return app_layer_.as_server();
    }

    virtual void as_gui(bool flag)
    {
        app_layer_.as_gui(flag);
    }

    virtual bool is_as_gui()
    {
        return app_layer_.as_gui();
    }

    virtual void terminate_cpm_process()
    {
        if (tao_layer_initialized_)
        {
            app_layer_.ctrl_end();
        }
    }

    virtual bool run()
    {
        if (!app_layer_.init_corba_objects())
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_corba_objects_init_failure,
                LM_ERROR);
            return false;
        }

        if (!app_layer_.run())
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_app_layer_failure,
                LM_ERROR);

            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_server_stopped,
                LM_ERROR,
                "CPMProcWx");

            return false;
        }

        return true;
    }

protected:
    CPMProcessArgs * args_;
    APPLAYER app_layer_;
    CPMTAOLayer tao_layer_;
    bool tao_layer_initialized_;
};


// Useful for debugging (better than a macro)
int ACE_TMAIN_internal(int argc, ACE_TCHAR ** argv)
{
    CPMProcessArgs args(argc, argv);

    std::string cpm_process_port;
    std::string cpmo_name;
    if (args.get_cpm_process_port_and_cpmo_name(cpm_process_port, cpmo_name))
    {
        std::stringstream ss;
        ss << szCPM_PROCESS_Mutex << cpmo_name;

        ACE_Process_Mutex mutex(ss.str().c_str());
        int rc = 0;
        ACE_Time_Value timeout(ACE_OS::gettimeofday().sec() + 10 /*sec*/ );
        if (mutex.acquire(timeout) == -1)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMProcWxEvents::ev_server_already_running,
                LM_ERROR,
                "CPMProcWx");
            return -1;
        }
        else
        {
            CPMProcessBoot<CPMProcWxAppLayer> cpm_process_boot(&args);
            if (cpm_process_boot.init_cpm_process())
            {
                cpm_process_boot.as_server(true);

                if (!cpm_process_boot.run())
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        }
    }

    return 0;
}

#define CPMProcessMain\
    int ACE_TMAIN (int argc, ACE_TCHAR * argv[])\
    {\
        return ACE_TMAIN_internal(argc, argv);\
    }

