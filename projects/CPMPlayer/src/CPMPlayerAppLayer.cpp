
#include "CPMPlayerAppLayer.h"
#include "CPMPlayer/version.h"
#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMEventRaiser.h"
#include <string>

CPMPlayerAppLayer::CPMPlayerAppLayer()
{
    CPMAPPLAYER->set(this);
}

void syntax(int argc, char * argv[])
{
    std::string program_name = argv[0];

    printf("\nSyntax: %s <boot-cpmd|boot-cpmm|shutdown-cpmd|shutdown-cpmm>\n\n", program_name.c_str());
}

int CPMPlayerAppLayer::run(int argc, char * argv[])
{
    INIT_RAISER_RETURN(-1);

    printf("CPMPlayerAppLayer - version: %d.%d.%d (%s)\n",
            CPMPLAYER_MAJOR,
            CPMPLAYER_MINOR,
            CPMPLAYER_PATCH,
            CPMPLAYER_DebugFlag);
    printf("\n");

    if (argc != 2)
    {
        syntax(argc, argv);
        return -2;
    }

    std::string cmd = argv[1];

    if (cmd == "boot-cpmd")
    {
        if (!CPMUtils_ns::CPMUtils::boot_cpmd())
        {
            printf("Failed to boot CPMD. See system log.\n\n");
            return -3;
        }
    }
    else if (cmd == "boot-cpmm")
    {
        if (!CPMUtils_ns::CPMUtils::boot_cpm())
        {
            printf("Failed to boot CPMM. See system log.\n\n");
            return -4;
        }
    }
    else if (cmd == "shutdown-cpmm")
    {
        if (!CPMUtils_ns::CPMUtils::shutdown_cpmm())
        {
            printf("Failed to shutdown CPMM. See system log.\n\n");
            return -5;
        }
    }
    else if (cmd == "shutdown-cpmd")
    {
        if (!CPMUtils_ns::CPMUtils::shutdown_cpmd())
        {
            printf("Failed to shutdown CPMD. See system log.\n\n");
            return -6;
        }
    }
    else
    {
        bool first(true);
        std::string args;
        for (int i = 1; i != argc; i++)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                args += ", ";
            }

            args += argv[i];
        }

        printf("Bad arguments: %s", args.c_str());

        syntax(argc, argv);

        return -7;
    }

    return 0;
}

