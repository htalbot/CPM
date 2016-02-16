
#include "ace/Init_ACE.h"
#include "ace/DLL.h"
#include "ace/Log_Msg.h"

// To reproduce the bug, comment the following #include.
//#include "tao/PortableServer/PortableServer.h"

// CPMCore provides another header that avoid the issue.
// You can include it instead of tao/PortableServer/PortableServer.h
//#include "CPMCore/CPMTAOLayerContrib.h"

// With MSVC, we need to rebuild (not just build)

int main(int, char * [])
{
    //ACE::init();


    ACE_DLL dll;

    int retval = dll.open (ACE_DLL_PREFIX ACE_TEXT("mydlld"));
    if (retval != 0)
    {
        ACE_DEBUG((LM_DEBUG, "Failed to load mydll\n"));
        return -1;
    }

    typedef int (*MY_DLL_VOI_FN_VOID_PARAM)();

    void *void_ptr = dll.symbol (ACE_TEXT ("init"));
    ptrdiff_t tmp = reinterpret_cast<ptrdiff_t> (void_ptr);
    MY_DLL_VOI_FN_VOID_PARAM init = reinterpret_cast<MY_DLL_VOI_FN_VOID_PARAM> (tmp);
    (*init)();


    typedef void (*SOMETHING)();
    void_ptr = dll.symbol (ACE_TEXT ("something"));
    tmp = reinterpret_cast<ptrdiff_t> (void_ptr);
    SOMETHING something = reinterpret_cast<SOMETHING> (tmp);
    (*something)();

    void_ptr = dll.symbol (ACE_TEXT ("fini"));
    tmp = reinterpret_cast<ptrdiff_t> (void_ptr);
    MY_DLL_VOI_FN_VOID_PARAM fini = reinterpret_cast<MY_DLL_VOI_FN_VOID_PARAM> (tmp);
    (*fini)();

    dll.close ();


    //ACE::fini();

    return 0;
}
