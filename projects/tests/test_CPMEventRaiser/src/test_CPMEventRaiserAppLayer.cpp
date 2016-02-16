
#include "test_CPMEventRaiserAppLayer.h"
#include "test_CPMEventRaiser/version.h"
#include "CPMCore/CPMEventRaiser.h"
#include "ace/Log_Msg.h"

// Needed to allow ACE_Object_Manager to take over
// static variables instanciated by dynamically loaded
// libraries via CPMUtils.
//#include "CPMCore/CPMTAOLayerContrib.h"

using namespace CPMEventRaiser_ns;

Test_CPMEventRaiserAppLayer::Test_CPMEventRaiserAppLayer()
{
    CPMAPPLAYER->set(this);
}

Test_CPMEventRaiserAppLayer::~Test_CPMEventRaiserAppLayer()
{
}


class MyTask: public ACE_Task_Base
{
protected:
    int svc()
    {
        BasicEventHandling_ns::BasicEvent ev("Test...", 1, "%1(%s) - ping failure on '%2(%s)'.");
        for (int i = 0; i != 20; i++)
        {
            ACE_DEBUG((LM_DEBUG, "%s - %d\n", _SRC, i));
            RAISE_EVENT(ev,
                LM_DEBUG,
                "start",
                "localhost");
        }

        return 0;
    }
};

int Test_CPMEventRaiserAppLayer::run()
{
    INIT_RAISER_RETURN(false);

    try
    {
        long lValue(2147483647);
        unsigned long ulValue(4294967290);
        float fValue(3.4f);
        double dValue(5.7);
        long long llVal(1);
        llVal <<= 60;
        unsigned long long ullVal(1);
        ullVal <<= 62;
        std::string string("Hello");

        BasicEventHandling_ns::BasicEvent ev0("test_cpmeventraiser_namespace", 1, "This is a test with CPMEventRaiser: "
            "long = %1(%d), "
            "unsigned long = %2(%u), "
            "float = %3(%f), "
            "double = %4(%f), "
            "long long = %5(%ll), "
            "unsigned long long = %6(%llu), "
            "string = '%7(%s)', "
            "%8(%s)");
        RAISE_EVENT(ev0,
            LM_DEBUG,
            lValue,
            ulValue,
            fValue,
            dValue,
            llVal,
            ullVal,
            "Hello",
            "point final.");

        BasicEventHandling_ns::BasicEvent ev1("test_cpmeventraiser_namespace", 1, "This is a test with CPMEventRaiser: int = %1(%d), float = %2(%lf), double = %3(%lf).");
        RAISE_EVENT(ev1,
            LM_DEBUG,
            10,
            fValue,
            dValue);

        BasicEventHandling_ns::BasicEvent ev2("Test...", 1, "%1(%s) - ping failure on '%2(%s)'.");
        RAISE_EVENT(ev2,
            LM_DEBUG,
            "start",
            "localhost");

        MyTask task;
        task.activate(0, 5);

        task.wait();
    }
    catch(...)
    {
        printf("Exception...\n");
    }

    return 0;
}

