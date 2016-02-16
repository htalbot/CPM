#include "CPMProcessMainIncludes.h"
#include "CPMProcessMain.h"

#ifdef CPMPROCESS_WX
    #include "CPMProcessMainWx.h"
    CPMProcessMainWx
#else
    CPMProcessMain
#endif


