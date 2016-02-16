#pragma once

#include "ace/Date_Time.h"
#include "ace/Task.h"

struct TraceFile
{
    std::string file_name_;
    time_t file_time_;
};


class CPMTraceEraserObj: public ACE_Task_Base
{
public:
    CPMTraceEraserObj();
    bool init();
    void terminate();

protected:
    bool load_settings();
    int svc();

protected:
    ACE_Atomic_Op<ACE_Recursive_Thread_Mutex,bool> terminate_;
    std::string traces_directory_;
    int frequency_ms_;
    int time_to_keep_ms_;
};
