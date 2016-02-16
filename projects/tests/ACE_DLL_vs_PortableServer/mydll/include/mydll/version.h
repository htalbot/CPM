
#pragma once

#define MYDLL_MAJOR 2
#define MYDLL_MINOR 1
#define MYDLL_PATCH 7


#if defined _MSC_VER
#   ifdef _DEBUG
        const char MYDLL_DebugFlag[] = "DEBUG";
#   else
        const char MYDLL_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char MYDLL_DebugFlag[] = "DEBUG";
#   else
        const char MYDLL_DebugFlag[] = "RELEASE";
#   endif
#endif


