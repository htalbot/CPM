
#ifndef CPMEVENTHANDLERABSTRACT_H
#define CPMEVENTHANDLERABSTRACT_H

#include "BasicEventHandling/BasicEventHandling.h"

namespace CPMEventHandlerAbstract_ns
{
    class CPMEventHandlerAbstract: public BasicEventHandling_ns::BasicEventHandler
    {
    public:

        virtual ~CPMEventHandlerAbstract() = 0
        {
        }

        virtual bool init() = 0;

        virtual bool handle_event(const char * szSource,
                                    BasicEventHandling_ns::BasicEvent & ev,
                                    ACE_Log_Priority nPriority,
                                    ...) = 0;
    };

} // CPMEventHandlerAbstract_ns namespace

#endif // CPMEVENTHANDLERABSTRACT_H
