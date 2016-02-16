
#include "CPMDServiceAppLayer.h"
#include "CPMDService/version.h"
#include <stdio.h>

CPMDServiceAppLayer::CPMDServiceAppLayer()
{
}


int CPMDServiceAppLayer::run()
{
    printf("CPMDServiceAppLayer - version: %d.%d.%d (%s)\n",
            CPMDSERVICE_MAJOR,
            CPMDSERVICE_MINOR,
            CPMDSERVICE_PATCH,
            CPMDSERVICE_DebugFlag);
    printf("\n");

    return 0;
}

