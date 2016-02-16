
#include "CPMGAppLayer.h"
#include "CPMG/version.h"
#include <stdio.h>

CPMGAppLayer::CPMGAppLayer()
{
}


int CPMGAppLayer::run()
{
    printf("CPMGAppLayer - version: %d.%d.%d (%s)\n",
            CPMG_MAJOR,
            CPMG_MINOR,
            CPMG_PATCH,
            CPMG_DebugFlag);
    printf("\n");

    return 0;
}

