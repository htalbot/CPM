#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Process.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMCore/CPMProcessStarter.h"
#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMUtils.evc.h"
#include "CPMCore/CPMSettingsCatch.evc.h"
#include "CPMCore/CPMAppLayer.h"
#include "CPMCore/CPMEventRaiser.h"
#include "EnvVarExpand/EnvVarExpand.h"

#include <list>

bool CPMProcessStarter::start(
    const ::ICPM_module::ProcessSeq & procs,
    int starting_proc_timeout_ms,
    int starting_proc_tries)
{
    // Start procs
    for(CORBA::ULong i = 0; i < procs.length(); i++)
    {
        ICPM_module::ProcessDef proc = procs[i];

        //ACE_DEBUG((LM_DEBUG, "proc = %s (enabled = %s)\n",
        //                    proc.process_id.operator const CORBA::Char *(),
        //                    proc.enabled ? "true" : "false"));

        if (proc.enabled)
        {
            ACE_Guard<ACE_Recursive_Thread_Mutex> guard (procs_to_manage_mutex_);

            std::string proc_id = proc.process_id.operator const CORBA::Char *();

            //ACE_DEBUG((LM_DEBUG, "%s - proc to start: %s\n", _SRC, proc_id.c_str()));

            if (procs_to_manage_.find(proc_id) == procs_to_manage_.end())
            {
                std::string env_var_id = proc.env_var_id.operator const CORBA::Char *();
                std::string module_name = proc.module_name.operator const CORBA::Char *();
                std::string port = proc.port_number;

                std::string new_port_number;
                if (!EnvVarExpand_ns::EnvVarExpand::expand_env_variables(port, new_port_number))
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_expand_env_var_failure,
                        LM_ERROR,
                        port.c_str());
                    return false;
                }
                port = new_port_number;

                //ACE_DEBUG((LM_DEBUG, "%s - launching: %s\n", _SRC, module_name.c_str()));

                char * szPath = getenv(env_var_id.c_str());
                if (!szPath)
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_env_var_not_defined,
                        LM_ERROR,
                        env_var_id.c_str());
                    return false;
                }

                std::string launch_path;
                if (!CPMUtils_ns::CPMUtils::find_process(module_name, szPath, launch_path))
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_process_not_found,
                        LM_ERROR,
                        module_name.c_str(),
                        szPath);
                    return false;
                }

                std::string cmd = launch_path + "/" +
                                    module_name +
                                    " --cpm_process_port " + port +
                                    " --cpmo_name " + proc_id;

                ACE_Process_Options options;
                options.command_line(cmd.c_str());

#               ifdef ACE_WIN32
                    // If it's a CPMProcess with no GUI, we must create a new console.
                    // When a CPMProcess has a GUI, the env. var xyz_<cpmo_name>_port
                    // is defined. So, by checking the existence of the env. var. 
                    // we know if we have to create a new console. In fact, when 
                    // the env. var. doesn't exist, we must create a new console.
                    std::stringstream ss_port_env_var;
                    ss_port_env_var << proc_id << "_port";
                    char * port2 = getenv (ss_port_env_var.str().c_str());
                    if (!port2)
                    {
                        if (std::string(proc.console) != "cpmd")
                        {
                            options.creation_flags(CREATE_NEW_CONSOLE);
                            STARTUPINFO * si = options.startup_info();
                            memset(si, 0, sizeof(si));
                            si->cb = sizeof(STARTUPINFO);
                            si->dwFlags = STARTF_USESHOWWINDOW;
                            si->wShowWindow = SW_HIDE;
                        }
                    }
#               else
                        // TO_DO: how does linux behave?
#               endif

                ACE_Process process;
                pid_t pid = process.spawn(options);
                if (pid == -1)
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_spawning_process_failure,
                        LM_ERROR,
                        cmd.c_str());
                    return false;
                }

                ACE_DEBUG((LM_DEBUG, "%T - %s - launched: %s\n", _SRC, proc_id.c_str()));

                procs_to_manage_[proc_id] = proc;
            }
            else
            {
                //ACE_DEBUG((LM_DEBUG, "%s - proc already launched: %s\n", _SRC, proc_id.c_str()));

                RAISE_EVENT(
                    CPMUtilsEvents::ev_process_already_spawned,
                    LM_INFO,
                    proc_id.c_str());
            }
        }
    }

    std::list<std::string> not_started_procs;

    for (int j = 0; j != starting_proc_tries; j++)
    {
        not_started_procs.clear();

        int count = 0;
        for(CORBA::ULong i = 0; i < procs.length(); i++)
        {
            ICPM_module::ProcessDef proc = procs[i];

            if (proc.enabled)
            {
                std::list<std::string> list;
                CPMUtils_ns::CPMUtils::get_system_processes_list(list);

                std::string process_name = proc.module_name.operator const CORBA::Char *();
                if (std::find(list.begin(), list.end(), process_name + CPMUtils_ns::CPMUtils::proc_extention()) != list.end())
                {
                    ++count;
                }
                else
                {
                    not_started_procs.push_back(proc.module_name.operator const CORBA::Char *());
                }
            }
        }

        if (count == procs.length())
        {
            break;
        }

        ACE_OS::sleep(ACE_Time_Value(0, starting_proc_timeout_ms));
    }

    if (not_started_procs.size() != 0)
    {
        for (auto it = not_started_procs.begin(); it != not_started_procs.end(); it++)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_process_not_started,
                LM_ERROR,
                it->c_str());
        }

        return false;
    }

    return true;
}

bool CPMProcessStarter::stop(const ::ICPM_module::ProcessSeq & procs, int processes_stopping_timeout_ms)
{
    for(CORBA::ULong i = 0; i < procs.length(); i++)
    {
        ICPM_module::ProcessDef proc = procs[i];

        std::string module_name = proc.module_name.operator const CORBA::Char *();

        //ACE_DEBUG((LM_DEBUG, "%T - %s - %s (%s)\n", _SRC, (const char *)proc.module_name, (const char *)proc.process_id));

        if (CPMUtils_ns::CPMUtils::is_process_running(module_name))
        {
            std::string location;
            if (!CPMUtils_ns::CPMUtils::get_object_location(proc, location))
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_getting_object_location_failure,
                    LM_ERROR,
                    proc.process_id.operator const CORBA::Char *());
                continue;
            }

            try
            {
                //ACE_DEBUG((LM_DEBUG, "%T - %s - %s is running, get_ref_from_iortable(%s) (%s)\n", _SRC, (const char *)proc.module_name, location.c_str(), (const char *)proc.process_id));

                CPMTAOLayer::GetRefStatus status;
                ::ICPM_module::ICPMProcess_var cpm_process_obj =
                    CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMProcess>(location.c_str(), status);
                if (cpm_process_obj)
                {
                    cpm_process_obj->ctrl_end();
                    //{
                    //    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (procs_to_manage_mutex_);
                    //    procs_to_manage_.erase(proc.module_name.operator const CORBA::Char *());
                    //}
                }
                else
                {
                    throw 0;
                }
            }
            catch(...)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_stopping_process_exception,
                    LM_ERROR,
                    location.c_str());
            }
        }
    }

    ACE_DEBUG((LM_DEBUG, "%T - %s - Waiting until all processes are stopped (or timer is reached)...\n", _SRC));

    std::list<std::string> running_list;
    ACE_Time_Value begin = ACE_OS::gettimeofday();
    while ((ACE_OS::gettimeofday() - begin).get_msec() < processes_stopping_timeout_ms
        || processes_stopping_timeout_ms == -1)
    {
        running_list.clear();

        for(CORBA::ULong i = 0; i < procs.length(); i++)
        {
            ICPM_module::ProcessDef proc = procs[i];

            std::string module_name = proc.module_name.operator const CORBA::Char *();

            //ACE_DEBUG((LM_DEBUG, "%T - %s - check if %s is running...\n", _SRC, module_name.c_str()));

            if (CPMUtils_ns::CPMUtils::is_process_running(module_name))
            {
                ACE_DEBUG((LM_DEBUG, "%T - %s - %s is still running...\n", _SRC, (const char *)proc.process_id));

                // Is the object alive?
                std::string location;
                if (!CPMUtils_ns::CPMUtils::get_object_location(proc, location))
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_getting_object_location_failure,
                        LM_ERROR,
                        proc.process_id.operator const CORBA::Char *());
                    continue;
                }

                try
                {
                    CPMTAOLayer::GetRefStatus status;
                    ::ICPM_module::ICPMProcess_var cpm_process_obj =
                        CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMProcess>(location.c_str(), status, 1, 0);

                    if (!cpm_process_obj)
                    {
                        ACE_Guard<ACE_Recursive_Thread_Mutex> guard (procs_to_manage_mutex_);
                        procs_to_manage_.erase(proc.process_id.operator const CORBA::Char *());
                    }
                    else
                    {
                        running_list.push_back(module_name);
                    }
                }
                catch(...)
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_stopping_process_exception,
                        LM_ERROR,
                        location.c_str());
                }
            }
            else
            {
                ACE_Guard<ACE_Recursive_Thread_Mutex> guard (procs_to_manage_mutex_);
                //ACE_DEBUG((LM_DEBUG, "%T - %s - NO, %s is not running. Erase it from procs_to_manage_...\n", _SRC, proc.process_id.operator const CORBA::Char *()));
                procs_to_manage_.erase(proc.process_id.operator const CORBA::Char *());
            }
        }

        if (running_list.size() == 0)
        {
            break;
        }

        ACE_OS::sleep(ACE_Time_Value(0, 1));
    }

    if (running_list.size() != 0)
    {
        std::string txt;
        for (auto it = running_list.begin(); it != running_list.end(); it++)
        {
            if (it != running_list.begin())
            {
                txt += ", ";
            }
            txt += *it;
        }

        ACE_DEBUG((LM_DEBUG, "%T - %s - Non stopped processes: %s after %u msec.\n", _SRC, txt.c_str(), processes_stopping_timeout_ms));
        RAISE_EVENT(
            CPMUtilsEvents::ev_non_stopped_processes,
            LM_ERROR,
            txt.c_str());
        return false;
    }
    else
    {
        ACE_DEBUG((LM_DEBUG, "%T - %s - Ok, all processes stopped.\n", _SRC));
    }

    return true;
}

// Stop all processes
bool CPMProcessStarter::stop(int processes_stopping_timeout_ms)
{
    ::ICPM_module::ProcessSeq procs;
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard (procs_to_manage_mutex_);

        procs.length((CORBA::ULong)procs_to_manage_.size());

        int i(0);
        for (auto it = procs_to_manage_.begin(); it != procs_to_manage_.end(); it++)
        {
            //ACE_DEBUG((LM_DEBUG, "proc to stop: %s\n", it->second.module_name.operator const CORBA::Char *()));
            procs[i] = it->second;
            ++i;
        }
    }

    return stop(procs, processes_stopping_timeout_ms);
}

void CPMProcessStarter::get_procs_to_manage(std::list<ICPM_module::ProcessDef> & procs)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (procs_to_manage_mutex_);

    ::CORBA::ULong i(0);
    for (auto it = procs_to_manage_.begin(); it != procs_to_manage_.end(); it++)
    {
        procs.push_back(it->second);
        ++i;
    }
}

void CPMProcessStarter::remove_process(const std::string & proc_id)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (procs_to_manage_mutex_);
    procs_to_manage_.erase(proc_id);
}

