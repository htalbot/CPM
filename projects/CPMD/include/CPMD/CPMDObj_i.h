#ifndef CPMD_CPMDOBJ_I_H__
#define CPMD_CPMDOBJ_I_H__

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Task.h"
#include "ICPM/ICPMS.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMCore/CPMProcessStarter.h"

#include <map>
#include <string>

class CPMDObj_i :
    public POA_ICPM_module::ICPMD,
    public ACE_Task<ACE_SYNCH>
{
public:
    CPMDObj_i();
    ~CPMDObj_i();

    virtual void get_version(
      ::CORBA::Long_out major,
      ::CORBA::Long_out minor,
      ::CORBA::Long_out patch);
    virtual void ctrl_ping();

    virtual ::CORBA::Boolean start(const ::ICPM_module::ProcessSeq & procs);
    virtual ::CORBA::Boolean stop(const ::ICPM_module::ProcessSeq & procs);
    virtual void show_console (
      const ::ICPM_module::ProcessSeq & procs,
      ::CORBA::Boolean show);
    virtual void show_gui (
      const ::ICPM_module::ProcessSeq & procs,
      ::CORBA::Boolean show);
    virtual void on_new_getting_reference_timeout (
      ::CORBA::Long loop_timeout_ms,
      ::CORBA::Long attempts);

public:
    virtual void get_launched_processes(::ICPM_module::ProcessSeq_out processes);
    void terminate();

    int svc();

public:
    enum {
        CHECK,
        TERMINATE
    };

protected:

protected:
    ACE_Recursive_Thread_Mutex start_stop_mutex_;
    bool terminated_;
    ACE_Time_Value start_timestamp_;
    int unreachable_object_timeout_at_start_ms_;
    int unreachable_object_timeout_ms_;
    std::map<std::string, ACE_Time_Value> map_unreachable_objects_;

    CPMProcessStarter starter_;
};

#endif /* CPMD_CPMDOBJ_I_H__ */


