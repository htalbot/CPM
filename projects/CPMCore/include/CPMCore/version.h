
#pragma once

#define CPMCORE_MAJOR 2
#define CPMCORE_MINOR 2
#define CPMCORE_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMCORE_DebugFlag[] = "DEBUG";
#   else
        const char CPMCORE_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMCORE_DebugFlag[] = "DEBUG";
#   else
        const char CPMCORE_DebugFlag[] = "RELEASE";
#   endif
#endif


