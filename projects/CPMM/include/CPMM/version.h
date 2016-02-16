#ifndef __CPMM_VERSION_H__
#define __CPMM_VERSION_H__

#define CPMM_MAJOR 2
#define CPMM_MINOR 2
#define CPMM_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMM_DebugFlag[] = "DEBUG";
#   else
        const char CPMM_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMM_DebugFlag[] = "DEBUG";
#   else
        const char CPMM_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMM_VERSION_H__
