#ifndef CPMM_CPMMOBJ_I_H__
#define CPMM_CPMMOBJ_I_H__

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Task.h"
#include "ICPM/ICPMS.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include <string>
#include <list>
#include <map>

class Monitor: public ACE_Task_Base
{
public:
    int svc();
};

class CPMMObj_i :
    public POA_ICPM_module::ICPMM,
    public ACE_Task_Base
{
public:
    CPMMObj_i();
    ~CPMMObj_i();

    virtual void get_settings(
      ::ICPM_module::CPMMSettings_out inital,
      ::ICPM_module::CPMMSettings_out current,
      ::CORBA::String_out current_settings_file);

    virtual ::CORBA::Boolean start(
      const ::ICPM_module::ProcessSeq & procs);

    virtual ::CORBA::Boolean stop(
      const ::ICPM_module::ProcessSeq & procs);

    virtual void show_console (
      const ::ICPM_module::ProcessSeq & procs,
      ::CORBA::Boolean show);

    virtual void show_gui (
      const ::ICPM_module::ProcessSeq & procs,
      ::CORBA::Boolean show);

    virtual ::CORBA::Boolean started();

    virtual ::CORBA::Boolean get_unavailable (
      ::ICPM_module::UnavailableElementIDSeq_out nodes,
      ::ICPM_module::UnavailableElementIDSeq_out processes);

    virtual void update_settings(
      const ::ICPM_module::CPMMSettings & new_initial_settings,
      const ::ICPM_module::CPMMSettings & new_current_settings,
      const char * file);

    virtual ::CORBA::Boolean save_settings_to(
      const char * target_file);

    virtual ::CORBA::Boolean set_as_default(
      void);

    enum enum_popup
    {
        action_start,
        action_stop,
        action_show_console,
        action_hide_console,
        action_show_gui,
        action_hide_gui
    };

public:

    bool init_i(bool overwrite_autostart_to_false);
    void terminate();
    int svc();

protected:
    ::ICPM_module::CPMMSettings initial_settings_;
    ::ICPM_module::CPMMSettings current_settings_;
    std::string current_settings_file_;
    std::string cpmd_port_;
    std::map<std::string, ::ICPM_module::ProcessDef> started_processes_;
    std::list<std::string> unavailable_nodes_;
    std::list<std::string> unavailable_processes_;
    ACE_Atomic_Op<ACE_Recursive_Thread_Mutex,bool> want_to_stop_;
    ACE_Recursive_Thread_Mutex start_stop_mutex_;
    ACE_Recursive_Thread_Mutex unavailable_mutex_;
    ACE_Recursive_Thread_Mutex settings_mutex_;
    ACE_Time_Value started_at_;
    ACE_Atomic_Op<ACE_Recursive_Thread_Mutex,bool> started_;

    bool validate_settings(const ::ICPM_module::CPMMSettings &);
    void launch(const ::ICPM_module::CPMMSettings & config);
    void start_or_stop(const ::ICPM_module::ProcessSeq & procs, enum_popup);
    void check(bool & have_to_sleep);

    bool get_procs_from_cpmd(std::string & node_id, std::list<std::string> & processes);
    bool stop_all_processes_on_all_nodes();
    void wait_for_required_time_before_stop_since_start();
};

#endif /* CPMM_CPMMOBJ_I_H__ */
