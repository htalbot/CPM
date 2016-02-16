
#ifndef __CPMEVENTRAISER_H__
#define __CPMEVENTRAISER_H__

#include "CPMCore/CPMCORE_Export.h"
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/DLL.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMEventHandlerAbstract/CPMEventHandlerAbstract.h"

namespace CPMEventRaiser_ns
{
    class CPMCORE_Export CPMEventRaiser: public BasicEventHandling_ns::BasicEventRaiser
    {
        friend ACE_Singleton<CPMEventRaiser_ns::CPMEventRaiser, ACE_Recursive_Thread_Mutex>;

    public:
        static bool initialized();

    public:
        ACE_Recursive_Thread_Mutex raise_mutex_;

    protected:
        CPMEventRaiser();
        ~CPMEventRaiser();

        bool init_plugins();

    protected:
        ACE_DLL event_handler_dll_;
        CPMEventHandlerAbstract_ns::CPMEventHandlerAbstract * event_handler_plugin_;
        static bool initialized_;

    private:
        //static void destroy(); // Defining destroy this way doesn't work. So, for those who are tempted to, don't do that!
    };
} // CPMEventRaiser_ns namespace

typedef ACE_Singleton<CPMEventRaiser_ns::CPMEventRaiser, ACE_Recursive_Thread_Mutex> CPMEventRaiserSingleton;


#ifdef ACE_WIN32
    #pragma warning(push)
    #pragma warning(disable: 4231) // will disapear when VC is conform to C++11
#endif
    CPMCORE_SINGLETON_DECLARE(ACE_Singleton, CPMEventRaiser_ns::CPMEventRaiser, ACE_Recursive_Thread_Mutex);
#ifdef ACE_WIN32
    #pragma warning(push)
#endif

#define RAISER CPMEventRaiserSingleton::instance()
#define RAISE_EVENT(ev, priority, ...)\
    if (CPMEventRaiser_ns::CPMEventRaiser::initialized())\
    {\
        try\
        {\
            ACE_Guard<ACE_Recursive_Thread_Mutex> guard(RAISER->raise_mutex_);\
            RAISER->raise_event(\
                __FUNCTION__,\
                ev,\
                priority,\
                ##__VA_ARGS__);\
        }\
        catch(...)\
        {\
            EVENT_HANDLER_SYSTEMLOG->handle_event(\
                __FUNCTION__,\
                ev,\
                priority,\
                ##__VA_ARGS__);\
        }\
    }\
    else\
    {\
        EVENT_HANDLER_SYSTEMLOG->handle_event(\
            __FUNCTION__,\
            ev,\
            priority,\
            ##__VA_ARGS__);\
    }

#define INIT_RAISER_RETURN(rc)\
    try\
    {\
        CPMEventRaiser_ns::CPMEventRaiser * r = RAISER;\
    }\
    catch(...)\
    {\
        return rc;\
    }

#endif // __CPMEVENTRAISER_H__
