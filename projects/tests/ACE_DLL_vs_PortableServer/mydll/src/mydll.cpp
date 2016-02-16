
#include "mydll/mydll.h"
#include "mydll/version.h"
#include <stdio.h>
#include "ace/Init_ACE.h"
#include "ace/Log_Msg.h"


void init()
{
    //int rc = ACE::init();
    //ACE_DEBUG((LM_DEBUG, "ACE::init() = %d\n", rc));

    ACE_DEBUG((LM_DEBUG, "Mydll_ns::init()\n"));
}

void fini()
{
    //int rc = ACE::fini();
    //ACE_DEBUG((LM_DEBUG, "Mydll_ns::fini() = %d\n", rc));

    ACE_DEBUG((LM_DEBUG, "Mydll_ns::fini()\n"));
}

void something()
{
    ACE_DEBUG((LM_DEBUG, "Mydll_ns::something()\n"));
}


namespace Mydll_ns
{
    void mydll_get_version(int & major, int & minor, int & patch)
    {
        major = MYDLL_MAJOR;
        minor = MYDLL_MINOR;
        patch = MYDLL_PATCH;
    }

    Mydll::Mydll(int value)
    :   m_value(value)
    {
        printf("Mydll::Mydll() - version = %d.%d.%d (%s)\n",
                MYDLL_MAJOR,
                MYDLL_MINOR,
                MYDLL_PATCH,
                MYDLL_DebugFlag);
    }

    void Mydll::show()
    {
        printf("Mydll::show() - value = %d\n",
                m_value);
        printf("\n");
    }
} // Mydll_ns namespace
