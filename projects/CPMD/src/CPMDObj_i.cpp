
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Process.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMD/CPMDObj_i.h"
#include "CPMD/version.h"
#include "CPMD/CPMD.evc.h"
#include "CPMCore/CPMAppLayer.h"
#include "CPMCore/CPMSettingsCatch.h"
#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMEventRaiser.h"

#include "libConfig/libconfig.h++"
#include "EnvVarExpand/EnvVarExpand.h"
#include "FileShortcut/FileShortcut.h"

#include <list>
#include <string>
#include <algorithm>

using namespace libconfig;

void CPMDObj_get_version(int & major, int & minor, int & patch)
{
    major = CPMD_MAJOR;
    minor = CPMD_MINOR;
    patch = CPMD_PATCH;
}

CPMDObj_i::CPMDObj_i()
: terminated_(false)
, unreachable_object_timeout_at_start_ms_(3000)
, unreachable_object_timeout_ms_(1000)
{
}

CPMDObj_i::~CPMDObj_i()
{
}

void CPMDObj_i::get_version(
      ::CORBA::Long_out major,
      ::CORBA::Long_out minor,
      ::CORBA::Long_out patch)
{

    CPMDObj_get_version(major, minor, patch);
}

void CPMDObj_i::ctrl_ping()
{
    //ACE_DEBUG((LM_DEBUG, "CPMObjDummy_i::ctrl_ping()\n"));
}

int CPMDObj_i::svc()
{
    ACE_Message_Block *mb = 0;
    ACE_NEW_NORETURN(mb, ACE_Message_Block(0, ACE_Message_Block::MB_EVENT));
    if (mb != 0)
    {
        mb->msg_type(CHECK);
        if (this->msg_queue()->message_bytes() + mb->total_size() >= this->msg_queue()->high_water_mark())
        {
            RAISE_EVENT(
                CPMDEvents::ev_signal_rejected,
                LM_INFO,
                "CHECK");
            mb->release();
        }
        else
        {
            putq(mb);
        }
    }
    else
    {
        RAISE_EVENT(
            CPMDEvents::ev_new_ace_message_block_failure,
            LM_CRITICAL);
        return -1;
    }

    bool stop(false);
    while (!stop)
    {
        ACE_Message_Block *mb = 0;
        if (this->getq(mb) != -1)
        {
            ACE_OS::sleep(ACE_Time_Value(0, 200000));

            ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

            switch(mb->msg_type())
            {
                case CHECK:
                    {
                        std::list<ICPM_module::ProcessDef> procs_to_check;
                        starter_.get_procs_to_manage(procs_to_check);

                        std::list<std::string> list;
                        CPMUtils_ns::CPMUtils::get_system_processes_list(list);

                        std::list<ICPM_module::ProcessDef>::iterator it;
                        for (it = procs_to_check.begin(); it != procs_to_check.end(); it++)
                        {
                            //ACE_DEBUG((LM_DEBUG, "%T - %s - check %s\n", _SRC, it->process_id.operator const CORBA::Char *()));

                            std::string proc_id = (const char *)it->process_id;

                            try
                            {
                                std::string module_name = it->module_name;
                                if (std::find(list.begin(), list.end(), module_name + CPMUtils_ns::CPMUtils::proc_extention()) == list.end())
                                {
                                    throw 0;
                                }

                                // Process is alive, test for this particular process (processes may have the same module name with different objects in there)
                                std::string location;
                                if (!CPMUtils_ns::CPMUtils::get_object_location(*it, location))
                                {
                                    ACE_DEBUG((LM_DEBUG, "%T - %s - **** bad location : %s !!!\n", _SRC, proc_id.c_str()));
                                }
                                else
                                {
                                    //ACE_DEBUG((LM_DEBUG, "%T - %s - get_ref on %s\n", _SRC, location.c_str()));
                                    CPMTAOLAYER->set_connection_timeout_orb(100);
                                    //ACE_DEBUG((LM_DEBUG, "%T - %s - after get_ref on %s\n", _SRC, location.c_str()));

                                    //ACE_DEBUG((LM_DEBUG, "%T - %s - get_ref on %s\n", _SRC, proc_id.c_str()));

                                    CPMTAOLayer::GetRefStatus status;
                                    ::ICPM_module::ICPMProcess_var cpm_process_obj = CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMProcess>(location.c_str(), status, 1);

                                    if (cpm_process_obj)
                                    {
                                        std::map<std::string, ACE_Time_Value>::iterator it = map_unreachable_objects_.find(location);
                                        if (it != map_unreachable_objects_.end())
                                        {
                                            map_unreachable_objects_.erase(it);
                                        }
                                    }
                                    else
                                    {
                                        //ACE_DEBUG((LM_DEBUG, "%T - %s - **** can't get reference on : %s !!!\n", _SRC, proc_id.c_str()));

                                        std::map<std::string, ACE_Time_Value>::iterator it = map_unreachable_objects_.find(location);
                                        if (it == map_unreachable_objects_.end())
                                        {
                                            ACE_Time_Value begin = ACE_OS::gettimeofday();
                                            map_unreachable_objects_[location] = begin;
                                            //ACE_DEBUG((LM_DEBUG, "%T - %s - **** can't get reference on : %s (begin = %u)\n", _SRC, proc_id.c_str(), begin.get_msec()));
                                        }
										else
										{
											ACE_Time_Value diff_with_start = ACE_OS::gettimeofday() - start_timestamp_;

											//ACE_DEBUG((LM_DEBUG, "%T - %s - %s - **** diff_with_start = %u\n", _SRC, location.c_str(), diff_with_start.get_msec()));

											if (diff_with_start.get_msec() > unreachable_object_timeout_at_start_ms_)
											{
												int timeout = unreachable_object_timeout_ms_;
												ACE_DEBUG((LM_DEBUG, "%T - %s - **** timeout = %d\n", _SRC, timeout));

												ACE_Time_Value diff = ACE_OS::gettimeofday() - map_unreachable_objects_[location];

												ACE_DEBUG((LM_DEBUG, "%T - %s - Compare %s (diff = %u, timeout = %d)\n", _SRC, location.c_str(), diff.get_msec(), timeout));

												if (diff.get_msec() > timeout)
												{
													map_unreachable_objects_.erase(it);
													throw 0;
												}
											}
										}
                                    }
                                }
                            }
                            catch(...)
                            {
                                ACE_DEBUG((LM_DEBUG, "%T - %s - %s is not there anymore. Remove it from the list to check.!!!\n", 
                                    _SRC, 
                                    it->process_id.operator const CORBA::Char *()));
                                RAISE_EVENT(
                                    CPMDEvents::ev_process_not_responding_not_checked_anymore,
                                    LM_ERROR,
                                    it->process_id.operator const CORBA::Char *(),
                                    unreachable_object_timeout_ms_);
                                starter_.remove_process(it->process_id.operator const CORBA::Char *());
                            }
                        }

                        ACE_Message_Block *mb = 0;
                        ACE_NEW_NORETURN(mb, ACE_Message_Block(0, ACE_Message_Block::MB_EVENT));
                        if (mb != 0)
                        {
                            mb->msg_type(CHECK);
                            if (this->msg_queue()->message_bytes() + mb->total_size() >= this->msg_queue()->high_water_mark())
                            {
                                RAISE_EVENT(
                                    CPMDEvents::ev_signal_rejected,
                                    LM_INFO,
                                    "CHECK");
                                mb->release();
                            }
                            else
                            {
                                putq(mb);
                            }
                        }
                    }

                    break;

                case TERMINATE:
                    {
                        // Get processes_stopping_timeout_ms parameter
                        char * pPath = getenv ("CPM_ETC");
                        if (!pPath)
                        {
                            RAISE_EVENT(
                                CPMDEvents::ev_env_var_not_defined,
                                LM_ERROR,
                                "CPM_ETC");
                            return 0;
                        }

                        std::string path(pPath);
                        CPMUtils_ns::CPMUtils::backslash_to_slash(path);

                        std::string file = path + "/cpm.cfg";
                        file = FileShortcut_ns::FileShortcut::resolve(file);

                        Config cfg;

                        int processes_stopping_timeout_ms;

                        try
                        {
                            cfg.readFile(file.c_str());

                            // root
                            Setting & root = cfg.getRoot();

                            // boot
                            Setting & boot = root["cpmd"];

                            // processes_stopping_timeout
                            if (!boot.lookupValue("processes_stopping_timeout_ms", processes_stopping_timeout_ms))
                            {
                                RAISE_EVENT(
                                    CPMUtils_ns::CPMUtils::settings_field_not_found(),
                                    LM_ERROR,
                                    "cpmd/processes_stopping_timeout_ms");
                                return false;
                            }
                        }
                        CPM_SETTINGS_CATCH(file)

                        starter_.stop(processes_stopping_timeout_ms);

                        terminated_ = true;
                    }
                    stop = true;
                    break;
            }

            mb->release();
        }
    }

    return 0;
}

::CORBA::Boolean CPMDObj_i::start(const ::ICPM_module::ProcessSeq & procs)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

    if (terminated_)
    {
        return 0;
    }

    // Get starting_proc_timeout_ms and unreachable_object_timeout_ms parameter
    char * pPath = getenv ("CPM_ETC");
    if (!pPath)
    {
        RAISE_EVENT(
            CPMDEvents::ev_env_var_not_defined,
            LM_ERROR,
            "CPM_ETC");
        return 0;
    }

    std::string path(pPath);
    CPMUtils_ns::CPMUtils::backslash_to_slash(path);

    std::string file = path + "/cpm.cfg";
    file = FileShortcut_ns::FileShortcut::resolve(file);

    Config cfg;

    int starting_proc_timeout_ms;
    int starting_proc_tries;

    try
    {
        cfg.readFile(file.c_str());

        // root
        Setting & root = cfg.getRoot();

        // boot
        Setting & boot = root["cpmd"];

        // starting_proc_timeout_ms
        if (!boot.lookupValue("starting_proc_timeout_ms", starting_proc_timeout_ms))
        {
            RAISE_EVENT(
                CPMUtils_ns::CPMUtils::settings_field_not_found(),
                LM_ERROR,
                "cpmd/starting_proc_timeout_ms");
            return false;
        }

        // starting_proc_tries
        if (!boot.lookupValue("starting_proc_tries", starting_proc_tries))
        {
            RAISE_EVENT(
                CPMUtils_ns::CPMUtils::settings_field_not_found(),
                LM_ERROR,
                "cpmd/starting_proc_tries");
            return false;
        }

        // unreachable_object_timeout_at_start_ms
        if (!boot.lookupValue("unreachable_object_timeout_at_start_ms", unreachable_object_timeout_at_start_ms_))
        {
            RAISE_EVENT(
                CPMUtils_ns::CPMUtils::settings_field_not_found(),
                LM_ERROR,
                "cpmd/unreachable_object_timeout_at_start_ms");
            return 0;
        }

        // unreachable_object_timeout
        if (!boot.lookupValue("unreachable_object_timeout_ms", unreachable_object_timeout_ms_))
        {
            RAISE_EVENT(
                CPMUtils_ns::CPMUtils::settings_field_not_found(),
                LM_ERROR,
                "cpmd/unreachable_object_timeout_ms");
            return 0;
        }
    }
    CPM_SETTINGS_CATCH(file)

    //ACE_DEBUG((LM_DEBUG, "%s - START procs\n", _SRC));

    bool started = starter_.start(procs, starting_proc_timeout_ms, starting_proc_tries);

    if (started)
    {
        start_timestamp_ = ACE_OS::gettimeofday();
    }

    return started;
}

::CORBA::Boolean CPMDObj_i::stop(const ::ICPM_module::ProcessSeq & procs)
{
    //ACE_DEBUG((LM_DEBUG, "%s - 0000\n", _SRC));
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);
    //ACE_DEBUG((LM_DEBUG, "%s - 1111\n", _SRC));

    // Get processes_stopping_timeout_ms parameter
    char * pPath = getenv ("CPM_ETC");
    if (!pPath)
    {
        RAISE_EVENT(
            CPMDEvents::ev_env_var_not_defined,
            LM_ERROR,
            "CPM_ETC");
        return 0;
    }

    std::string path(pPath);
    CPMUtils_ns::CPMUtils::backslash_to_slash(path);

    std::string file = path + "/cpm.cfg";
    file = FileShortcut_ns::FileShortcut::resolve(file);

    Config cfg;

    int processes_stopping_timeout_ms;

    try
    {
        cfg.readFile(file.c_str());

        // root
        Setting & root = cfg.getRoot();

        // boot
        Setting & boot = root["cpmd"];

        // processes_stopping_timeout
        if (!boot.lookupValue("processes_stopping_timeout_ms", processes_stopping_timeout_ms))
        {
            RAISE_EVENT(
                CPMUtils_ns::CPMUtils::settings_field_not_found(),
                LM_ERROR,
                "cpmd/processes_stopping_timeout_ms");
            return false;
        }
    }
    CPM_SETTINGS_CATCH(file)

    return starter_.stop(procs, processes_stopping_timeout_ms);
}

void CPMDObj_i::show_console (
    const ::ICPM_module::ProcessSeq & procs,
    ::CORBA::Boolean show)
{
    for(CORBA::ULong i = 0; i < procs.length(); i++)
    {
        ICPM_module::ProcessDef proc = procs[i];

        std::string module_name = proc.module_name.operator const CORBA::Char *();

        if (CPMUtils_ns::CPMUtils::is_process_running(module_name))
        {
            std::string location;
            if (!CPMUtils_ns::CPMUtils::get_object_location(proc, location))
            {
                RAISE_EVENT(
                    CPMDEvents::ev_getting_object_location_failure,
                    LM_ERROR,
                    proc.process_id.operator const CORBA::Char *());
                continue;
            }

            try
            {
                CPMTAOLayer::GetRefStatus status;
                ::ICPM_module::ICPMProcess_var cpm_process_obj =
                    CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMProcess>(location.c_str(), status);
                if (cpm_process_obj)
                {
                    //ACE_DEBUG((LM_DEBUG, "Show/Hide console...\n"));
                    cpm_process_obj->show_console(show);
                }
                else
                {
                    throw 0;
                }
            }
            catch(...)
            {
                RAISE_EVENT(
                    CPMDEvents::ev_showing_exception,
                    LM_ERROR,
                    module_name.c_str(),
                    show ? "show" : "hide");
            }
        }
    }
}

void CPMDObj_i::show_gui (
    const ::ICPM_module::ProcessSeq & procs,
    ::CORBA::Boolean show)
{
    for(CORBA::ULong i = 0; i < procs.length(); i++)
    {
        ICPM_module::ProcessDef proc = procs[i];

        std::string module_name = proc.module_name.operator const CORBA::Char *();

        if (CPMUtils_ns::CPMUtils::is_process_running(module_name))
        {
            std::string location;
            if (!CPMUtils_ns::CPMUtils::get_object_location(proc, location))
            {
                RAISE_EVENT(
                    CPMDEvents::ev_getting_object_location_failure,
                    LM_ERROR,
                    proc.process_id.operator const CORBA::Char *());
                continue;
            }

            try
            {
                CPMTAOLayer::GetRefStatus status;
                ::ICPM_module::ICPMProcess_var cpm_process_obj =
                    CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMProcess>(location.c_str(), status);
                if (cpm_process_obj)
                {
                    //ACE_DEBUG((LM_DEBUG, "Show/Hide console...\n"));
                    cpm_process_obj->show_gui(show);
                }
                else
                {
                    throw 0;
                }
            }
            catch(...)
            {
                RAISE_EVENT(
                    CPMDEvents::ev_showing_exception,
                    LM_ERROR,
                    module_name.c_str(),
                    show ? "show" : "hide");
            }
        }
    }
}

void CPMDObj_i::on_new_getting_reference_timeout (
    ::CORBA::Long loop_timeout_ms,
    ::CORBA::Long attempts)
{
    static int greatest(0);

    int prod = loop_timeout_ms * attempts;

    if (unreachable_object_timeout_at_start_ms_ < prod
        && greatest < prod)
    {
        greatest = prod;

        RAISE_EVENT(
            CPMDEvents::ev_new_getting_reference_timeout,
            LM_NOTICE,
            loop_timeout_ms,
            attempts,
            CPMUtils_ns::CPMUtils::get_local_addr().c_str(),
            greatest);
    }
}

void CPMDObj_i::get_launched_processes (::ICPM_module::ProcessSeq_out processes)
{
    std::list<ICPM_module::ProcessDef> procs;
    starter_.get_procs_to_manage(procs);

    processes = new ::ICPM_module::ProcessSeq;
    processes->length(static_cast<::CORBA::ULong>(procs.size()));

    ::CORBA::ULong i(0);
    for (auto it = procs.begin(); it != procs.end(); it++)
    {
        processes[i] = *it;
        ++i;
    }
}

void CPMDObj_i::terminate()
{
    ACE_Message_Block *mb = 0;
    ACE_NEW_NORETURN(mb, ACE_Message_Block(0, ACE_Message_Block::MB_EVENT));
    if (mb != 0)
    {
        mb->msg_type(TERMINATE);
        if (this->msg_queue()->message_bytes() + mb->total_size() >= this->msg_queue()->high_water_mark())
        {
            RAISE_EVENT
                (CPMDEvents::ev_signal_rejected,
                LM_INFO,
                "TERMINATE");
            mb->release();
        }
        else
        {
            putq(mb);
            wait();
        }
    }
    else
    {
        RAISE_EVENT(
            CPMDEvents::ev_new_ace_message_block_failure,
            LM_CRITICAL);
        return;
    }
}
