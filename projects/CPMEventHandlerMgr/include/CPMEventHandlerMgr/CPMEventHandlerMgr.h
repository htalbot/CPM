
#ifndef __CPMEVENTHANDLERMGR_H__
#define __CPMEVENTHANDLERMGR_H__

#include "CPMEventHandlerAbstract/plugin_macros.h"
#include "CPMEventHandlerAbstract/CPMEventHandlerAbstract.h"

#include "CPMEventHandlerMgr/CPMEVENTHANDLERMGR_Export.h"
#include "CPMEventHandlerMgr/version.h"

#include "ICPM/ICPMC.h"
#include "BasicEventHandling/BasicEventHandling.h"

namespace CPMEventHandlerMgr_ns
{
    extern "C" CPMEVENTHANDLERMGR_Export void CPMEventHandlerMgr_get_version(int & major, int & minor, int & patch);

    class CPMEVENTHANDLERMGR_Export CPMEventHandlerMgr:
        public CPMEventHandlerAbstract_ns::CPMEventHandlerAbstract,
        public BasicEventHandling_ns::BasicEventRaiser
    {
    public:
        CPMEventHandlerMgr();
        ~CPMEventHandlerMgr();

        virtual bool init();
        virtual bool handle_event(const char * szSource,
                                    BasicEventHandling_ns::BasicEvent & ev,
                                    ACE_Log_Priority nPriority,
                                    ...);
        virtual bool vhandle_event(const char * szSource,
                                    BasicEventHandling_ns::BasicEvent & ev,
                                    ACE_Log_Priority nPriority,
                                    va_list args);
        virtual void get_version(int & major,
                                    int & minor,
                                    int & patch);

        PLUGIN_MEM_DECLARE;

    protected:
        ICPM_module::ICPMEventMgr_var timed_cpm_event_mgr_;
        bool initialized_;
        ACE_Recursive_Thread_Mutex init_mutex_;
        ACE_Recursive_Thread_Mutex handle_event_mutex_;
        ACE_Recursive_Thread_Mutex vhandle_event_mutex_;
    };

} // CPMEventHandlerMgr_ns namespace


#endif // __CPMEVENTHANDLERMGR_H__
