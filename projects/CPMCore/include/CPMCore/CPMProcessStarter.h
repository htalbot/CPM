#pragma once

#include "CPMCore/CPMCORE_Export.h"
#include "ICPM/ICPMC.h"
#include <list>
#include <map>

class CPMCORE_Export CPMProcessStarter
{
public:
    bool start(
        const ::ICPM_module::ProcessSeq & procs,
        int starting_proc_timeout_ms,
        int starting_proc_tries);
    bool stop(
        const ::ICPM_module::ProcessSeq & procs,
        int processes_stopping_timeout_ms);
    bool stop(
        int processes_stopping_timeout_ms);
    void get_procs_to_manage(std::list<ICPM_module::ProcessDef> & procs);
    void remove_process(const std::string & proc_id);
    bool launched_by_another_process_than_cpmd(ICPM_module::ProcessDef &);

protected:
    std::map<std::string, ICPM_module::ProcessDef> procs_to_manage_;
    ACE_Recursive_Thread_Mutex procs_to_manage_mutex_;
};