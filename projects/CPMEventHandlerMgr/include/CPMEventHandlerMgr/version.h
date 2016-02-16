
#pragma once

#define CPMEVENTHANDLERMGR_MAJOR 2
#define CPMEVENTHANDLERMGR_MINOR 2
#define CPMEVENTHANDLERMGR_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMEVENTHANDLERMGR_DebugFlag[] = "DEBUG";
#   else
        const char CPMEVENTHANDLERMGR_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMEVENTHANDLERMGR_DebugFlag[] = "DEBUG";
#   else
        const char CPMEVENTHANDLERMGR_DebugFlag[] = "RELEASE";
#   endif
#endif


